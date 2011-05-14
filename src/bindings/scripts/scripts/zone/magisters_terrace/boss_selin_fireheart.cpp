/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Selin_Fireheart
SD%Complete: 90
SDComment: Heroic and Normal Support. Needs further testing.
SDCategory: Magister's Terrace
EndScriptData */

#include "precompiled.h"
#include "def_magisters_terrace.h"

#define SAY_AGGRO                       -1585000
#define SAY_ENERGY                      -1585001
#define SAY_EMPOWERED                   -1585002
#define SAY_KILL_1                      -1585003
#define SAY_KILL_2                      -1585004
#define SAY_DEATH                       -1585005
#define EMOTE_CRYSTAL                   -1585006

//Crystal efect spells
#define SPELL_FEL_CRYSTAL_COSMETIC      44374
#define SPELL_FEL_CRYSTAL_DUMMY         44329
#define SPELL_FEL_CRYSTAL_VISUAL        44355
#define SPELL_MANA_RAGE                 44320               // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target

//Selin's spells
#define SPELL_DRAIN_LIFE                (Heroic?46155:44294)
#define SPELL_FEL_EXPLOSION             44314

#define SPELL_DRAIN_MANA                46153               // Heroic only

#define CRYSTALS_NUMBER                 5
#define DATA_CRYSTALS                   6

#define CREATURE_FEL_CRYSTAL            24722  

struct TRINITY_DLL_DECL boss_selin_fireheartAI : public ScriptedAI
{
    boss_selin_fireheartAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
        Heroic = c->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    uint32 DrainLifeTimer;
    uint32 DrainManaTimer;
    uint32 FelExplosionTimer;
    uint32 DrainCrystalTimer;
    uint32 CheckTimer;

    bool IsDraining;
    bool DrainingCrystal;
    bool Heroic;
    bool DrainingJustFinished;
    uint64 CrystalGUID;                                     // This will help us create a pointer to the crystal we are draining. We store GUIDs, never units in case unit is deleted/offline (offline if player of course).

    void Reset()
    {
        std::list<Creature*> fel_crystals = DoFindAllCreaturesWithEntry(CREATURE_FEL_CRYSTAL, 100);
        for(std::list<Creature*>::iterator it = fel_crystals.begin(); it != fel_crystals.end(); it++)
        {
            (*it)->Respawn();
            (*it)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            (*it)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if(pInstance)
            pInstance->SetData(DATA_SELIN_EVENT, NOT_STARTED);

        DrainLifeTimer = 3000 + rand()%4000;
        DrainManaTimer = DrainLifeTimer + 5000;
        FelExplosionTimer = 2100;
        DrainCrystalTimer = 10000 + rand()%5000;
        DrainCrystalTimer = 20000 + rand()%5000;
        CheckTimer = 1000;

        IsDraining = false;
        DrainingCrystal = false;
        DrainingJustFinished = false;
        CrystalGUID = 0;

        m_creature->SetPower(POWER_MANA, 0);
    }
    
    void SelectNearestCrystal()
    {
        Creature* CrystalChosen = NULL;

        Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*m_creature, CREATURE_FEL_CRYSTAL, true, 100.0f);
        Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(CrystalChosen, creature_check);

        Cell::VisitGridObjects(me, searcher, 100.0f);

        if( CrystalChosen )
        {
            CrystalGUID = CrystalChosen->GetGUID();

            DoScriptText(SAY_ENERGY, m_creature);
            DoScriptText(EMOTE_CRYSTAL, m_creature);

            float x, y, z;                                  // coords that we move to, close to the crystal.
            switch(CrystalChosen->GetGUIDLow())
            {
                // for two crystals close point is not good enough, because selin ends up out of los
                case 96870:
                    x = 250;
                    y = 13
                    z = 4;
                    break;
                case 96873:
                    x = 250;
                    y = -13;
                    z = 4;
                    break;
                default:
                    CrystalChosen->GetClosePoint(x, y, z, m_creature->GetObjectSize(), CONTACT_DISTANCE);
            }

            m_creature->RemoveUnitMovementFlag(SPLINEFLAG_WALKMODE_MODE);
            m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
            DrainingCrystal = true;
        }
    }

    void ShatterRemainingCrystals()
    {
        std::list<Creature*> fel_crystals = DoFindAllCreaturesWithEntry(CREATURE_FEL_CRYSTAL, 100);
        for(std::list<Creature*>::iterator it = fel_crystals.begin(); it != fel_crystals.end(); it++)
            if((*it)->isAlive())
                (*it)->Kill(*it);
    }

    void EnterCombat(Unit* who)
    {
        m_creature->SetPower(POWER_MANA, 0);
        DoScriptText(SAY_AGGRO, m_creature);

        // already did in Reset(), will not hurt to do it again
        std::list<Creature*> fel_crystals = DoFindAllCreaturesWithEntry(CREATURE_FEL_CRYSTAL, 100);
        for(std::list<Creature*>::iterator it = fel_crystals.begin(); it != fel_crystals.end(); it++)
        {
            (*it)->Respawn();
            (*it)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            (*it)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if( pInstance )
            pInstance->SetData(DATA_SELIN_EVENT, IN_PROGRESS);

    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), m_creature);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(type == POINT_MOTION_TYPE && id == 1)
        {
            Unit* CrystalChosen = me->GetUnit(CrystalGUID);
            if(CrystalChosen && CrystalChosen->isAlive())
            {
                CrystalChosen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                CrystalChosen->CastSpell(m_creature, SPELL_MANA_RAGE, false);
                me->CastSpell(CrystalChosen, SPELL_FEL_CRYSTAL_COSMETIC, false);
                IsDraining = true;
            }
            else
            {
                // Make an error message in case something weird happened here
                error_log("TSCR: Selin Fireheart unable to drain crystal as the crystal is either dead or despawned");
                DrainingCrystal = false;
            }
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
            pInstance->SetData(DATA_SELIN_EVENT, DONE);         // Encounter complete!
    }

    void OnAuraRemove(Aura *aur, bool stack)
    {
        if(aur->GetSpellProto()->Id == SPELL_MANA_RAGE)
        {
            IsDraining = false;
            DrainingCrystal = false;

            DoScriptText(SAY_EMPOWERED, m_creature);

            DrainingJustFinished = true; // killing crystal here causes crash, we kill it on next update         

            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(DrainingJustFinished)
        {
            Unit* CrystalChosen = m_creature->GetUnit(CrystalGUID);
            if( CrystalChosen && CrystalChosen->isAlive() )
                CrystalChosen->DealDamage(CrystalChosen, CrystalChosen->GetHealth());
            CrystalGUID = 0;
            DrainingJustFinished = false;
        }

        if(!DrainingCrystal)
        {
            if(!UpdateVictim())
                return;

            uint32 maxPowerMana = m_creature->GetMaxPower(POWER_MANA);
            if( maxPowerMana && ((m_creature->GetPower(POWER_MANA)*100 / maxPowerMana) < 10) )
            {
                if( DrainLifeTimer < diff )
                {
                    if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 20, true))
                        AddSpellToCast(target, SPELL_DRAIN_LIFE);
                    DrainLifeTimer = 8000+rand()%4000;
                }else 
                    DrainLifeTimer -= diff;

                // Heroic only
                if( Heroic )
                {
                    if( DrainManaTimer < diff )
                    {
                        if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 45, true, POWER_MANA))
                            AddSpellToCast(target, SPELL_DRAIN_MANA);
                        DrainManaTimer = 8000+rand()%4000;
                    }else 
                        DrainManaTimer -= diff;
                }

                if(DrainCrystalTimer < diff)
                {
                    SelectNearestCrystal();
                    if(Heroic)   
                        DrainCrystalTimer = 10000 + rand()%5000;
                    else         
                        DrainCrystalTimer = 20000 + rand()%5000;
                }else 
                    DrainCrystalTimer -= diff;
            } 
            else 
            {
                if( FelExplosionTimer < diff )
                {
                    AddSpellToCast(m_creature, SPELL_FEL_EXPLOSION);
                    FelExplosionTimer = 1500;
                } else 
                    FelExplosionTimer -= diff;
            }

            CastNextSpellIfAnyAndReady();
            DoMeleeAttackIfReady();  
        }
    }
};

CreatureAI* GetAI_boss_selin_fireheart(Creature *_Creature)
{
    return new boss_selin_fireheartAI (_Creature);
};

struct TRINITY_DLL_DECL mob_fel_crystalAI : public ScriptedAI
{
    mob_fel_crystalAI(Creature *c) : ScriptedAI(c) 
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;
    uint32 Check_Timer;

    void Reset() 
    {
        Check_Timer = 1000;
    }
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff) 
    {
        if(Check_Timer < diff)
        {
            if(pInstance)
            {
                uint32 data = pInstance->GetData(DATA_SELIN_EVENT);
                if(data == IN_PROGRESS)
                    me->InterruptNonMeleeSpells(true, SPELL_FEL_CRYSTAL_VISUAL);
                else if(data == NOT_STARTED && !me->IsNonMeleeSpellCasted(true))
                    me->CastSpell((Unit*)NULL, SPELL_FEL_CRYSTAL_VISUAL, false);
            }
            Check_Timer = 2000;
        } else
            Check_Timer -= diff;
    }

    void JustDied(Unit* killer)
    {
    }
};

CreatureAI* GetAI_mob_fel_crystal(Creature *_Creature)
{
    return new mob_fel_crystalAI (_Creature);
};

void AddSC_boss_selin_fireheart()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_selin_fireheart";
    newscript->GetAI = &GetAI_boss_selin_fireheart;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_fel_crystal";
    newscript->GetAI = &GetAI_mob_fel_crystal;
    newscript->RegisterSelf();
}

