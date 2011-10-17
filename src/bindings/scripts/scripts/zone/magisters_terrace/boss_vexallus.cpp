/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Vexallus
SD%Complete: 90
SDComment: Heroic and Normal support. Needs further testing.
SDCategory: Magister's Terrace
EndScriptData */

#include "precompiled.h"
#include "def_magisters_terrace.h"

#define SAY_AGGRO                   -1585007
#define SAY_ENERGY                  -1585008
#define SAY_OVERLOAD                -1585009
#define SAY_KILL                    -1585010
#define EMOTE_DISCHARGE_ENERGY      -1585011
//is this text for real?
#define SAY_DEATH                   "What...happen...ed."

//Pure energy spell info
#define SPELL_ENERGY_BOLT_AURA          46156
#define SPELL_ENERGY_FEEDBACK           44335
#define SPELL_ENERGY_FEEDBACK_CHANNEL   44328
#define SPELL_ENERGY_PASSIVE            44326

//Vexallus spell info
#define SPELL_CHAIN_LIGHTNING           (HeroicMode?46380:44318)
#define SPELL_ARCANE_SHOCK              (HeroicMode?46381:44319)
#define SPELL_SUMMON_PURE_ENERGY        44322
#define SPELL_H_SUMMON_PURE_ENERGY1     46159
#define SPELL_H_SUMMON_PURE_ENERGY2     46154
#define SPELL_OVERLOAD                  44352

struct TRINITY_DLL_DECL boss_vexallusAI : public ScriptedAI
{
    boss_vexallusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint32 ChainLightningTimer;
    uint32 ArcaneShockTimer;
    uint32 SpawnAddInterval;
    uint32 AlreadySpawnedAmount;

    void Reset()
    {
        ChainLightningTimer = urand(12000, 20000);
        ArcaneShockTimer = urand(14000, 19000);
        SpawnAddInterval = 15;
        AlreadySpawnedAmount = 0;

        if(pInstance)
            pInstance->SetData(DATA_VEXALLUS_EVENT, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit *victim)
    {
        DoYell(SAY_DEATH, LANG_UNIVERSAL, NULL);
        if (pInstance)
            pInstance->SetData(DATA_VEXALLUS_EVENT, DONE);
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (pInstance)
            pInstance->SetData(DATA_VEXALLUS_EVENT, IN_PROGRESS);
    }

    void JustSummoned(Creature *c)
    {
        if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
        {
            c->AI()->AttackStart(target);
            c->AddThreat(target, 1000);
        }
        c->CastSpell(c, SPELL_ENERGY_PASSIVE, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if(!HealthBelowPct(11))
        {
            //used for check, when Vexallus cast adds 85%, 70%, 55%, 40%, 25%
            if(HealthBelowPct(100-(15*(AlreadySpawnedAmount+1))))
            {
                DoScriptText(SAY_ENERGY, m_creature);
                DoScriptText(EMOTE_DISCHARGE_ENERGY, m_creature);
                if(HeroicMode)
                {
                    AddSpellToCast(SPELL_H_SUMMON_PURE_ENERGY1, CAST_SELF);
                    AddSpellToCast(SPELL_H_SUMMON_PURE_ENERGY2, CAST_SELF);
                }
                else
                    AddSpellToCast(SPELL_SUMMON_PURE_ENERGY, CAST_SELF);

                // refresh timers each pure energy spawn
                ChainLightningTimer = urand(12000, 20000);
                ArcaneShockTimer = urand(14000, 19000);
                ++AlreadySpawnedAmount;
            };

            if(ChainLightningTimer < diff)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    AddSpellToCast(target, SPELL_CHAIN_LIGHTNING);
                ChainLightningTimer = urand(12000, 20000);
            }
            else
                ChainLightningTimer -= diff;

            if(ArcaneShockTimer < diff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    AddSpellToCast(target, SPELL_ARCANE_SHOCK);
                ArcaneShockTimer = urand(14000, 19000);
            }
            else
                ArcaneShockTimer -= diff;
        }
        else
            ForceSpellCast(SPELL_OVERLOAD, CAST_SELF, INTERRUPT_AND_CAST_INSTANTLY);

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vexallus(Creature *_Creature)
{
    return new boss_vexallusAI (_Creature);
};

struct TRINITY_DLL_DECL mob_pure_energyAI : public ScriptedAI
{
    mob_pure_energyAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        me->SetLevitate(true);
        me->SetSpeed(MOVE_FLIGHT, 0.5);   //to be tested
        DoCast(me, SPELL_ENERGY_BOLT_AURA);
        DoCast(me, SPELL_ENERGY_PASSIVE);
    }

    void JustDied(Unit* killer)
    {
        float x, y, z;
        me->GetPosition(x, y, z);
        if(Unit* Trigger = me->SummonTrigger(x, y, z, 0, 10000))
            Trigger->CastSpell(killer, SPELL_ENERGY_FEEDBACK_CHANNEL, false);
        killer->CastSpell(killer, SPELL_ENERGY_FEEDBACK, true, 0, 0, m_creature->GetGUID());
        me->RemoveCorpse();
    }

    void EnterCombat(Unit *who){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            me->Kill(me, false);
    }
};

CreatureAI* GetAI_mob_pure_energy(Creature *_Creature)
{
    return new mob_pure_energyAI (_Creature);
};

void AddSC_boss_vexallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_vexallus";
    newscript->GetAI = &GetAI_boss_vexallus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_pure_energy";
    newscript->GetAI = &GetAI_mob_pure_energy;
    newscript->RegisterSelf();
}

