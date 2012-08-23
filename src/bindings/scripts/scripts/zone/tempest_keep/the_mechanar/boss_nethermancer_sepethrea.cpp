/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 90
SDComment: Need adjustments to initial summons
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "precompiled.h"
#include "def_mechanar.h"

#define SAY_AGGRO                       -1554013
#define SAY_SUMMON                      -1554014
#define SAY_DRAGONS_BREATH_1            -1554015
#define SAY_DRAGONS_BREATH_2            -1554016
#define SAY_SLAY1                       -1554017
#define SAY_SLAY2                       -1554018
#define SAY_DEATH                       -1554019

#define SPELL_SUMMON_RAGIN_FLAMES       35275
#define H_SPELL_SUMMON_RAGIN_FLAMES     39084

#define SPELL_FROST_ATTACK              35263
#define SPELL_ARCANE_BLAST              35314
#define SPELL_DRAGONS_BREATH            35250
#define SPELL_KNOCKBACK                 37317
#define SPELL_SOLARBURN                 35267

struct HELLGROUND_DLL_DECL boss_nethermancer_sepethreaAI : public ScriptedAI
{
    boss_nethermancer_sepethreaAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    uint32 arcane_blast_Timer;
    uint32 dragons_breath_Timer;
    uint32 knockback_Timer;
    uint32 solarburn_Timer;

    SummonList summons;

    void Reset()
    {
        arcane_blast_Timer = urand(12000, 18000);
        dragons_breath_Timer = urand(18000, 22000);
        knockback_Timer = uarnd(22000, 28000);
        solarburn_Timer = 30000;

        pInstance->SetData(DATA_NETHERMANCER_EVENT, NOT_STARTED);

        summons.DespawnAll();
    }

    void EnterCombat(Unit *who)
    {
        pInstance->SetData(DATA_NETHERMANCER_EVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        AddSpellToCastWithScriptText(HeroicMode ? H_SPELL_SUMMON_RAGIN_FLAMES : SPELL_SUMMON_RAGIN_FLAMES, CAST_SELF, SAY_SUMMON);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        pInstance->SetData(DATA_NETHERMANCER_EVENT, DONE);
    }

    void JustSummoned(Creature * summoned)
    {
        if (summoned)
            summons.Summon(summoned);
    }

    void DamageMade(Unit * target, uint32 & damage, bool direct)
    {
        if (direct)
            m_creature->CastSpell(target, SPELL_FROST_ATTACK, true);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim() )
            return;

        //Arcane Blast
        if (arcane_blast_Timer < diff)
        {
            AddSpellToCast(SPELL_ARCANE_BLAST, CAST_TANK);
            arcane_blast_Timer = urand(12500, 17500);
        }
        else
            arcane_blast_Timer -= diff;

        //Dragons Breath
        if (dragons_breath_Timer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_DRAGONS_BREATH, CAST_TANK, RAND(SAY_DRAGONS_BREATH_1, SAY_DRAGONS_BREATH_2, 0, 0));
            dragons_breath_Timer = urand(12000 ,22000);
        }
        else
            dragons_breath_Timer -= diff;

        //Knockback
        if (knockback_Timer < diff)
        {
            AddSpellToCast(SPELL_KNOCKBACK, CAST_TANK);
            knockback_Timer = urand(15000, 25000);
        }
        else
            knockback_Timer -= diff;

        //Solarburn
        if (solarburn_Timer < diff)
        {
            AddSpellToCast(SPELL_SOLARBURN, CAST_RANDOM);
            solarburn_Timer = 30000;
        }
        else
            solarburn_Timer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nethermancer_sepethrea(Creature *_Creature)
{
    return new boss_nethermancer_sepethreaAI (_Creature);
}

#define SPELL_INFERNO                   35268
#define H_SPELL_INFERNO                 39346
#define SPELL_FIRE_TAIL                 35278

struct HELLGROUND_DLL_DECL mob_ragin_flamesAI : public ScriptedAI
{
    mob_ragin_flamesAI(Creature *c) : ScriptedAI(c) { }

    uint32 infernoTimer;
    uint64 currentTarget;

    void Reset()
    {
        infernoTimer = 10000;
        flameTimer = 500;
        prevTarget = 0;

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
        m_creature->SetSpeed(MOVE_RUN, heroic ? 0.7f : 0.5f);

        SetAutocast(SPELL_FIRE_TAIL, 500, false, CAST_SELF);
    }

    void ChangeTarget()
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0, 5000, true))
        {
            Unit * prevTarUnit = me->GetMap()->GetUnit(currentTarget);
            if (prevTarUnit)
                m_creature->AddThreat(prevTarUnit, -5000000.0f);

            currentTarget = target->GetGUID();
            me->AI()->AttackStart(target);
            me->AddThreat(target, 5000000.0f);
        }
    }

    void JustRespawned()
    {
        DoZoneInCombat();
        ChangeTarget();
        StartAutocast();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoSpecialThings(diff, DO_COMBAT_N_SPEED, 200.0f, HeroicMode ? 0.7f : 0.5f);

        if (inferno_Timer < diff)
        {
            AddSpellToCast(HeroicMode ? H_SPELL_INFERNO : SPELL_INFERNO, CAST_SELF);
            ChangeTarget();

            inferno_Timer = 10000;
        }
        else
            inferno_Timer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_ragin_flames(Creature *_Creature)
{
    return new mob_ragin_flamesAI (_Creature);
}

void AddSC_boss_nethermancer_sepethrea()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_nethermancer_sepethrea";
    newscript->GetAI = &GetAI_boss_nethermancer_sepethrea;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ragin_flames";
    newscript->GetAI = &GetAI_mob_ragin_flames;
    newscript->RegisterSelf();
}
