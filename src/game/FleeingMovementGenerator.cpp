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
#include "FleeingMovementGenerator.h"
#include "DestinationHolderImp.h"
#include "ObjectAccessor.h"
#include "VMapFactory.h"

#define MIN_QUIET_DISTANCE 28.0f
#define MAX_QUIET_DISTANCE 43.0f

template<class T>
void
FleeingMovementGenerator<T>::_setTargetLocation(T &owner)
{
    if( !&owner )
        return;

    if( owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED) )
        return;

//    if(!_setMoveData(owner))
//        return;

    float x, y, z;
    if(!_getPoint(owner, x, y, z))
        return;

    owner.addUnitState(UNIT_STAT_FLEEING);
    Traveller<T> traveller(owner);
    i_destinationHolder.SetDestination(traveller, i_dest_x, i_dest_y, i_dest_z);
}

template<>
bool FleeingMovementGenerator<Creature>::GetDestination(float &x, float &y, float &z) const
{
    if(i_destinationHolder.HasArrived())
        return false;

    i_destinationHolder.GetDestination(x, y, z);
    return true;
}

template<>
bool FleeingMovementGenerator<Player>::GetDestination(float &x, float &y, float &z) const
{
    return false;
}

template<class T>
bool
FleeingMovementGenerator<T>::_getPoint(T &owner, float &x, float &y, float &z)
{
    if(!&owner)
        return false;

    x = owner.GetPositionX();
    y = owner.GetPositionY();
    z = owner.GetPositionZ();

    float temp_x, temp_y, temp_z, angle;
    const Map *_map = owner.GetBaseMap();
    //primitive path-finding
    for(uint8 i = 0; i < 8; ++i)
    {
        float distance = 14.0f;

        switch(i)
        {
            case 0:
                angle = i_cur_angle;
                break;
            case 1:
                angle = i_cur_angle + M_PI/4.0f;
                break;
            case 2:
                angle = i_cur_angle + M_PI/2.0f;
                break;
            case 3:
                angle = i_cur_angle - M_PI/4.0f;
                break;
            case 4:
                angle = i_cur_angle - M_PI/2.0f;
                break;
            case 5:
                angle = i_cur_angle + M_PI*3/4.0f;
                break;
            case 6:
                angle = i_cur_angle - M_PI*3/4.0f;
                break;
            case 7:
                angle = i_cur_angle + M_PI;
                break;
        }

        // destination point
        temp_x = x + distance * cos(angle);
        temp_y = y + distance * sin(angle);
        Trinity::NormalizeMapCoord(temp_x);
        Trinity::NormalizeMapCoord(temp_y);

        float ground, floor;
        ground = _map->GetHeight(temp_x, temp_y, MAX_HEIGHT, true);
        floor = _map->GetHeight(temp_x, temp_y, z, true);

        temp_z = (fabs(ground - z) >= fabs(floor - z)) ? floor : ground;

        if(temp_z <= INVALID_HEIGHT)
            continue;

        // if something on way get Hit Position and update distance
        if(VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(owner.GetMapId(), x, y, z+0.5f, temp_x, temp_y, temp_z+0.5f, temp_x, temp_y, temp_z, -1.0f))
            distance = owner.GetDistance2d(temp_x, temp_y);

        float dest_floor, dest_ground;
        float dest_x = x;
        float dest_y = y;
        float dest_z = z;


        for(int j = 1; j <= 7; ++j)
        {
            float dist = j*2.0f;
            if(dist > distance)
            {
                dist = distance;
                j = 8; // end loop after executing code below
            }

            temp_x = x + dist*cos(angle);
            temp_y = y + dist*sin(angle);
            dest_ground = _map->GetHeight(temp_x, temp_y, MAX_HEIGHT, true);
            dest_floor  = _map->GetHeight(temp_x, temp_y, dest_z, true);
            temp_z = (fabs(dest_ground - dest_z) >= fabs(dest_floor - dest_z)) ? dest_floor : dest_ground;

            if(temp_z < INVALID_HEIGHT || fabs(temp_z - dest_z) > 1.6f)
                break;

            /*
            // height on left from new point
            float tx, ty;
            tx = temp_x + 2.0f*cos(angle+M_PI);
            ty = temp_y + 2.0f*sin(angle+M_PI);
            dest_ground = _map->GetHeight(tx, ty, MAX_HEIGHT, true);
            dest_floor  = _map->GetHeight(tx, ty, dest_z, true);

            float left_z  = (fabs(dest_ground - dest_z) >= fabs(dest_floor - dest_z)) ? dest_floor : dest_ground;
            if(left_z < INVALID_HEIGHT || fabs(left_z - temp_z) > 1.6f)
                break;

            tx = temp_x + 2.0f*cos(angle-M_PI);
            ty = temp_y + 2.0f*sin(angle-M_PI);
            dest_ground = _map->GetHeight(tx, ty, MAX_HEIGHT, true);
            dest_floor  = _map->GetHeight(tx, ty, dest_z, true);

            float right_z  = (fabs(dest_ground - dest_z) >= fabs(dest_floor - dest_z)) ? dest_floor : dest_ground;
            if(right_z < INVALID_HEIGHT || fabs(right_z - temp_z) > 1.6f)
                break;*/

            dest_x = temp_x;
            dest_y = temp_y;
            dest_z = temp_z;
        }

        if(i_dest_x != dest_x || i_dest_y != dest_y)
        {
            i_dest_x = dest_x;
            i_dest_y = dest_y;
            i_dest_z = dest_z;
            return true;
        }
    }
    return false;
}

template<class T>
bool
FleeingMovementGenerator<T>::_setMoveData(T &owner)
{
//    Unit *fright = Unit::GetUnit(owner, i_frightGUID);
//    if(i_last_distance_from_caster < fright->GetDistance(&owner))

//    float cur_dist_xyz = owner.GetDistance(i_caster_x, i_caster_y, i_caster_z);

    /*
    if(i_to_distance_from_caster > 0.0f)
    {
        if((i_last_distance_from_caster > i_to_distance_from_caster && cur_dist_xyz < i_to_distance_from_caster)   ||
                                                            // if we reach lower distance
           (i_last_distance_from_caster > i_to_distance_from_caster && cur_dist_xyz > i_last_distance_from_caster) ||
                                                            // if we can't be close
           (i_last_distance_from_caster < i_to_distance_from_caster && cur_dist_xyz > i_to_distance_from_caster)   ||
                                                            // if we reach bigger distance
           (cur_dist_xyz > MAX_QUIET_DISTANCE) ||           // if we are too far
           (i_last_distance_from_caster > MIN_QUIET_DISTANCE && cur_dist_xyz < MIN_QUIET_DISTANCE) )
                                                            // if we leave 'quiet zone'
        {
            // we are very far or too close, stopping
            i_to_distance_from_caster = 0.0f;
            i_nextCheckTime.Reset( urand(500,1000) );
            return false;
        }
        else
        {
            // now we are running, continue
            i_last_distance_from_caster = cur_dist_xyz;
            return true;
        }
    }

    float cur_dist;
    float angle_to_caster;

    Unit * fright = ObjectAccessor::GetUnit(owner, i_frightGUID);

    if(fright)
    {
        cur_dist = fright->GetDistance(&owner);
        if(cur_dist < cur_dist_xyz)
        {
            i_caster_x = fright->GetPositionX();
            i_caster_y = fright->GetPositionY();
            i_caster_z = fright->GetPositionZ();
            angle_to_caster = fright->GetAngle(&owner);
        }
        else
        {
            cur_dist = cur_dist_xyz;
            angle_to_caster = owner.GetAngle(i_caster_x, i_caster_y) + M_PI;
        }
    }
    else
    {
        cur_dist = cur_dist_xyz;
        angle_to_caster = owner.GetAngle(i_caster_x, i_caster_y) + M_PI;
    }

    // if we too close may use 'path-finding' else just stop
    i_only_forward = cur_dist >= MIN_QUIET_DISTANCE/3;

    //get angle and 'distance from caster' to run
    float angle;

    if(i_cur_angle == 0.0f && i_last_distance_from_caster == 0.0f) //just started, first time
    {
        angle = rand_norm()*(1.0f - cur_dist/MIN_QUIET_DISTANCE) * M_PI/3 + rand_norm()*M_PI*2/3;
        i_to_distance_from_caster = MIN_QUIET_DISTANCE;
        i_only_forward = true;
    }
    else if(cur_dist < MIN_QUIET_DISTANCE)
    {
        angle = M_PI/6 + rand_norm()*M_PI*2/3;
        i_to_distance_from_caster = cur_dist*2/3 + rand_norm()*(MIN_QUIET_DISTANCE - cur_dist*2/3);
    }
    else if(cur_dist > MAX_QUIET_DISTANCE)
    {
        angle = rand_norm()*M_PI/3 + M_PI*2/3;
        i_to_distance_from_caster = MIN_QUIET_DISTANCE + 2.5f + rand_norm()*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE - 2.5f);
    }
    else
    {
        angle = rand_norm()*M_PI;
        i_to_distance_from_caster = MIN_QUIET_DISTANCE + 2.5f + rand_norm()*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE - 2.5f);
    }

    int8 sign = rand_norm() > 0.5f ? 1 : -1;
    i_cur_angle = sign*angle + angle_to_caster;

    // current distance
    i_last_distance_from_caster = cur_dist;
*/
    return true;
}

template<class T>
void
FleeingMovementGenerator<T>::Initialize(T &owner)
{
    if(!&owner)
        return;

    Unit * fright = ObjectAccessor::GetUnit(owner, i_frightGUID);
    if(!fright)
        return;

    _Init(owner);
    owner.CastStop();
    owner.addUnitState(UNIT_STAT_FLEEING);
    owner.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner.SetUInt64Value(UNIT_FIELD_TARGET, 0);
    owner.RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
    i_dest_x = i_caster_x = fright->GetPositionX();
    i_dest_y = i_caster_y = fright->GetPositionY();
    i_dest_z = i_caster_z = fright->GetPositionZ();
    i_only_forward = true;
    i_cur_angle = fright->GetAngle(&owner);
//    i_angle_multi = 1.0f;
//    i_last_distance_from_caster = 0.0f;
//    i_to_distance_from_caster = 0.0f;
    _setTargetLocation(owner);
}

template<>
void
FleeingMovementGenerator<Creature>::_Init(Creature &owner)
{
    if(!&owner)
        return;

    is_water_ok = owner.canSwim();
    is_land_ok  = owner.canWalk();
}

template<>
void
FleeingMovementGenerator<Player>::_Init(Player &)
{
    is_water_ok = true;
    is_land_ok  = true;
}

template<class T>
void
FleeingMovementGenerator<T>::Finalize(T &owner)
{
    owner.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner.clearUnitState(UNIT_STAT_FLEEING);
    if(owner.GetTypeId() == TYPEID_UNIT && owner.getVictim())
        owner.SetUInt64Value(UNIT_FIELD_TARGET, owner.getVictim()->GetGUID());
}

template<class T>
void
FleeingMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
bool
FleeingMovementGenerator<T>::Update(T &owner, const uint32 & time_diff)
{
    if( !&owner || !owner.isAlive() )
        return false;
    if( owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED) )
        return true;

    Traveller<T> traveller(owner);
/*
    if( (owner.IsStopped() && !i_destinationHolder.HasArrived()) || !i_destinationHolder.HasDestination() )
    {
        _setTargetLocation(owner);
        return true;
    }*/

    if (i_destinationHolder.UpdateTraveller(traveller, time_diff))
        if(i_destinationHolder.HasArrived())
            _setTargetLocation(owner);
    return true;
}

template void FleeingMovementGenerator<Player>::Initialize(Player &);
template void FleeingMovementGenerator<Creature>::Initialize(Creature &);
template bool FleeingMovementGenerator<Player>::_setMoveData(Player &);
template bool FleeingMovementGenerator<Creature>::_setMoveData(Creature &);
template bool FleeingMovementGenerator<Player>::_getPoint(Player &, float &, float &, float &);
template bool FleeingMovementGenerator<Creature>::_getPoint(Creature &, float &, float &, float &);
template void FleeingMovementGenerator<Player>::_setTargetLocation(Player &);
template void FleeingMovementGenerator<Creature>::_setTargetLocation(Creature &);
template void FleeingMovementGenerator<Player>::Finalize(Player &);
template void FleeingMovementGenerator<Creature>::Finalize(Creature &);
template void FleeingMovementGenerator<Player>::Reset(Player &);
template void FleeingMovementGenerator<Creature>::Reset(Creature &);
template bool FleeingMovementGenerator<Player>::Update(Player &, const uint32 &);
template bool FleeingMovementGenerator<Creature>::Update(Creature &, const uint32 &);

