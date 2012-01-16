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
SDName: Boss_Noth
SD%Complete: 40
SDComment: Missing Balcony stage !
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SOUND_DEATH         8848
#define PLAGUED_WARRIOR_ID  16984

#define BALCONY_LOC 2631.370, -3529.680, 274.040, 6.277

enum NothTexts
{
    SAY_AGGRO1              = -1533075,
    SAY_AGGRO2              = -1533076,
    SAY_AGGRO3              = -1533077,
    SAY_SUMMON              = -1533078,
    SAY_SLAY1               = -1533079,
    SAY_SLAY2               = -1533080,
    SAY_DEATH               = -1533081
};

enum NothSpells
{
    SPELL_BLINK                     = 29211,    //29208, 29209 and 29210 too
    SPELL_CRIPPLE                   = 29212,
    SPELL_CURSE_PLAGUEBRINGER       = 29213
};

enum NothEvents
{
    EVENT_BLINK             = 1,
    EVENT_CURSE             = 2,
    EVENT_SKELETONS         = 3
};

struct TRINITY_DLL_DECL boss_nothAI : public BossAI
{
    boss_nothAI(Creature *c) : BossAI(c, DATA_NOTH_THE_PLAGUEBRINGER) { }

    uint32 Blink_Timer;
    uint32 Curse_Timer;
    uint32 Summon_Timer;

    void Reset()
    {
        events.Reset();
        events.ScheduleEvent(EVENT_BLINK, 25000);
        events.ScheduleEvent(EVENT_CURSE, 4000);
        events.ScheduleEvent(EVENT_SKELETONS, 12000);

        instance->SetData(DATA_NOTH_THE_PLAGUEBRINGER, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), m_creature);
        instance->SetData(DATA_NOTH_THE_PLAGUEBRINGER, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), m_creature);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            summoned->AI()->AttackStart(target);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        instance->SetData(DATA_NOTH_THE_PLAGUEBRINGER, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoSpecialThings(diff, DO_EVERYTHING);

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BLINK:
                {
                    AddSpellToCast(SPELL_CRIPPLE, CAST_NULL);
                    AddSpellToCast(SPELL_BLINK, CAST_SELF);
                    events.ScheduleEvent(EVENT_BLINK, 25000);
                    break;
                }
                case EVENT_CURSE:
                {
                    AddSpellToCast(SPELL_CURSE_PLAGUEBRINGER, CAST_NULL);
                    events.ScheduleEvent(EVENT_CURSE, urand(10000, 20000));
                    break;
                }
                case EVENT_SKELETONS:
                {
                    DoScriptText(SAY_SUMMON, m_creature);
                    Creature * tmpCreature;
                    Unit * tmpUnit;

                    for (uint8 i = 0; i < 6; ++i)
                    {
                        tmpCreature = m_creature->SummonCreature(PLAGUED_WARRIOR_ID, 2684.804, -3502.517, 261.313, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
                        tmpUnit = SelectUnit(SELECT_TARGET_RANDOM);
                        if (tmpCreature && tmpUnit)
                            tmpCreature->AI()->AttackStart(tmpUnit);
                    }

                    events.ScheduleEvent(EVENT_SKELETONS, 30000);
                    break;
                }
                default:
                    break;
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_noth(Creature *_Creature)
{
    return new boss_nothAI (_Creature);
}

void AddSC_boss_noth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_noth";
    newscript->GetAI = &GetAI_boss_noth;
    newscript->RegisterSelf();
}
