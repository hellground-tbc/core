#include "precompiled.h"

//#define SPELL_

#define SPELL_GHOST_DISGUISE            46786
#define SPELL_SLIPPERY_FLOOR            45945
#define SPELL_AHUNE_SHIELD              45954
#define SPELL_AHUNE_RESURFACES          46402
#define SPELL_AHUNE_SELF_STUN           46416
#define SPELL_SUMMON_ICE_SPEAR_BUNNY    46359
#define SPELL_ICE_SPEAR_KNOWCKBACK      46360

//#define SPELL_

#define CREATURE_CORE               25865
#define CREATURE_HAILSTONE          25755
#define CREATURE_COLDWAVE           25756
#define CREATURE_FROSTWIND          25757
// #define CREATURE_HAILSTONE_HEROIC   26342
// #define CREATURE_COLDWAVE_HEROIC    26340
// #define CREATURE_FROSTWIND_HEROIC   26341
//#define CREATURE_

// #define GO_ICE_CHEST                    187892

struct TRINITY_DLL_DECL boss_lord_ahune_coreAI : public Scripted_NoMovementAI
{
    boss_lord_ahune_coreAI(Creature *c) : Scripted_NoMovementAI(c) {}


    bool DamageRecentlyTaken;
    uint32 AhuneGhostShow_Timer;
    bool NeedToHideGhost;
    uint32 AhuneGhostHide_Timer;
    uint64 Ahune;

    void Reset()
    {
        DamageRecentlyTaken = false;
        NeedToHideGhost = false;
        AhuneGhostShow_Timer = 3000;
        Ahune = 0;
    }

    void Aggro(Unit *who)
    {
    }

    void DamageTaken(Unit*, uint32 &)
    {
        DamageRecentlyTaken = true;
    }

    void JustDied(Unit *u)
    {
       if(Ahune)
            if(Unit *ahune = me->GetUnit(*me, Ahune))
            {
                ahune->SetVisibility(VISIBILITY_ON);
                ahune->RemoveAurasDueToSpell(SPELL_AHUNE_SELF_STUN);
                u->DealDamage(ahune, ahune->GetHealth() - 1);
            }
               
    }

    void SetAhuneVisibility(bool val)
    {
        if(Ahune)
            if(Unit *ahune = me->GetUnit(*me, Ahune))
                ahune->SetVisibility(val ? VISIBILITY_ON : VISIBILITY_OFF);
    }

    void UpdateAI(const uint32 diff)
    {
        if(NeedToHideGhost)
        {
            if(AhuneGhostHide_Timer < diff)
            {
                SetAhuneVisibility(false);
                NeedToHideGhost = false;
            } else
                AhuneGhostHide_Timer -= diff;
        }

        if(DamageRecentlyTaken && !NeedToHideGhost)
        {
            if(AhuneGhostShow_Timer < diff)
            {
                SetAhuneVisibility(true);
                DamageRecentlyTaken = false;
                NeedToHideGhost = true;
                AhuneGhostHide_Timer = 3000;
                AhuneGhostShow_Timer = 3000;
            } else
                AhuneGhostShow_Timer -= diff;
        } 
    }
};

CreatureAI* GetAI_boss_lord_ahune_core(Creature *_Creature)
{
    return new boss_lord_ahune_coreAI (_Creature);
}


struct TRINITY_DLL_DECL boss_lord_ahuneAI : public Scripted_NoMovementAI
{

    SummonList Summons;

    uint8 Phase;            // 1, 2
    uint64 Core;            // GUID of core creature

    uint32 Phase_Timer;
    bool FirstPhase;
    bool HeroicMode;
   
    uint32 IceSpear_Timer; 
    uint32 Elementals_Timer;
    uint32 Hailstone_Timer;

    boss_lord_ahuneAI(Creature *c) : Scripted_NoMovementAI(c), Summons(m_creature)
    {
        HeroicMode = m_creature->GetMap()->IsHeroic();
    }

    void Reset()
    {
        FirstPhase = true;
        Phase = 1;
        Core = 0;
        Phase_Timer = 60000;
        Summons.DespawnAll();
        me->SetVisibility(VISIBILITY_ON);

        Elementals_Timer = 5000;
        Hailstone_Timer = 3000;
        IceSpear_Timer = 5000;
    }

    void Aggro(Unit *who)
    {
        DoCast(me, SPELL_AHUNE_SHIELD, true);
        DoCast(me, SPELL_SLIPPERY_FLOOR, true);
        float x, y, z;
        me->GetPosition(x, y, z);
        Creature *core = me->SummonCreature(CREATURE_CORE, x, y, z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
        if(core)
        {
            ((boss_lord_ahune_coreAI*)core->AI())->Ahune = me->GetGUID();
            core->SetVisibility(VISIBILITY_OFF);
            Core = core->GetGUID();
        }
//        else
//            EnterEvadeMode();
    }

    void KilledUnit(Unit *victim)
    {

    }


    void JustSummoned(Creature* summoned)
    {
        Summons.Summon(summoned);
    }


    void SummonedCreatureDespawn(Creature *summon)
    {
        Summons.Despawn(summon);
    }

    void JustDied(Unit *victim)
    {
//        float x, y, z;
//        victim->GetPosition(x, y, z);
//        victim->SummonGameObject(GO_ICE_CHEST, x, y, z, 0, 0, 0, 0, 0, 0);
    }

    void SummonAtRandomPlayer(uint32 entry)
    {
        Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
        float x, y, z;
        if(target)
            target->GetPosition(x, y, z);
        else
            me->GetPosition(x, y, z);
         me->SummonCreature(entry, x, y, z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Phase_Timer < diff)
        {
            if(Phase == 1)
            {
                Phase = 2;
                if(Unit *core = me->GetUnit(*me, Core))
                {
                    core->SetVisibility(VISIBILITY_ON);
                    me->SetVisibility(VISIBILITY_OFF);
                    DoCast(me, SPELL_GHOST_DISGUISE, true);
                }
                else
                {
 //                   EnterEvadeMode();
                    return;
                }
                Phase_Timer = 30000;
            }
            else
            {
                FirstPhase = false;
                Phase = 1;
                if(Unit *core = me->GetUnit(*me, Core))
                {
                    core->SetVisibility(VISIBILITY_OFF);
                    me->RemoveAurasDueToSpell(SPELL_GHOST_DISGUISE);
                    me->RemoveAurasDueToSpell(SPELL_AHUNE_SELF_STUN);
                    me->SetVisibility(VISIBILITY_ON);
                    DoCast(me, SPELL_AHUNE_SHIELD, true);
                    DoCast(me, SPELL_SLIPPERY_FLOOR, true);
                    DoCast(me, SPELL_AHUNE_RESURFACES, true);
                }
                else
                {
  //                  EnterEvadeMode();
                    return;
                }
                Phase_Timer = 60000;
            }
        } else
            Phase_Timer -= diff;

        if(Phase == 1)
        {
            if(Hailstone_Timer < diff)
            {
                SummonAtRandomPlayer(CREATURE_HAILSTONE);
                Hailstone_Timer = 60000;
            } else
                Hailstone_Timer -= diff;

            if(Elementals_Timer < diff)
            {
                SummonAtRandomPlayer(CREATURE_COLDWAVE);
                SummonAtRandomPlayer(CREATURE_COLDWAVE);
                if(!FirstPhase)
                    SummonAtRandomPlayer(CREATURE_FROSTWIND);
                Elementals_Timer = 8000;
            } else
                Elementals_Timer -= diff;

            if(IceSpear_Timer < diff)
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_SUMMON_ICE_SPEAR_BUNNY, true);
                IceSpear_Timer = 5500;
            }else
                IceSpear_Timer -= diff;
        }
        
    }
};


CreatureAI* GetAI_boss_lord_ahune(Creature *_Creature)
{
    return new boss_lord_ahuneAI (_Creature);
}

struct TRINITY_DLL_DECL npc_ice_spear_bunnyAI : public Scripted_NoMovementAI
{
    npc_ice_spear_bunnyAI(Creature *c) : Scripted_NoMovementAI(c)  {}

    uint32 Timer;

    void Reset()
    {
        Timer = 0;
    }

    void Aggro(Unit *) {}

    void KnockbackPlayers()
    {
        std::list<Unit*> PlayerList;
        uint32 knockbackRadius = 5;
        Trinity::AnyUnitInObjectRangeCheck  check(me, knockbackRadius);
        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck > searcher(PlayerList, check);
        me->VisitNearbyWorldObject(knockbackRadius, searcher);

        for(std::list<Unit*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
        {
            (*i)->CastSpell((*i), SPELL_ICE_SPEAR_KNOWCKBACK, true);
        }
        
    }

    void UpdateAI(const uint32 diff)
    {

        if(Timer > 2500)
        {
            KnockbackPlayers();
            me->setDeathState(JUST_DIED);
            me->RemoveCorpse();
        }
         
        Timer += diff;
    }

};

CreatureAI* GetAI_npc_ice_spear_bunny(Creature *_Creature)
{
    return new npc_ice_spear_bunnyAI (_Creature);
}

void AddSC_boss_lord_ahune()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_lord_ahune";
    newscript->GetAI = &GetAI_boss_lord_ahune;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_lord_ahune_core";
    newscript->GetAI = &GetAI_boss_lord_ahune_core;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_ice_spear_bunny";
    newscript->GetAI = &GetAI_npc_ice_spear_bunny;
    newscript->RegisterSelf();
}

