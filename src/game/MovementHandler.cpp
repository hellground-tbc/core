/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "Corpse.h"
#include "Player.h"
#include "MapManager.h"
#include "Transports.h"
#include "BattleGround.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"
#include "AntiCheat.h"
#include "ObjectMgr.h"

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket & /*recv_data*/)
{
    sLog.outDebug("WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // get start teleport coordinates (will used later in fail case)
    WorldLocation old_loc;
    GetPlayer()->GetPosition(old_loc);

    // get the teleport destination
    WorldLocation &loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation))
    {
        // stop teleportation else we would try this again and again in LogoutPlayer...
        GetPlayer()->SetSemaphoreTeleport(false);
        // player don't gets saved - so his coords will stay at the point where
        // he was last saved
        LogoutPlayer(false);
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.mapid);

    Map *map = NULL;

    // prevent crash at attempt landing to not existed battleground instance
    if(mEntry->IsBattleGroundOrArena())
    {
        if (GetPlayer()->GetBattleGroundId())
            map = sMapMgr.FindMap(loc.mapid, GetPlayer()->GetBattleGroundId());

        if (!map)
        {
            GetPlayer()->SetSemaphoreTeleport(false);

            // Teleport to previous place, if cannot be ported back TP to homebind place
            if (!GetPlayer()->TeleportTo(old_loc))
                GetPlayer()->TeleportToHomebind();

            return;
        }
    }

    InstanceTemplate const* mInstance = ObjectMgr::GetInstanceTemplate(loc.mapid);

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    GetPlayer()->SetSemaphoreTeleport(false);

    // relocate the player to the teleport destination
    if (!map)
        map = sMapMgr.CreateMap(loc.mapid, GetPlayer());

    GetPlayer()->SetMapId(loc.mapid);
    GetPlayer()->SetMap(map);

    GetPlayer()->Relocate(loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation);

    // since the MapId is set before the GetInstance call, the InstanceId must be set to 0
    // to let GetInstance() determine the proper InstanceId based on the player's binds
    GetPlayer()->SetInstanceId(map->GetInstanceId());

    // check this before Map::Add(player), because that will create the instance save!
    bool reset_notify = (GetPlayer()->GetBoundInstance(GetPlayer()->GetMapId(), GetPlayer()->GetDifficulty()) == NULL);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!GetPlayer()->GetMap()->Add(GetPlayer()))
    {
        // Teleport to previous place, if cannot be ported back TP to homebind place
        GetPlayer()->SetDontMove(false);
        if (!GetPlayer()->TeleportTo(old_loc))
            GetPlayer()->TeleportToHomebind();

        return;
    }

    //this will set player's team ... so IT MUST BE CALLED BEFORE SendInitialPacketsAfterAddToMap()
    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (_player->InBattleGround())
    {
        // cleanup seting if outdated
        if (!mEntry->IsBattleGroundOrArena())
        {
            // Do next only if found in battleground
            _player->SetBattleGroundId(0);                          // We're not in BG.
            // reset destination bg team
            _player->SetBGTeam(TEAM_NONE);
        }
        // join to bg case
        else if (BattleGround *bg = _player->GetBattleGround())
        {
            if (_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType()==FLIGHT_MOTION_TYPE)
    {
        if (!_player->InBattleGround())
        {
            // short preparations to continue flight
            GetPlayer()->SetDontMove(false);
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Reset(*GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    Corpse *corpse = GetPlayer()->GetCorpse();
    if (corpse && corpse->GetType() != CORPSE_BONES && corpse->GetMapId() == GetPlayer()->GetMapId())
    {
        if (mEntry->IsDungeon())
        {
            GetPlayer()->ResurrectPlayer(0.5f,false);
            GetPlayer()->SpawnCorpseBones();
        }
    }

    if (mEntry->IsRaid() && mInstance)
    {
        if (reset_notify)
        {
            uint32 timeleft = sInstanceSaveManager.GetResetTimefor (GetPlayer()->GetMapId()) - time(NULL);
            GetPlayer()->SendInstanceResetWarning(GetPlayer()->GetMapId(), timeleft); // greeting at the entrance of the resort raid instance
        }
    }

    // mount allow check
    if (!mEntry->IsMountAllowed())
        _player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

    // honorless target
    if (GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // resummon pet
    if (GetPlayer()->m_temporaryUnsummonedPetNumber)
    {
        Pet* NewPet = new Pet;
        if (!NewPet->LoadPetFromDB(GetPlayer(), 0, GetPlayer()->m_temporaryUnsummonedPetNumber, true))
            delete NewPet;

        GetPlayer()->m_temporaryUnsummonedPetNumber = 0;
    }

    GetPlayer()->SetDontMove(false);
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recv_data)
{
    sLog.outDebug("MSG_MOVE_TELEPORT_ACK");
}

void WorldSession::HandleMovementOpcodes(WorldPacket & recv_data)
{
    /* extract packet */

    MovementInfo movementInfo;
    recv_data >> movementInfo;

    /*----------------*/

    if (recv_data.size() != recv_data.rpos())
    {
        sLog.outError("MovementHandler: player %s (guid %d, account %u) sent a packet (opcode %u) that is %u bytes larger than it should be. Kicked as cheater.", _player->GetName(), _player->GetGUIDLow(), _player->GetSession()->GetAccountId(), recv_data.GetOpcode(), recv_data.size() - recv_data.rpos());
        KickPlayer();
        return;
    }

    if (!Trinity::IsValidMapCoord(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o))
        return;

    Player * pPlayer = GetPlayer();

    // Handle possessed unit movement separately
    Unit* pos_unit = pPlayer->GetCharm();
    if (pos_unit && pos_unit->isPossessed()) // can be charmed but not possessed
    {
        HandlePossessedMovement(recv_data, movementInfo);
        return;
    }

    if (pPlayer->GetDontMove() || pPlayer->isCharmed())
        return;

    MovementInfo oldMovementInfo = pPlayer->m_movementInfo;

    // No fall damage cheat
    if (oldMovementInfo.GetMovementFlags() & (MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR))
    {
        if (oldMovementInfo.GetFallTime() == 357)
        {
            pPlayer->m_AC_NoFall_count ++;
            // falltime = 357 <--- WEH  No Fall Damage Cheat
            sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible no fall damage cheat. MapId: %u, falltime: %u, coords old: %f, %f, %f,coords new: %f, %f, %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                       pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), oldMovementInfo.GetFallTime(), oldMovementInfo.pos.x, oldMovementInfo.pos.y, oldMovementInfo.pos.z, movementInfo.pos.x, movementInfo.pos.y, movementInfo.pos.z, movementInfo.pos.z, movementInfo.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

            //pPlayer->Kill(pPlayer, true);
            if (!(pPlayer->m_AC_NoFall_count % 5))
                sWorld.SendGMText(LANG_ANTICHEAT_NOFALLDMG, pPlayer->GetName(), pPlayer->GetName(), pPlayer->m_AC_NoFall_count);
        }
    }

/*
    // TP cheat
    UnitMoveType m_type = pPlayer->IsFlying() ? MOVE_FLIGHT : pPlayer->IsUnderWater() ? MOVE_SWIM : MOVE_RUN;

    float m_speedRate = pPlayer->GetSpeedRate(m_type);
    float m_lastSpeedRate = pPlayer->GetLastSpeedRate();
    float m_speed = pPlayer->GetSpeed(m_type);

    if (!pPlayer->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_TAXI_FLIGHT) && pPlayer->m_taxi.empty() && m_speedRate == m_speed)
    {
        Position m_pos = oldMovementInfo.pos;
        float dx = movementInfo.pos.x - m_pos.x;
        float dy = movementInfo.pos.y - m_pos.y;
        float dz = movementInfo.pos.z - m_pos.z;
        float fDistance3d = sqrt(dx*dx + dy*dy + dz*dz);

        // time between packets
        uint32 uiDiffTime = WorldTimer::getMSTimeDiff(oldMovementInfo.time, movementInfo.time);

        float fClientRate = (fDistance3d * 1000 / uiDiffTime) / m_speed;
        float fServerRate = m_speed * uiDiffTime / 1000 +1.0f;

        if (fDistance3d > 500.0f && fClientRate > fServerRate*4 && m_speed +0.2 < m_speed*fClientRate)
        {
            pPlayer->m_AC_count++;
            pPlayer->m_AC_timer = IN_MILISECONDS;   // 1 sek

            sWorld.SendGMText(LANG_ANTICHEAT, pPlayer->GetName(), pPlayer->m_AC_count, m_speed, m_speed*fClientRate);

            sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) moved for distance %f with server speed : %f (client speed: %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                          pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), fDistance3d, m_speed, m_speed*fClientRate, pPlayer->GetMapId(), m_pos.x, m_pos.y, m_pos.z, movementInfo.pos.x, movementInfo.pos.y, movementInfo.pos.z, movementInfo.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
            pPlayer->Relocate(oldMovementInfo.pos);
        }
    }
*/

    //Save movement flags
    pPlayer->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    /* handle special cases */
    if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        // transports size limited
        // (also received at zeppelin leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (movementInfo.GetTransportPos()->x > 50 || movementInfo.GetTransportPos()->y > 50 || movementInfo.GetTransportPos()->z > 50)
            return;

        if (!Trinity::IsValidMapCoord(
            movementInfo.GetPos()->x + movementInfo.GetTransportPos()->x,
            movementInfo.GetPos()->y + movementInfo.GetTransportPos()->y,
            movementInfo.GetPos()->z + movementInfo.GetTransportPos()->z,
            movementInfo.GetPos()->o + movementInfo.GetTransportPos()->o
           ))
            return;

        // if we boarded a transport, add us to it
        if (!pPlayer->m_transport)
        {
            // elevators also cause the client to send MOVEFLAG_ONTRANSPORT - just unmount if the guid can be found in the transport list
            for (MapManager::TransportSet::iterator iter = sMapMgr.m_Transports.begin(); iter != sMapMgr.m_Transports.end(); ++iter)
            {
                if ((*iter)->GetGUID() == movementInfo.t_guid)
                {
                    // unmount before boarding
                    pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    pPlayer->m_transport = (*iter);
                    (*iter)->AddPassenger(pPlayer);
                    break;
                }
            }
        }
    }
    else if (pPlayer->m_transport)                      // if we were on a transport, leave
    {
        pPlayer->m_transport->RemovePassenger(pPlayer);
        pPlayer->m_transport = NULL;
        movementInfo.ClearTransportData();
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (recv_data.GetOpcode() == MSG_MOVE_FALL_LAND && !pPlayer->IsTaxiFlying())
        pPlayer->HandleFallDamage(movementInfo);

    if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != pPlayer->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        pPlayer->SetInWater(!pPlayer->IsInWater() || pPlayer->GetTerrain()->IsUnderWater(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z));
    }

    if (sWorld.m_ac.activated() && !pPlayer->hasUnitState(UNIT_STAT_LOST_CONTROL | UNIT_STAT_NOT_MOVE) && !pPlayer->isGameMaster() && pPlayer->m_AC_timer == 0 && recv_data.GetOpcode() != MSG_MOVE_SET_FACING)
        sWorld.m_ac.execute(new ACRequest(pPlayer, GetLatency(), pPlayer->m_movementInfo, movementInfo, pPlayer->GetLastSpeedRate()));

    /*----------------------*/
    uint8 uiMoveType = 0;

    if (pPlayer->IsFlying())
       uiMoveType = MOVE_FLIGHT;
    else if (pPlayer->IsUnderWater())
        uiMoveType = MOVE_SWIM;
    else
        uiMoveType = MOVE_RUN;

    pPlayer->SetLastSpeedRate(pPlayer->GetSpeedRate(UnitMoveType(uiMoveType)));

    /* process position-change */
    recv_data.put<uint32>(5, WorldTimer::getMSTime());                  // offset flags(4) + unk(1)
    WorldPacket data(recv_data.GetOpcode(), (pPlayer->GetPackGUID().size()+recv_data.size()));
    data << pPlayer->GetPackGUID();
    data.append(recv_data.contents(), recv_data.size());
    pPlayer->SendMessageToSet(&data, false);

    pPlayer->SetPosition(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o);
    pPlayer->m_movementInfo = movementInfo;
    pPlayer->UpdateFallInformationIfNeed(movementInfo, recv_data.GetOpcode());

    if (pPlayer->isMoving())
        pPlayer->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOVE | AURA_INTERRUPT_FLAG_NOT_SEATED);

    if (pPlayer->isTurning())
        pPlayer->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING | AURA_INTERRUPT_FLAG_NOT_SEATED);

    if (movementInfo.GetPos()->z < -500.0f)
        pPlayer->HandleFallUnderMap();
}

void WorldSession::HandlePossessedMovement(WorldPacket& recv_data, MovementInfo& movementInfo)
{
    // Whatever the client is controlling, it will send the GUID of the original player.
    // If current player is controlling, it must be handled like the controlled player sent these opcodes

    Unit* pos_unit = GetPlayer()->GetCharm();

    if (pos_unit->GetTypeId() == TYPEID_PLAYER && ((Player*)pos_unit)->GetDontMove())
        return;

    //Save movement flags
    pos_unit->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    // Remove possession if possessed unit enters a transport
    if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        GetPlayer()->Uncharm();
        return;
    }

    recv_data.put<uint32>(5, WorldTimer::getMSTime());
    WorldPacket data(recv_data.GetOpcode(), pos_unit->GetPackGUID().size()+recv_data.size());
    data << pos_unit->GetPackGUID();
    data.append(recv_data.contents(), recv_data.size());
    // Send the packet to self but not to the possessed player; for creatures the first bool is irrelevant
    pos_unit->SendMessageToSet(&data, true, false);

    // Possessed is a player
    if (pos_unit->GetTypeId() == TYPEID_PLAYER)
    {
        Player* plr = (Player*)pos_unit;

        if (recv_data.GetOpcode() == MSG_MOVE_FALL_LAND)
            plr->HandleFallDamage(movementInfo);

        if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != plr->IsInWater())
        {
            // Now client not include swimming flag in case jumping under water
            plr->SetInWater(!plr->IsInWater() || plr->GetTerrain()->IsUnderWater(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z));
        }

        plr->SetPosition(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o, false);
        plr->m_movementInfo = movementInfo;

        if (plr->isTurning())
            plr->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

        if (movementInfo.GetPos()->z < -500.0f)
        {
            GetPlayer()->Uncharm();
            plr->HandleFallUnderMap();
        }
    }
    else // Possessed unit is a creature
    {
        Map* map = pos_unit->GetMap();
        map->CreatureRelocation(((Creature*)pos_unit), movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o);
    }
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recv_data)
{
    /* extract packet */
    ObjectGuid guid;
    MovementInfo movementInfo;
    float newspeed;

    recv_data >> guid;
    recv_data >> Unused<uint32>();                          // counter or moveEvent
    recv_data >> movementInfo;
    recv_data >> newspeed;

    // now can skip not our packet
    if (_player->GetGUID() != guid.GetRawValue())
        return;

    // continue parse packet
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack" };

    uint16 opcode = recv_data.GetOpcode();
    switch (opcode)
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        default:
            sLog.outError("WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if (_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            sLog.outError("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeed(move_type,_player->GetSpeedRate(move_type),true);
        }
        else                                                // must be lesser - cheating
        {
            sLog.outBasic("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName(),_player->GetSession()->GetAccountId(),_player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    recv_data >> Unused<uint64>();                          // guid
}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");
    recv_data.hexlike();

    uint64 old_mover_guid;
    MovementInfo mi;

    old_mover_guid = recv_data.readPackGUID();
    recv_data >> mi;

    if (!old_mover_guid)
        return;

    _player->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    //sLog.outDebug("WORLD: Recvd CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recv_data)
{
    sLog.outDebug("CMSG_MOVE_KNOCK_BACK_ACK");
    // Currently not used but maybe use later for recheck final player position
    // (must be at call same as into "recv_data >> x >> y >> z >> orientation;"

    MovementInfo movementInfo;
    uint64 guid;

    recv_data >> guid;                                      // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;

    if (GetPlayer()->GetGUID() != guid)
        return;

    // Save movement flags
    _player->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    // skip not personal message;
    GetPlayer()->m_movementInfo = movementInfo;
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_HOVER_ACK");

    MovementInfo movementInfo;

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_WATER_WALK_ACK");

    MovementInfo movementInfo;                         // unk2

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!_player->isAlive() || _player->isInCombat())
        return;

    uint64 summoner_guid;
    bool agree;
    recv_data >> summoner_guid;
    recv_data >> agree;

    _player->SummonIfPossible(agree);
}
