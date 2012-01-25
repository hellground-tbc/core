#ifndef _ANTICHEAT_H
#define _ANTICHEAT_H

#include <ace/Method_Request.h>

#include "Common.h"
#include "Player.h"
#include "Language.h"
#include "ObjectMgr.h"

class Player;

#define AC_MAX_LATENCY  500

class ACRequest : public ACE_Method_Request
{
    public:
        ACRequest(Player *player, uint32 latency, MovementInfo pOldPacket, MovementInfo pNewPacket, float fLastSpeedRate)
            : m_ownerGUID(player->GetGUID()), m_latency(latency), m_oldPacket(pOldPacket), m_newPacket(pNewPacket), m_lastSpeedRate(fLastSpeedRate)
        {
            UnitMoveType m_type = player->IsFlying() ? MOVE_FLIGHT : player->IsUnderWater() ? MOVE_SWIM : MOVE_RUN;

            m_speedRate = player->GetSpeedRate(m_type);
            m_speed = player->GetSpeed(m_type);

            player->GetPosition(m_pos.x, m_pos.y, m_pos.z);
        }

        virtual int call()
        {
            Player *pPlayer = objmgr.GetPlayer(m_ownerGUID);
            if (!pPlayer)
                return -1;

            // teleport to plane cheat
            if (m_newPacket.pos.z == 0.0f)
            {
                float tmpZ = pPlayer->GetTerrain()->GetHeight(m_newPacket.pos.x, m_newPacket.pos.y, MAX_HEIGHT, false);
                if ((tmpZ > 2.0f || tmpZ < -2.0f) && tmpZ > -100000.0f)
                {
                    sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - teleport to plane cheat. MapId: %u, MapHeight: %f, coords: %f, %f, %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                           pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), tmpZ, m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
                    pPlayer->Relocate(m_pos.x, m_pos.y, tmpZ, m_pos.o);
                    pPlayer->GetSession()->KickPlayer();
                    return 0;
                }
            }

            // is on taxi
            if (pPlayer->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_TAXI_FLIGHT) || !pPlayer->m_taxi.empty())
                return -1;

            // speed rate differs from prev packet
            if (m_speedRate != m_lastSpeedRate)
                return -1;

            if (m_newPacket.GetMovementFlags() != m_oldPacket.GetMovementFlags())
                return -1;

            // we don't love that movement flags
            if (m_newPacket.GetMovementFlags() & (MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR | MOVEFLAG_ONTRANSPORT))
                return -1;

            // charging
            if (pPlayer->hasUnitState(UNIT_STAT_CHARGING))
                return -1;

            uint32 latency = pPlayer->GetSession()->GetLatency();

            // fly cheat
            if (!m_newPacket.HasMovementFlag(MOVEFLAG_SWIMMING) && m_newPacket.GetMovementFlags() & (MOVEFLAG_CAN_FLY | SPLINEFLAG_FLYINGING | MOVEFLAG_FLYING) &&
                !(pPlayer->HasAuraType(SPELL_AURA_FLY) || pPlayer->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) || pPlayer->HasAuraType(SPELL_AURA_MOD_SPEED_FLIGHT)))
            {
                if (latency == 0 || latency > AC_MAX_LATENCY)
                {
                    sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible Fly Cheat (LATENCY KICKED). MapId: %u, coords: x: %f, y: %f, z: %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                              pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

                    pPlayer->GetSession()->KickPlayer();
                    return 0;
                }

                sWorld.SendGMText(LANG_ANTICHEAT_FLY, pPlayer->GetName(), pPlayer->GetName());
                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible Fly Cheat. MapId: %u, coords: x: %f, y: %f, z: %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                              pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
            }

            // water walk cheat
            if ((m_newPacket.HasMovementFlag(MOVEFLAG_WATERWALKING))
                && !pPlayer->HasAuraType(SPELL_AURA_WATER_WALK) && pPlayer->isAlive() && !pPlayer->isGameMaster())
            {
                if (latency == 0 || latency > AC_MAX_LATENCY)
                {
                    sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible water walk Cheat (LATENCY KICKED). MapId: %u, coords: %f %f %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                                      pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
                    pPlayer->GetSession()->KickPlayer();
                    return 0;
                }

                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible water walk Cheat. MapId: %u, coords: %f %f %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                                      pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

                //m_newPacket.RemoveMovementFlag(MOVEFLAG_WATERWALKING);
                sWorld.SendGMText(LANG_ANTICHEAT_WATERWALK, pPlayer->GetName(), pPlayer->GetName());
            }

/*  NOT USED
            // hover cheat
            if ((newPacket.HasMovementFlag(MOVEFLAG_HOVER))
                && !pPlayer->HasAuraType(SPELL_AURA_HOVER) && !pPlayer->isGameMaster())
            {
                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible Hover Cheat. MapId: %u, coords: x: %f, y: %f, z: %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                                      pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

                //m_newPacket.RemoveMovementFlag(MOVEFLAG_HOVER);
                sWorld.SendGMText(LANG_ANTICHEAT_FLY, pPlayer->GetName());
            }

            // safe fall cheat
            if ((newPacket.HasMovementFlag(MOVEFLAG_SAFE_FALL))
                && !pPlayer->HasAuraType(SPELL_AURA_SAFE_FALL) && !pPlayer->HasAuraType(SPELL_AURA_FEATHER_FALL) && !pPlayer->isGameMaster())
            {
                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) - possible safe fall Cheat. MapId: %u, coords: %f %f %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                                      pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

                //m_newPacket.RemoveMovementFlag(MOVEFLAG_SAFE_FALL);
                sWorld.SendGMText(LANG_ANTICHEAT_FLY, pPlayer->GetName());
            }
*/

            //// who is cheating on arena ?
            //if (pPlayer->GetMap() && pPlayer->GetMap()->IsBattleGroundOrArena())
            //    return -1;

            float dx = m_newPacket.pos.x - m_pos.x;
            float dy = m_newPacket.pos.y - m_pos.y;
            float fDistance2d = sqrt(dx*dx + dy*dy);

            // time between packets
            uint32 uiDiffTime = WorldTimer::getMSTimeDiff(m_oldPacket.time, m_newPacket.time);

            float fClientRate = (fDistance2d * 1000 / uiDiffTime) / m_speed;
            float fServerRate = m_speed * uiDiffTime / 1000 +1.0f;

            if (fDistance2d > 0.5f && fClientRate > fServerRate)
            {
                if (m_speed +0.2 > m_speed*fClientRate)
                    return -1;

                if (latency == 0 || latency > AC_MAX_LATENCY)
                {
                    sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) (LATENCY KICKED) moved for distance %f with server speed : %f (client speed: %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                              pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), fDistance2d, m_speed, m_speed*fClientRate, pPlayer->GetMapId(), m_pos.x, m_pos.y, m_pos.z, m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
                    //pPlayer->GetSession()->KickPlayer();
                    return 0;
                }

                pPlayer->m_AC_count++;
                pPlayer->m_AC_timer = IN_MILISECONDS;   // 1 sek

                //if (!(pPlayer->m_AC_count %5))
                    sWorld.SendGMText(LANG_ANTICHEAT, pPlayer->GetName(), pPlayer->GetName(), pPlayer->m_AC_count, m_speed, m_speed*fClientRate);

                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) moved for distance %f with server speed : %f (client speed: %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
                              pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), fDistance2d, m_speed, m_speed*fClientRate, pPlayer->GetMapId(), m_pos.x, m_pos.y, m_pos.z, m_newPacket.pos.x, m_newPacket.pos.y, m_newPacket.pos.z, m_newPacket.GetMovementFlags(), m_latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
            }
            return 0;
        }

    private:
        uint64 m_ownerGUID;

        MovementInfo m_oldPacket;
        MovementInfo m_newPacket;

        Position m_pos;

        uint32 m_latency;

        float m_speed;
        float m_speedRate;
        float m_lastSpeedRate;
};

#endif
