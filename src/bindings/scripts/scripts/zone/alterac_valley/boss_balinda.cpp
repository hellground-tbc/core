/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Balinda
SD%Complete: 
SDComment: Timers should be adjusted
EndScriptData */

#include "precompiled.h"

#define YELL_AGGRO                        -2100019
#define YELL_EVADE                        -2100020

#define SPELL_ARCANE_EXPLOSION            46608
#define SPELL_CONE_OF_COLD                38384
#define SPELL_FIREBALL                    46988
#define SPELL_FROSTBOLT                   46987
#define SPELL_WATER_ELEMENTAL             45067

#define SPELL_ICE_BLOCK                   46604
#define SPELL_HYPOTHERMIA                 41425


struct TRINITY_DLL_DECL boss_balindaAI : public ScriptedAI
{
    boss_balindaAI(Creature *c) : ScriptedAI(c) 
    {
        m_creature->GetPosition(wLoc);
    }

    uint32 CoCTimer;
    uint32 CheckTimer;
    uint32 IceBlockTimer;
    uint32 WaterElementalTimer;
    uint32 CastTimer;
    uint32 SpellId;
    WorldLocation wLoc;


    void Reset()
    {
        IceBlockTimer       = 10000;
        CoCTimer            = 8000;
        CheckTimer          = 2000;
        CastTimer            = 0;
        SpellId             = 0;
        WaterElementalTimer = 0;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustRespawned()
    {
        InCombat = false;
        Reset();
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void DoMeleeAttackIfReady()
    {
        if(me->hasUnitState(UNIT_STAT_CASTING))
            return;

        // if SpellId then cast spell and has mana
        if(SpellId && m_creature->GetPower(POWER_MANA) >= 800)
            DoCast(m_creature->getVictim(), SpellId);
        else // if not selected SpellId try to use melee attack
        {
            //Make sure our attack is ready and we aren't currently casting before checking distance
            if (me->isAttackReady())
            {
                //If we are within range melee the target
                if (me->IsWithinMeleeRange(me->getVictim()))
                {
                    me->AttackerStateUpdate(me->getVictim());
                    me->resetAttackTimer();
                }
            }
            if (me->haveOffhandWeapon() && me->isAttackReady(OFF_ATTACK))
            {
                //If we are within range melee the target
                if (me->IsWithinMeleeRange(me->getVictim()))
                {
                    me->AttackerStateUpdate(me->getVictim(), OFF_ATTACK);
                    me->resetAttackTimer(OFF_ATTACK);
                }
            }
        }
}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            if(m_creature->GetDistance(wLoc.x,wLoc.y,wLoc.z) >= 20.0f)
            {
                m_creature->InterruptNonMeleeSpells(false);
                EnterEvadeMode();
                return;
            }
            CheckTimer = 2000;
        }
        else CheckTimer -= diff;

        if(WaterElementalTimer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_WATER_ELEMENTAL);
            WaterElementalTimer = 90000; // 90s
        }
        else WaterElementalTimer -= diff;

        if(IceBlockTimer < diff)
        {
            if(!m_creature->HasAura(SPELL_HYPOTHERMIA, 0))
            {
                uint32 negativeAuras = m_creature->GetAurasAmountByType(SPELL_AURA_PERIODIC_DAMAGE) + m_creature->GetAurasAmountByType(SPELL_AURA_PERIODIC_LEECH);
                // cast if no hypothermia && has 3 or more dot/leech auras
                if(negativeAuras >= 3)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature, SPELL_ICE_BLOCK);
                    DoCast(m_creature, SPELL_HYPOTHERMIA, true);
                    IceBlockTimer = 60000; // 60s
                }
            }
        }
        else IceBlockTimer -= diff;

        // update CoC timer
        if(CoCTimer > diff)
            CoCTimer -= diff;
        else CoCTimer = 0;

        // select spell
        if(CastTimer < diff)
        {
            // if victim is in range of 6.5 yards and there are 3 attackers cast explosion or CoC if ready
            if(m_creature->getAttackers().size() >= 3 && m_creature->IsWithinDistInMap(m_creature->getVictim(), 6.5f, false))
            {
                if(!CoCTimer)
                {
                    SpellId = SPELL_CONE_OF_COLD;
                    CoCTimer = 8000 + rand()%4000;
                    CastTimer = 0;
                }
                else
                {
                    SpellId = SPELL_ARCANE_EXPLOSION;
                    CastTimer = 2000;
                }
            }
            else
            {
                // if victim out of range cast frostbolt or fireball
                SpellId = (rand()%2 == 0 ? SPELL_FROSTBOLT : SPELL_FIREBALL);
                CastTimer = 2500;
            }
        }
        else CastTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_balinda(Creature *_Creature)
{
    return new boss_balindaAI (_Creature);
}


// WATER ELEMENTAL

#define SPELL_WATER_BOLT                46983

struct TRINITY_DLL_DECL mob_av_water_elementalAI : public ScriptedAI
{
    mob_av_water_elementalAI(Creature *c) : ScriptedAI(c) {}

    void Reset() {}

    void Aggro(Unit *who) {}

    void JustRespawned()
    {
        InCombat = false;
        if(Unit *owner = m_creature->GetOwner())
        {
            m_creature->setFaction(owner->getFaction());
            m_creature->SetLevel(owner->getLevel());
            m_creature->SetMaxHealth(6300 + (m_creature->getLevel() - 60)*360);
            m_creature->SetPower(POWER_MANA, 6000 + (m_creature->getLevel() - 60)*300);
        }
        Reset();
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void DoMeleeAttackIfReady()
    {
        if(!me->hasUnitState(UNIT_STAT_CASTING))
            DoCast(m_creature->getVictim(), SPELL_WATER_BOLT);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_av_water_elemental(Creature *_Creature)
{
    return new mob_av_water_elementalAI (_Creature);
}


void AddSC_boss_balinda()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_balinda";
    newscript->GetAI = &GetAI_boss_balinda;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_av_water_elemental";
    newscript->GetAI = &GetAI_mob_av_water_elemental;
    newscript->RegisterSelf();
}
