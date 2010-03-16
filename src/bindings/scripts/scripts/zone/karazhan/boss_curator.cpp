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
SDName: Boss_Curator
SD%Complete: 100
SDComment: Evocation may cause client crash (Core related)
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"

#define SAY_AGGRO                       -1532057
#define SAY_SUMMON1                     -1532058
#define SAY_SUMMON2                     -1532059
#define SAY_EVOCATE                     -1532060
#define SAY_ENRAGE                      -1532061
#define SAY_KILL1                       -1532062
#define SAY_KILL2                       -1532063
#define SAY_DEATH                       -1532064

//Flare spell info
#define SPELL_ASTRAL_FLARE_PASSIVE      30234               //Visual effect + Flare damage
#define SPELL_ASTRAL_FLARE_NE           30236
#define SPELL_ASTRAL_FLARE_NW           30239
#define SPELL_ASTRAL_FLARE_SE           30240
#define SPELL_ASTRAL_FLARE_SW           30241

//Curator spell info
#define SPELL_HATEFUL_BOLT              30383
#define SPELL_EVOCATION                 30254
#define SPELL_ENRAGE                    30403               //Arcane Infusion: Transforms Curator and adds damage.
#define SPELL_BERSERK                   26662

struct TRINITY_DLL_DECL boss_curatorAI : public ScriptedAI
{
    boss_curatorAI(Creature *c) : ScriptedAI(c) 
    {
        m_creature->GetPosition(wLoc);
    }

    uint32 AddTimer;
    uint32 HatefulBoltTimer;
    uint32 BerserkTimer;
    uint32 CheckTimer;

    WorldLocation wLoc;

    bool Enraged;
    bool Evocating;

    void Reset()
    {
        AddTimer = 10000;
        HatefulBoltTimer = 15000;                           //This time may be wrong
        BerserkTimer = 720000;                              //12 minutes
        CheckTimer = 3000;
        Enraged = false;
        Evocating = false;
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        m_creature->ApplySpellImmune(2, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
        m_creature->ApplySpellImmune(3, IMMUNITY_STATE, SPELL_AURA_PERIODIC_LEECH, true);
        m_creature->ApplySpellImmune(4, IMMUNITY_STATE, SPELL_AURA_PERIODIC_MANA_LEECH, true);
        m_creature->ApplySpellImmune(5, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(6, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_KILL1, m_creature); break;
        case 1: DoScriptText(SAY_KILL2, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() )
            return;

        if(CheckTimer < diff)
        {
            if(m_creature->GetDistance(wLoc.x,wLoc.y,wLoc.z) > 135.0f)
                EnterEvadeMode();
            else
                DoZoneInCombat();
            
            CheckTimer = 3000;
        }else CheckTimer -= diff;

        if (Evocating && !m_creature->HasAura(SPELL_EVOCATION, 0))
            Evocating = false;

        if (m_creature->GetPower(POWER_MANA) <= 1000 && !Evocating)
        {
            DoScriptText(SAY_EVOCATE, m_creature);
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_EVOCATION);
            Evocating = true;
        }

        if (!Enraged && !Evocating)
        {
            if (AddTimer < diff)
            {
                //Summon Astral Flare
                Creature* AstralFlare = DoSpawnCreature(17096, rand()%37, rand()%37, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                Unit* target = NULL;
                target = SelectUnit(SELECT_TARGET_RANDOM, 0);

                if (AstralFlare && target)
                {
                    AstralFlare->CastSpell(AstralFlare, SPELL_ASTRAL_FLARE_PASSIVE, false);
                    AstralFlare->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                    AstralFlare->AI()->AttackStart(target);
                }

                //Reduce Mana by 10%
                int32 mana = (int32)(0.1f*(m_creature->GetMaxPower(POWER_MANA)));
                m_creature->ModifyPower(POWER_MANA, -mana);
                switch(rand()%4)
                {
                    case 0: DoScriptText(SAY_SUMMON1, m_creature);break;
                    case 1: DoScriptText(SAY_SUMMON2, m_creature);break;
                }
                AddTimer = 10000;
            }else AddTimer -= diff;

            if (HatefulBoltTimer < diff)
            {
                Unit* target = NULL;
                target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1);
                if(target)
                    DoCast(target, SPELL_HATEFUL_BOLT);

                HatefulBoltTimer = (Enraged) ? 7000 : 15000;
            }else HatefulBoltTimer -= diff;

            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 15)
            {
                Enraged = true;
                DoCast(m_creature, SPELL_ENRAGE);
                DoScriptText(SAY_ENRAGE, m_creature);
            }
        }

        if (BerserkTimer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            DoScriptText(SAY_ENRAGE, m_creature);
        }else BerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_curator(Creature *_Creature)
{
    return new boss_curatorAI (_Creature);
}

void AddSC_boss_curator()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_curator";
    newscript->GetAI = &GetAI_boss_curator;
    newscript->RegisterSelf();
}

