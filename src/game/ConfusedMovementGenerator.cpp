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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "Creature.h"
#include "MapManager.h"
#include "Opcodes.h"
#include "ConfusedMovementGenerator.h"
#include "DestinationHolderImp.h"

template<class T>
void
ConfusedMovementGenerator<T>::Initialize(T &unit)
{
    const float wander_distance = 5.0f;
    float x,y,z;
    x = unit.GetPositionX();
    y = unit.GetPositionY();
    z = unit.GetPositionZ();
    uint32 mapid=unit.GetMapId();

    Map const* map = MapManager::Instance().GetBaseMap(mapid);

    i_nextMove = 1;

    bool is_water_ok, is_land_ok;
    _InitSpecific(unit, is_water_ok, is_land_ok);

    for(unsigned int idx = 0; idx < MAX_CONF_WAYPOINTS+1; ++idx)
    {
        const float wanderX = wander_distance*rand_norm();
        const float wanderY = wander_distance*rand_norm();

        i_waypoints[idx][0] = x + wanderX;
        i_waypoints[idx][1] = y + wanderY;

        // prevent invalid coordinates generation
        Trinity::NormalizeMapCoord(i_waypoints[idx][0]);
        Trinity::NormalizeMapCoord(i_waypoints[idx][1]);


        float ground, floor;
        ground = map->GetHeight(i_waypoints[idx][0], i_waypoints[idx][1], MAX_HEIGHT, true);
        floor  = map->GetHeight(i_waypoints[idx][0], i_waypoints[idx][1], z, true);
        i_waypoints[idx][2] = fabs(ground - z) <= fabs(floor - z) ? ground : floor;

        bool is_water = map->IsInWater(i_waypoints[idx][0],i_waypoints[idx][1], z);
        // if in water set higher z coord
        if(is_water)
            i_waypoints[idx][2] = (z >= i_waypoints[idx][2]) ? z : i_waypoints[idx][2];

        // if generated wrong path just ignore
        if( is_water && !is_water_ok || !is_water && !is_land_ok ||
            !Trinity::IsValidMapCoord(i_waypoints[idx][0], i_waypoints[idx][1]) ||
            fabs(i_waypoints[idx][2] - z) > 2.0f)
        {
            i_waypoints[idx][0] = idx > 0 ? i_waypoints[idx-1][0] : x;
            i_waypoints[idx][1] = idx > 0 ? i_waypoints[idx-1][1] : y;
            i_waypoints[idx][2] = idx > 0 ? i_waypoints[idx-1][2] : z;
        }
        unit.UpdateGroundPositionZ(i_waypoints[idx][0], i_waypoints[idx][1], i_waypoints[idx][2]);
    }

    unit.CastStop();
    unit.StopMoving();
    unit.RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
    unit.addUnitState(UNIT_STAT_CONFUSED); //.cast back 12824 triggered
    unit.SetUInt64Value(UNIT_FIELD_TARGET, 0);
    unit.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);

}

template<>
void
ConfusedMovementGenerator<Creature>::_InitSpecific(Creature &creature, bool &is_water_ok, bool &is_land_ok)
{
    is_water_ok = creature.canSwim();
    is_land_ok  = creature.canWalk();
}

template<>
void
ConfusedMovementGenerator<Player>::_InitSpecific(Player &, bool &is_water_ok, bool &is_land_ok)
{
    is_water_ok = true;
    is_land_ok  = true;
}

template<class T>
void
ConfusedMovementGenerator<T>::Reset(T &unit)
{
    i_nextMove = 1;
    i_destinationHolder.ResetUpdate();
    unit.StopMoving();
}

template<class T>
bool
ConfusedMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    if(!&unit)
        return true;

    if(unit.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_DISTRACTED))
        return true;

    Traveller<T> traveller(unit);

    if(i_destinationHolder.UpdateTraveller(traveller, diff))
        if( i_destinationHolder.HasArrived())
        {
            assert( i_nextMove <= MAX_CONF_WAYPOINTS );
            const float x = i_waypoints[i_nextMove][0];
            const float y = i_waypoints[i_nextMove][1];
            const float z = i_waypoints[i_nextMove][2];
            i_destinationHolder.SetDestination(traveller, x, y, z);
            i_nextMove = urand(1,MAX_CONF_WAYPOINTS);
        }
    return true;
}

template<class T>
void
ConfusedMovementGenerator<T>::Finalize(T &unit)
{
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.clearUnitState(UNIT_STAT_CONFUSED);
    if(unit.GetTypeId() == TYPEID_UNIT && unit.getVictim())
        unit.SetUInt64Value(UNIT_FIELD_TARGET, unit.getVictim()->GetGUID());
}

template void ConfusedMovementGenerator<Player>::Initialize(Player &player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Finalize(Player &player);
template void ConfusedMovementGenerator<Creature>::Finalize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Reset(Player &player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature &creature);
template bool ConfusedMovementGenerator<Player>::Update(Player &player, const uint32 &diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff);

