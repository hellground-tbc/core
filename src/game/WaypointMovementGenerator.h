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

#ifndef HELLGROUND_WAYPOINTMOVEMENTGENERATOR_H
#define HELLGROUND_WAYPOINTMOVEMENTGENERATOR_H

/** @page PathMovementGenerator is used to generate movements
 * of way points and flight paths.  Each serves the purpose
 * of generate activities so that it generates updated
 * packets for the players.
 */

#include "MovementGenerator.h"
#include "WaypointManager.h"
#include "Path.h"

#include "Player.h"

#include <vector>
#include <set>

#define FLIGHT_TRAVEL_UPDATE  100
#define STOP_TIME_FOR_PLAYER  3 * 60 * 1000                         // 3 Minutes
#define TIMEDIFF_NEXT_WP      250

template<class T, class P>
class HELLGROUND_DLL_SPEC PathMovementBase
{
    public:
        PathMovementBase() : i_currentNode(0), i_path(NULL) {}
        virtual ~PathMovementBase() {};

        // template pattern, not defined .. override required
        void LoadPath(T &);
        uint32 GetCurrentNode() const { return i_currentNode; }

    protected:
        P i_path;
        uint32 i_currentNode;
};

template<class T>
class WaypointMovementGenerator;

template<>
class WaypointMovementGenerator<Creature> : public MovementGeneratorMedium< Creature, WaypointMovementGenerator<Creature> >, public PathMovementBase<Creature, WaypointPath const*>
{
    public:
        WaypointMovementGenerator(const Creature&) : i_nextMoveTime(0), path_id(0), m_isArrivalDone(false), repeating(false) {}
        WaypointMovementGenerator(uint32 _path_id = 0, bool _repeating = true) : i_nextMoveTime(0), path_id(_path_id), m_isArrivalDone(false), repeating(_repeating) {}
        ~WaypointMovementGenerator() { i_path = NULL; }
        void Initialize(Creature &);
        void Interrupt(Creature &);
        void Finalize(Creature &);
        void Reset(Creature &);
        bool Update(Creature &, const uint32 &diff);

        void MovementInform(Creature &);

        const char* Name() const { return "<Waypoint>"; }
        MovementGeneratorType GetMovementGeneratorType() const { return WAYPOINT_MOTION_TYPE; }

        // now path movement implmementation
        void LoadPath(Creature &c);

        bool GetResetPosition(Creature&, float& x, float& y, float& z);

    private:
        void StopMovement(uint32 time) { i_nextMoveTime.Reset(time); }
        bool Stopped() { return !i_nextMoveTime.Passed();}

        bool CanMove(int32 diff)
        {
            i_nextMoveTime.Update(diff);
            return i_nextMoveTime.Passed();
        }

        bool OnArrived(Creature&);
        bool StartMove(Creature&);

        void StartMoveNow(Creature& creature)
        {
            i_nextMoveTime.Reset(0);
            StartMove(creature);
        }

        TimeTrackerSmall i_nextMoveTime;
        bool m_isArrivalDone;
        uint32 path_id;
        bool repeating;
};

/** FlightPathMovementGenerator generates movement of the player for the paths
 * and hence generates ground and activities for the player.
 */
class HELLGROUND_DLL_SPEC FlightPathMovementGenerator
: public MovementGeneratorMedium< Player, FlightPathMovementGenerator >,
public PathMovementBase<Player,TaxiPathNodeList const*>
{
    public:
        explicit FlightPathMovementGenerator(TaxiPathNodeList const& pathnodes, uint32 startNode = 0)
        {
            i_path = &pathnodes;
            i_currentNode = startNode;
        }
        virtual void Initialize(Player &u) { _Initialize(u); };
        virtual void Finalize(Player &u)   { _Finalize(u); };
        virtual void Interrupt(Player &u)  { _Interrupt(u); };
        virtual void Reset(Player &u)      { _Reset(u); };

        bool Update(Player &, const uint32 &);

        const char* Name() const { return "<Flight>"; }
        MovementGeneratorType GetMovementGeneratorType() const { return FLIGHT_MOTION_TYPE; }

        TaxiPathNodeList const& GetPath() { return *i_path; }
        uint32 GetPathAtMapEnd() const;
        bool HasArrived() const { return (i_currentNode >= i_path->size()); }
        void SetCurrentNodeAfterTeleport();
        void SkipCurrentNode() { ++i_currentNode; }
        void DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure);
        bool GetResetPosition(Player&, float& x, float& y, float& z);

    protected:
        void _Initialize(Player &);
        void _Finalize(Player &);
        void _Interrupt(Player &);
        void _Reset(Player &);
};

#endif
