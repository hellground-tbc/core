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

struct TRINITY_DLL_DECL mob_sunwell_mageblood_guardAI : public ScriptedAI
{
    mob_sunwell_mageblood_guardAI(Creature *c) : ScriptedAI(c) {}

    uint32 Glaive_Timer;
    uint32 Magic_Field_Timer;

    void Reset()
    {
        Glaive_Timer=2000;
        Magic_Field_Timer=4000;
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Glaive_Timer < diff)
            {
                 if (m_creature->GetEntry()==24683)
                     AddSpellToCast(m_creature->getVictim(),44478);    //normal

                 else if (m_creature->GetEntry()==25568)
                     AddSpellToCast(m_creature->getVictim(),46028);    //hero
                     Glaive_Timer=2500;

            } else
                Glaive_Timer -= diff;

       if(Magic_Field_Timer < diff)
         {
            AddSpellToCast(m_creature->getVictim(),44475);
            Magic_Field_Timer=urand(15000,30000);
          }
       else
            Magic_Field_Timer -= diff;

    CastNextSpellIfAnyAndReady(diff);
    DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_sunblade_magisterAI : public ScriptedAI
{
    mob_sunblade_magisterAI(Creature *c) : ScriptedAI(c) {}

    uint32 Frostbolt_Timer;
    uint32 Arcane_Nova_Timer;

        void Reset()
        {
         Frostbolt_Timer=2000;
         Arcane_Nova_Timer=9000;
         DoCast(m_creature,44604,true);
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Frostbolt_Timer < diff)
            {
                 if (m_creature->GetEntry()==24685)
                     AddSpellToCast(m_creature->getVictim(),44606);    //normal

                 else if (m_creature->GetEntry()==25569)
                     AddSpellToCast(m_creature->getVictim(),46035);    //hero
                     Frostbolt_Timer=2000;

            } else
                Frostbolt_Timer -= diff;

            if(Arcane_Nova_Timer < diff)
            {
                 if (m_creature->GetEntry()==24685)
                     AddSpellToCast(m_creature->getVictim(),44644);    //normal

                 else if (m_creature->GetEntry()==25569)
                     AddSpellToCast(m_creature->getVictim(),46036);    //hero
                     Arcane_Nova_Timer=10000;

            } else
                Arcane_Nova_Timer -= diff;

    CastNextSpellIfAnyAndReady(diff);
    DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_sunblade_warlockAI : public ScriptedAI
{
    mob_sunblade_warlockAI(Creature *c) : ScriptedAI(c) {}

    uint32 Inci_Timer;
    uint32 Immolate_Timer;

        void Reset()
        {
         Inci_Timer=5000;
         Immolate_Timer=9000;
        }

    void JustSpawned()
    {
         DoCast(m_creature,44520,true);
         DoCast(m_creature,44517,false);
    }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Immolate_Timer < diff)
            {
                 if (m_creature->GetEntry()==24686)
                     AddSpellToCast(m_creature->getVictim(),44518);    //normal

                 else if (m_creature->GetEntry()==25572)
                     AddSpellToCast(m_creature->getVictim(),46042);    //hero
                     Immolate_Timer=20000;

            } else
                Immolate_Timer -= diff;

            if(Inci_Timer < diff)
            {
                 if (m_creature->GetEntry()==24686)
                     AddSpellToCast(m_creature->getVictim(),44519);    //normal

                 else if (m_creature->GetEntry()==25572)
                     AddSpellToCast(m_creature->getVictim(),46043);    //hero
                    Inci_Timer=8000;

            } else
                Inci_Timer -= diff;

    CastNextSpellIfAnyAndReady(diff);
    DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_sunblade_physicianAI : public ScriptedAI
{
    mob_sunblade_physicianAI(Creature *c) : ScriptedAI(c) {}

    uint32 Poison_Timer;
    uint32 Prayer_of_Mending_Timer;

        void Reset()
        {
         Poison_Timer=6000;
         Prayer_of_Mending_Timer=3000;
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Poison_Timer < diff)
            {
                 if (m_creature->GetEntry()==24687)
                     AddSpellToCast(m_creature->getVictim(),44599);    //normal

                 else if (m_creature->GetEntry()==25570)
                     AddSpellToCast(m_creature->getVictim(),46046);    //hero
                     Poison_Timer=25000;

            } else
                Poison_Timer -= diff;

            if(Prayer_of_Mending_Timer < diff)
            {
                Unit* healTarget = SelectLowestHpFriendly(40.0f);
                 if (m_creature->GetEntry()==24687)
                     AddSpellToCast(healTarget,44583);    //normal

                 else if (m_creature->GetEntry()==25570)
                     AddSpellToCast(healTarget,46045);    //hero
                    Prayer_of_Mending_Timer=8000;

            } else
                Prayer_of_Mending_Timer -= diff;

    CastNextSpellIfAnyAndReady(diff);
    DoMeleeAttackIfReady();
    }
};




struct TRINITY_DLL_DECL mob_sunblade_blood_knightAI : public ScriptedAI
{
    mob_sunblade_blood_knightAI(Creature *c) : ScriptedAI(c) {}

    uint32 Judgement_Timer;
    uint32 Holy_Light_Timer;
    uint32 Seal_Timer;

        void Reset()
        {
         Judgement_Timer=1000;
         Holy_Light_Timer=8000;
         Seal_Timer=2000;
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Judgement_Timer < diff)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                 if (m_creature->GetEntry()==24684)
                     AddSpellToCast(target,44482);    //normal

                 else if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                      if(m_creature->GetEntry()==25565)
                     AddSpellToCast(target,46033);    //hero
                    Judgement_Timer=5000;

            } else
                Judgement_Timer -= diff;

       if(Seal_Timer < diff)
            {
                 if (m_creature->GetEntry()==24684)
                     AddSpellToCast(me,44480);    //normal

                 else if (m_creature->GetEntry()==25565)
                     AddSpellToCast(me,46030);    //hero
                     Seal_Timer=15000;

            } else
                Seal_Timer -= diff;

            if(Holy_Light_Timer < diff)
            {
                Unit* healTarget = SelectLowestHpFriendly(40.0f);
                 if (m_creature->GetEntry()==24684)
                     AddSpellToCast(healTarget,44479);    //normal

                 else if (m_creature->GetEntry()==25565)
                     AddSpellToCast(healTarget,46029);    //hero
                    Holy_Light_Timer=4000;

            } else
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
         Drink_Timer=1000;
         Wretched_Stab_Timer=4000;
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Drink_Timer < diff)
            {
               AddSpellToCast(me,44505);
                 Drink_Timer=15000;

            } else
                Drink_Timer -= diff;

       if(Wretched_Stab_Timer < diff)
            {
                AddSpellToCast(m_creature->getVictim(),44533);
                 Wretched_Stab_Timer=3000;

            } else
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
         Drink_Timer=1000;
         Wretched_Strike_Timer=3000;
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Drink_Timer < diff)
            {
               AddSpellToCast(me,44505);
                 Drink_Timer=15000;

            } else
                Drink_Timer -= diff;

       if(Wretched_Strike_Timer < diff)
            {
               AddSpellToCast(m_creature->getVictim(),44533);
                 Wretched_Strike_Timer=3000;

            } else
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
         Drink_Timer=1000;
         Wretched_Fireball_Timer=3000;
         Wretched_Frostbolt_Timer=1500;
        }

    void UpdateAI(const uint32 diff)
    {
      if(!UpdateVictim())
      return;

            if(Drink_Timer < diff)
            {
               AddSpellToCast(me,44505);
                 Drink_Timer=15000;

            } else
                Drink_Timer -= diff;

       if(Wretched_Fireball_Timer < diff)
            {
               AddSpellToCast(m_creature->getVictim(),44503);
                 Wretched_Fireball_Timer=3000;

            } else
                Wretched_Fireball_Timer -= diff;

       if(Wretched_Frostbolt_Timer < diff)
            {
               AddSpellToCast(m_creature->getVictim(),44504);
                 Wretched_Frostbolt_Timer=4000;

            } else
                Wretched_Frostbolt_Timer -= diff;

    CastNextSpellIfAnyAndReady(diff);
    DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunwell_mageblood_guard(Creature *_Creature)
{
    return new mob_sunwell_mageblood_guardAI (_Creature);
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
    newscript->Name = "mob_sunwell_mageblood_guard";
    newscript->GetAI = &GetAI_mob_sunwell_mageblood_guard;
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
