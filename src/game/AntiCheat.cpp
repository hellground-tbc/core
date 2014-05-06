/*
 * Copyright (C) 2008-2014 HellGround <http://www.hellground.pl/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "AntiCheat.h"

#include "ObjectMgr.h"
#include "Language.h"

int ACRequest::call()
{
    Player *pPlayer = sObjectMgr.GetPlayer(m_ownerGUID);
    if (!pPlayer)
        return -1;

    // is on taxi
    if (pPlayer->IsTaxiFlying() || pPlayer->GetTransport())
        return -1;

    // charging
    if (pPlayer->hasUnitState(UNIT_STAT_CHARGING))
        return -1;

    uint32 latency = pPlayer->GetSession()->GetLatency();

    if (DetectFlyHack(pPlayer))
    {
        sLog.outLog(LOG_CHEAT, "Player %s (GUID: %u / ACCOUNT_ID: %u) - possible Fly Cheat. MapId: %u, coords: x: %f, y: %f, z: %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
            pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), GetNewMovementInfo().pos.x, GetNewMovementInfo().pos.y, GetNewMovementInfo().pos.z, GetNewMovementInfo().GetMovementFlags(), latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
        
        pPlayer->CumulativeACReport(ANTICHEAT_CHECK_FLYHACK);
        pPlayer->SetFlying(false);
        return -1;
    }
    
    if (DetectSpeedHack(pPlayer))
        return -1;
        
    if (DetectWaterWalkHack(pPlayer))
    {
        sLog.outLog(LOG_CHEAT, "Player %s (GUID: %u / ACCOUNT_ID: %u) - possible water walk Cheat. MapId: %u, coords: %f %f %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s",
            pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), GetNewMovementInfo().pos.x, GetNewMovementInfo().pos.y, GetNewMovementInfo().pos.z, GetNewMovementInfo().GetMovementFlags(), latency, pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

        pPlayer->CumulativeACReport(ANTICHEAT_CHECK_WATERWALKHACK);
        pPlayer->SetMovement(MOVE_LAND_WALK); 
        return -1;
    }

    if (DetectTeleportToPlane(pPlayer))
        return -1;

    return 0;
}

bool ACRequest::DetectTeleportToPlane(Player *pPlayer)
{
    // teleport to plane cheat
    if (GetNewMovementInfo().pos.z == 0.0f)
    {
        float ground_Z = pPlayer->GetTerrain()->GetHeight(GetNewMovementInfo().pos.x, GetNewMovementInfo().pos.y, GetNewMovementInfo().pos.z);
        float z_diff = fabs(ground_Z - pPlayer->GetPositionZ());

        // we are not really walking there
        if (z_diff > 1.0f)
        {
            sLog.outLog(LOG_CHEAT, "Player %s (GUID: %u / ACCOUNT_ID: %u) - teleport to plane cheat. MapId: %u, MapHeight: %f, coords: %f, %f, %f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s", pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetMapId(), ground_Z, GetNewMovementInfo().pos.x, GetNewMovementInfo().pos.y, GetNewMovementInfo().pos.z, GetNewMovementInfo().GetMovementFlags(), pPlayer->GetSession()->GetLatency() , pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");

            pPlayer->Relocate(GetLastMovementInfo().pos.x, GetLastMovementInfo().pos.y, ground_Z, GetLastMovementInfo().pos.o);
            pPlayer->GetSession()->KickPlayer();
            return true;
        }
    }
    return false;
}

bool ACRequest::DetectFlyHack(Player *pPlayer)
{
    // forced fly by calling ->SetFlying
    if (pPlayer->HasByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02))
        return false;

    if (!GetLastMovementInfo().HasMovementFlag(MOVEFLAG_FLYING))
        return false;

    if (!GetNewMovementInfo().HasMovementFlag(MOVEFLAG_FLYING))
        return false;

    if (pPlayer->HasAuraType(SPELL_AURA_FLY) ||
        pPlayer->HasAuraType(SPELL_AURA_MOD_SPEED_FLIGHT) ||
        pPlayer->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) ||
        pPlayer->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_ALWAYS) ||
        pPlayer->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK))
        return false;

    return true;
}

bool ACRequest::DetectWaterWalkHack(Player *pPlayer)
{
    if (!GetLastMovementInfo().HasMovementFlag(MOVEFLAG_WATERWALKING))
        return false;

    // if we are a ghost we can walk on water
    if (!pPlayer->isAlive())
        return false;

    if (pPlayer->HasAuraType(SPELL_AURA_FEATHER_FALL) ||
        pPlayer->HasAuraType(SPELL_AURA_SAFE_FALL) ||
        pPlayer->HasAuraType(SPELL_AURA_WATER_WALK))
        return false;

    return true;
}

bool ACRequest::DetectSpeedHack(Player *pPlayer)
{
    uint8 moveType = 0;
    if (pPlayer->HasUnitMovementFlag(MOVEFLAG_SWIMMING))
        moveType = MOVE_SWIM;
    else if (pPlayer->IsFlying())
        moveType = MOVE_FLIGHT;
    else if (pPlayer->HasUnitMovementFlag(MOVEFLAG_WALK_MODE))
        moveType = MOVE_WALK;
    else
        moveType = MOVE_RUN;

    Position n = GetNewMovementInfo().pos;
    Position o = GetLastMovementInfo().pos;

    n.x = n.x - o.x;
    n.y = n.y - o.y;

    float exact2dDist = sqrt(n.x*n.x + n.y*n.y);

    // how many yards the player should do in one sec. (server-side speed)
    float speedRate = pPlayer->GetSpeed(UnitMoveType(moveType)) + GetNewMovementInfo().j_xyspeed;

    // how long the player took to move to here.
    uint32 timeDiff = WorldTimer::getMSTimeDiff(GetLastMovementInfo().time, GetNewMovementInfo().time);
    if (!timeDiff)
        timeDiff = 1;

    //client-side speed, traveled distance div by movement time.
    float clientSpeedRate = exact2dDist * 1000 / timeDiff;

    if (clientSpeedRate <= speedRate * sWorld.getConfig(CONFIG_ANTICHEAT_SPEEDHACK_TOLERANCE))
        return false;

    pPlayer->m_AC_timer = IN_MILISECONDS;   // 1 sek

    sWorld.SendGMText(LANG_ANTICHEAT_SPEEDHACK, pPlayer->GetName(), pPlayer->GetName(), 0, speedRate, clientSpeedRate);
    sLog.outLog(LOG_CHEAT, "Player %s (GUID: %u / ACCOUNT_ID: %u) moved for distance %f with server speed : %f (client speed: %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. MOVEMENTFLAGS: %u LATENCY: %u. BG/Arena: %s\n",
        pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetSession()->GetAccountId(), exact2dDist, speedRate,
        clientSpeedRate, pPlayer->GetMapId(), GetLastMovementInfo().pos.x, GetLastMovementInfo().pos.y, GetLastMovementInfo().pos.z,
        GetNewMovementInfo().pos.x, GetNewMovementInfo().pos.y, GetNewMovementInfo().pos.z,
        GetNewMovementInfo().GetMovementFlags(), pPlayer->GetSession()->GetLatency(),
        pPlayer->GetMap() ? (pPlayer->GetMap()->IsBattleGroundOrArena() ? "Yes" : "No") : "No");
    return true;
}
