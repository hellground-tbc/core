#include "PlayerAI.h"
#include "Player.h"
#include "Database/DBCStores.h"

class Player;
struct SpellEntry;

bool PlayerAI::UpdateVictim(float range)
{
    if(Unit *charmer = me->GetCharmer())
    {
        if(charmer->isAlive())
        {
            if(!me->getVictim() || !me->getVictim()->isAlive())
            {
                if(Unit *victim = me->SelectNearbyTarget(range))
                    AttackStart(victim);
                else
                    if(charmer->getVictim())
                        AttackStart(charmer->getVictim());
             }
        }
        else
            me->RemoveCharmAuras();
    }
    else
        me->RemoveCharmAuras();

    if(Unit *target = me->getVictim())
        me->SetOrientation(me->GetAngle(target));

    return me->getVictim();
}

SpellEntry const *PlayerAI::selectHighestRank(uint32 spell_id)
{
    SpellEntry const *spell_info = sSpellStore.LookupEntry(spell_id);
    if (!spell_info)
        return NULL;

    PlayerSpellMap const &sp_list = me->GetSpellMap();

    SpellEntry const *highest_rank = spell_info;
    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
    {
        if(!itr->second->active || itr->second->disabled || itr->second->state == PLAYERSPELL_REMOVED)
            continue;

        spell_info = sSpellStore.LookupEntry(itr->first);
        if (!spell_info)
            continue;

        if (highest_rank->SpellFamilyName == spell_info->SpellFamilyName && (highest_rank->SpellFamilyFlags & spell_info->SpellFamilyFlags))
        {
            if (spell_info->spellLevel > highest_rank->spellLevel)
                highest_rank = spell_info;
        }
    }
    return highest_rank;
}

void WarriorAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void HunterAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void PaladinAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void WarlockAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void DruidAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void RogueAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void ShamanAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void PriestAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}

void MageAI::UpdateAI(const uint32 diff)
{
    if(!UpdateVictim())
        return;
}


