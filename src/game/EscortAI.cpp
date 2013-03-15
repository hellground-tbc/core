/*
 * Copyright (C) 2012 HellGround <http://hellground.net/>
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

#include "EscortAI.h"

#include "CreatureAI.h"
#include "Creature.h"
#include "Player.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "StateMgr.h"
#include "StateMgrImpl.h"

Waypoint::Waypoint(uint32 id, uint32 delay, float x, float y, float z)
{
    Id = id;
    Delay = delay;

    Pos.x = x;
    Pos.y = y;
    Pos.z = z;
}

Waypoint::Waypoint(const Waypoint& b)
{
    Id = b.Id;
    Delay = b.Delay;

    Pos.x = b.Pos.x;
    Pos.y = b.Pos.y;
    Pos.z = b.Pos.z;
}

void EscortAI::AttackStart(Unit* who)
{
    if (flags & (FLAG_IS_AGGRESSIVE | FLAG_IS_DEFENSIVE))
        CreatureAI::AttackStart(who);
}

void EscortAI::MoveInLineOfSight(Unit* who)
{
    if (flags & FLAG_IS_AGGRESSIVE)
        CreatureAI::MoveInLineOfSight(who);
}

void EscortAI::JustDied(Unit* killer)
{
    EscortJustDied(killer);

    if (GetState() != ESCORT_IN_PROGRESS)
        return;

    if (escort.IsEmpty() || !questId)
        return;

    Player* player = me->GetPlayer(escort.GetRawValue());
    if (player == NULL)
        return;

    if (Group* group = player->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref != NULL; ref = ref->next())
        {
            player = ref->getSource();

            if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                player->FailQuest(questId);
        }
    }
    else
    {
        if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
            player->FailQuest(questId);
    }

    DespawnOrRespawn();
}

void EscortAI::UpdateAI(const uint32 diff)
{
    switch (GetState())
    {
        case ESCORT_PAUSED:
        case ESCORT_NOT_STARTED:
            break;
        case ESCORT_IN_PROGRESS:
        {
            if (!EscortInRange())
                setState(ESCORT_DONE);

            // to be sure, we always have ESCORT movement if allowed
            if (!me->hasUnitState(UNIT_STAT_ROAMING) && (!me->isInCombat() || (flags & (FLAG_IS_DEFENSIVE | FLAG_IS_AGGRESSIVE)) == 0))
                setState(ESCORT_NEXT_POINT);

           break;
        }
        case ESCORT_NEXT_POINT:
        {
            // don't start next way point if we can fight and are in combat
            if (flags & (FLAG_IS_DEFENSIVE | FLAG_IS_AGGRESSIVE) && me->isInCombat())
                break;

            Waypoint& wp = path[pathIndex];
            if (!startDone)
            {
                delayTimer.Update(diff);
                if (!delayTimer.Passed())
                    break;

                WaypointStart(wp.Id);
                startDone = true;
            }

            // pointId already incremented in MovementInform
            me->GetMotionMaster()->MovePoint(wp.Id, wp.Pos.x, wp.Pos.y, wp.Pos.z, UNIT_ACTION_ESCORT);

            setState(ESCORT_IN_PROGRESS);
            break;
        }
        case ESCORT_DONE:
        {
            DespawnOrRespawn();
            return;
        }
    }

    EscortUpdateAI(diff);
}

void EscortAI::EnterCombat(Unit* who)
{
    me->SetHomePosition(me->GetPosition());

    EscortEnterCombat(who);
}

void EscortAI::MovementInform(uint32 type, uint32 data)
{
    if (type != POINT_MOTION_TYPE)
        return;

    // we just returned from combat to last position
    if (data == 0xFF)
    {
        setState(ESCORT_NEXT_POINT);
        return;
    }

    Waypoint& wp = path[pathIndex];
    // different shitty PointMovegen were called, ignore :P
    if (data != wp.Id)
        return;

    WaypointReached(wp.Id);

    if (++pathIndex > path.size())
    {
        setState(ESCORT_DONE);
        return;
    }

    wp = path[pathIndex];
    delayTimer.Reset(wp.Delay);

    startDone = false;

    setState(ESCORT_NEXT_POINT);
}

void EscortAI::Reset()
{
    me->RestoreFaction();

    me->GetPosition(origin);

    pathIndex = 0;
    delayTimer.Reset(0);

    path.clear();

    setState(ESCORT_NOT_STARTED);

    EscortReset();
}

void EscortAI::EnterEvadeMode()
{
    CreatureAI::_EnterEvadeMode();

    EscortEnterEvadeMode();

    if (HasState(ESCORT_PAUSED) || HasState(ESCORT_DONE) || HasState(ESCORT_NOT_STARTED))
        return;

    me->GetMotionMaster()->MovePoint(0xFF, me->GetHomePosition().coord_x, me->GetHomePosition().coord_y, me->GetHomePosition().coord_z, UNIT_ACTION_ESCORT);

    EscortReset();
}

bool EscortAI::EscortInRange() const
{
    Player* player = me->GetPlayer(escort.GetRawValue());
    if (player == nullptr)
        return false;

    if (Group* group = player->GetGroup())
    {
        for (GroupReference* ref = group->GetFirstMember(); ref != NULL; ref = ref->next())
        {
            player = ref->getSource();
            if (me->IsWithinDistInMap(player, 30.0f, true))
                return true;
        }
        return false;
    }
    else
        return me->IsWithinDistInMap(player, 30.0f, true);
}

void EscortAI::DespawnOrRespawn()
{
    if (me->isInCombat())
        EnterEvadeMode();

    me->Relocate(origin);
    if (flags & FLAG_RESPAWN_AT_END)
        me->Respawn();
    else if (flags & FLAG_DESPAWN_AT_END)
        me->ForcedDespawn();
}

void EscortAI::JustRespawned()
{
    me->SetHomePosition(me->GetPosition());

    EscortAI::Reset();
}

void EscortAI::AddWaypoint(uint32 id, float x, float y, float z, uint32 delay)
{
    path.push_back(Waypoint(id, delay, x, y, z));
}

void EscortAI::WaypointStart(uint32 pointId)
{

}

void EscortAI::EscortEnterCombat(Unit* who)
{
    CreatureAI::EnterCombat(who);
}

void EscortAI::EscortUpdateAI(const uint32 diff)
{
    if (!UpdateVictim())
        return;

	DoMeleeAttackIfReady();
}

EscortAI::EscortAI(Creature* owner) : CreatureAI(owner), pathIndex(0)
{

}

void EscortAI::EscortStart(uint32 quest, Player* invoker, Flags flag)
{
    if (!HasState(ESCORT_NOT_STARTED))
        return;

    if (!FillWaypointsList())
        return;

    flags = flag;

    SetGUID(invoker->GetGUID(), quest);

    setState(ESCORT_NEXT_POINT);
}

bool EscortAI::FillWaypointsList()
{
    path = sScriptMgr.GetWaypointsForEntry(me->GetEntry());
    return !path.empty();
}

void EscortAI::EscortPause()
{
    me->GetUnitStateMgr().DropAction(UNIT_ACTION_ESCORT);

    setState(ESCORT_PAUSED);
}

void EscortAI::ClearWaypoints()
{
    path.clear();
    pathIndex = 0;
}

void EscortAI::SetGUID(uint64 guid, int32 quest)
{
    escort = ObjectGuid(guid);
    questId = quest;
}
