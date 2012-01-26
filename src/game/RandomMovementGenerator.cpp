/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
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

#include "Creature.h"
#include "MapManager.h"
#include "RandomMovementGenerator.h"
#include "Map.h"
#include "Util.h"

#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

template<>
void RandomMovementGenerator<Creature>::_setRandomLocation(Creature &creature)
{
    Position dest;
    creature.GetRespawnCoord(dest.x, dest.y, dest.z, &dest.o, &wander_distance);

    bool is_air_ok = creature.CanFly();

    const float angle = frand(0.0f, M_PI*2.0f);
    const float range = frand(0.0f, wander_distance);

    creature.GetValidPointInAngle(dest, range, angle, false);

    i_nextMoveTime.Reset(urand(500, 10000));

    creature.addUnitState(UNIT_STAT_ROAMING_MOVE);

    Movement::MoveSplineInit init(creature);
    init.MoveTo(dest.x, dest.y, dest.z);
    init.SetWalk(true);
    init.Launch();
}

template<>
void RandomMovementGenerator<Creature>::Initialize(Creature &creature)
{
    if (!creature.isAlive())
        return;

    creature.addUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);

    _setRandomLocation(creature);
}

template<>
void RandomMovementGenerator<Creature>::Reset(Creature &creature)
{
    Initialize(creature);
}

template<>
void RandomMovementGenerator<Creature>::Finalize(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
    creature.SetWalk(false);
}

template<>
bool RandomMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff)
{
    if (creature.hasUnitState(UNIT_STAT_CASTING | UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_DISTRACTED | UNIT_STAT_DIED))
    {
        i_nextMoveTime.Reset(0);  // Expire the timer
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);
        return true;
    }

    i_nextMoveTime.Update(diff);

    if (creature.movespline->Finalized())
    {
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
            _setRandomLocation(creature);
    }
    return true;
}

template<>
bool RandomMovementGenerator<Creature>::GetResetPosition(Creature &creature, float& x, float& y, float& z)
{
    float radius;
    creature.GetRespawnCoord(x, y, z, 0, &radius);

    // use current if in range
    if (creature.IsInRange2d(x,y,0.0f,radius))
        creature.GetPosition(x,y,z);

    return true;
}
