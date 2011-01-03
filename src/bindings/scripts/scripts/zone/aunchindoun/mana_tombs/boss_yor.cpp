#include "precompiled.h"

#define BOSS_YOR 22930

#define SPELL_DOUBLE_BREATH     38361
#define SPELL_STOMP             36405

struct TRINITY_DLL_DECL boss_yorAI : public ScriptedAI
{
    boss_yorAI(Creature *c) : ScriptedAI(c)
    {
    }

    uint32 DoubleBreath_Timer;
    uint32 Stomp_Timer;

    void Reset() 
    {
        DoubleBreath_Timer = 8000+rand()%5000;
        Stomp_Timer = 15000+rand()%5000;
    }

    void JustDied(Unit* Killer)
    {
    }

    void EnterCombat(Unit *who)
    {
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

bool GOHello_go_shaffars_stasis_chamber(Player *player, GameObject* _GO)
{
    _GO->SummonCreature(BOSS_YOR, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
    return true;
}

void AddSC_boss_yor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="go_shaffars_stasis_chamber";
    newscript->pGOHello = &GOHello_go_shaffars_stasis_chamber;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_yor";
    newscript->GetAI = &GetAI_boss_yor;
    newscript->RegisterSelf();
}
