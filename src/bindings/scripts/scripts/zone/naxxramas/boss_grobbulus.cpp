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
SDName: Boss_Grobbulus
SD%Complete: 0
SDComment: Place holder
SDCategory: Naxxramas
EndScriptData */

/*Poison Cloud 26590
Slime Spray 28157
Fallout slime 28218
Mutating Injection 28169
Enrages 26527*/

#include "precompiled.h"
#include "def_naxxramas.h"

struct TRINITY_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    void Reset()
    {
        if (pInstance && pInstance->GetData(DATA_GROBBULUS) != DONE)
            pInstance->SetData(DATA_GROBBULUS, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(DATA_GROBBULUS, IN_PROGRESS);
    }

    void JustDied(Unit * killer)
    {
        if (pInstance)
            pInstance->SetData(DATA_GROBBULUS, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_grobbulus(Creature *_Creature)
{
    return new boss_grobbulusAI (_Creature);
}
void AddSC_boss_grobbulus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_grobbulus";
    newscript->GetAI = &GetAI_boss_grobbulus;
    newscript->RegisterSelf();
}