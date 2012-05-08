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
SDName: Boss_Vanndar
SD%Complete:
SDComment: Some spells listed on wowwiki but doesn't exist on wowhead
EndScriptData */

#include "precompiled.h"

#define YELL_AGGRO              -2100008

#define YELL_EVADE              -2100009
#define YELL_RESPAWN1           -2100010
#define YELL_RESPAWN2           -2100011

#define YELL_RANDOM1            -2100012
#define YELL_RANDOM2            -2100013
#define YELL_RANDOM3            -2100014
#define YELL_RANDOM4            -2100015
#define YELL_RANDOM5            -2100016
#define YELL_RANDOM6            -2100017
#define YELL_RANDOM7            -2100018


#define SPELL_AVATAR            19135
#define SPELL_THUNDERCLAP       15588
#define SPELL_STORMBOLT         20685 // not sure


struct HELLGROUND_DLL_DECL boss_vanndarAI : public ScriptedAI
{
    boss_vanndarAI(Creature *c) : ScriptedAI(c)
    {
        m_creature->GetPosition(wLoc);
    }

    uint32 AvatarTimer;
    uint32 ThunderclapTimer;
    uint32 StormboltTimer;
    uint32 YellTimer;
    uint32 CheckTimer;
    WorldLocation wLoc;

    void Reset()
    {
        AvatarTimer             = 3000;
        ThunderclapTimer        = 4000;
        StormboltTimer          = 6000;
        YellTimer               = urand(20000, 30000); //20 to 30 seconds
        CheckTimer              = 2000;
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustRespawned()
    {
        Reset();
        DoScriptText(RAND(YELL_RESPAWN1, YELL_RESPAWN2), m_creature);
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(&wLoc, 20.0f))
                EnterEvadeMode();

            CheckTimer = 2000;
        }
        else
            CheckTimer -= diff;

        if (AvatarTimer < diff)
        {
            ForceSpellCast(m_creature->getVictim(), SPELL_AVATAR);
            AvatarTimer = urand(15000, 20000);
        }
        else
            AvatarTimer -= diff;

        if (ThunderclapTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_THUNDERCLAP);
            ThunderclapTimer = urand(5000, 15000);
        }
        else
            ThunderclapTimer -= diff;

        if (StormboltTimer < diff)
        {
            Unit * victim = SelectUnit(SELECT_TARGET_RANDOM, 1, 30.0f, true);
            if (victim)
                AddSpellToCast(victim, SPELL_STORMBOLT);
            StormboltTimer = urand(10000, 25000);
        }
        else
            StormboltTimer -= diff;

        if (YellTimer < diff)
        {
            DoScriptText(RAND(YELL_RANDOM1, YELL_RANDOM2, YELL_RANDOM3, YELL_RANDOM4, YELL_RANDOM5, YELL_RANDOM6, YELL_RANDOM7), m_creature);
            YellTimer = urand(20000, 30000); //20 to 30 seconds
        }
        else
            YellTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vanndar(Creature *_Creature)
{
    return new boss_vanndarAI (_Creature);
}

void AddSC_boss_vanndar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_vanndar";
    newscript->GetAI = &GetAI_boss_vanndar;
    newscript->RegisterSelf();
}
