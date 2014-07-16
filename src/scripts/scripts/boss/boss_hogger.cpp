
#include "precompiled.h"

#define START_SAY_1 -5
#define START_SAY_2 -6

#define SPELL_HEAD_BUTT 6730
#define SPELL_RUSHING_CHARGE 6268
#define SPELL_PIERCE_ARMOR 6016

struct boss_hoggerAI : public ScriptedAI
{
    boss_hoggerAI(Creature *c) : ScriptedAI(c) {}

    void EnterCombat(Unit* who)
    {
        m_creature->Yell(RAND(START_SAY_1,START_SAY_2),0,0);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!m_creature->IsWithinMeleeRange(m_creature->getVictim()) && !m_creature->HasAura(SPELL_RUSHING_CHARGE))
            DoCast(m_creature,SPELL_RUSHING_CHARGE);
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hogger(Creature *_Creature)
{
    return new boss_hoggerAI (_Creature);
}

void AddSC_boss_hogger()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_hogger_the";
    newscript->GetAI = &GetAI_boss_hogger;
    newscript->RegisterSelf();

}