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
#include "ObjectAccessor.h"
#include "VMapFactory.h"
#include "CreatureAI.h"

#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

#define MIN_QUIET_DISTANCE 28.0f
#define MAX_QUIET_DISTANCE 43.0f

template<class T>
void FleeingMovementGenerator<T>::_setTargetLocation(T &owner)
{
    if (owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED))
        return;

    float x, y, z;
    if (!_getPoint(owner, x, y, z))
        return;

    owner.addUnitState(UNIT_STAT_FLEEING_MOVE);

    Movement::MoveSplineInit init(owner);
    init.MoveTo(x,y,z);
    init.SetWalk(false);
    init.Launch();

    i_nextCheckTime.Reset(urand(500,1000));
}

template<class T>
bool FleeingMovementGenerator<T>::_getPoint(T &owner, float &x, float &y, float &z)
{
    Position temp;

    float angle = 0.0f;
    float distance = 8.0f;

    //primitive path-finding
    for (uint8 i = 0; i < 8; ++i)
    {
        angle = i_cur_angle;

        if (roll_chance_i(33))
        {
            switch (urand(1, 7))
            {
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
        }

        // destination point
        owner.GetValidPointInAngle(temp, distance, angle, true);

        if (i_dest_x != temp.x || i_dest_y != temp.y)
        {
            x = i_dest_x = temp.x;
            y = i_dest_y = temp.y;
            z = i_dest_z = temp.z;
            return true;
        }

        distance /= 2;
    }

    return false;
}

template<class T>
void FleeingMovementGenerator<T>::Initialize(T &owner)
{
    Unit * fright = owner.GetMap()->GetUnit(i_frightGUID);
    if (!fright)
        return;

    owner.CastStop();
    owner.addUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
    owner.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);

    _Init(owner);

    i_dest_x = i_caster_x = fright->GetPositionX();
    i_dest_y = i_caster_y = fright->GetPositionY();
    i_dest_z = i_caster_z = fright->GetPositionZ();

    i_cur_angle = fright->GetAngle(&owner);

    _setTargetLocation(owner);
}

template<>
void FleeingMovementGenerator<Creature>::_Init(Creature &owner)
{
    is_water_ok = owner.CanSwim();
    is_land_ok  = owner.CanWalk();
}

template<>
void FleeingMovementGenerator<Player>::_Init(Player &)
{
    is_water_ok = true;
    is_land_ok  = true;
}

template<>
void FleeingMovementGenerator<Player>::Finalize(Player &owner)
{
    owner.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
}

template<>
void FleeingMovementGenerator<Creature>::Finalize(Creature &owner)
{
    owner.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
    if (uint64 vGUID = owner.getVictimGUID())
        owner.SetSelection(vGUID);
}

template<class T>
void FleeingMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<class T>
bool FleeingMovementGenerator<T>::Update(T &owner, const uint32 &time_diff)
{
    if (!&owner || !owner.isAlive())
        return false;

    if (owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED))
    {
        owner.clearUnitState(UNIT_STAT_FLEEING_MOVE);
        return true;
    }

    i_nextCheckTime.Update(time_diff);
    if (i_nextCheckTime.Passed() && owner.movespline->Finalized())
        _setTargetLocation(owner);

    return true;
}

template void FleeingMovementGenerator<Player>::Initialize(Player &);
template void FleeingMovementGenerator<Creature>::Initialize(Creature &);
template bool FleeingMovementGenerator<Player>::_getPoint(Player &, float &, float &, float &);
template bool FleeingMovementGenerator<Creature>::_getPoint(Creature &, float &, float &, float &);
template void FleeingMovementGenerator<Player>::_setTargetLocation(Player &);
template void FleeingMovementGenerator<Creature>::_setTargetLocation(Creature &);
template void FleeingMovementGenerator<Player>::Reset(Player &);
template void FleeingMovementGenerator<Creature>::Reset(Creature &);
template bool FleeingMovementGenerator<Player>::Update(Player &, const uint32 &);
template bool FleeingMovementGenerator<Creature>::Update(Creature &, const uint32 &);

void TimedFleeingMovementGenerator::Finalize(Unit &owner)
{
    owner.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner.clearUnitState(UNIT_STAT_FLEEING|UNIT_STAT_FLEEING_MOVE);
    if (Unit* victim = owner.getVictim())
    {
        if (owner.isAlive())
        {
            owner.AttackStop();
            ((Creature*)&owner)->AI()->AttackStart(victim);
        }
    }
}

bool TimedFleeingMovementGenerator::Update(Unit & owner, const uint32 & time_diff)
{
    if (!owner.isAlive())
        return false;

    if (owner.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED))
    {
        owner.clearUnitState(UNIT_STAT_FLEEING_MOVE);
        return true;
    }

    i_totalFleeTime.Update(time_diff);
    if (i_totalFleeTime.Passed())
        return false;

    // This calls grant-parent Update method hiden by FleeingMovementGenerator::Update(Creature &, const uint32 &) version
    // This is done instead of casting Unit& to Creature& and call parent method, then we can use Unit directly
    return MovementGeneratorMedium< Creature, FleeingMovementGenerator<Creature> >::Update(owner, time_diff);
}
