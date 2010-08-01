/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * Copyright (C) 2008-2009 Trinity <http://www.trinitycore.org/>
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
#include "Database/DBCStores.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "World.h"
#include "Util.h"

int PetAI::Permissible(const Creature *creature)
{
    if( creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature *c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK), m_forceTimer(0)
{
    m_AllySet.clear();
    UpdateAllies();

    m_owner = m_creature->GetCharmerOrOwner();
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
    if(m_creature->isCharmed() && m_creature->getVictim() == m_creature->GetCharmer())
        return true;

    return !m_creature->canAttack(m_creature->getVictim()) || targetHasInterruptableAura(m_creature->getVictim());
}

void PetAI::_stopAttack()
{
    if( !m_creature->isAlive() )
    {
        DEBUG_LOG("Creature stoped attacking cuz his dead [guid=%u]", m_creature->GetGUIDLow());
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->CombatStop();
        m_creature->getHostilRefManager().deleteReferences();

        return;
    }

    m_creature->AttackStop();
    me->GetCharmInfo()->SetIsCommandAttack(false);
    HandleReturnMovement();

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

    Spell *spell = new Spell(m_creature, spellInfo, false, 0);

    if(inCombat && !m_creature->hasUnitState(UNIT_STAT_FOLLOW) && spell->CanAutoCast(m_creature->getVictim()))
    {
        m_targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(m_creature->getVictim(), spell));
        return;
    }
    else
    {

        bool spellUsed = false;
        for(std::set<uint64>::iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
        {
            Unit* Target = ObjectAccessor::GetUnit(*m_creature,*tar);

            //only buff targets that are in combat, unless the spell can only be cast while out of combat
            if(!Target)
                continue;

            if(spell->CanAutoCast(Target))
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

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
    if (!spellInfo)
        return;    

    Spell *spell = new Spell(m_creature, spellInfo, false, 0);
    if(spell->CanAutoCast(target))
        m_targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(target, spell));
    else
        delete spell;
}

void PetAI::AutocastPreparedSpells()
{
    //found units to cast on to
    if(!m_targetSpellStore.empty())
    {
        uint32 index = urand(0, m_targetSpellStore.size() - 1);

        Spell* spell  = m_targetSpellStore[index].second;
        Unit*  target = m_targetSpellStore[index].first;

        m_targetSpellStore.erase(m_targetSpellStore.begin() + index);

        SpellCastTargets targets;
        targets.setUnitTarget( target );

        if(!m_creature->HasInArc(M_PI, target))
        {
            m_creature->SetInFront(target);
            if(target && target->GetTypeId() == TYPEID_PLAYER )
                m_creature->SendCreateUpdateToPlayer( (Player*)target );

            if(m_owner && m_owner->GetTypeId() == TYPEID_PLAYER)
                m_creature->SendCreateUpdateToPlayer( (Player*)m_owner );
        }

        m_creature->AddCreatureSpellCooldown(spell->m_spellInfo->Id);

        if(m_creature->isPet())
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

    if (m_forceTimer)
    {
        if (m_forceTimer < diff)
            m_forceTimer = 0;
        else
            m_forceTimer -= diff;
    }

    if(m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    // m_creature->getVictim() can't be used for check in case stop fighting, m_creature->getVictim() clear at Unit death etc.
    if( m_creature->getVictim() )
    {
        if( _needToStop() )
        {
            DEBUG_LOG("Pet AI stoped attacking [guid=%u]", m_creature->GetGUIDLow());
            _stopAttack();
            return;
        }

        DoMeleeAttackIfReady();
    }
    else if(m_owner && m_creature->GetCharmInfo()) //no victim
    {
        Unit *nextTarget = SelectNextTarget();

        if (nextTarget)
            AttackStart(nextTarget);
        else
            HandleReturnMovement();
    }
    else if (m_owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW)) // no charm info and no victim
        m_creature->GetMotionMaster()->MoveFollow(m_owner,PET_FOLLOW_DIST, m_creature->GetFollowAngle());

    if(!me->GetCharmInfo())
        return;

    bool inCombat = me->getVictim();

    // Autocast (casted only in combat or persistent spells in any state)
    if (m_creature->GetGlobalCooldown() == 0 && !m_creature->hasUnitState(UNIT_STAT_CASTING))
    {
        //Autocast
        for (uint8 i = 0; i < m_creature->GetPetAutoSpellSize(); i++)
            PrepareSpellForAutocast(m_creature->GetPetAutoSpellOnPos(i));

        AutocastPreparedSpells();
    }
}

void PetAI::UpdateAllies()
{
    Unit* owner = m_creature->GetCharmerOrOwner();
    Group *pGroup = NULL;

    m_updateAlliesTimer = 10*IN_MILISECONDS;                //update friendly targets every 10 seconds, lesser checks increase performance

    if(!owner)
        return;
    else if(owner->GetTypeId() == TYPEID_PLAYER)
        pGroup = ((Player*)owner)->GetGroup();

    //only pet and owner/not in group->ok
    if(m_AllySet.size() == 2 && !pGroup)
        return;
    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if(pGroup && !pGroup->isRaidGroup() && m_AllySet.size() == (pGroup->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(m_creature->GetGUID());
    if(pGroup)                                              //add group
    {
        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if(!Target || !pGroup->SameSubGroup((Player*)owner, Target))
                continue;

            if(Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

void PetAI::KilledUnit(Unit *victim)
{
    // Called from Unit::Kill() in case where pet or owner kills something
    // if owner killed this victim, pet may still be attacking something else
    if (me->getVictim() && me->getVictim() != victim)
        return;

    // Clear target just in case. May help problem where health / focus / mana
    // regen gets stuck. Also resets attack command.
    // Can't use _stopAttack() because that activates movement handlers and ignores
    // next target selection
    me->AttackStop();
    me->GetCharmInfo()->SetIsCommandAttack(false);

    Unit *nextTarget = SelectNextTarget();

    if (nextTarget)
        AttackStart(nextTarget);
    else
        HandleReturnMovement(); // Return
}

void PetAI::AttackStart(Unit *target)
{
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!_CanAttack(target))
        return;

    m_forceTimer = 5000;

    // We can attack, should we chase or not?
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        DoAttack(target,true); // FOLLOW, attack with chase
    else
    {
        if (me->GetCharmInfo()->IsCommandAttack())
            DoAttack(target,true); // STAY or FOLLOW, player clicked "attack" so attack with chase
        else
            DoAttack(target,false); // STAY, target in range, attack not clicked so attack without chase
    }
}

Unit *PetAI::SelectNextTarget()
{
    // Provides next target selection after current target death

    // Passive pets don't do next target selection
    if (me->HasReactState(REACT_PASSIVE))
        return NULL;

    // Check pet's attackers first to prevent dragging mobs back
    // to owner
    if (me->getAttackerForHelper())
        return me->getAttackerForHelper();

    // Check owner's attackers if pet didn't have any
    if (me->GetCharmerOrOwner()->getAttackerForHelper())
        return me->GetCharmerOrOwner()->getAttackerForHelper();

    // Default
    return NULL;
}

void PetAI::HandleReturnMovement()
{
    // Handles moving the pet back to stay or owner

    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
    {
        if (!me->GetCharmInfo()->IsAtStay() && !me->GetCharmInfo()->IsReturning())
        {
            // Return to previous position where stay was clicked
            if (!me->GetCharmInfo()->IsCommandAttack())
            {
                float x,y,z;

                me->GetCharmInfo()->GetStayPosition(x, y, z);
                me->GetCharmInfo()->SetIsReturning(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(me->GetGUIDLow(),x,y,z);
            }
        }
    }
    else // COMMAND_FOLLOW
    {
        if (!me->GetCharmInfo()->IsFollowing() && !me->GetCharmInfo()->IsReturning())
        {
            if (!me->GetCharmInfo()->IsCommandAttack())
            {
                me->GetCharmInfo()->SetIsReturning(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveFollow(me->GetCharmerOrOwner(), PET_FOLLOW_DIST, me->GetFollowAngle());
            }
        }
    }

}

void PetAI::DoAttack(Unit *target, bool chase)
{
    // Handles attack with or without chase and also resets all
    // PetAI flags for next update / creature kill

    // me->GetCharmInfo()->SetIsCommandAttack(false);

    // The following conditions are true if chase == true
    // (Follow && (Aggressive || Defensive))
    // ((Stay || Follow) && (Passive && player clicked attack))

    if (chase)
    {
        if (me->Attack(target,true))
        {
            me->GetCharmInfo()->SetIsAtStay(false);
            me->GetCharmInfo()->SetIsFollowing(false);
            me->GetCharmInfo()->SetIsReturning(false);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target);
        }
    }
    else // (Stay && ((Aggressive || Defensive) && In Melee Range)))
    {
        me->GetCharmInfo()->SetIsAtStay(true);
        me->GetCharmInfo()->SetIsFollowing(false);
        me->GetCharmInfo()->SetIsReturning(false);
        me->Attack(target,true);
    }
}

void PetAI::MovementInform(uint32 moveType, uint32 data)
{
    // Receives notification when pet reaches stay or follow owner
    switch (moveType)
    {
        case POINT_MOTION_TYPE:
        {
            // Pet is returning to where stay was clicked. data should be
            // pet's GUIDLow since we set that as the waypoint ID
            if (data == me->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                me->GetCharmInfo()->SetIsAtStay(true);
                me->GetCharmInfo()->SetIsReturning(false);
                me->GetCharmInfo()->SetIsFollowing(false);
                me->GetCharmInfo()->SetIsCommandAttack(false);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
            }
        }
        break;

        case TARGETED_MOTION_TYPE:
        {
            // If data is owner's GUIDLow then we've reached follow point,
            // otherwise we're probably chasing a creature
            if (me->GetCharmerOrOwner() && me->GetCharmInfo() && data == me->GetCharmerOrOwner()->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                me->GetCharmInfo()->SetIsAtStay(false);
                me->GetCharmInfo()->SetIsReturning(false);
                me->GetCharmInfo()->SetIsFollowing(true);
                me->GetCharmInfo()->SetIsCommandAttack(false);
                me->addUnitState(UNIT_STAT_FOLLOW);
            }
        }
        break;

        default:
            break;
    }
}

bool PetAI::_CanAttack(Unit *target)
{
    // Evaluates wether a pet can attack a specific
    // target based on CommandState, ReactState and other flags

    // Returning - check first since pets returning ignore attacks
    if (me->GetCharmInfo()->IsReturning())
        return false;

    // Passive - check now so we don't have to worry about passive in later checks
    if (me->HasReactState(REACT_PASSIVE))
        return me->GetCharmInfo()->IsCommandAttack();

    //  Pets commanded to attack should not stop their approach if attacked by another creature
    if (me->getVictim() && (me->getVictim() != target))
        return !me->GetCharmInfo()->IsCommandAttack();

    // From this point on, pet will always be either aggressive or defensive

    // Stay - can attack if target is within range or commanded to
    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY))
        return (me->IsWithinMeleeRange(target, MIN_MELEE_REACH) || me->GetCharmInfo()->IsCommandAttack());

    // Follow
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        return true;

    // default, though we shouldn't ever get here
    return false;
}
int ImpAI::Permissible(const Creature *creature)
{
    if( creature->isPet())
        return PERMIT_BASE_SPECIAL;
 
    return PERMIT_BASE_NO;
}
 
void ImpAI::AttackStart(Unit *victim)
{
    if(!victim)
         return;
 
    if(me->Attack(victim, true))
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
 
    if(m_updateAlliesTimer <= diff)
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
    if( Unit *target = m_creature->getVictim() )
    {
        if( _needToStop() )
        {
            DEBUG_LOG("Pet AI stoped attacking [guid=%u]", m_creature->GetGUIDLow());
            _stopAttack();
            return;
        }
        float dist = m_creature->GetDistance2d(target);
        if(dist < 30 && m_chasing)
        {
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_chasing = false;
        }
        if(dist > 30 && !m_chasing)
        {
            m_creature->GetMotionMaster()->MoveChase(target);
            m_chasing = true;
        }
    }
    else
    {
        if(me->isInCombat())
           _stopAttack();
        else if(m_owner && m_creature->GetCharmInfo()) //no victim
        {
            if(m_owner->isInCombat() && !(m_creature->HasReactState(REACT_PASSIVE) || m_creature->GetCharmInfo()->HasCommandState(COMMAND_STAY)))
                AttackStart(m_owner->getAttackerForHelper());
            else if(m_creature->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW) && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
                m_creature->GetMotionMaster()->MoveFollow(m_owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
        }
    }
 
    if(!me->GetCharmInfo())
        return;
 
    if (m_creature->GetGlobalCooldown() == 0 && !m_creature->hasUnitState(UNIT_STAT_CASTING))
    {
        //Autocast
        for (uint8 i = 0; i < m_creature->GetPetAutoSpellSize(); i++)
            PrepareSpellForAutocast(m_creature->GetPetAutoSpellOnPos(i));

        AutocastPreparedSpells();
    }
}
 
int FelhunterAI::Permissible(const Creature *creature)
{
    if( creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}
 
void FelhunterAI::PrepareSpellForAutocast(uint32 spellID)
{
    if(!spellID)
        return;

    if(spellmgr.GetFirstSpellInChain(spellID) == 19505) // Devour Magic
    {
        SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
        Unit *target = m_creature->getVictim();
        if (!spellInfo || !target)
            return;   
        Unit::AuraMap const& auras = target->GetAuras();
        for(Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            Aura *aur = (*itr).second;
            if (aur && aur->GetSpellProto()->Dispel == DISPEL_MAGIC)
            {
                bool positive = true;
                if (!aur->IsPositive())
                    positive = false;
                else
                    positive = (aur->GetSpellProto()->AttributesEx & SPELL_ATTR_EX_NEGATIVE)==0;
                if(positive)
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