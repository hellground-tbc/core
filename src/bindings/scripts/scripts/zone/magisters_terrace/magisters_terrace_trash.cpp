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
        Frostbolt_Timer = 2000;
        Arcane_Nova_Timer = urand (12000, 20000);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
          return;

      if(Frostbolt_Timer < diff)
      {
          AddSpellToCast(me->getVictim(), SPELL_FROSTBOLT);
          Frostbolt_Timer = GetSpellCastTime(GetSpellStore()->LookupEntry(SPELL_FROSTBOLT));
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

    CastNextSpellIfAnyAndReady(diff);
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
        hasSummoned = false;
    }

    bool hasSummoned;
    uint32 FelArmor_Timer;
    uint32 Immolate_Timer;
    
    void Reset()
    {
        if(!hasSummoned)
            DoCast(m_creature,44517,false);

        SetAutocast(SPELL_INCINERATE, 2000);
        Immolate_Timer = 1000;
    }

    void JustSummoned(Creature* cre)
    {
         if(cre->GetEntry() == 24815)
             hasSummoned = true;
    }

    void UpdateAI(const uint32 diff)
    {
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
          AddSpellToCast(m_creature->getVictim(), SPELL_IMMOLATE);
          StartAutocast();
          Immolate_Timer = urand(14000, 20000);
      }
      else
          Immolate_Timer -= diff;

      CastNextSpellIfAnyAndReady(diff);
      DoMeleeAttackIfReady();
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
        Prayer_of_Mending_Timer = urand(10000, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

      if(Poison_Timer < diff)
      {
          AddSpellToCast(SPELL_INJECT_POISON, CAST_SELF);
          Poison_Timer = urand(16000, 22000);
      }
      else
          Poison_Timer -= diff;

      if(Prayer_of_Mending_Timer < diff)
      {
          Unit* healTarget = SelectLowestHpFriendly(40.0f);
          AddSpellToCast(healTarget, SPELL_PRAYER_OF_MENDING);
          Prayer_of_Mending_Timer = urand(20000, 30000);
      }
      else
          Prayer_of_Mending_Timer -= diff;

      CastNextSpellIfAnyAndReady();
      DoMeleeAttackIfReady();
    }
};

#define SPELL_SEAL_OF_WRATH                 (HeroicMode?46030:44480)
#define SPELL_JUDGEMENT_OF_WRATH            (HeroicMode?46045:44583)
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
                Seal_Timer = urand(2000, 5000);
                Judgement_Timer = urand(13000, 20000);
            }
        }
        else
            Judgement_Timer -= diff;

        if(Holy_Light_Timer < diff)
        {
            Unit* healTarget = SelectLowestHpFriendly(40.0f, 10000);
            AddSpellToCast(healTarget, SPELL_HOLY_LIGHT);
            Holy_Light_Timer = urand(10000, 15000);
        }
        else
            Holy_Light_Timer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_wretched_skulkerAI : public ScriptedAI
{
    mob_wretched_skulkerAI(Creature *c) : ScriptedAI(c) {}

    uint32 Drink_Timer;
    uint32 Wretched_Stab_Timer;

    void Reset()
    {
        Drink_Timer = 1000;
        Wretched_Stab_Timer = 4000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            AddSpellToCast(me, 44505);
            Drink_Timer = 15000;
        }
        else
            Drink_Timer -= diff;

       if(Wretched_Stab_Timer < diff)
       {
           AddSpellToCast(m_creature->getVictim(), 44533);
           Wretched_Stab_Timer = 3000;
       }
       else
           Wretched_Stab_Timer -= diff;

       CastNextSpellIfAnyAndReady(diff);
       DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_wretched_bruiserAI : public ScriptedAI
{
    mob_wretched_bruiserAI(Creature *c) : ScriptedAI(c) {}

    uint32 Drink_Timer;
    uint32 Wretched_Strike_Timer;

    void Reset()
    {
        Drink_Timer = 1000;
        Wretched_Strike_Timer = 3000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            AddSpellToCast(me, 44505);
            Drink_Timer = 15000;
        }
        else
            Drink_Timer -= diff;

        if(Wretched_Strike_Timer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), 44533);
            Wretched_Strike_Timer = 3000;
        }
        else
            Wretched_Strike_Timer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_wretched_huskAI : public ScriptedAI
{
    mob_wretched_huskAI(Creature *c) : ScriptedAI(c) {}

    uint32 Drink_Timer;
    uint32 Wretched_Fireball_Timer;
    uint32 Wretched_Frostbolt_Timer;

    void Reset()
    {
        Drink_Timer = 1000;
        Wretched_Fireball_Timer = 3000;
        Wretched_Frostbolt_Timer = 1500;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Drink_Timer < diff)
        {
            AddSpellToCast(me, 44505);
            Drink_Timer = 15000;
        }
        else
            Drink_Timer -= diff;

       if(Wretched_Fireball_Timer < diff)
       {
           AddSpellToCast(m_creature->getVictim(), 44503);
           Wretched_Fireball_Timer = 3000;
       }
       else
           Wretched_Fireball_Timer -= diff;

       if(Wretched_Frostbolt_Timer < diff)
       {
           AddSpellToCast(m_creature->getVictim(), 44504);
           Wretched_Frostbolt_Timer = 4000;
       }
       else
           Wretched_Frostbolt_Timer -= diff;

       CastNextSpellIfAnyAndReady(diff);
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
}
