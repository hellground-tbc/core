#include "precompiled.h"

#define SPELL_HAMSTRING     9080
#define SPELL_HEAD_CRACK    16172
#define YELL_AGGRO          -678
#define YELL_OOC1           -679
#define YELL_OOC2           -680
#define YELL_OOC3           -681
#define YELL_OOC4           -683
#define YELL_OOC5           -684
#define YELL_OOC6           -685


struct mob_coilfang_slavehandlerAI : public ScriptedAI
{
    mob_coilfang_slavehandlerAI(Creature *c) : ScriptedAI(c) {}

    uint32 hamstringtimer;
    uint32 headcracktimer;
    uint32 yelltimer;

    void Reset()
    {
        hamstringtimer = urand(5800,6200);
        headcracktimer = 11100;
        yelltimer = urand(60000,120000);
    }

    void JustDied(Unit* )
    {
        std::list<Creature*> alist= FindAllCreaturesWithEntry(17963, 100);
        std::list<Creature*> blist= FindAllCreaturesWithEntry(17964, 100);
        for(std::list<Creature*>::iterator itr = blist.begin(); itr != blist.end(); itr++)
            alist.push_front(*itr);
        for(std::list<Creature*>::iterator itr = alist.begin(); itr != alist.end(); itr++)
        {
            if ((*itr)->GetFormation() && (*itr)->GetFormation()->getLeader() == me)
            {
                (*itr)->SetHomePosition(120,-132,-0.6,4);
                // this is EnterEvadeMode() but without formation reset
                (*itr)->AI()->_EnterEvadeMode();
                (*itr)->AI()->Reset();
                (*itr)->GetMotionMaster()->MoveTargetedHome();
            }
        }
    }

    void EnterCombat(Unit* attacker)
    {
        me->Yell(YELL_AGGRO,0,0);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->isInCombat())
            if(yelltimer < diff)
            {
                me->Yell(RAND(YELL_OOC1,YELL_OOC2,YELL_OOC3,YELL_OOC4,YELL_OOC5,YELL_OOC6),0,0);
                yelltimer = urand(60000,120000);
            }
            else
                yelltimer -=diff;

        if(!UpdateVictim())
            return;

        if(hamstringtimer < diff)
        {
            AddSpellToCast(SPELL_HAMSTRING);
            hamstringtimer = 9000;
        }
        else
            hamstringtimer -= diff;

        if(headcracktimer < diff)
        {
            AddSpellToCast(SPELL_HEAD_CRACK,CAST_RANDOM);
            headcracktimer = urand(20000,25000);
        }
        else
            headcracktimer -= diff;

        DoMeleeAttackIfReady();
        CastNextSpellIfAnyAndReady(diff);
    }
};

CreatureAI* GetAI_mob_coilfang_slavehandler(Creature *_creature)
{
    return new mob_coilfang_slavehandlerAI (_creature);
}

struct npc_slave_pens_dispenserAI : public Scripted_NoMovementAI
{
    npc_slave_pens_dispenserAI(Creature *c) : Scripted_NoMovementAI(c) {}

    uint32 dispensetimer;
    void Reset()
    {
        dispensetimer=3000;
    }
    void UpdateAI(const uint32 diff) 
    {
        if (dispensetimer <= diff)
        {
            Unit* slave = FindCreature(17963, 20, m_creature);
            if (slave)
            {
                slave->ToCreature()->ForcedDespawn();
                slave->ToCreature()->RemoveCorpse();
            }
            slave = FindCreature(17964, 20, m_creature);
            if (slave)
            {
                slave->ToCreature()->ForcedDespawn();
                slave->ToCreature()->RemoveCorpse();
            }
            dispensetimer = 3000;
        }
        else
        dispensetimer -= diff;
    }
    void AttackStart() {}

};

CreatureAI* GetAI_npc_slave_pens_dispenser(Creature *_creature)
{
    return new npc_slave_pens_dispenserAI (_creature);
}

void AddSC_slave_pens_trash()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="mob_coilfang_slavehandler";
    newscript->GetAI = &GetAI_mob_coilfang_slavehandler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_slave_pens_dispenser";
    newscript->GetAI = &GetAI_npc_slave_pens_dispenser;
    newscript->RegisterSelf();
}