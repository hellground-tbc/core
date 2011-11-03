#include "precompiled.h"
#include "def_serpent_shrine.h"


#define SPELL_FRIGHTENING_SHOUT     38945
#define SPELL_FRENZY                38947
#define SPELL_KNOCKBACK             38576
#define SPELL_EXECUTE               38959
#define SPELL_MORTAL_CLEAVE         38572

struct TRINITY_DLL_DECL mob_vashjir_honor_guardAI : public ScriptedAI
{
    mob_vashjir_honor_guardAI(Creature *c) : ScriptedAI(c) {}

    uint32 Shout_Timer;
    uint32 Knockback_Timer;
    uint32 Execute_Timer;
    uint32 Cleave_Timer;
    bool Frenzy_Casted;

    uint32 Talk_Timer;
    uint32 Check_Timer;
    bool Talking;
    uint64 Talk_Creature;

    void Reset()
    {
        Shout_Timer = urand(25000, 40000);
        Knockback_Timer = urand(10000, 30000);
        Execute_Timer = 0;
        Cleave_Timer = urand(3000, 6000);
        Frenzy_Casted = false;
        Talk_Timer = 10000;
        Check_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
        {
            //UpdateAIOOC(diff);  // not working in 100%
            return;
        }

        if(Shout_Timer < diff)
        {
            AddSpellToCast(SPELL_FRIGHTENING_SHOUT, CAST_TANK);
            Shout_Timer = urand(30000, 40000);
        } 
        else
            Shout_Timer -= diff;

        if(Knockback_Timer < diff)
        {
            AddSpellToCast(SPELL_KNOCKBACK, CAST_TANK);
            Knockback_Timer = urand(10000, 15000);
        }
        else
            Knockback_Timer -= diff;

        if(Execute_Timer < diff)
        {
            if(me->getVictim()->GetHealth() * 5 <= me->getVictim()->GetMaxHealth()) // below 20%
            {
                AddSpellToCast(SPELL_EXECUTE, CAST_TANK);
                Execute_Timer = 30000;
            }
        }
        else
            Execute_Timer -= diff;

        if(Cleave_Timer < diff)
        {
            AddSpellToCast(SPELL_MORTAL_CLEAVE, CAST_TANK);
            Cleave_Timer = urand(3000, 6000);
        }
        else
            Cleave_Timer -= diff;

        if(!Frenzy_Casted && HealthBelowPct(50))
        {
            AddSpellToCast(SPELL_FRENZY, CAST_SELF);
            Frenzy_Casted = true;
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }

    void UpdateAIOOC(const uint32 diff)
    {
        // need some tweaking 
        if(!Talking)
        {
            if(Talk_Timer < diff)
            {
                if(Check_Timer < diff)
                {
                    std::list<Creature*> friends = FindAllFriendlyInGrid(3);
                    if(!friends.empty())
                    {
                        
                        Creature *c = friends.front();                        
                        c->GetMotionMaster()->MoveIdle();
                        c->SetInFront(me);
                        me->GetMotionMaster()->MoveIdle();
                        me->SetInFront(c);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ROAR);

                        Check_Timer = 1500;
                        Talk_Timer = urand(4000, 6000);
                        Talking = true;
                        Talk_Creature = c->GetGUID();

                        //c->Say("Check1", 0, 0);
                        //me->Say("Check0", 0, 0);
                    }
                } 
                else
                    Check_Timer -= diff;
            } 
            else
                Talk_Timer -= diff;
        }
        else // Talking
        {
            if(Check_Timer)
            {
                if(Check_Timer <= diff)
                {
                    //me->Say("Check2", 0, 0);
                    if(Creature *c = me->GetCreature(Talk_Creature))
                    {
                        c->SetInFront(me);
                        me->SetInFront(c);
                        c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                    }
                    Check_Timer = 0;
                } 
                else
                    Check_Timer -= diff;
            }

            if(Talk_Timer < diff)
            {
                //me->Say("Check3", 0, 0);
                Talking = false;
                Talk_Timer = urand(4000, 8000);
                me->GetMotionMaster()->MovementExpired();
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                
                if(Creature *c = me->GetCreature(Talk_Creature))
                {
                    c->GetMotionMaster()->MovementExpired();
                    c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                }
            } 
            else
                Talk_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_mob_vashjir_honor_guard(Creature *_Creature)
{
    return new mob_vashjir_honor_guardAI(_Creature);
}

struct TRINITY_DLL_DECL mob_coilfang_frenzyAI : public ScriptedAI
{
    mob_coilfang_frenzyAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {

    }

    void EnterEvadeMode()
    {
        float x, y, z;
        me->GetPosition(x, y, z);
        me->SetHomePosition(x, y, z, me->GetOrientation());
        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(const uint32 diff)
    {
        float x, y, z;
        me->GetPosition(x, y, z);

        if(z > WATER_Z)
            me->Relocate(x, y, WATER_Z, me->GetOrientation());

        if(!UpdateVictim())
            return;

        Unit *victim = me->getVictim();

        victim->GetPosition(x, y, z);
        if(z - 5 > WATER_Z)
        {
            EnterEvadeMode();
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_coilfang_frenzy(Creature *_Creature)
{
    return new mob_coilfang_frenzyAI(_Creature);
}

void AddSC_serpent_shrine_trash()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_vashjir_honor_guard";
    newscript->GetAI = &GetAI_mob_vashjir_honor_guard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilfang_frenzy";
    newscript->GetAI = &GetAI_mob_coilfang_frenzy;
    newscript->RegisterSelf();
}