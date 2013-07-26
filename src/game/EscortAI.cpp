#include "EscortAI.h"

Waypoint::Waypoint( uint32 id, uint32 delay, float x, float y, float z )
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
}

void EscortAI::JustDied(Unit* killer)
{
    if (state != ESCORT_IN_PROGRESS)
        return;

    if (escort.IsEmpty())
        return;

    Player* player = me->GetPlayer(escort.GetRawValue());
    if (player == NULL)
        return;

    if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
        player->FailQuest(questId);
}
