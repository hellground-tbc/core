/* Copyright (C) 2008 - 2010 HellgroundDev <http://gamefreedom.pl/>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* ScriptData
SDName: Magister_Terrace_Trash
SD%Complete: 25% (1/4)
SDComment: Trash NPCs divided by to boss links
SDCategory: Magister Terrace
EndScriptData */

#include "precompiled.h"
#include "def_magisters_terrace.h"

/**********
* Contents:
*
* mob_sunwell_mage_guard      - ID 24683
* mob_sunblade_magister       - ID 
* mob_sunblade_warlock        - ID 
* mob_sunblade_physician      - ID 
* mob_sunblade_blood_knight   - ID 
* mob_wretched_skulker        - ID 
* mob_wretched_bruiser        - ID 
* mob_wretched_husk           - ID 
*
**********/

#define SPELL_GLAIVE_THROW              (HeroicMode?46028:44478)
#define SPELL_MAGIC_DAMPENING_FIELD     44475

struct TRINITY_DLL_DECL mob_sunwell_mage_guardAI : public ScriptedAI
{
    mob_sunwell_mage_guardAI(Creature *c) : ScriptedAI(c) {}

    uint32 Glaive_Timer;
    uint32 Magic_Field_Timer;

    void Reset()
    {
        Glaive_Timer = urand(5000,10000);
        Magic_Field_Timer = (10000, 20000);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
          return;

      if(Glaive_Timer < diff)
      {
          if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0, true))
              AddSpellToCast(target, SPELL_GLAIVE_THROW);
          Glaive_Timer = urand(16000,20000);
      }
      else
          Glaive_Timer -= diff;

       if(Magic_Field_Timer < diff)
       {
           if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0, true))
               AddSpellToCast(m_creature->getVictim(), SPELL_MAGIC_DAMPENING_FIELD);
           Magic_Field_Timer = urand(50000,65000);
       }
       else
            Magic_Field_Timer -= diff;

       CastNextSpellIfAnyAndReady();
       DoMeleeAttackIfReady();
    }
};

#define SPELL_ARCANE_NOVA                   (HeroicMode?46036:44644)
#define SPELL_FROSTBOLT                     (HeroicMode?46035:44606)
#define SPELL_ENCHANTMENT_OF_SPELL_HASTE    44604
#define SPELL_SPELL_HASTE                   44605

struct TRINITY_DLL_DECL mob_sunblade_magisterAI : public ScriptedAI
{
    mob_sunblade_magisterAI(Creature *c) : ScriptedAI(c)
    {
        DoCast(c, SPELL_ENCHANTMENT_OF_SPELL_HASTE, true);
    }

    uint32 Frostbolt_Timer;
    uint32 Arcane_Nova_Timer;

    void Reset()
    {
        Frostbolt_Timer = 0;
        Arcane_Nova_Timer = urand (12000, 20000);
    }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
          return;

      if(Frostbolt_Timer < diff)
      {
          AddSpellToCast(me->getVictim(), SPELL_FROSTBOLT);
          Frostbolt_Timer = GetSpellCastTime(GetSpellStore()->LookupEntry(SPELL_FROSTBOLT))-(diff+100);
      }
      else
          Frostbolt_Timer -= diff;

      if(Arcane_Nova_Timer < diff)
      {
          ClearCastQueue();
          AddSpellToCast(SPELL_ARCANE_NOVA, CAST_SELF);
          Arcane_Nova_Timer = urand(16000, 20000);
      }
      else
          Arcane_Nova_Timer -= diff;

    CastNextSpellIfAnyAndReady();
    DoMeleeAttackIfReady();
    }
};

#define SPELL_FEL_ARMOR                     44520
#define SPELL_SUMMON_SUNBLADE_IMP           44517
#define SPELL_IMMOLATE                      (HeroicMode?46042:44518)
#define SPELL_INCINERATE                    (HeroicMode?46043:44519)

struct TRINITY_DLL_DECL mob_sunblade_warlockAI : public ScriptedAI
{
    mob_sunblade_warlockAI(Creature *c) : ScriptedAI(c)
    {
        DoCast(c, SPELL_FEL_ARMOR, true);
        FelArmor_Timer = 120000;    //check each 2 minutes
        SummonGUID = NULL;
    }

    uint32 SummonImp_Timer;
    uint32 FelArmor_Timer;
    uint32 Immolate_Timer;
    uint64 SummonGUID;

    void Reset()
    {
        SummonImp_Timer = 5000;
        SetAutocast(SPELL_INCINERATE, 1900);
        Immolate_Timer = 1000;
    }

    void JustSummoned(Creature* summon)
    {
        SummonGUID = summon->GetGUID();
    }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!me->isInCombat())
      {
          if(SummonImp_Timer < diff)
          {
              // check if still having pet ;]
              if(!me->GetMap()->GetCreature(SummonGUID))
                  SummonGUID = NULL;

              if(!SummonGUID)
                  DoCast(m_creature, SPELL_SUMMON_SUNBLADE_IMP, false);
              SummonImp_Timer = 15000;
          }
          else
              SummonImp_Timer -= diff;
      }

      if(!UpdateVictim())
      return;

      if(FelArmor_Timer < diff)
      {
          if(!me->HasAura(SPELL_FEL_ARMOR, 0))
              DoCast(me, SPELL_FEL_ARMOR, true);
          FelArmor_Timer = 120000;
      }
      else
          FelArmor_Timer -= diff;

      if(Immolate_Timer < diff)
      {
          ClearCastQueue();
          AddSpellToCast(m_creature->getVictim(), SPELL_IMMOLATE);
          StartAutocast();
          Immolate_Timer = urand(16000, 25000);
      }
      else
          Immolate_Timer -= diff;

      CastNextSpellIfAnyAndReady(diff);
      DoMeleeAttackIfReady();
    }
};

#define SPELL_FIREBALL                      (HeroicMode?46044:44577)

struct TRINITY_DLL_DECL mob_sunblade_impAI : public ScriptedAI
{
    mob_sunblade_impAI(Creature *c) : ScriptedAI(c) { }

    uint64 OwnerGUID;

    void Reset() { }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who);
        DoCast(who, SPELL_FIREBALL);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

      CastNextSpellIfAnyAndReady(diff);
    }
};

#define SPELL_INJECT_POISON                 (HeroicMode?46046:44599)
#define SPELL_PRAYER_OF_MENDING             (HeroicMode?46045:44583)

struct TRINITY_DLL_DECL mob_sunblade_physicianAI : public ScriptedAI
{
    mob_sunblade_physicianAI(Creature *c) : ScriptedAI(c) { }

    uint32 Poison_Timer;
    uint32 Prayer_of_Mending_Timer;
    
    void Reset()
    {
        Poison_Timer = urand(5000, 8000);
        Prayer_of_Mending_Timer = urand(3000, 8000);
    }

    bool CanCastPoM()
    {
        std::list<Creature*> FriendList = FindAllFriendlyInGrid(40);
        if(FriendList.empty())
            return false;

        for(std::list<Creature*>::iterator i = FriendList.begin(); i !=  FriendList.end(); ++i)
        {
            if((*i)->HasAura(44586, 0))
                return false;
        }
        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

      if(Poison_Timer < diff)
      {
          AddSpellToCast(SPELL_INJECT_POISON, CAST_SELF);
          Poison_Timer = urand(16000, 20000);
      }
      else
          Poison_Timer -= diff;

      if(Prayer_of_Mending_Timer < diff)
      {
          if(CanCastPoM())  // only one PoM active at a time
          {
              if(Unit* healTarget = SelectLowestHpFriendly(40))
                  AddSpellToCast(healTarget, SPELL_PRAYER_OF_MENDING);
          }
          Prayer_of_Mending_Timer = urand(7000, 12000);
      }
      else
          Prayer_of_Mending_Timer -= diff;

      CastNextSpellIfAnyAndReady();
      DoMeleeAttackIfReady();
    }
};

#define SPELL_SEAL_OF_WRATH                 (HeroicMode?46030:44480)
#define SPELL_JUDGEMENT_OF_WRATH            (HeroicMode?46033:44482)
#define SPELL_HOLY_LIGHT                    (HeroicMode?46029:44479)

struct TRINITY_DLL_DECL mob_sunblade_blood_knightAI : public ScriptedAI
{
    mob_sunblade_blood_knightAI(Creature *c) : ScriptedAI(c) { }

    uint32 Judgement_Timer;
    uint32 Holy_Light_Timer;
    uint32 Seal_Timer;

    void Reset()
    {
        Seal_Timer = urand(1000, 6000);
        Judgement_Timer = urand(10000, 15000);
        Holy_Light_Timer = urand(8000, 20000);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Seal_Timer < diff)
        {
            AddSpellToCast(SPELL_SEAL_OF_WRATH, CAST_SELF);
            Seal_Timer = urand(20000, 30000);
        }
        else
            Seal_Timer -= diff;

        if(Judgement_Timer < diff)
        {
            if(me->HasAura(SPELL_SEAL_OF_WRATH, 0))
            {
                AddSpellToCast(me->getVictim(), SPELL_JUDGEMENT_OF_WRATH);
                me->RemoveAurasDueToSpell(SPELL_SEAL_OF_WRATH);
                Seal_Timer = urand(5000, 10000);
                Judgement_Timer = urand(13000, 20000);
            }
        }
        else
            Judgement_Timer -= diff;

        if(Holy_Light_Timer < diff)
        {
            Unit* healTarget = SelectLowestHpFriendly(40.0f, 10000);
            AddSpellToCast(healTarget, SPELL_HOLY_LIGHT);
            Holy_Light_Timer = urand(7000, 10000);
        }
        else
            Holy_Light_Timer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

uint32 DrainingList[8] = 
{
    96837, 96829,
    96828, 96838,
    96836, 96826,
    96835, 96831
};

const char* SAY_OOC1     = "The power! More, more, more!";
const char* SAY_OOC2     = "It seethes and burns...";
const char* SAY_AGGRO1   = "Get away from my crystals!";
const char* SAY_AGGRO2   = "It's MINE!";
const char* SAY_AGGRO3   = "You wish to steal the power! Die!";

#define SPELL_DRINK_FEL_INFUSION            44505
#define SPELL_FEL_CRYSTAL_COSMETIC          44374
#define SPELL_WRETCHED_STAB                 44533
#define SPELL_DUAL_WIELD                    29651

struct TRINITY_DLL_DECL mob_wretched_skulkerAI : public ScriptedAI
{
    mob_wretched_skulkerAI(Creature *c) : ScriptedAI(c) {}

    uint32 Drink_Timer;
    uint32 Wretched_Stab_Timer;
    uint32 OOCTimer;

    void Reset()
    {
        me->SetStandState(PLAYER_STATE_SIT);
        DoCast(me, SPELL_DUAL_WIELD);
        OOCTimer = 10000;
        Drink_Timer = urand(25000, 35000);
        Wretched_Stab_Timer = urand(4000, 7000);
    }

    void EnterCombat(Unit* who)
    {
        if(me->IsNonMeleeSpellCasted(false))
            me->InterruptNonMeleeSpells(false);
        if(roll_chance_f(10.0))
            DoSay(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), 0, me->getVictim());
    }

    void HandleOffCombatEffects()
    {
        for (uint8 i = 0; i < 8; ++i)
        {
            if(me->GetDBTableGUIDLow() == DrainingList[i]) // draining fel crystal
                me->CastSpell((Unit*)NULL, SPELL_FEL_CRYSTAL_COSMETIC, false);
            if(roll_chance_f(2.0f))
                    DoSay(RAND(SAY_OOC1, SAY_OOC2), 0, me);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->IsNonMeleeSpellCasted(false) && !me->isInCombat())
        {
            if(OOCTimer < diff)
            {
                HandleOffCombatEffects();
                OOCTimer = 10000;
            }
            else
                OOCTimer -= diff;
        }

        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            AddSpellToCast(SPELL_DRINK_FEL_INFUSION, CAST_SELF);
            Drink_Timer = urand(25000, 35000);
        }
        else
            Drink_Timer -= diff;

       if(Wretched_Stab_Timer < diff)
       {
           AddSpellToCast(m_creature->getVictim(), SPELL_WRETCHED_STAB);
           Wretched_Stab_Timer = urand(3000, 7000);
       }
       else
           Wretched_Stab_Timer -= diff;

       CastNextSpellIfAnyAndReady(diff);
       DoMeleeAttackIfReady();
    }
};

#define SPELL_WRETCHED_STRIKE               44534

struct TRINITY_DLL_DECL mob_wretched_bruiserAI : public ScriptedAI
{
    mob_wretched_bruiserAI(Creature *c) : ScriptedAI(c) {}

    uint32 OOCTimer;
    uint32 Drink_Timer;
    uint32 Wretched_Strike_Timer;

    void Reset()
    {
        me->SetStandState(PLAYER_STATE_SIT);
        OOCTimer = 10000;
        Drink_Timer = urand(25000, 35000);
        Wretched_Strike_Timer = urand(6000, 10000);
    }

    void EnterCombat(Unit* who)
    {
        if(me->IsNonMeleeSpellCasted(false))
            me->InterruptNonMeleeSpells(false);
        if(roll_chance_f(10.0))
            DoSay(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), 0, me->getVictim());
    }

    void HandleOffCombatEffects()
    {
        for (uint8 i = 0; i < 8; ++i)
        {
            if(me->GetDBTableGUIDLow() == DrainingList[i]) // draining fel crystal
                me->CastSpell((Unit*)NULL, SPELL_FEL_CRYSTAL_COSMETIC, false);
            if(roll_chance_f(2.0f))
                    DoSay(RAND(SAY_OOC1, SAY_OOC2), 0, me);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->IsNonMeleeSpellCasted(false) && !me->isInCombat())
        {
            if(OOCTimer < diff)
            {
                HandleOffCombatEffects();
                OOCTimer = 10000;
            }
            else
                OOCTimer -= diff;
        }

        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            AddSpellToCast(SPELL_DRINK_FEL_INFUSION, CAST_SELF);
            Drink_Timer = urand(25000, 35000);
        }
        else
            Drink_Timer -= diff;

        if(Wretched_Strike_Timer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_WRETCHED_STRIKE);
            Wretched_Strike_Timer = urand(7000, 16000);
        }
        else
            Wretched_Strike_Timer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

#define SPELL_WRETCHED_FIREBALL               44503
#define SPELL_WRETCHED_FROSTBOLT              44504

struct TRINITY_DLL_DECL mob_wretched_huskAI : public ScriptedAI
{
    mob_wretched_huskAI(Creature *c) : ScriptedAI(c) {}

    uint32 OOCTimer;
    uint32 Drink_Timer;
    uint32 Wretched_Cast_Timer;

    void Reset()
    {
        me->SetStandState(PLAYER_STATE_SIT);
        OOCTimer = 5000;
        Drink_Timer = urand(25000, 35000);
        Wretched_Cast_Timer = 0;
    }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who);
    }

    void EnterCombat(Unit* who)
    {
        if(me->IsNonMeleeSpellCasted(false))
            me->InterruptNonMeleeSpells(false);
        if(roll_chance_f(10.0))
            DoSay(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), 0, me->getVictim());
    }

    void HandleOffCombatEffects()
    {
        for (uint8 i = 0; i < 8; ++i)
        {
            if(me->GetDBTableGUIDLow() == DrainingList[i]) // draining fel crystal
            {
                me->CastSpell((Unit*)NULL, SPELL_FEL_CRYSTAL_COSMETIC, false);
                if(roll_chance_f(2.0f))
                    DoSay(RAND(SAY_OOC1, SAY_OOC2), 0, me);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->IsNonMeleeSpellCasted(false) && !me->isInCombat())
        {
            if(OOCTimer < diff)
            {
                HandleOffCombatEffects();
                OOCTimer = 10000;
            }
            else
                OOCTimer -= diff;
        }

        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            ClearCastQueue();
            AddSpellToCast(SPELL_DRINK_FEL_INFUSION, CAST_SELF);
            Drink_Timer = urand(25000, 35000);
        }
        else
            Drink_Timer -= diff;

       if(Wretched_Cast_Timer < diff)
       {
           AddSpellToCast(m_creature->getVictim(), RAND(SPELL_WRETCHED_FIREBALL, SPELL_WRETCHED_FROSTBOLT));
           Wretched_Cast_Timer = me->HasAura(SPELL_DRINK_FEL_INFUSION, 1) ? 1400 : 2900;
       }
       else
           Wretched_Cast_Timer -= diff;

       CastNextSpellIfAnyAndReady(diff);
       DoMeleeAttackIfReady();
    }
};

#define SPELL_ENERGY_INFUSION                 44406

struct TRINITY_DLL_DECL mob_brightscale_wyrmAI : public ScriptedAI
{
    mob_brightscale_wyrmAI(Creature *c) : ScriptedAI(c) {}
    //TODO: make Nether Energy Feeding cosmetics when Sunblade Keeper implemented?
    void Reset()
    {
        me->SetLevitate(true);
        me->GetMotionMaster()->MoveRandom(10.0);
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if(damage && damage >= me->GetHealth())
            DoCast((Unit*)NULL, SPELL_ENERGY_INFUSION);
    }

    /*void JustDied(Unit* killer)
    {
        ForceSpellCast(SPELL_ENERGY_INFUSION, CAST_NULL);
    }*/

    void UpdateAI(const uint32 diff)
    {
       DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunwell_mage_guard(Creature *_Creature)
{
    return new mob_sunwell_mage_guardAI (_Creature);
}
CreatureAI* GetAI_mob_sunblade_magister(Creature *_Creature)
{
    return new mob_sunblade_magisterAI (_Creature);
}
CreatureAI* GetAI_mob_sunblade_warlock(Creature *_Creature)
{
    return new mob_sunblade_warlockAI (_Creature);
}
CreatureAI* GetAI_mob_sunblade_imp(Creature *_Creature)
{
    return new mob_sunblade_impAI (_Creature);
}
CreatureAI* GetAI_mob_sunblade_physician(Creature *_Creature)
{
    return new mob_sunblade_physicianAI (_Creature);
}
CreatureAI* GetAI_mob_sunblade_blood_knight(Creature *_Creature)
{
    return new mob_sunblade_blood_knightAI (_Creature);
}
CreatureAI* GetAI_mob_wretched_skulker(Creature *_Creature)
{
    return new mob_wretched_skulkerAI (_Creature);
}
CreatureAI* GetAI_mob_wretched_bruiser(Creature *_Creature)
{
    return new mob_wretched_bruiserAI (_Creature);
}
CreatureAI* GetAI_mob_wretched_husk(Creature *_Creature)
{
    return new mob_wretched_huskAI (_Creature);
}
CreatureAI* GetAI_mob_brightscale_wyrm(Creature *_Creature)
{
    return new mob_brightscale_wyrmAI (_Creature);
}

void AddSC_magisters_terrace_trash()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_sunwell_mage_guard";
    newscript->GetAI = &GetAI_mob_sunwell_mage_guard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_magister";
    newscript->GetAI = &GetAI_mob_sunblade_magister;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_warlock";
    newscript->GetAI = &GetAI_mob_sunblade_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_imp";
    newscript->GetAI = &GetAI_mob_sunblade_imp;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_physician";
    newscript->GetAI = &GetAI_mob_sunblade_physician;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_blood_knight";
    newscript->GetAI = &GetAI_mob_sunblade_blood_knight;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_wretched_skulker";
    newscript->GetAI = &GetAI_mob_wretched_skulker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_wretched_bruiser";
    newscript->GetAI = &GetAI_mob_wretched_bruiser;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_wretched_husk";
    newscript->GetAI = &GetAI_mob_wretched_husk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_brightscale_wyrm";
    newscript->GetAI = &GetAI_mob_brightscale_wyrm;
    newscript->RegisterSelf();
}
