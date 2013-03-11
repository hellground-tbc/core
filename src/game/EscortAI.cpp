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
    if (flags & FLAG_CAN_ATTACK)
        CreatureAI::AttackStart(who);
}

void EscortAI::MoveInLineOfSight(Unit* who)
{
    if (flags & FLAG_IS_AGGRESSIVE && !me->HasReactState(REACT_PASSIVE))
        CreatureAI::MoveInLineOfSight(who);

    if (flags & FLAG_ASSIST_IN_COMBAT && who->getVictimGUID() == escort.GetRawValue())
        CreatureAI::AttackStart(who);
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

    if (flags & FLAG_RESPAWN_AT_END)
    {
        me->Relocate(origin);
        me->Respawn();
    }
}

void EscortAI::UpdateAI(const uint32 diff)
{
    switch (GetState())
    {
        case ESCORT_PAUSED:
        case ESCORT_NOT_STARTED:
            return;
        case ESCORT_NEXT_POINT:
        {
            WaypointStart(pointId);

            delayTimer.Update(diff);
            if (!delayTimer.Passed())
                break;

            // pointId already incremented in MovementInform
            Waypoint& wp = path[pointId];
            me->GetMotionMaster()->MovePoint(wp.Id, wp.Pos.x, wp.Pos.y, wp.Pos.z);

            SetState(ESCORT_IN_PROGRESS);
            break;
        }
    }

    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

void EscortAI::EnterCombat(Unit* who)
{
    CreatureAI::EnterCombat(who);
}

void EscortAI::MovementInform(uint32 type, uint32 data)
{
    if (type != POINT_MOTION_TYPE)
        return;

    if (data != pointId)
        return;

    WaypointReached(pointId);

    if (++pointId > path.size())
    {
        me->Relocate(origin);
        if (flags & FLAG_RESPAWN_AT_END)
            me->Respawn();
        else if (flags & FLAG_DESPAWN_AT_END)
            me->ForcedDespawn();

        SetState(ESCORT_NOT_STARTED);

        return;
    }

    Waypoint& wp = path[pointId];
    delayTimer.Reset(wp.Delay);

    SetState(ESCORT_NEXT_POINT);
}

void EscortAI::Reset()
{
    me->RestoreFaction();

    me->GetPosition(origin);

    pointId = 0;
    delayTimer.Reset(0);

    SetState(ESCORT_NOT_STARTED);
}

void EscortAI::EnterEvadeMode()
{
    CreatureAI::_EnterEvadeMode();

    if (HasState(ESCORT_PAUSED))
        return;

    SetState(ESCORT_NEXT_POINT);
}
