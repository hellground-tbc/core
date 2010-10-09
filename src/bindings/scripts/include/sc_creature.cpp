/* Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Thanks to the original authors: ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"
#include "Item.h"
#include "Spell.h"
#include "ObjectMgr.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

void SummonList::DoAction(uint32 entry, uint32 info)
{
    for (iterator i = begin(); i != end(); )
    {
         Creature *summon = Unit::GetCreature(*m_creature, *i);
         i++;
         if(summon && summon->IsAIEnabled && (!entry || summon->GetEntry() == entry))
             summon->AI()->DoAction(info);
    }
}
void SummonList::Despawn(Creature *summon)
{
    uint64 guid = summon->GetGUID();
    for(iterator i = begin(); i != end(); ++i)
    {
        if(*i == guid)
        {
            erase(i);
            return;
        }
    }
}

void SummonList::DespawnEntry(uint32 entry)
{
    for(iterator i = begin(); i != end(); ++i)
    {
        if(Creature *summon = Unit::GetCreature(*m_creature, *i))
        {
            if(summon->GetEntry() == entry)
            {
                summon->setDeathState(JUST_DIED);
                summon->RemoveCorpse();
                i = erase(i);
                --i;
            }
        }
        else
        {
            i = erase(i);
            --i;
        }
    }
}

void SummonList::AuraOnEntry(uint32 entry, uint32 spellId, bool apply)
{
    for(iterator i = begin(); i != end(); ++i)
    {
        if(Creature *summon = Unit::GetCreature(*m_creature, *i))
        {
            if(summon->GetEntry() == entry)
            {
                if(apply)
                    summon->AddAura(spellId, summon);
                else
                    summon->RemoveAurasDueToSpell(spellId);
            }
        }
    }
}

void SummonList::DespawnAll()
{
    for(iterator i = begin(); i != end(); ++i)
    {
        if(Creature *summon = Unit::GetCreature(*m_creature, *i))
        {
            summon->setDeathState(JUST_DIED);
            summon->RemoveCorpse();
        }
    }
    clear();
}


ScriptedAI::ScriptedAI(Creature* pCreature) :
CreatureAI(pCreature), m_creature(pCreature), IsFleeing(false), m_bCombatMovement(true), m_uiEvadeCheckCooldown(2500)
{
    HeroicMode = m_creature->GetMap()->IsHeroic();
}

void ScriptedAI::AttackStartNoMove(Unit* pWho)
{
    if (!pWho)
        return;

    if(m_creature->Attack(pWho, false))
        DoStartNoMovement(pWho);
}

void ScriptedAI::AttackStart(Unit* pWho)
{
    if (!pWho)
        return;

    if (m_creature->Attack(pWho, true))
         DoStartMovement(pWho);
}

void ScriptedAI::AttackStart(Unit* pWho, bool melee)
{
    if (!pWho)
        return;

    if (!melee)
        AttackStartNoMove(pWho);
    else
        AttackStart(pWho);
}

void ScriptedAI::UpdateAI(const uint32 uiDiff)
{
    //Check if we have a current target
    if (!UpdateVictim())
        return;

    if (m_creature->isAttackReady())
    {
        //If we are within range melee the target
        if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
        {
            m_creature->AttackerStateUpdate(m_creature->getVictim());
            m_creature->resetAttackTimer();
        }
    }
}

void ScriptedAI::DoStartMovement(Unit* pVictim, float fDistance, float fAngle)
{
    if (pVictim)
        m_creature->GetMotionMaster()->MoveChase(pVictim, fDistance, fAngle);
}

void ScriptedAI::DoStartNoMovement(Unit* pVictim)
{
    if (!pVictim)
        return;

    m_creature->GetMotionMaster()->MoveIdle();
}

void ScriptedAI::DoStopAttack()
{
    if (m_creature->getVictim())
        m_creature->AttackStop();
}

void ScriptedAI::CastNextSpellIfAnyAndReady(uint32 diff)
{
    //clear spell list if caster isn't alive
    if (!m_creature->isAlive())
    {
        spellList.clear();
        autocast = false;
        return;
    }

    bool casted = false;

    if (m_creature->hasUnitState(UNIT_STAT_CASTING))
        casted = true;

    if (!spellList.empty() && !casted)
    {
        SpellToCast *temp = &spellList.front();

        if (!temp && !temp->triggered)
            return;

        if (temp->scriptTextEntry)
            DoScriptText(temp->scriptTextEntry, m_creature, m_creature->getVictim());

        if (temp->targetGUID)
        {
            Unit * tempU = m_creature->GetUnit(*m_creature, temp->targetGUID);

            if (tempU && tempU->IsInWorld() && tempU->isAlive() && tempU->IsInMap(m_creature))
                if (temp->spellId)
                {
                    if(temp->setAsTarget)
                        m_creature->SetSelection(temp->targetGUID);

                    m_creature->CastSpell(tempU, temp->spellId, temp->triggered);
                }
        }
        else
        {
            if (temp->isAOECast)
            {
                if (temp->spellId)
                    m_creature->CastSpell(m_creature, temp->spellId, temp->triggered);
            }
            else
                m_creature->CastSpell((Unit*)NULL, temp->spellId, temp->triggered);
        }

        spellList.pop_front();
        casted = true;
    }

    if (autocast)
    {
        if (autocastTimer < diff)
        {
            if (!casted)
            {
                Unit * victim = NULL;

                switch(autocastMode)
                {
                    case AUTOCAST_TANK:
                        victim = m_creature->getVictim();
                        break;
                    case AUTOCAST_NULL:
                        m_creature->CastSpell((Unit*)NULL, autocastId, false);
                        break;
                    case AUTOCAST_RANDOM:
                        victim = SelectUnit(SELECT_TARGET_RANDOM, 0, autocastTargetRange, autocastTargetPlayer);
                        break;
                    case AUTOCAST_RANDOM_WITHOUT_TANK:
                        victim = SelectUnit(SELECT_TARGET_RANDOM, 1, autocastTargetRange, autocastTargetPlayer);
                        break;
                    case AUTOCAST_SELF:
                        victim = m_creature;
                        break;
                    default:    //unsupported autocast, stop
                        {
                            autocast = false;
                            return;
                        }
                }

                if (victim)
                    m_creature->CastSpell(victim, autocastId, false);

                autocastTimer = autocastTimerDef;
            }
        }
        else
            autocastTimer -= diff;
    }
}

void ScriptedAI::DoCast(Unit* victim, uint32 spellId, bool triggered)
{
    if (/*!victim || */m_creature->hasUnitState(UNIT_STAT_CASTING) && !triggered)
        return;

    //m_creature->StopMoving();
    m_creature->CastSpell(victim, spellId, triggered);
}

void ScriptedAI::DoCastAOE(uint32 spellId, bool triggered)
{
    if(!triggered && m_creature->hasUnitState(UNIT_STAT_CASTING))
        return;

    m_creature->CastSpell((Unit*)NULL, spellId, triggered);
}

void ScriptedAI::DoCastSpell(Unit* who,SpellEntry const *spellInfo, bool triggered)
{
    if (/*!who || */m_creature->IsNonMeleeSpellCasted(false))
        return;

    m_creature->StopMoving();
    m_creature->CastSpell(who, spellInfo, triggered);
}

void ScriptedAI::AddSpellToCast(Unit* victim, uint32 spellId, bool triggered, bool visualTarget)
{
    /*if (!victim)
        return;*/

    SpellToCast temp(victim ? victim->GetGUID() : NULL, spellId, triggered, 0, false, visualTarget);

    spellList.push_back(temp);
}

void ScriptedAI::AddSpellToCastWithScriptText(Unit* victim, uint32 spellId, int32 scriptTextEntry, bool triggered, bool visualTarget)
{
    /*if (!victim)
        return;*/

    SpellToCast temp(victim ? victim->GetGUID() : NULL, spellId, triggered, scriptTextEntry, false, visualTarget);

    spellList.push_back(temp);
}

void ScriptedAI::AddAOESpellToCast(uint32 spellId, bool triggered)
{
    SpellToCast temp(m_creature, spellId, triggered, 0, true, false);

    spellList.push_back(temp);
}

void ScriptedAI::AddAOESpellToCastWithScriptText(uint32 spellId, int32 scriptTextEntry, bool triggered)
{
    SpellToCast temp(m_creature, spellId, triggered, scriptTextEntry, true, false);

    spellList.push_back(temp);
}

void ScriptedAI::ForceSpellCast(Unit *victim, uint32 spellId, interruptSpell interruptCurrent, bool triggered, bool visualTarget)
{
    /*if (!victim)
        return;*/

    switch (interruptCurrent)
    {
    case INTERRUPT_AND_CAST:
        m_creature->InterruptNonMeleeSpells(false);
        break;
    case INTERRUPT_AND_CAST_INSTANTLY:
        if(visualTarget)
            m_creature->SetSelection(victim->GetGUID());

        m_creature->CastSpell(victim, spellId, triggered);
        return;
    default:
        break;
    }

    SpellToCast temp(victim ? victim->GetGUID() : NULL, spellId, triggered, 0, false, visualTarget);

    spellList.push_front(temp);
}

void ScriptedAI::ForceSpellCastWithScriptText(Unit *victim, uint32 spellId, int32 scriptTextEntry, interruptSpell interruptCurrent, bool triggered, bool visualTarget)
{
    /*if (!victim)
        return;*/

    switch(interruptCurrent)
    {
    case INTERRUPT_AND_CAST:
        m_creature->InterruptNonMeleeSpells(false);
        break;
    case INTERRUPT_AND_CAST_INSTANTLY:
        if (scriptTextEntry)
            DoScriptText(scriptTextEntry, m_creature, victim);

        if (visualTarget)
            m_creature->SetSelection(victim->GetGUID());

        m_creature->CastSpell(victim, spellId, triggered);
        return;
    default:
        break;
    }

    SpellToCast temp(victim ? victim->GetGUID() : NULL, spellId, triggered, scriptTextEntry, false, visualTarget);

    spellList.push_front(temp);
}

void ScriptedAI::ForceAOESpellCast(uint32 spellId, interruptSpell interruptCurrent, bool triggered)
{
    switch(interruptCurrent)
    {
    case INTERRUPT_AND_CAST:
        m_creature->InterruptNonMeleeSpells(false);
        break;
    case INTERRUPT_AND_CAST_INSTANTLY:
        m_creature->CastSpell(m_creature, spellId, triggered);
        return;
    default:
        break;
    }

    SpellToCast temp(m_creature, spellId, triggered, 0, true, false);

    spellList.push_front(temp);
}

void ScriptedAI::ForceAOESpellCastWithScriptText(uint32 spellId, int32 scriptTextEntry, interruptSpell interruptCurrent, bool triggered)
{
    switch(interruptCurrent)
    {
    case INTERRUPT_AND_CAST:
        m_creature->InterruptNonMeleeSpells(false);
        break;
    case INTERRUPT_AND_CAST_INSTANTLY:
        if (m_creature->getVictim() && scriptTextEntry)
            DoScriptText(scriptTextEntry, m_creature, m_creature->getVictim());
        m_creature->CastSpell(m_creature, spellId, triggered);
        return;
    default:
        break;
    }

    SpellToCast temp(m_creature, spellId, triggered, scriptTextEntry, true, false);

    spellList.push_front(temp);
}

void ScriptedAI::SetAutocast (uint32 spellId, uint32 timer, bool startImmediately, autocastTargetMode mode, uint32 range, bool player)
{
    if (!spellId)
        return;

    autocastId = spellId;

    autocastTimer = timer;
    autocastTimerDef = timer;

    autocastMode = mode;
    autocastTargetRange = range;
    autocastTargetPlayer = player;

    autocast = startImmediately;
}

void ScriptedAI::DoSay(const char* text, uint32 language, Unit* target, bool SayEmote)
{
    if (target)
    {
        m_creature->Say(text, language, target->GetGUID());
        if(SayEmote)
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    }
    else m_creature->Say(text, language, 0);
}

void ScriptedAI::DoYell(const char* text, uint32 language, Unit* target)
{
    if (target) m_creature->Yell(text, language, target->GetGUID());
    else m_creature->Yell(text, language, 0);
}

void ScriptedAI::DoTextEmote(const char* text, Unit* target, bool IsBossEmote)
{
    if (target) m_creature->TextEmote(text, target->GetGUID(), IsBossEmote);
    else m_creature->TextEmote(text, 0, IsBossEmote);
}

void ScriptedAI::DoWhisper(const char* text, Unit* reciever, bool IsBossWhisper)
{
    if (!reciever || reciever->GetTypeId() != TYPEID_PLAYER)
        return;

    m_creature->Whisper(text, reciever->GetGUID(), IsBossWhisper);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* unit, uint32 sound)
{
    if (!unit)
        return;

    if (!GetSoundEntriesStore()->LookupEntry(sound))
    {
        error_log("TSCR: Invalid soundId %u used in DoPlaySoundToSet (by unit TypeId %u, guid %u)", sound, unit->GetTypeId(), unit->GetGUID());
        return;
    }

    WorldPacket data(4);
    data.SetOpcode(SMSG_PLAY_SOUND);
    data << uint32(sound);
    unit->SendMessageToSet(&data,false);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 id, float x, float y, float z, float angle, uint32 type, uint32 despawntime)
{
    return m_creature->SummonCreature(id,m_creature->GetPositionX() + x,m_creature->GetPositionY() + y,m_creature->GetPositionZ() + z, angle, (TempSummonType)type, despawntime);
}


struct TargetDistanceOrder : public std::binary_function<const Unit *, const Unit *, bool>
{
    const Unit * me;
    TargetDistanceOrder(const Unit* Target) : me(Target) {};
    // functor for operator ">"
    bool operator()(const Unit * _Left, const Unit * _Right) const
    {
        return (me->GetExactDistSq(_Left) < me->GetExactDistSq(_Right));
    }
};

Unit* ScriptedAI::SelectUnit(SelectAggroTarget target, uint32 position)
{
    //ThreatList m_threatlist;
    std::list<HostilReference*>& m_threatlist = me->getThreatManager().getThreatList();
    std::list<HostilReference*>::iterator i = m_threatlist.begin();
    std::list<HostilReference*>::reverse_iterator r = m_threatlist.rbegin();

    if (position >= m_threatlist.size() || !m_threatlist.size())
        return NULL;

    switch (target)
    {
        case SELECT_TARGET_RANDOM:
        {
            advance ( i , position +  (rand() % (m_threatlist.size() - position ) ));
            return Unit::GetUnit(*me,(*i)->getUnitGuid());
        }
        case SELECT_TARGET_TOPAGGRO:
        {
            advance ( i , position);
            return Unit::GetUnit(*me,(*i)->getUnitGuid());
        }
        case SELECT_TARGET_BOTTOMAGGRO:
        {
            advance ( r , position);
            return Unit::GetUnit(*me,(*r)->getUnitGuid());
        }
    }
    return NULL;
}

Unit* ScriptedAI::SelectUnit(SelectAggroTarget targetType, uint32 position, float dist, bool playerOnly, uint64 exclude, float mindist)
{
    if(targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
    {
        std::list<HostilReference*> &m_threatlist = m_creature->getThreatManager().getThreatList();
        if(m_threatlist.empty()) return NULL;
        std::list<Unit*> targetList;
        std::list<HostilReference*>::iterator itr = m_threatlist.begin();
        for(; itr!= m_threatlist.end(); ++itr)
        {
            Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
            if(!target
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || dist && !m_creature->IsWithinCombatRange(target, dist)
                || exclude && exclude == target->GetGUID()
                || mindist && m_creature->IsWithinCombatRange(target, mindist))
            {
                continue;
            }
            targetList.push_back(target);
        }
        if(position >= targetList.size())
            return NULL;

        targetList.sort(TargetDistanceOrder(m_creature));

        if(targetType == SELECT_TARGET_NEAREST)
        {
            std::list<Unit*>::iterator i = targetList.begin();
            advance(i, position);
            return *i;
        }
        else
        {
            std::list<Unit*>::reverse_iterator i = targetList.rbegin();
            advance(i, position);
            return *i;
        }
    }
    else
    {
        std::list<HostilReference*> m_threatlist = m_creature->getThreatManager().getThreatList();
        std::list<HostilReference*>::iterator i;
        Unit *target;
        while(position < m_threatlist.size())
        {
            if(targetType == SELECT_TARGET_BOTTOMAGGRO)
            {
                i = m_threatlist.end();
                advance(i, - (int32)position - 1);
            }
            else
            {
                i = m_threatlist.begin();
                if(targetType == SELECT_TARGET_TOPAGGRO)
                    advance(i, position);
                else // random
                    advance(i, position + rand()%(m_threatlist.size() - position));
            }

            target = Unit::GetUnit(*m_creature,(*i)->getUnitGuid());
            if(!target
                || !target->isAlive()
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || dist && !m_creature->IsWithinCombatRange(target, dist)
                || exclude && exclude == target->GetGUID()
                || mindist && m_creature->IsWithinCombatRange(target, mindist))
            {
                m_threatlist.erase(i);
            }
            else
            {
                return target;
            }
        }
    }

    return NULL;
}

Unit* ScriptedAI::SelectUnit(SelectAggroTarget targetType, uint32 position, float maxdist, bool playerOnly, Powers power)
{
    if(targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
    {
        std::list<HostilReference*> &m_threatlist = m_creature->getThreatManager().getThreatList();
        if(m_threatlist.empty()) return NULL;
        std::list<Unit*> targetList;
        std::list<HostilReference*>::iterator itr = m_threatlist.begin();
        for(; itr!= m_threatlist.end(); ++itr)
        {
            Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
            if(!target
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || maxdist && !m_creature->IsWithinCombatRange(target, maxdist)
                || power != target->getPowerType())
            {
                continue;
            }
            targetList.push_back(target);
        }
        if(position >= targetList.size())
            return NULL;

        targetList.sort(TargetDistanceOrder(m_creature));

        if(targetType == SELECT_TARGET_NEAREST)
        {
            std::list<Unit*>::iterator i = targetList.begin();
            advance(i, position);
            return *i;
        }
        else
        {
            std::list<Unit*>::reverse_iterator i = targetList.rbegin();
            advance(i, position);
            return *i;
        }
    }
    else
    {
        std::list<HostilReference*> m_threatlist = m_creature->getThreatManager().getThreatList();
        std::list<HostilReference*>::iterator i;
        Unit *target;
        while(position < m_threatlist.size())
        {
            if(targetType == SELECT_TARGET_BOTTOMAGGRO)
            {
                i = m_threatlist.end();
                advance(i, - (int32)position - 1);
            }
            else
            {
                i = m_threatlist.begin();
                if(targetType == SELECT_TARGET_TOPAGGRO)
                    advance(i, position);
                else // random
                    advance(i, position + rand()%(m_threatlist.size() - position));
            }

            target = Unit::GetUnit(*m_creature,(*i)->getUnitGuid());
            if(!target
                || !target->isAlive()
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || maxdist && !m_creature->IsWithinCombatRange(target, maxdist)
                || power != target->getPowerType())
            {
                m_threatlist.erase(i);
            }
            else
            {
                return target;
            }
        }
    }

    return NULL;
}

void ScriptedAI::SelectUnitList(std::list<Unit*> &targetList, uint32 num, SelectAggroTarget targetType, float dist, bool playerOnly)
{
    if(targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
    {
        std::list<HostilReference*> &m_threatlist = m_creature->getThreatManager().getThreatList();
        if(m_threatlist.empty()) return;
        std::list<HostilReference*>::iterator itr = m_threatlist.begin();
        for(; itr!= m_threatlist.end(); ++itr)
        {
            Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
            if(!target
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || dist && !m_creature->IsWithinCombatRange(target, dist))
            {
                continue;
            }
            targetList.push_back(target);
        }
        targetList.sort(TargetDistanceOrder(m_creature));
        targetList.resize(num);
        if(targetType == SELECT_TARGET_FARTHEST)
            targetList.reverse();
    }
    else
    {
        std::list<HostilReference*> m_threatlist = m_creature->getThreatManager().getThreatList();
        std::list<HostilReference*>::iterator i;
        Unit *target;
        while(m_threatlist.size() && num)
        {
            if(targetType == SELECT_TARGET_BOTTOMAGGRO)
            {
                i = m_threatlist.end();
                --i;
            }
            else
            {
                i = m_threatlist.begin();
                if(targetType == SELECT_TARGET_RANDOM)
                    advance(i, rand()%m_threatlist.size());
            }

            target = Unit::GetUnit(*m_creature,(*i)->getUnitGuid());
            m_threatlist.erase(i);
            if(!target
                || playerOnly && target->GetTypeId() != TYPEID_PLAYER
                || dist && !m_creature->IsWithinCombatRange(target, dist))
            {
                continue;
            }
            targetList.push_back(target);
            --num;
        }
    }
}

SpellEntry const* ScriptedAI::SelectSpell(Unit* pTarget, int32 uiSchool, int32 uiMechanic, SelectTargetType selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffects)
{
    //No target so we can't cast
    if (!pTarget)
        return false;

    //Silenced so we can't cast
    if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return false;

    //Using the extended script system we first create a list of viable spells
    SpellEntry const* apSpell[CREATURE_MAX_SPELLS];
    memset(apSpell, 0, sizeof(SpellEntry*)*CREATURE_MAX_SPELLS);

    uint32 uiSpellCount = 0;

    SpellEntry const* pTempSpell;
    SpellRangeEntry const* pTempRange;

    //Check if each spell is viable(set it to null if not)
    for (uint32 i = 0; i < CREATURE_MAX_SPELLS; i++)
    {
        pTempSpell = GetSpellStore()->LookupEntry(m_creature->m_spells[i]);

        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (selectTargets && !(SpellSummary[m_creature->m_spells[i]].Targets & (1 << (selectTargets-1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (selectEffects && !(SpellSummary[m_creature->m_spells[i]].Effects & (1 << (selectEffects-1))))
            continue;

        //Check for school if specified
        if (uiSchool >= 0 && pTempSpell->SchoolMask & uiSchool)
            continue;

        //Check for spell mechanic if specified
        if (uiMechanic >= 0 && pTempSpell->Mechanic != uiMechanic)
            continue;

        //Make sure that the spell uses the requested amount of power
        if (uiPowerCostMin && pTempSpell->manaCost < uiPowerCostMin)
            continue;

        if (uiPowerCostMax && pTempSpell->manaCost > uiPowerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (pTempSpell->manaCost > m_creature->GetPower((Powers)pTempSpell->powerType))
            continue;

        //Get the Range
        pTempRange = GetSpellRangeStore()->LookupEntry(pTempSpell->rangeIndex);

        //Spell has invalid range store so we can't use it
        if (!pTempRange)
            continue;

        //Check if the spell meets our range requirements
        if (fRangeMin && pTempRange->maxRange < fRangeMin)
            continue;

        if (fRangeMax && pTempRange->maxRange > fRangeMax)
            continue;

        //Check if our target is in range
        if (m_creature->IsWithinDistInMap(pTarget, pTempRange->minRange) || !m_creature->IsWithinDistInMap(pTarget, pTempRange->maxRange))
            continue;

        //All good so lets add it to the spell list
        apSpell[uiSpellCount] = pTempSpell;
        ++uiSpellCount;
    }

    //We got our usable spells so now lets randomly pick one
    if (!uiSpellCount)
        return NULL;

    return apSpell[rand()%uiSpellCount];
}

float ScriptedAI::GetSpellMaxRange(uint32 id)
{
    SpellEntry const *spellInfo = GetSpellStore()->LookupEntry(id);
    if(!spellInfo)
        return 0;

    SpellRangeEntry const *range = GetSpellRangeStore()->LookupEntry(spellInfo->rangeIndex);
    if(!range)
        return 0;
    return range->maxRange;
}

void FillSpellSummary()
{
    SpellSummary = new TSpellSummary[GetSpellStore()->GetNumRows()];

    SpellEntry const* TempSpell;

    for (int i=0; i < GetSpellStore()->GetNumRows(); i++ )
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        TempSpell = GetSpellStore()->LookupEntry(i);
        //This spell doesn't exist
        if (!TempSpell)
            continue;

        for (int j=0; j<3; j++)
        {
            //Spell targets self
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER )
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF-1);

            //Spell targets a single enemy
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY )
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY-1);

            //Spell targets AoE at enemy
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY )
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY-1);

            //Spell targets an enemy
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY )
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY-1);

            //Spell targets a single friend(or self)
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY )
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND-1);

            //Spell targets aoe friends
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND-1);

            //Spell targets any friend(or self)
            if ( TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                TempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND-1);

            //Make sure that this spell includes a damage effect
            if ( TempSpell->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE ||
                TempSpell->Effect[j] == SPELL_EFFECT_INSTAKILL ||
                TempSpell->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                TempSpell->Effect[j] == SPELL_EFFECT_HEALTH_LEECH )
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE-1);

            //Make sure that this spell includes a healing effect (or an apply aura with a periodic heal)
            if ( TempSpell->Effect[j] == SPELL_EFFECT_HEAL ||
                TempSpell->Effect[j] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                TempSpell->Effect[j] == SPELL_EFFECT_HEAL_MECHANICAL ||
                (TempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA  && TempSpell->EffectApplyAuraName[j]== 8 ))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);

            //Make sure that this spell applies an aura
            if ( TempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA )
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
        }
    }
}

void ScriptedAI::DoResetThreat()
{
    if (!m_creature->CanHaveThreatList() || m_creature->getThreatManager().isThreatListEmpty())
    {
        error_log("TSCR: DoResetThreat called for creature that either cannot have threat list or has empty threat list (m_creature entry = %d)", m_creature->GetEntry());

        return;
    }

    std::list<HostilReference*>& m_threatlist = m_creature->getThreatManager().getThreatList();
    std::list<HostilReference*>::iterator itr;

    for(itr = m_threatlist.begin(); itr != m_threatlist.end(); ++itr)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
        if(pUnit && DoGetThreat(pUnit))
            DoModifyThreatPercent(pUnit, -100);
    }
}

float ScriptedAI::DoGetThreat(Unit* pUnit)
{
    if(!pUnit) return 0.0f;
    return m_creature->getThreatManager().getThreat(pUnit);
}

void ScriptedAI::DoModifyThreatPercent(Unit *pUnit, int32 pct)
{
    if(!pUnit) return;
    m_creature->getThreatManager().modifyThreatPercent(pUnit, pct);
}

void ScriptedAI::DoTeleportTo(float x, float y, float z, uint32 time)
{
    m_creature->Relocate(x,y,z);
    m_creature->SendMonsterMove(x, y, z, time);
}

void ScriptedAI::DoTeleportPlayer(Unit* pUnit, float x, float y, float z, float o)
{
    if(!pUnit || pUnit->GetTypeId() != TYPEID_PLAYER)
    {
        if(pUnit)
            error_log("TSCR: Creature %u (Entry: %u) Tried to teleport non-player unit (Type: %u GUID: %u) to x: %f y:%f z: %f o: %f. Aborted.", m_creature->GetGUID(), m_creature->GetEntry(), pUnit->GetTypeId(), pUnit->GetGUID(), x, y, z, o);
        return;
    }

    ((Player*)pUnit)->TeleportTo(pUnit->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
}

void ScriptedAI::DoTeleportAll(float x, float y, float z, float o)
{
    Map *map = m_creature->GetMap();
    if (!map->IsDungeon())
        return;

    Map::PlayerList const &PlayerList = map->GetPlayers();
    for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        if (Player* i_pl = i->getSource())
            if (i_pl->isAlive())
                i_pl->TeleportTo(m_creature->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* FindCreature(uint32 entry, float range, Unit* Finder)
{
    if(!Finder)
        return NULL;
    Creature* target = NULL;
    Trinity::AllCreaturesOfEntryInRange check(Finder, entry, range);
    Trinity::CreatureSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(target, check);
    Finder->VisitNearbyObject(range, searcher);
    return target;
}

GameObject* FindGameObject(uint32 entry, float range, Unit* Finder)
{
    if(!Finder)
        return NULL;
    GameObject* target = NULL;
    Trinity::AllGameObjectsWithEntryInGrid go_check(entry);
    Trinity::GameObjectSearcher<Trinity::AllGameObjectsWithEntryInGrid> searcher(target, go_check);
    Finder->VisitNearbyGridObject(range, searcher);
    return target;
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float range, uint32 MinHPDiff)
{
    Unit* pUnit = NULL;
    Trinity::MostHPMissingInRange u_check(m_creature, range, MinHPDiff);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingInRange> searcher(pUnit, u_check);
    m_creature->VisitNearbyObject(range, searcher);
    return pUnit;
}

std::list<Creature*> ScriptedAI::DoFindAllCreaturesWithEntry(uint32 entry, float range)
{
    std::list<Creature*> pList;
    Trinity::AllCreaturesOfEntryInRange u_check(m_creature, entry, range);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(pList, u_check);
    m_creature->VisitNearbyObject(range, searcher);
    return pList;
}

std::list<Creature*> ScriptedAI::DoFindAllFriendlyInGrid(float range)
{
    std::list<Creature*> pList;
    Trinity::AllFriendlyCreaturesInGrid u_check(m_creature);
    Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(pList, u_check);
    m_creature->VisitNearbyGridObject(range, searcher);
    return pList;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float range)
{
    std::list<Creature*> pList;
    Trinity::FriendlyCCedInRange u_check(m_creature, range);
    Trinity::CreatureListSearcher<Trinity::FriendlyCCedInRange> searcher(pList, u_check);
    m_creature->VisitNearbyObject(range, searcher);
    return pList;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float range, uint32 spellid)
{
    std::list<Creature*> pList;
    Trinity::FriendlyMissingBuffInRange u_check(m_creature, range, spellid);
    Trinity::CreatureListSearcher<Trinity::FriendlyMissingBuffInRange> searcher(pList, u_check);
    m_creature->VisitNearbyObject(range, searcher);
    return pList;
}

void ScriptedAI::SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand, int32 uiOffHand, int32 uiRanged)
{
    if (bLoadDefault)
    {
        if (CreatureInfo const* pInfo = GetCreatureTemplateStore(m_creature->GetEntry()))
            m_creature->LoadEquipment(pInfo->equipmentId,true);

        return;
    }

    if (uiMainHand >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 0, uint32(uiMainHand));

    if (uiOffHand >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, uint32(uiOffHand));

    if (uiRanged >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 2, uint32(uiRanged));
}

void ScriptedAI::SetCombatMovement(bool bCombatMove)
{
    m_bCombatMovement = bCombatMove;
}

void Scripted_NoMovementAI::AttackStart(Unit* pWho)
{
    if (!pWho)
        return;

    if (m_creature->Attack(pWho, true))
        DoStartNoMovement(pWho);
}

#define GOBJECT(x) (const_cast<GameObjectInfo*>(GetGameObjectInfo(x)))

void LoadOverridenSQLData()
{
    GameObjectInfo *goInfo;

    // Sunwell Plateau : Kalecgos : Spectral Rift
    if(goInfo = GOBJECT(187055))
        if(goInfo->type == GAMEOBJECT_TYPE_GOOBER)
            goInfo->goober.lockId = 57; // need LOCKTYPE_QUICK_OPEN

    // Naxxramas : Sapphiron Birth
    if(goInfo = GOBJECT(181356))
        if(goInfo->type == GAMEOBJECT_TYPE_TRAP)
            goInfo->trap.radius = 50;
}

void LoadOverridenDBCData()
{
    SpellEntry *spellInfo;

    // Black Temple : Illidan : Parasitic Shadowfiend Passive
    spellInfo = const_cast<SpellEntry*>(GetSpellStore()->LookupEntry(41913));
    if(spellInfo)
        spellInfo->EffectApplyAuraName[0] = 4; // proc debuff, and summon infinite fiends
}

Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 Entry, float MaxSearchRange)
{
    Creature* pCreature = NULL;
    CellPair pair(Trinity::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*pSource, Entry, true, MaxSearchRange);
    Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);

    TypeContainerVisitor<Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);

    cell.Visit(pair, creature_searcher,*(pSource->GetMap()));

    return pCreature;
}
