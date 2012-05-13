/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ByteBuffer.h"
#include "TargetedMovementGenerator.h"
#include "Errors.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "World.h"

#include "Spell.h"

#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

#include <cmath>

template<class T, typename D>
void TargetedMovementGeneratorMedium<T,D>::_setTargetLocation(T &owner)
{
    if (!_target.isValid() || !_target->IsInWorld())
        return;

    if (owner.hasUnitState(UNIT_STAT_NOT_MOVE | UNIT_STAT_CASTING_NOT_MOVE))
        return;

    float x, y, z;

    if (_offset && _target->IsWithinDistInMap(&owner,2*_offset))
    {
        if (!owner.IsStopped())
            return;

        owner.GetPosition(x, y, z);
    }
    else if (!_offset)
    {
        if (_target->IsWithinMeleeRange(&owner))
            return;

        // to nearest random contact position
        _target->GetRandomContactPoint(&owner, x, y, z, 0, MELEE_RANGE - 0.5f);
    }
    else
    {
        if (_target->IsWithinDistInMap(&owner, _offset + 1.0f))
            return;

        // to at _offset distance from target and _angle from target facing
        _target->GetClosePoint(x, y, z, owner.GetObjectSize(), _offset, _angle);
    }

    if (!_path)
        _path = new PathFinder(&owner);

    // allow pets following their master to cheat while generating paths
    bool forceDest = (owner.GetTypeId() == TYPEID_UNIT && ((Creature*)&owner)->isPet() && owner.hasUnitState(UNIT_STAT_FOLLOW));
    _path->calculate(x, y, z, forceDest);
    if (_path->getPathType() & PATHFIND_NOPATH)
        return;

    _targetReached = false;
    _recalculateTravel = false;

    Movement::MoveSplineInit init(owner);
    init.MovebyPath(_path->getPath());
    init.SetWalk(((D*)this)->EnableWalking(owner));
    init.Launch();
}

template<>
void TargetedMovementGeneratorMedium<Player,ChaseMovementGenerator<Player> >::UpdateFinalDistance(float /*fDistance*/)
{
    // nothing to do for Player
}

template<>
void TargetedMovementGeneratorMedium<Player,FollowMovementGenerator<Player> >::UpdateFinalDistance(float /*fDistance*/)
{
    // nothing to do for Player
}

template<>
void TargetedMovementGeneratorMedium<Creature,ChaseMovementGenerator<Creature> >::UpdateFinalDistance(float fDistance)
{
    _offset = fDistance;
    _recalculateTravel = true;
}

template<>
void TargetedMovementGeneratorMedium<Creature,FollowMovementGenerator<Creature> >::UpdateFinalDistance(float fDistance)
{
    _offset = fDistance;
    _recalculateTravel = true;
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T,D>::Update(T &owner, const uint32 & time_diff)
{
    if (!_target.isValid() || !_target->IsInWorld())
        return false;

    if (!owner.isAlive())
        return true;

    if (owner.hasUnitState(UNIT_STAT_NOT_MOVE | UNIT_STAT_CASTING_NOT_MOVE))
    {
        owner.StopMoving();
        return true;
    }

    //if (!_target->isAlive())
    //    return false;

    // prevent crash after creature killed pet
    if (static_cast<D*>(this)->_lostTarget(owner))
        return true;

    _recheckDistance.Update(time_diff);
    if (_recheckDistance.Passed())
    {
        _recheckDistance.Reset(100);

        float allowed_dist = owner.GetObjectBoundingRadius() + sWorld.getConfig(CONFIG_RATE_TARGET_POS_RECALCULATION_RANGE);
        G3D::Vector3 dest = owner.movespline->FinalDestination();

        bool targetMoved = false;
        targetMoved = !_target->IsWithinDist3d(dest.x, dest.y, dest.z, allowed_dist);

        if (targetMoved)
            _setTargetLocation(owner);
    }

    if (owner.IsStopped())
    {
        if (_angle == 0.f && !owner.HasInArc(0.01f, _target.getTarget()))
            owner.SetInFront(_target.getTarget());

        if (!_targetReached)
        {
            _targetReached = true;
            static_cast<D*>(this)->_reachTarget(owner);
        }
    }
    else
    {
        if (_recalculateTravel)
            _setTargetLocation(owner);
    }
    return true;
}

//-----------------------------------------------//
template<class T>
void ChaseMovementGenerator<T>::_reachTarget(T &owner)
{
    if (owner.IsWithinMeleeRange(this->_target.getTarget()))
        owner.Attack(this->_target.getTarget(),true);
}

template<>
void ChaseMovementGenerator<Player>::Initialize(Player &owner)
{
    owner.StopMoving();
    owner.addUnitState(UNIT_STAT_CHASE);
    _setTargetLocation(owner);
}

template<>
void ChaseMovementGenerator<Creature>::Initialize(Creature &owner)
{
    owner.StopMoving();
    owner.SetWalk(false);
    owner.addUnitState(UNIT_STAT_CHASE);
    _setTargetLocation(owner);
}

template<class T>
void ChaseMovementGenerator<T>::Finalize(T &owner)
{
    Interrupt(owner);
}

template<class T>
void ChaseMovementGenerator<T>::Interrupt(T &owner)
{
    owner.StopMoving();
    owner.clearUnitState(UNIT_STAT_CHASE);
}

template<class T>
void ChaseMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

template<>
bool ChaseMovementGenerator<Creature>::EnableWalking(Creature &creature) const
{
    return creature.GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_ALWAYS_WALK;
}

template<>
bool ChaseMovementGenerator<Player>::EnableWalking(Player &) const
{
    return false;
}

//-----------------------------------------------//
template<>
bool FollowMovementGenerator<Creature>::EnableWalking(Creature &creature) const
{
    return _target.isValid() && _target->IsWalking();
}

template<>
bool FollowMovementGenerator<Player>::EnableWalking(Player &) const
{
    return false;
}

template<>
void FollowMovementGenerator<Player>::_updateSpeed(Player &/*u*/)
{
    // nothing to do for Player
}

template<>
void FollowMovementGenerator<Creature>::_updateSpeed(Creature &u)
{
    // pet only sync speed with owner
    if (!((Creature&)u).isPet() || !_target.isValid() || _target->GetGUID() != u.GetOwnerGUID())
        return;

    u.UpdateSpeed(MOVE_RUN,true);
    u.UpdateSpeed(MOVE_WALK,true);
    u.UpdateSpeed(MOVE_SWIM,true);
}

template<>
void FollowMovementGenerator<Player>::Initialize(Player &owner)
{
    owner.StopMoving();

    owner.addUnitState(UNIT_STAT_FOLLOW);
    _updateSpeed(owner);
    _setTargetLocation(owner);
}

template<>
void FollowMovementGenerator<Creature>::Initialize(Creature &owner)
{
    owner.StopMoving();

    owner.addUnitState(UNIT_STAT_FOLLOW);
    _updateSpeed(owner);
    _setTargetLocation(owner);
}

template<class T>
void FollowMovementGenerator<T>::Finalize(T &owner)
{
    Interrupt(owner);
}

template<class T>
void FollowMovementGenerator<T>::Interrupt(T &owner)
{
    owner.StopMoving();

    owner.clearUnitState(UNIT_STAT_FOLLOW);
    _updateSpeed(owner);
}

template<class T>
void FollowMovementGenerator<T>::Reset(T &owner)
{
    Initialize(owner);
}

//-----------------------------------------------//
template void TargetedMovementGeneratorMedium<Player,ChaseMovementGenerator<Player> >::_setTargetLocation(Player &);
template void TargetedMovementGeneratorMedium<Player,FollowMovementGenerator<Player> >::_setTargetLocation(Player &);
template void TargetedMovementGeneratorMedium<Creature,ChaseMovementGenerator<Creature> >::_setTargetLocation(Creature &);
template void TargetedMovementGeneratorMedium<Creature,FollowMovementGenerator<Creature> >::_setTargetLocation(Creature &);
template bool TargetedMovementGeneratorMedium<Player,ChaseMovementGenerator<Player> >::Update(Player &, const uint32 &);
template bool TargetedMovementGeneratorMedium<Player,FollowMovementGenerator<Player> >::Update(Player &, const uint32 &);
template bool TargetedMovementGeneratorMedium<Creature,ChaseMovementGenerator<Creature> >::Update(Creature &, const uint32 &);
template bool TargetedMovementGeneratorMedium<Creature,FollowMovementGenerator<Creature> >::Update(Creature &, const uint32 &);

template void ChaseMovementGenerator<Player>::_reachTarget(Player &);
template void ChaseMovementGenerator<Creature>::_reachTarget(Creature &);
template void ChaseMovementGenerator<Player>::Finalize(Player &);
template void ChaseMovementGenerator<Creature>::Finalize(Creature &);
template void ChaseMovementGenerator<Player>::Interrupt(Player &);
template void ChaseMovementGenerator<Creature>::Interrupt(Creature &);
template void ChaseMovementGenerator<Player>::Reset(Player &);
template void ChaseMovementGenerator<Creature>::Reset(Creature &);

template void FollowMovementGenerator<Player>::Finalize(Player &);
template void FollowMovementGenerator<Creature>::Finalize(Creature &);
template void FollowMovementGenerator<Player>::Interrupt(Player &);
template void FollowMovementGenerator<Creature>::Interrupt(Creature &);
template void FollowMovementGenerator<Player>::Reset(Player &);
template void FollowMovementGenerator<Creature>::Reset(Creature &);
