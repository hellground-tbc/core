#include "PlayerAI.h"

bool PlayerAI::UpdateVictim()
{
    Unit *charmer = me->GetCharmer();
    if(!charmer || !charmer->isInCombat())
        return false;

    if(me->getVictim() && me->getVictim()->isAlive())
        return true;

    if(Unit *victim = me->SelectNearbyTarget(20.0f))
        AttackStart(victim);

    return me->getVictim();
}