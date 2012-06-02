/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
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
//Basic headers
#include "WaypointMovementGenerator.h"
//Extended headers
#include "ObjectMgr.h"
#include "ScriptMgr.h"
//Creature-specific headers
#include "Creature.h"
#include "CreatureAI.h"
//Player-specific
#include "Player.h"

#include "movement/MoveSplineInit.h"
#include "movement/MoveSpline.h"

void WaypointMovementGenerator<Creature>::LoadPath(Creature &creature)
{
    if (!path_id)
        path_id = creature.GetWaypointPath();

    i_path = WaypointMgr.GetPath(path_id);
    if (!i_path)
        return;

    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::Initialize(Creature &creature)
{
    LoadPath(creature);
    creature.addUnitState(UNIT_STAT_ROAMING);

    if (creature.GetMap()->WaypointMovementAutoActive())
        creature.setActive(true, ACTIVE_BY_WAYPOINT_MOVEMENT);
}

void WaypointMovementGenerator<Creature>::Finalize(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING);
    creature.SetWalk(false);
    creature.setActive(false, ACTIVE_BY_WAYPOINT_MOVEMENT);
}

void WaypointMovementGenerator<Creature>::Reset(Creature &creature)
{
    creature.addUnitState(UNIT_STAT_ROAMING);
    StartMoveNow(creature);
}

void WaypointMovementGenerator<Creature>::Interrupt(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING);
    creature.SetWalk(false);
    creature.setActive(false, ACTIVE_BY_WAYPOINT_MOVEMENT);
}

bool WaypointMovementGenerator<Creature>::OnArrived(Creature& creature)
{
    if (!i_path || i_path->empty())
        return false;

    if (m_isArrivalDone)
        return true;

    m_isArrivalDone = true;

    const WaypointData *node = i_path->at(i_currentNode);

    if (node->event_id && urand(0, 99) < node->event_chance)
        creature.GetMap()->ScriptsStart(sWaypointScripts, node->event_id, &creature, NULL/*, false*/);

    // Inform script
    MovementInform(creature);
    //Stop(node->delay);    this causes creature to stop and never move again if delay > 0

    if ((i_currentNode == i_path->size() - 1) && !repeating) // If that's our last waypoint
    {
        creature.SetHomePosition(node->x, node->y, node->z, creature.GetOrientation());
        creature.GetMotionMaster()->Initialize();
        return false;
    }
    i_currentNode = (i_currentNode+1) % i_path->size();
    return true;
}

bool WaypointMovementGenerator<Creature>::StartMove(Creature &creature)
{
    if (!i_path || i_path->empty() || i_currentNode == i_path->size())
        return false;

    if (Stopped())
        return true;

    const WaypointData *node = i_path->at(i_currentNode);

    if (creature.IsFormationLeader() && !creature.GetFormation()->AllUnitsReachedWaypoint())
    {
        StopMovement(1);
        return true;
    }

    m_isArrivalDone = false;


    Movement::MoveSplineInit init(creature);
    init.MoveTo(node->x, node->y, node->z);

    if (node->moveType == M_FLY)
        init.SetFly();
    else
        init.SetWalk(node->moveType != M_RUN);

    init.Launch();

    //Call for creature group update
    if (creature.IsFormationLeader())
        creature.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);

    return true;
}

bool WaypointMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff)
{
    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (creature.hasUnitState(UNIT_STAT_NOT_MOVE))
        return true;

    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty())
        return false;

    if (Stopped() && CanMove(diff))
        return StartMove(creature);

    /*if (creature.IsStopped())
        StopMovement(STOP_TIME_FOR_PLAYER);
    else */if (creature.movespline->Finalized())
    {
        if(!OnArrived(creature))
            return false;
        return StartMove(creature);
    }

    return true;
}

void WaypointMovementGenerator<Creature>::MovementInform(Creature &creature)
{
     if (creature.IsAIEnabled)
         creature.AI()->MovementInform(WAYPOINT_MOTION_TYPE, i_currentNode);
}

bool WaypointMovementGenerator<Creature>::GetResetPosition(Creature&, float& x, float& y, float& z)
{
    // prevent a crash at empty waypoint path.
    if (!i_path || i_path->empty())
        return false;

    const WaypointData* node = i_path->at(i_currentNode);
    x = node->x; y = node->y; z = node->z;
    return true;
}

//----------------------------------------------------//
uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (i_currentNode >= i_path->size())
        return i_path->size();

    uint32 curMapId = (*i_path)[i_currentNode].mapid;

    for(uint32 i = i_currentNode; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != curMapId)
            return i;
    }

    return i_path->size();
}

void FlightPathMovementGenerator::_Initialize(Player &player)
{
    _Reset(player);
}

void FlightPathMovementGenerator::_Finalize(Player & player)
{
    if (player.m_taxi.empty())
    {
        // update z position to ground and orientation for landing point
        // this prevent cheating with landing  point at lags
        // when client side flight end early in comparison server side
        player.StopMoving();
    }
}

void FlightPathMovementGenerator::_Reset(Player & player)
{
    Movement::MoveSplineInit init(player);
    uint32 end = GetPathAtMapEnd();
    for (uint32 i = GetCurrentNode(); i != end; ++i)
    {
        G3D::Vector3 vertice((*i_path)[i].x,(*i_path)[i].y,(*i_path)[i].z);
        init.Path().push_back(vertice);
    }
    init.SetFirstPointId(GetCurrentNode());
    init.SetFly();
    init.SetVelocity(32.0f);
    init.Launch();
}

void FlightPathMovementGenerator::_Interrupt(Player & player)
{
}

bool FlightPathMovementGenerator::Update(Player &player, const uint32 &diff)
{
    uint32 pointId = (uint32)player.movespline->currentPathIdx();
    if (pointId > i_currentNode)
    {
        bool departureEvent = true;
        do
        {
            DoEventIfAny(player,(*i_path)[i_currentNode],departureEvent);
            if (pointId == i_currentNode)
                break;

            i_currentNode += (uint32)departureEvent;
            departureEvent = !departureEvent;
        }
        while(true);
    }

    return !(player.movespline->Finalized() || i_currentNode >= (i_path->size()-1));
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path->empty())
        return;

    uint32 map0 = (*i_path)[0].mapid;

    for (size_t i = 1; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure)
{
    if (uint32 eventid = departure ? node.departureEventID : node.arrivalEventID)
    {
        //DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node.index, node.path, player.GetName());

        if (!sScriptMgr.OnProcessEvent(eventid, &player, &player, departure))
            player.GetMap()->ScriptsStart(sEventScripts, eventid, &player, &player);
    }
}

bool FlightPathMovementGenerator::GetResetPosition(Player&, float& x, float& y, float& z)
{
    const TaxiPathNodeEntry& node = (*i_path)[i_currentNode];
    x = node.x; y = node.y; z = node.z;
    return true;
}
