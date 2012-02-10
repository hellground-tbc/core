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

#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "DBCStores.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "Util.h"

int PetAI::Permissible(const Creature *creature)
{
    if (creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature *c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK), m_forceTimer(0)
{
    m_AllySet.clear();
    m_owner = m_creature->GetCharmerOrOwner();
    UpdateAllies();
}

void PetAI::EnterEvadeMode()
{
}

bool PetAI::targetHasInterruptableAura(Unit *target) const
{
    if (!target)
        return false;

    if (m_forceTimer)
        return false;

    Unit::AuraMap const &auramap = target->GetAuras();
    for (Unit::AuraMap::const_iterator itr = auramap.begin(); itr != auramap.end(); ++itr)
    {
        if (itr->second && (itr->second->GetSpellProto()->AuraInterruptFlags & (AURA_INTERRUPT_FLAG_DIRECT_DAMAGE | AURA_INTERRUPT_FLAG_HITBYSPELL | AURA_INTERRUPT_FLAG_DAMAGE)))
            return true;
    }
    return false;
}

bool PetAI::_needToStop() const
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if (m_creature->isCharmed() && m_creature->getVictim() == m_creature->GetCharmer())
        return true;

    return targetHasInterruptableAura(m_creature->getVictim()) || !m_creature->canAttack(m_creature->getVictim());
}

void PetAI::_stopAttack()
{
    if (!m_creature->isAlive())
    {
        DEBUG_LOG("Creature stoped attacking cuz his dead [guid=%u]", m_creature->GetGUIDLow());
        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->CombatStop();
        m_creature->getHostilRefManager().deleteReferences();

        return;
    }

    UpdateMotionMaster();

    m_creature->CombatStop();
}

void PetAI::UpdateMotionMaster()
{
    if (m_owner && m_creature->GetCharmInfo() && m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
    {
        m_creature->GetMotionMaster()->MoveFollow(m_owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
    }
    else
    {
        m_creature->clearUnitState(UNIT_STAT_FOLLOW);
        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveIdle();
    }
}

void PetAI::PrepareSpellForAutocast(uint32 spellID)
{
    if (!spellID)
        return;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
    if (!spellInfo)
        return;

    bool inCombat = me->getVictim();

    // ignore some combinations of combat state and combat/noncombat spells
    if (!inCombat)
    {
        if (!IsPositiveSpell(spellInfo->Id))
            return;
    }
    else
    {
        if (IsNonCombatSpell(spellInfo))
            return;
    }
/*
    if (m_owner && m_owner->GetTypeId() == TYPEID_PLAYER)
        if(((Player*)m_owner)->HasSpellCooldown(spellID))
            return;
*/
    Spell *spell = new Spell(m_creature, spellInfo, false, 0);

    if (inCombat && !m_creature->hasUnitState(UNIT_STAT_FOLLOW) && spell->CanAutoCast(m_creature->getVictim()))
    {
        m_targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(m_creature->getVictim(), spell));
        return;
    }
    else
    {

        bool spellUsed = false;
        for (std::set<uint64>::iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
        {
            Unit* Target = m_creature->GetMap()->GetUnit(*tar);

            //only buff targets that are in combat, unless the spell can only be cast while out of combat
            if (!Target)
                continue;

            if (spell->CanAutoCast(Target))
            {
                m_targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(Target, spell));
                spellUsed = true;
                break;
            }
        }
        if (!spellUsed)
            delete spell;
    }
}

void PetAI::AddSpellForAutocast(uint32 spellID, Unit* target)
{
    if (!spellID)
        return;
/*
    if (m_owner && m_owner->GetTypeId() == TYPEID_PLAYER)
        if(((Player*)m_owner)->HasSpellCooldown(spellID))
            return;
*/
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
    if (!spellInfo)
        return;

    Spell *spell = new Spell(m_creature, spellInfo, false, 0);
    if (spell->CanAutoCast(target))
        m_targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(target, spell));
    else
        delete spell;
}

void PetAI::AutocastPreparedSpells()
{
    //found units to cast on to
    if (!m_targetSpellStore.empty())
    {
        uint32 index = urand(0, m_targetSpellStore.size() - 1);

        Spell* spell  = m_targetSpellStore[index].second;
        Unit*  target = m_targetSpellStore[index].first;

        m_targetSpellStore.erase(m_targetSpellStore.begin() + index);

        SpellCastTargets targets;
        targets.setUnitTarget(target);

        if (!m_creature->HasInArc(M_PI, target))
        {
            m_creature->SetInFront(target);
            if (target->GetTypeId() == TYPEID_PLAYER)
                m_creature->SendCreateUpdateToPlayer((Player*)target);

            if (m_owner && m_owner->GetTypeId() == TYPEID_PLAYER)
                m_creature->SendCreateUpdateToPlayer((Player*)m_owner);
        }

        m_creature->AddCreatureSpellCooldown(spell->m_spellInfo->Id);

        if (m_creature->isPet())
            ((Pet*)m_creature)->CheckLearning(spell->m_spellInfo->Id);

        spell->prepare(&targets);
    }

    while (!m_targetSpellStore.empty())
    {
        Spell *temp = m_targetSpellStore.begin()->second;
        m_targetSpellStore.erase(m_targetSpellStore.begin());
        delete temp;
    }
}

void PetAI::UpdateAI(const uint32 diff)
{
    m_owner = m_creature->GetCharmerOrOwner();

    // quest support - Razorthorn Ravager, switch to ScriptedAI when charmed and not in combat
    if (m_creature->GetEntry() == 24922 && m_creature->isCharmed() && !me->isInCombat())
        m_creature->NeedChangeAI = true;

    if (m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    if (m_forceTimer)
    {
        if (m_forceTimer < diff)
            m_forceTimer = 0;
        else
            m_forceTimer -= diff;
    }

   // m_creature->getVictim() can't be used for check in case stop fighting, m_creature->getVictim() clear at Unit death etc.
    if (m_creature->getVictim())
    {
        if (_needToStop())
        {
            DEBUG_LOG("Pet AI stoped attacking [guid=%u]", m_creature->GetGUIDLow());
            _stopAttack();
            return;
        }

        DoMeleeAttackIfReady();
    }
    else
    {
        if (me->isInCombat())
           _stopAttack();
        else if (m_owner && m_creature->GetCharmInfo()) //no victim
        {
            if (m_owner->isInCombat() && !(m_creature->HasReactState(REACT_PASSIVE) || m_creature->GetCharmInfo()->HasCommandState(COMMAND_STAY)))
                AttackStart(m_owner->getAttackerForHelper());
            else if (m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW) && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
                m_creature->GetMotionMaster()->MoveFollow(m_owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
        }
    }

    if (!me->GetCharmInfo())
        return;

    if (!m_creature->hasUnitState(UNIT_STAT_CASTING))
    {
        //Autocast
        for (uint8 i = 0; i < m_creature->GetPetAutoSpellSize(); i++)
            PrepareSpellForAutocast(m_creature->GetPetAutoSpellOnPos(i));

        AutocastPreparedSpells();
    }
}

void PetAI::UpdateAllies()
{
    Group *pGroup = NULL;

    m_updateAlliesTimer = 10000;                            //update friendly targets every 10 seconds, lesser checks increase performance

    if (!m_owner)
        return;
    else if (m_owner->GetTypeId() == TYPEID_PLAYER)
        pGroup = ((Player*)m_owner)->GetGroup();

    //only pet and owner/not in group->ok
    if (m_AllySet.size() == 2 && !pGroup)
        return;
    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if (pGroup && !pGroup->isRaidGroup() && m_AllySet.size() == (pGroup->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(m_creature->GetGUID());
    if (pGroup)                                              //add group
    {
        for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if (!Target || !pGroup->SameSubGroup((Player*)m_owner, Target))
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(m_owner->GetGUID());
}

int ImpAI::Permissible(const Creature *creature)
{
    if (creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

void ImpAI::AttackStart(Unit *victim)
{
    if (!victim)
        return;

    if (me->Attack(victim, true))
    {
        //DEBUG_LOG("Creature %s tagged a victim to kill [guid=%u]", me->GetName(), victim->GetGUIDLow());
        me->GetMotionMaster()->MoveChase(victim, 0, 0);
        m_chasing = true;
    }
}

void ImpAI::UpdateAI(const uint32 diff)
{
    if (!m_creature->isAlive())
        return;

    m_owner = m_creature->GetCharmerOrOwner();

    if (m_updateAlliesTimer <= diff)
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    if (m_forceTimer)
    {
        if (m_forceTimer < diff)
            m_forceTimer = 0;
        else
            m_forceTimer -= diff;
    }

   // m_creature->getVictim() can't be used for check in case stop fighting, m_creature->getVictim() clear at Unit death etc.
    if (Unit *target = m_creature->getVictim())
    {
        if (_needToStop())
        {
            DEBUG_LOG("Pet AI stoped attacking [guid=%u]", m_creature->GetGUIDLow());
            _stopAttack();
            return;
        }
        float dist = m_creature->GetDistance2d(target);
        if (dist < 30 && m_chasing)
        {
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveIdle();
            m_chasing = false;
        }
        if (dist > 30 && !m_chasing)
        {
            m_creature->GetMotionMaster()->MoveChase(target);
            m_chasing = true;
        }
    }
    else
    {
        if (me->isInCombat())
           _stopAttack();
        else if (m_owner && m_creature->GetCharmInfo()) //no victim
        {
            if (m_owner->isInCombat() && !(m_creature->HasReactState(REACT_PASSIVE) || m_creature->GetCharmInfo()->HasCommandState(COMMAND_STAY)))
                AttackStart(m_owner->getAttackerForHelper());
            else if (m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW) && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
                m_creature->GetMotionMaster()->MoveFollow(m_owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
        }
    }

    if (!me->GetCharmInfo())
        return;

    if (!m_creature->hasUnitState(UNIT_STAT_CASTING))
    {
        //Autocast
        for (uint8 i = 0; i < m_creature->GetPetAutoSpellSize(); i++)
            PrepareSpellForAutocast(m_creature->GetPetAutoSpellOnPos(i));

        AutocastPreparedSpells();
    }
}

int FelhunterAI::Permissible(const Creature *creature)
{
    if (creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

void FelhunterAI::PrepareSpellForAutocast(uint32 spellID)
{
    if (!spellID)
        return;

    if (spellmgr.GetFirstSpellInChain(spellID) == 19505) // Devour Magic
    {
        SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
        Unit *target = m_creature->getVictim();
        if (!spellInfo || !target)
            return;
        Unit::AuraMap const& auras = target->GetAuras();
        for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            Aura *aur = (*itr).second;
            if (aur && aur->GetSpellProto()->Dispel == DISPEL_MAGIC)
            {
                if (aur->IsPositive())
                {
                    AddSpellForAutocast(spellID, target);
                    return;
                }
            }
        }
    }
    else
        PetAI::PrepareSpellForAutocast(spellID);
}
