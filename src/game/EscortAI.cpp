#include "EscortAI.h"
#include "Group.h"

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
    if (GetState() != ESCORT_IN_PROGRESS)
        return;

    if (escort.IsEmpty())
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
                SetState(ESCORT_DONE);

            // to be sure, we always have ESCORT movement if allowed
            if (me->GetUnitStateMgr().GetAction(UNIT_ACTION_PRIORITY_ESCORT) != nullptr)
                break;

            // no break
        }
        case ESCORT_NEXT_POINT:
        {
            // don't start next waypoint if we can fight and are in combat
            if (flags & (FLAG_IS_DEFENSIVE | FLAG_IS_AGGRESSIVE) && me->isInCombat())
                break;

            delayTimer.Update(diff);
            if (!delayTimer.Passed())
                break;

            Waypoint& wp = path[pathIndex];
            if (!startDone)
            {
                WaypointStart(wp.Id);
                startDone = true;
            }

            // pointId already incremented in MovementInform
            me->GetMotionMaster()->MovePoint(wp.Id, wp.Pos.x, wp.Pos.y, wp.Pos.z, UNIT_ACTION_ESCORT);

            SetState(ESCORT_IN_PROGRESS);
            break;
        }
        case ESCORT_DONE:
        {
            DespawnOrRespawn();
            return;
        }
    }

    UpdateEscortAI(diff);
}

void EscortAI::EnterCombat(Unit* who)
{
    me->SetHomePosition(me->GetPosition());

    CreatureAI::EnterCombat(who);
}

void EscortAI::MovementInform(uint32 type, uint32 data)
{
    if (type != POINT_MOTION_TYPE)
        return;

    // we just returned from combat to last position
    if (data == 0xFF)
    {
        SetState(ESCORT_NEXT_POINT);
        return;
    }

    Waypoint& wp = path[pathIndex];
    // different shitty PointMovegen were called, ignore :P
    if (data != wp.Id)
        return;

    WaypointReached(wp.Id);

    if (++pathIndex > path.size())
    {
        SetState(ESCORT_DONE);
        return;
    }

    wp = path[pathIndex];
    delayTimer.Reset(wp.Delay);

    startDone = false;

    SetState(ESCORT_NEXT_POINT);
}

void EscortAI::Reset()
{
    me->RestoreFaction();

    me->GetPosition(origin);

    pathIndex = 0;
    delayTimer.Reset(0);

    SetState(ESCORT_NOT_STARTED);
}

void EscortAI::EnterEvadeMode()
{
    CreatureAI::_EnterEvadeMode();

    if (HasState(ESCORT_PAUSED) || HasState(ESCORT_DONE) || HasState(ESCORT_NOT_STARTED))
        return;

    me->GetMotionMaster()->MovePoint(0xFF, me->GetHomePosition().coord_x, me->GetHomePosition().coord_y, me->GetHomePosition().coord_z, UNIT_ACTION_ESCORT);
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

void EscortAI::UpdateEscortAI(const uint32 diff)
{
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
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
