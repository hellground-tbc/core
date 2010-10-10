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
SDName: Boss_Razuvious
SD%Complete: 50
SDComment: Missing adds and event is impossible without Mind Control
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Razuvious - NO TEXT sound only
//8852 aggro01 - Hah hah, I'm just getting warmed up!
//8853 aggro02 Stand and fight!
//8854 aggro03 Show me what you've got!
//8861 slay1 - You should've stayed home!
//8863 slay2-
//8858 cmmnd3 - You disappoint me, students!
//8855 cmmnd1 - Do as I taught you!
//8856 cmmnd2 - Show them no mercy!
//8859 cmmnd4 - The time for practice is over! Show me what you've learned!
//8861 Sweep the leg! Do you have a problem with that?
//8860 death - An honorable... death...
//8947 - Aggro Mixed? - ?

#define SOUND_AGGRO1    8852
#define SOUND_AGGRO2    8853
#define SOUND_AGGRO3    8854
#define SOUND_SLAY1     8861
#define SOUND_SLAY2     8863
#define SOUND_COMMND1   8855
#define SOUND_COMMND2   8856
#define SOUND_COMMND3   8858
#define SOUND_COMMND4   8859
#define SOUND_COMMND5   8861
#define SOUND_DEATH     8860
#define SOUND_AGGROMIX  8847

#define SPELL_UNBALANCINGSTRIKE     26613
#define SPELL_DISRUPTINGSHOUT       29107

struct TRINITY_DLL_DECL boss_razuviousAI : public ScriptedAI
{
    boss_razuviousAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance * pInstance;

    uint32 UnbalancingStrike_Timer;
    uint32 DisruptingShout_Timer;
    uint32 CommandSound_Timer;

    void Reset()
    {
        UnbalancingStrike_Timer = 30000;                    //30 seconds
        DisruptingShout_Timer = 25000;                      //25 seconds
        CommandSound_Timer = 40000;                         //40 seconds

        if (pInstance)
            pInstance->SetData(DATA_INSTRUCTOR_RAZUVIOUS, NOT_STARTED);
    }

    void KilledUnit(Unit* Victim)
    {
        if (rand()%3)
            return;

        DoPlaySoundToSet(m_creature, RAND(SOUND_SLAY1, SOUND_SLAY2));
    }

    void JustDied(Unit* Killer)
    {
        DoPlaySoundToSet(m_creature, SOUND_DEATH);
        if (pInstance)
            pInstance->SetData(DATA_INSTRUCTOR_RAZUVIOUS, DONE);
    }

    void EnterCombat(Unit *who)
    {
        DoPlaySoundToSet(m_creature, RAND(SOUND_AGGRO1, SOUND_AGGRO2, SOUND_AGGRO3));

        if (pInstance)
            pInstance->SetData(DATA_INSTRUCTOR_RAZUVIOUS, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //UnbalancingStrike_Timer
        if (UnbalancingStrike_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_UNBALANCINGSTRIKE);
            UnbalancingStrike_Timer = 30000;
        }
        else
            UnbalancingStrike_Timer -= diff;

        //DisruptingShout_Timer
        if (DisruptingShout_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_DISRUPTINGSHOUT);
            DisruptingShout_Timer = 25000;
        }
        else
            DisruptingShout_Timer -= diff;

        //CommandSound_Timer
        if (CommandSound_Timer < diff)
        {
            DoPlaySoundToSet(m_creature, RAND(SOUND_COMMND1, SOUND_COMMND2, SOUND_COMMND3, SOUND_COMMND4, SOUND_COMMND5));
            CommandSound_Timer = 40000;
        }
        else
            CommandSound_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_razuvious(Creature *_Creature)
{
    return new boss_razuviousAI (_Creature);
}

void AddSC_boss_razuvious()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_razuvious";
    newscript->GetAI = &GetAI_boss_razuvious;
    newscript->RegisterSelf();
}

