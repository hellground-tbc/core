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
#include "VMapFactory.h"

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T &unit)
{
    i_nextMove = 1;

    GenerateMovement(unit);

    unit.CastStop();
    unit.StopMoving();

    unit.RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);

    unit.addUnitState(UNIT_STAT_CONFUSED);
    unit.SetUInt64Value(UNIT_FIELD_TARGET, 0);
    unit.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);

}

template<class T>
void ConfusedMovementGenerator<T>::GenerateMovement(T &unit)
{
    for (uint8 idx = 0; idx <= MAX_RANDOM_POINTS; ++idx)
    {
        unit.GetRandomPoint(owner.GetPositionX(), owner.GetPositionY(), owner.GetPositionZ(), WANDER_DISTANCE, randomPosition[idx].x, randomPosition[idx].y, randomPosition[idx].z);

        // this will be changed soon :]
        if (unit.IsWithinLOS(randomPosition[idx].x, randomPosition[idx].y, randomPosition[idx].z))
            continue;
        else
            unit.GetPosition(randomPosition[idx]);
    }
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T &unit)
{
    i_nextMove = 1;
    i_destinationHolder.ResetUpdate();
    unit.StopMoving();
}

template<class T>
bool ConfusedMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    if (unit.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_DISTRACTED))
        return true;

    Traveller<T> traveller(unit);
    if (i_destinationHolder.UpdateTraveller(traveller, diff))
    {
        if (i_destinationHolder.HasArrived())
        {
            assert(i_nextMove <= MAX_RANDOM_POINTS);
            i_destinationHolder.SetDestination(traveller, randomPosition[i_nextMove].x, randomPosition[i_nextMove].y, randomPosition[i_nextMove].z);
            i_nextMove = urand(0, MAX_RANDOM_POINTS);
        }
    }
    return true;
}

template<class T>
void ConfusedMovementGenerator<T>::Finalize(T &unit)
{
    unit.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit.clearUnitState(UNIT_STAT_CONFUSED);
    if (unit.GetTypeId() == TYPEID_UNIT && unit.getVictim())
        unit.SetUInt64Value(UNIT_FIELD_TARGET, unit.getVictimGUID());
}

template void ConfusedMovementGenerator<Player>::Initialize(Player &player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Finalize(Player &player);
template void ConfusedMovementGenerator<Creature>::Finalize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Reset(Player &player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature &creature);
template bool ConfusedMovementGenerator<Player>::Update(Player &player, const uint32 &diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff);
