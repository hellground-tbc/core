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
#include "Player.h"
#include "MapManager.h"
#include "Opcodes.h"
#include "ConfusedMovementGenerator.h"
#include "VMapFactory.h"

#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

template<class T>
void ConfusedMovementGenerator<T>::Initialize(T &unit)
{
    i_nextMove = 0;

    GenerateMovement(unit);

    unit.CastStop();
    unit.StopMoving();
    unit.addUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
void ConfusedMovementGenerator<T>::Interrupt(T &unit)
{
    // confused state still applied while movegen disabled
    unit.clearUnitState(UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
void ConfusedMovementGenerator<T>::GenerateMovement(T &unit)
{
    for (uint8 idx = 0; idx <= MAX_RANDOM_POINTS; ++idx)
        unit.GetValidPointInAngle(randomPosition[idx], WANDER_DISTANCE, frand(0, 2*M_PI), true);
}

template<class T>
void ConfusedMovementGenerator<T>::Reset(T &unit)
{
    i_nextMove = 1;
    i_nextMoveTime.Reset(0);

    unit.StopMoving();
    unit.addUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template<class T>
bool ConfusedMovementGenerator<T>::Update(T &unit, const uint32 &diff)
{
    // ignore in case other no reaction state
    if (unit.hasUnitState(UNIT_STAT_CAN_NOT_REACT & ~UNIT_STAT_CONFUSED))
        return true;

    if (i_nextMoveTime.Passed())
    {
        // currently moving, update location
        unit.addUnitState(UNIT_STAT_CONFUSED_MOVE);

        if (unit.movespline->Finalized())
        {
            i_nextMove = urand(0, MAX_RANDOM_POINTS);
            i_nextMoveTime.Reset(urand(0, 1500));     // TODO: check the minimum reset time, should be probably higher
        }
    }
    else
    {
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
        {
            unit.addUnitState(UNIT_STAT_CONFUSED_MOVE);
            Movement::MoveSplineInit init(unit);
            init.MoveTo(randomPosition[i_nextMove].x, randomPosition[i_nextMove].y, randomPosition[i_nextMove].z);
            init.SetWalk(true);
            init.Launch();
        }
    }

    return true;
}

template<>
void ConfusedMovementGenerator<Player>::Finalize(Player &unit)
{
    unit.clearUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template<>
void ConfusedMovementGenerator<Creature>::Finalize(Creature &unit)
{
    unit.clearUnitState(UNIT_STAT_CONFUSED|UNIT_STAT_CONFUSED_MOVE);
}

template void ConfusedMovementGenerator<Player>::Initialize(Player &player);
template void ConfusedMovementGenerator<Creature>::Initialize(Creature &creature);
template void ConfusedMovementGenerator<Player>::Interrupt(Player &player);
template void ConfusedMovementGenerator<Creature>::Interrupt(Creature &creature);
template void ConfusedMovementGenerator<Player>::Reset(Player &player);
template void ConfusedMovementGenerator<Creature>::Reset(Creature &creature);
template bool ConfusedMovementGenerator<Player>::Update(Player &player, const uint32 &diff);
template bool ConfusedMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff);
