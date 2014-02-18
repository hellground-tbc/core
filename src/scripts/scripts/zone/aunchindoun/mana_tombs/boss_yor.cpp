/* 
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
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

#include "precompiled.h"
#include "def_mana_tombs.h"

#define BOSS_YOR 22930

#define SPELL_DOUBLE_BREATH     38361
#define SPELL_STOMP             36405

struct boss_yorAI : public ScriptedAI
{
    boss_yorAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;
    uint32 DoubleBreath_Timer;
    uint32 Stomp_Timer;

    void Reset()
    {
        ClearCastQueue();

        DoubleBreath_Timer = 8000+rand()%5000;
        Stomp_Timer = 15000+rand()%5000;

        if(pInstance)
            pInstance->SetData(DATA_YOREVENT, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
        if(pInstance)
            pInstance->SetData(DATA_YOREVENT, DONE);
    }

    void EnterCombat(Unit *who)
    {
        if(pInstance)
            pInstance->SetData(DATA_YOREVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (DoubleBreath_Timer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_DOUBLE_BREATH);
            DoubleBreath_Timer = 7500 + rand()%5000;
        }
        else
            DoubleBreath_Timer -= diff;

        if (Stomp_Timer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_STOMP);
            Stomp_Timer = 15000+rand()%5000;
        }
        else
            Stomp_Timer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_yor(Creature *_Creature)
{
    return new boss_yorAI (_Creature);
}

bool GOUse_go_shaffars_stasis_chamber(Player *player, GameObject* _GO)
{
    if(player->GetInstanceData() && player->GetInstanceData()->GetData(DATA_YOREVENT) != DONE)
    {
        _GO->SummonCreature(BOSS_YOR, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
        _GO->Delete();
    }
    return true;
}

void AddSC_boss_yor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="go_shaffars_stasis_chamber";
    newscript->pGOUse = &GOUse_go_shaffars_stasis_chamber;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_yor";
    newscript->GetAI = &GetAI_boss_yor;
    newscript->RegisterSelf();
}
