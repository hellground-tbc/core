#include "precompiled.h"
#include "def_shattered_halls.h"

#define SPELL_CLEAVE 15496

struct TRINITY_DLL_DECL boss_blood_guard_porungAI : public ScriptedAI
{
    boss_blood_guard_porungAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance* pInstance;
    uint32 Cleave_Timer;

    void Reset() 
    {
        Cleave_Timer = 10000;

        if (pInstance)
            pInstance->SetData(TYPE_PORUNG, NOT_STARTED); 
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PORUNG, DONE);
    }

    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PORUNG, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Cleave_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE, false);
            Cleave_Timer = 7500 + rand()%5000;
        }
        else
            Cleave_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blood_guard_porung(Creature *_Creature)
{
    return new boss_blood_guard_porungAI (_Creature);
}

void AddSC_boss_blood_guard_porung()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_blood_guard_porung";
    newscript->GetAI = &GetAI_boss_blood_guard_porung;
    newscript->RegisterSelf();
}