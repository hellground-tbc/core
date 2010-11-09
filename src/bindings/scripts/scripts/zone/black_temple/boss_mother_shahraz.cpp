/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Mother_Shahraz
SD%Complete: 80
SDComment: Saber Lash missing, Fatal Attraction slightly incorrect; need to damage only if affected players are within range of each other
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

//Speech'n'Sounds
#define SAY_TAUNT1              -1564018
#define SAY_TAUNT2              -1564019
#define SAY_TAUNT3              -1564020
#define SAY_AGGRO               -1564021
#define SAY_SPELL1              -1564022
#define SAY_SPELL2              -1564023
#define SAY_SPELL3              -1564024
#define SAY_SLAY1               -1564025
#define SAY_SLAY2               -1564026
#define SAY_ENRAGE              -1564027
#define SAY_DEATH               -1564028

//Spells
#define SPELL_BEAM_SINISTER     40859
#define SPELL_BEAM_VILE         40860
#define SPELL_BEAM_WICKED       40861
#define SPELL_BEAM_SINFUL       40827
#define SPELL_SILENCING_SHRIEK  40823
#define SPELL_ENRAGE            23537
#define SPELL_SABER_LASH        40810
#define SPELL_SABER_LASH_IMM    43690
#define SPELL_FATAL_ATTRACTION  40869
#define SPELL_BERSERK           45078

#define SPELL_PRISMATIC_SHIELD  40879
#define SPELL_SABER_LASH_AURA   40816

float positions[34][2] =
{
    {927.979, 181.61},
    {942.904, 181.996},
    {956.791, 180.219},
    {972.212, 186.743},
    {961.126, 195.864},
    {947.358, 192.311},
    {932.846, 191.636},
    {931.008, 202.818},
    {940.404, 206.899},
    {951.583, 205.207},
    {961.637, 207.041},
    {966.439, 219.553},
    {955.098, 218.286},
    {945.183, 220.835},
    {927.697, 220.758},
    {926.898, 231.017},
    {939.866, 233.547},
    {961.485, 234.489},
    {976.464, 240.407},
    {967.097, 248.814},
    {952.011, 251.172},
    {943.619, 250.804},
    {933.353, 250.352},
    {920.665, 247.924},
    {919.703, 262.54},
    {937.174, 262.04},
    {946.208, 258.092},
    {955.213, 262.29},
    {971.454, 261.922},
    {970.511, 271.813},
    {958.936, 272.276},
    {947.011, 271.858},
    {938.205, 271.412},
    {929.774, 262.897}
};

struct TRINITY_DLL_DECL boss_shahrazAI : public ScriptedAI
{
    boss_shahrazAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_creature->GetPosition(wLoc);
    }

    ScriptedInstance* pInstance;

    uint8 position;
    uint32 BeamTimer;
    uint32 BeamCount;
    uint32 CurrentBeam;
    uint32 ShriekTimer;
    uint32 SaberTimer;
    uint32 RandomYellTimer;
    uint32 EnrageTimer;
    uint32 FatalAttractionTimer;

    uint32 CheckTimer;
    WorldLocation wLoc;

    bool Enraged;

    void SpellHitTarget(Unit *pTarget, const SpellEntry *pSpell)
    {
        if (pSpell->Id == SPELL_FATAL_ATTRACTION)
        {
            float x = positions[position][0];
            float y = positions[position][1];
            float z = 192.82;

            ((Player *)pTarget)->TeleportTo(pTarget->GetMapId(), x, y, z, 0.0f, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
        }
    }

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, NOT_STARTED);

        BeamTimer = 20000; // Timers may be incorrect
        BeamCount = 0;
        CurrentBeam = 0;                                    // 0 - Sinister, 1 - Vile, 2 - Wicked, 3 - Sinful
        FatalAttractionTimer = 60000;
        ShriekTimer = 30000;
        SaberTimer = 35000;
        RandomYellTimer = urand(70000, 111000);
        EnrageTimer = 600000;

        CheckTimer = 1000;

        position = 0;

        Enraged = false;
    }

    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, IN_PROGRESS);

        m_creature->RemoveAurasDueToSpell(SPELL_PRISMATIC_SHIELD);
        m_creature->RemoveAurasDueToSpell(SPELL_SABER_LASH_AURA);
        ForceSpellCast(m_creature, SPELL_PRISMATIC_SHIELD, INTERRUPT_AND_CAST_INSTANTLY);
        ForceSpellCast(m_creature, SPELL_SABER_LASH_AURA, INTERRUPT_AND_CAST_INSTANTLY);

        DoZoneInCombat();
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), m_creature);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CheckTimer < diff)
        {
            DoZoneInCombat();
            me->SetSpeed(MOVE_RUN, 3.0);
            CheckTimer = 2000;
        }
        else
            CheckTimer -= diff;

        if (!Enraged && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 10))
        {
            Enraged = true;
            ForceSpellCastWithScriptText(m_creature, SPELL_ENRAGE, SAY_ENRAGE, INTERRUPT_AND_CAST_INSTANTLY);
        }

        //Randomly cast one beam.
        if (BeamTimer < diff)
        {
            Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true);
            if (!pTarget)
                return;

            BeamTimer = 9000;

            switch (CurrentBeam)
            {
                case 0:
                    AddSpellToCast(pTarget, SPELL_BEAM_SINISTER);
                    BeamCount++;
                    break;
                case 1:
                    AddSpellToCast(pTarget, SPELL_BEAM_VILE);
                    BeamCount++;
                    break;
                case 2:
                    AddSpellToCast(pTarget, SPELL_BEAM_WICKED);
                    BeamCount++;
                    break;
                case 3:
                    AddSpellToCast(pTarget, SPELL_BEAM_SINFUL);
                    BeamCount++;
                    break;
            }

            if (BeamCount > 3)
            {
                uint32 Beam = CurrentBeam;
                while (CurrentBeam == Beam)
                    CurrentBeam = urand(0, 3);

                BeamCount = 0;
            }
        }
        else
            BeamTimer -= diff;

        // Select 3 random targets (can select same target more than once), teleport to a random location then make them cast explosions until they get away from each other.
        if (FatalAttractionTimer < diff)
        {
            position = urand(0, 33);            
            ForceSpellCastWithScriptText(m_creature, SPELL_FATAL_ATTRACTION, RAND(SAY_SPELL2, SAY_SPELL3), INTERRUPT_AND_CAST_INSTANTLY);
            FatalAttractionTimer = urand(40000, 71000);
        }
        else
            FatalAttractionTimer -= diff;

        if (ShriekTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_SILENCING_SHRIEK);
            ShriekTimer = urand(25000, 126000);
        }
        else
            ShriekTimer -= diff;

        //Enrage
        if (EnrageTimer)
        {
            if (!m_creature->HasAura(SPELL_BERSERK, 0))
            {
                if (EnrageTimer <= diff)
                {
                    EnrageTimer = 0;
                    ForceSpellCastWithScriptText(m_creature, SPELL_BERSERK, SAY_ENRAGE, INTERRUPT_AND_CAST_INSTANTLY);
                }
                else
                    EnrageTimer -= diff;
            }
        }

        //Random taunts
        if (RandomYellTimer < diff)
        {
            DoScriptText(RAND(SAY_TAUNT1, SAY_TAUNT2, SAY_TAUNT3), m_creature);
            RandomYellTimer = urand(60000, 151000);
        }
        else
            RandomYellTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shahraz(Creature *_Creature)
{
    return new boss_shahrazAI (_Creature);
}

void AddSC_boss_mother_shahraz()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_mother_shahraz";
    newscript->GetAI = &GetAI_boss_shahraz;
    newscript->RegisterSelf();
}

