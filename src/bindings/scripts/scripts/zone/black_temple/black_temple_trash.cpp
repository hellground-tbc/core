/* Copyright (C) 2008 - 2010 HellgroundDev <http://gamefreedom.pl/>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* ScriptData
SDName: Black_Temple_Trash
SD%Complete: 13% (9/68)
SDComment: Trash NPCs divided by to boss links
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

/* ============================
*
*      HIGL  LORD  NAJENTUS
*
* ============================*/

/* Content Data
    * Aqueous Lord
    * Aqueous Spawn
    * Coilskar General
    * Coilskar Harpooner
    * Coilskar Sea-Caller
    * Coilskar Soothsayer
    * Coilskar Wrangler
    * Dragon Turtle
    * Leviathan
*/

/****************
* Aqueous Lord - id 22878
*****************/

#define SPELL_VILE_SLIME        40099
#define SPELL_CRASHING_WAVE     40100

#define NPC_AQUEOUS_SPAWN       22883



struct TRINITY_DLL_DECL mob_aqueous_lordAI : public ScriptedAI
{
    mob_aqueous_lordAI(Creature *c) : ScriptedAI(c) {}

    uint32 VileSlime;
    uint32 SummonTimer;
    uint32 CrashingWave;

    void Reset()
    {
        VileSlime = 5000;
        SummonTimer = urand(5000,10000);
        CrashingWave = 15000;
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(VileSlime < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
                AddSpellToCast(target, SPELL_VILE_SLIME);
            VileSlime = urand(20000, 30000);
        }
        else
            VileSlime -= diff;

        if(SummonTimer < diff)
        {
            WorldLocation temp;
            for(uint8 i=0; i<2;++i)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
                {
                    target->GetClosePoint(temp.x, temp.y, temp.z, 15.0);
                    Creature* Spawn = m_creature->SummonCreature(NPC_AQUEOUS_SPAWN, temp.x, temp.y, temp.z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    Spawn->AI()->AttackStart(target);
                }
            }
            SummonTimer = urand(20000, 40000);
        }
        else
            SummonTimer -= diff;

        if(CrashingWave < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CRASHING_WAVE);
            CrashingWave = 15000;
        }
        else
            CrashingWave -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Aqueous Spawn - id 22883
*****************/

#define SPELL_SLUDGE_NOVA       40102
#define SPELL_MERGE             40106

#define NPC_AQUEOUS_LORD       22878

struct TRINITY_DLL_DECL mob_aqueous_spawnAI : public ScriptedAI
{
    mob_aqueous_spawnAI(Creature *c) : ScriptedAI(c) {}

    uint32 SludgeNova;
    uint32 MergeTimer;
    bool merging;

    void Reset()
    {
        SludgeNova = 5000;
        MergeTimer = urand(10000, 50000);
        merging = false;

    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim() && !merging)
            return;

        if(SludgeNova < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                    AddSpellToCast(target, SPELL_SLUDGE_NOVA);
            SludgeNova = urand(15000,18000);
        }
        else
            SludgeNova -= diff;


        if(!merging && MergeTimer < diff)
        {
            if(Unit* pLord = FindCreature(NPC_AQUEOUS_LORD, 80, m_creature))
            {
                AddSpellToCast(pLord, SPELL_MERGE, false, true);
                merging = true;
            }
        }
        else
            MergeTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Coilscar General - id 22873
*****************/

#define SAY_FREE_FRIEND             -1650022

#define SPELL_BOOMING_VOICE            40080
#define SPELL_FREE_FRIEND              40081
#define SPELL_CLEAVE                   40504    // guessed

struct TRINITY_DLL_DECL mob_coilskar_generalAI : public ScriptedAI
{
    mob_coilskar_generalAI(Creature *c) : ScriptedAI(c) {}

    uint32 FreeFriend;
    uint32 Cleave;
    uint32 BoomingVoice;

    void Reset()
    {
        FreeFriend = 1000;
        Cleave = 10000;
        BoomingVoice = 40000;
    }

    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Cleave < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave = urand(15000, 25000);
        }
        else
            Cleave -= diff;

        if(FreeFriend < diff )
        {
            std::list<Creature*> pList = DoFindFriendlyCC(100);
            if (!pList.empty())
            {
                Unit* target = *(pList.begin());

                if(target && target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_STUN, SPELLFAMILY_ROGUE, 0x80))    //ignore Sap
                {
                    pList.remove((Creature*)target);
                    if (!pList.empty())
                        Unit* target = *(pList.begin());
                    else
                        target = NULL;
                }

                if(target)
                {
                    DoScriptText(SAY_FREE_FRIEND, m_creature);
                    AddSpellToCast(target, SPELL_FREE_FRIEND);
                    pList.remove((Creature*)target);

                    if(!m_creature->HasAura(SPELL_BOOMING_VOICE, 0))
                    {
                        AddSpellToCast(m_creature, SPELL_BOOMING_VOICE);
                        BoomingVoice = urand(30000, 60000);
                    }
                }
            }
            FreeFriend = urand(10000, 15000);
        }
        else
            FreeFriend -= diff;

        if(BoomingVoice < diff)     //make Booming Voice from time to time even if no creature in CC
        {
            AddSpellToCast(m_creature, SPELL_BOOMING_VOICE);
            BoomingVoice = urand(40000, 60000);
        }
        else
            BoomingVoice -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Coilscar Harpooner - id 22874
*****************/

#define SPELL_HARPOONERS_MARK          40084
#define SPELL_HOOKED_NET               40082
#define SPELL_SPEAR_THROW              40083

struct TRINITY_DLL_DECL mob_coilskar_harpoonerAI : public ScriptedAI
{
    mob_coilskar_harpoonerAI(Creature *c) : ScriptedAI(c) {}

    uint32 SpearThrow;
    uint32 HookedNet;
    uint32 HarpoonersMark;

    uint64 MarkTargetGUID;

    void Reset()
    {
        SpearThrow = urand(1000, 5000);
        HookedNet = urand(15000, 20000);
        HarpoonersMark = 15000;
        MarkTargetGUID = 0;
    }

    void EnterCombat(Unit* who)
    {
        DoZoneInCombat();
        if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 80, true))
        {
            if(!target->HasAura(40084, 0))
            {
                DoCast(target, SPELL_HARPOONERS_MARK, false);
                MarkTargetGUID = target->GetGUID();     //constant target for Mark per creature, selected at aggro
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(HarpoonersMark < diff)
        {
            if(Player* target = m_creature->GetPlayer(MarkTargetGUID))
            {
                if(!target->HasAura(40084, 0))
                    DoCast(target, SPELL_HARPOONERS_MARK, true);
            }
            HarpoonersMark = 15000;     //check Mark each 15s
        }
        else
            HarpoonersMark -= diff;

        if(HookedNet < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                AddSpellToCast(target, SPELL_HOOKED_NET);
            HookedNet = urand(10000, 20000);
        }
        else
            HookedNet -= diff;

        if(SpearThrow < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                AddSpellToCast(target, SPELL_SPEAR_THROW);
            SpearThrow = urand(5000, 15000);
        }
        else
            SpearThrow -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Coilskar Sea-Caller - id 22875
*****************/

#define SPELL_FORKED_LIGHTNING         40088
#define SPELL_HURRICANE                40090
#define SPELL_SUMMON_GEYSER            40091

struct TRINITY_DLL_DECL mob_coilskar_seacallerAI : public ScriptedAI
{
    mob_coilskar_seacallerAI(Creature *c) : ScriptedAI(c)
    {
        SpellEntry* hurricaneSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_HURRICANE);
        if(hurricaneSpell)
            hurricaneSpell->AttributesEx |= SPELL_ATTR_EX_CHANNELED_1;
    }

    uint32 ForkedLightning;
    uint32 Hurricane;
    uint32 SummonGeyser;

    void Reset()
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        ForkedLightning = urand(1000, 3000);
        Hurricane = urand(20000, 30000);
        SummonGeyser = urand(3000, 8000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ForkedLightning < diff)
        {
            Unit* target = NULL;
            AddSpellToCast(target, SPELL_FORKED_LIGHTNING);
            ForkedLightning = urand(10000, 20000);
        }
        else
            ForkedLightning -= diff;

        if(Hurricane < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                AddSpellToCast(target, SPELL_HURRICANE);
            Hurricane = urand(40000, 60000);
        }
        else
            Hurricane -= diff;

        if(SummonGeyser < diff)
        {
            AddSpellToCast(m_creature, SPELL_SUMMON_GEYSER);
            SummonGeyser = urand(8000, 16000);
        }
        else
            SummonGeyser -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/***
* Coilskar Geyser - id 23080
****/

#define SPELL_GEYSER                   40089

#define MOB_COILSCAR_SEACALLER         22875

struct TRINITY_DLL_DECL mob_coilskar_geyserAI : public Scripted_NoMovementAI
{
    mob_coilskar_geyserAI(Creature *c) : Scripted_NoMovementAI(c) {}

    void Reset()
    {
        Unit* SeaCaller = FindCreature(MOB_COILSCAR_SEACALLER, 20, m_creature);
        if(SeaCaller)
        {
            m_creature->SetLevel(SeaCaller->getLevel());
            m_creature->setFaction(SeaCaller->getFaction());
        }
        else
        {
            m_creature->SetLevel(71);
            m_creature->setFaction(14);
        }
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->CastSpell(m_creature, SPELL_GEYSER, false);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
};

/****************
* Coilskar Soothsayer - id 22876
*****************/

#define SPELL_HOLY_NOVA                40096
#define SPELL_RESTORATION              40097

struct TRINITY_DLL_DECL mob_coilskar_soothsayerAI : public ScriptedAI
{
    mob_coilskar_soothsayerAI(Creature *c) : ScriptedAI(c) {}

    uint32 HolyNova;
    uint32 Restoration;

    void Reset()
    {
        HolyNova = urand(5000, 15000);
        Restoration = (8000, 12000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(HolyNova < diff)
        {
            DoCastAOE(SPELL_HOLY_NOVA);
            HolyNova = urand(9000, 14000);
        }
        else
            HolyNova -= diff;

        if(Restoration < diff)
        {
            Unit* healTarget = DoSelectLowestHpFriendly(40.0f);
            if(healTarget)
                AddSpellToCast(healTarget, SPELL_RESTORATION);
            Restoration = urand(15000, 20000);     //check targets each 15-20s
        }
        else
            Restoration -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Coilscar Wrangler - id 22877
*****************/

#define SPELL_CLEAVE                   15284
#define SPELL_ELECTRIC_SPUR            40076
#define SPELL_LIGHTNING_PROD           40066

#define MOB_LEVIATHAN                  22884

struct TRINITY_DLL_DECL mob_coilskar_wranglerAI : public ScriptedAI
{
    mob_coilskar_wranglerAI(Creature *c) : ScriptedAI(c) {}

    uint32 Cleave;
    uint32 ElectricSpur;
    uint32 LightningProd;

    void Reset()
    {
        Cleave = urand(2000, 6000);
        ElectricSpur = urand(15000, 40000);
        LightningProd = urand(8000, 15000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Cleave < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave = urand(10000, 15000);
        }
        else
            Cleave -= diff;

        if(ElectricSpur && ElectricSpur < diff)
        {
            Unit* target = NULL;
            AddSpellToCast(target, SPELL_ELECTRIC_SPUR, false);
            ElectricSpur = 0;
        }
        else
            ElectricSpur -= diff;

        if(LightningProd < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_LIGHTNING_PROD);
            LightningProd = urand(10000, 20000);
        }
        else
            LightningProd -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Dragon Turtle - id 22885
*****************/

#define SPELL_SHELL_SHIELD             40087
#define SPELL_WATER_SPIT               40086

struct TRINITY_DLL_DECL mob_dragon_turtleAI : public ScriptedAI
{
    mob_dragon_turtleAI(Creature *c) : ScriptedAI(c) {}

    bool CanBeShielded;
    uint32 ShellShield;
    uint32 WaterSpit;

    void Reset()
    {
        WaterSpit = urand(2000, 10000);
        ShellShield = 3000;
        CanBeShielded = false;
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ShellShield < diff)
        {
            if(!CanBeShielded)
            {
                if(m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 50)
                    CanBeShielded = true;
                ShellShield = 3000;
            }
            else
            {
                AddSpellToCast(m_creature, SPELL_SHELL_SHIELD);
                ShellShield = urand(20000, 30000);
            }
        }
        else
            ShellShield -= diff;

        if(WaterSpit < diff)
        {
            if(!CanBeShielded)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                    AddSpellToCast(target, SPELL_WATER_SPIT);
                WaterSpit = urand(8000,12000);
            }
            else
            {
                if(!urand(0, 2))    //30% on cast to random player
                {
                    if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                        AddSpellToCast(target, SPELL_WATER_SPIT);
                }
                else
                    AddSpellToCast(m_creature->getVictim(), SPELL_WATER_SPIT);
                WaterSpit = urand(2500, 3500);
            }
        }
        else
            WaterSpit -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/****************
* Leviathan - id 22884
*****************/

#define SPELL_DEBILITATING_SPRAY       40079
#define SPELL_POISON_SPIT              40078
#define SPELL_TAIL_SWEEP               40077

struct TRINITY_DLL_DECL mob_leviathanAI : public ScriptedAI
{
    mob_leviathanAI(Creature *c) : ScriptedAI(c) {}

    uint32 DebilitatingSpray;
    uint32 PoisonSpit;
    uint32 TailSweep;

    void Reset()
    {
        DebilitatingSpray = urand(3000, 9000);
        PoisonSpit = urand(6000, 15000);
        TailSweep = 6000;
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(DebilitatingSpray < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 45, true))
                AddSpellToCast(target, SPELL_DEBILITATING_SPRAY);
            DebilitatingSpray = urand(10000, 20000);
        }
        else
            DebilitatingSpray -= diff;

        if(PoisonSpit < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                AddSpellToCast(target, SPELL_POISON_SPIT);
            PoisonSpit = urand(12000, 18000);
        }
        else
            PoisonSpit -= diff;

        if(TailSweep < diff)
        {
            AddSpellToCast((Unit*)NULL, SPELL_TAIL_SWEEP);
            TailSweep = urand(5000, 8000);
        }
        else
            TailSweep -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};


/*####
##  GetAI's
###########*/

CreatureAI* GetAI_mob_aqueous_lord(Creature *_Creature)
{
    return new mob_aqueous_lordAI(_Creature);
}

CreatureAI* GetAI_mob_aqueous_spawn(Creature *_Creature)
{
    return new mob_aqueous_spawnAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_general(Creature *_Creature)
{
    return new mob_coilskar_generalAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_harpooner(Creature *_Creature)
{
    return new mob_coilskar_harpoonerAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_seacaller(Creature *_Creature)
{
    return new mob_coilskar_seacallerAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_geyser(Creature *_Creature)
{
    return new mob_coilskar_geyserAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_soothsayer(Creature *_Creature)
{
    return new mob_coilskar_soothsayerAI(_Creature);
}

CreatureAI* GetAI_mob_coilskar_wrangler(Creature *_Creature)
{
    return new mob_coilskar_wranglerAI(_Creature);
}

CreatureAI* GetAI_mob_dragon_turtle(Creature *_Creature)
{
    return new mob_dragon_turtleAI(_Creature);
}

CreatureAI* GetAI_mob_leviathan(Creature *_Creature)
{
    return new mob_leviathanAI(_Creature);
}


/* ============================
*
*          SUPREMUS
*
* ============================*/

/* Content Data
    * Bonechewer Taskmaster
    * Bonechewer Worker
    * Dragonmaw Sky Stalker
    * Dragonmaw Wind Reaver
    * Dragonmaw Wyrmcaller
    * Illidari Fearbringer
*/

#define BONECHEWER_ID                       23028

#define SPELL_BONECHEWER_DISGRUNTLED        40851
#define SPELL_BONECHEWER_FURY               40845

#define SPELL_WORKER_THROW_PICK             40844

#define SPELL_SKYSTALKER_SHOOT              40873
#define SPELL_SKYSTALKER_IMMOLATION         40872

#define SPELL_WINDREAVER_DOOM_BOLT          40876
#define SPELL_WINDREAVER_FIREBALL           40877
#define SPELL_WINDREAVER_FREEZE             40875

#define SPELL_WYRMCALLER_CLEAVE             15284
#define SPELL_WYRMCALLER_FIXATE             40892
#define SPELL_WYRMCALLER_FIXATE_TRIGGER     40893
#define SPELL_WYRMCALLER_JAB                40895
#define AURA_WYRMCALLER_FIXATED             40893

#define SPELL_FEARBRINGER_ILLIDARI_FLAMES   40938
#define SPELL_FEARBRINGER_RAIN_OF_CHAOS     40946
#define SPELL_FEARBRINGER_WAR_STOMP         40936

/*#####
##  mob Bonechewer Taskmaster - id 23028
###########*/

struct TRINITY_DLL_DECL mob_bonechewer_taskmasterAI : public ScriptedAI
{
    mob_bonechewer_taskmasterAI(Creature *c) : ScriptedAI(c){}

    uint32 disgruntledTimer;
    bool disgruntledCasted;

    void Reset()
    {
        disgruntledTimer = 20000;
        disgruntledCasted = false;
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        if (urand(0, 100) < 25)
        {
            m_creature->CastSpell(m_creature, SPELL_BONECHEWER_DISGRUNTLED, false);
            disgruntledCasted = true;
        }
    }

    void JustDied(Unit *victim){}

    void WorkerDied()
    {
        m_creature->CastSpell(m_creature, SPELL_BONECHEWER_FURY, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (!disgruntledCasted)
        {
            if (disgruntledTimer < diff || m_creature->GetHealth()*100/m_creature->GetMaxHealth() < 75)
            {
                m_creature->CastSpell(m_creature, SPELL_BONECHEWER_DISGRUNTLED, false);
                disgruntledCasted = true;
            }
            else
                disgruntledTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

/*#####
##  mob Bonechewer Worker - id 22963
###########*/

struct TRINITY_DLL_DECL mob_bonechewer_workerAI : public ScriptedAI
{
    mob_bonechewer_workerAI(Creature *c) : ScriptedAI(c){}

    uint32 throwTimer;

    void Reset()
    {
        throwTimer = 15000 + urand(0, 5000);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        if (who)
        {
            if (urand(0, 100) < 20)
            {
                m_creature->CastSpell(who, SPELL_WORKER_THROW_PICK, false);
            }
        }
    }

    void JustDied(Unit *victim)
    {
        std::list<Creature*> tmp = DoFindAllCreaturesWithEntry(BONECHEWER_ID, 20.0);

        if (tmp.empty())
            return;

        for (std::list<Creature*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
        {
            if((*itr) && (*itr)->isAlive())
                ((mob_bonechewer_taskmasterAI*)(*itr)->AI())->WorkerDied();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (throwTimer < diff)
        {
            Unit * victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 35, true);

            if (victim)
                m_creature->CastSpell(victim, SPELL_WORKER_THROW_PICK, false);

            throwTimer = 15000 + urand(0, 5000);
        }
        else
            throwTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*#####
##  mob Dragonmaw Sky Stalker - id 23030
###########*/

struct TRINITY_DLL_DECL mob_dragonmaw_skystalkerAI : public ScriptedAI
{
    mob_dragonmaw_skystalkerAI(Creature *c) : ScriptedAI(c){}

    uint32 immolationArrowTimer;
    uint32 distCheckTimer;

    void Reset()
    {
        immolationArrowTimer = 10000 + urand(0, 5000);
        distCheckTimer = 1000;
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        SetAutocast(SPELL_SKYSTALKER_SHOOT, 2500, true);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        m_creature->StopMoving();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveChase(who, 15);
    }

    void JustDied(Unit *victim){}

    Unit * GetNewTarget()
    {
        Map::PlayerList const &PlayerList = ((InstanceMap*)m_creature->GetMap())->GetPlayers();
        std::list<Unit*> rangedList;
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl && i_pl->isAlive() && !i_pl->isGameMaster() && !m_creature->IsWithinDistInMap(i_pl, 15) && m_creature->IsWithinDistInMap(i_pl, 40))
                rangedList.push_back((Unit*)i_pl);
        }

        if (rangedList.empty())
            return NULL;

        int n = urand(1, rangedList.size()) - 1;

        for (int i = 0; i < n; i++)
            rangedList.pop_front();

        if (!rangedList.empty())
            return rangedList.front();

        return NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        Unit * victim = m_creature->getVictim();

        if (!victim)
            return;

        if (distCheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(victim, 37))
            {
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(victim, 25);
                distCheckTimer = 3000;
            }
            else
            {
                if (m_creature->IsWithinDistInMap(victim, 10))
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        DoResetThreat();
                        victim = GetNewTarget();

                        if (!victim)
                        {
                            victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true);
                            m_creature->StopMoving();
                            m_creature->GetMotionMaster()->Clear();
                            m_creature->GetMotionMaster()->MoveChase(victim, 25);
                        }
                        distCheckTimer = 3000;
                    }
                    else
                        distCheckTimer = 500;
                }
            }
        }
        else
            distCheckTimer -= diff;

        if (immolationArrowTimer < diff)
        {
            ForceSpellCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true), SPELL_SKYSTALKER_IMMOLATION);
            immolationArrowTimer = 10000 + urand(0, 5000);
        }
        else
            immolationArrowTimer -= diff;

        CastNextSpellIfAnyAndReady(diff);
    }
};

/*#####
##  mob Dragonmaw Wind Reaver - id 23330
###########*/

struct TRINITY_DLL_DECL mob_dragonmaw_windreaverAI : public ScriptedAI
{
    mob_dragonmaw_windreaverAI(Creature *c) : ScriptedAI(c){}

    uint32 doomBoltTimer;
    uint32 freezeTimer;
    uint32 distCheckTimer;

    void Reset()
    {
        doomBoltTimer = 5000 + urand(0, 5000);
        freezeTimer = 10000 + urand(0, 5000);
        distCheckTimer = 3000;
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        SetAutocast(SPELL_WINDREAVER_FIREBALL, 2500, true);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        m_creature->StopMoving();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveChase(who, 15);
    }

    void JustDied(Unit *victim){}

    Unit * CheckMeleeRange()
    {
        Map::PlayerList const &PlayerList = ((InstanceMap*)m_creature->GetMap())->GetPlayers();
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl && i_pl->isAlive() && !i_pl->isGameMaster() && m_creature->IsWithinDistInMap(i_pl, 6))
                return (Unit*)i_pl;
        }

        return NULL;
    }

    Unit * GetNewTarget()
    {
        Map::PlayerList const &PlayerList = ((InstanceMap*)m_creature->GetMap())->GetPlayers();
        std::list<Unit*> rangedList;
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl && i_pl->isAlive() && !i_pl->isGameMaster() && !m_creature->IsWithinDistInMap(i_pl, 15) && m_creature->IsWithinDistInMap(i_pl, 40))
                rangedList.push_back((Unit*)i_pl);
        }

        if (rangedList.empty())
            return NULL;

        int n = urand(0, rangedList.size() - 1) ;

        for (int i = 0; i < n; i++)
            rangedList.pop_front();

        if (!rangedList.empty())
            return rangedList.front();

        return NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        Unit * victim = m_creature->getVictim();

        if (!victim)
            return;

        if (distCheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(victim, 37))
            {
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(victim, 25);
            }
            else
            {
                if (m_creature->IsWithinDistInMap(victim, 10))
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        DoResetThreat();
                        victim = GetNewTarget();

                        if (!victim)
                        {
                            victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true);
                            m_creature->StopMoving();
                            m_creature->GetMotionMaster()->Clear();
                            m_creature->GetMotionMaster()->MoveChase(victim, 25);
                        }
                        distCheckTimer = 3000;
                    }
                    else
                        distCheckTimer = 500;
                }
            }
            distCheckTimer = 3000;
        }
        else
            distCheckTimer -= diff;

        if (doomBoltTimer < diff)
        {
            AddSpellToCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true), SPELL_WINDREAVER_DOOM_BOLT);
            doomBoltTimer = 5000 + urand(0, 5000);
        }
        else
            doomBoltTimer -= diff;

        if (freezeTimer < diff)
        {
            Unit * tmpTarget = CheckMeleeRange();
            if (tmpTarget)
            {
                m_creature->CastSpell(tmpTarget, SPELL_WINDREAVER_FREEZE, false);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(tmpTarget, 15);
                distCheckTimer = 5000;
            }
            else
            {
                ForceSpellCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true), SPELL_WINDREAVER_FREEZE);
            }

            freezeTimer = 10000 + urand(0, 5000);
        }
        else
            freezeTimer -= diff;

        CastNextSpellIfAnyAndReady(diff);
    }
};

/*#####
##  mob Dragonmaw Wyrmcaller - id 22960
###########*/

struct TRINITY_DLL_DECL mob_dragonmaw_wyrmcallerAI : public ScriptedAI
{
    mob_dragonmaw_wyrmcallerAI(Creature *c) : ScriptedAI(c){}

    uint32 cleaveTimer;
    uint32 fixateTimer;
    uint32 jabTimer;

    void Reset()
    {
        cleaveTimer = 5000 + urand(0, 10000);
        fixateTimer = 15000 + urand(0, 10000);
        jabTimer = 5000 + urand(0, 5000);
    }

    void EnterCombat(Unit *who) { DoZoneInCombat(); }

    void JustDied(Unit *victim) {}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        Unit * victim = m_creature->getVictim();

        if (!victim)
            return;

        if (cleaveTimer < diff)
        {
            m_creature->CastSpell(victim, SPELL_WYRMCALLER_CLEAVE, false);
            cleaveTimer = 5000 + urand(0, 10000);
        }
        else
            cleaveTimer -= diff;

        if (jabTimer < diff)
        {
            m_creature->CastSpell(victim, SPELL_WYRMCALLER_JAB, false);
            jabTimer = 5000 + urand(0, 5000);
        }
        else
            jabTimer -= diff;

        if (fixateTimer < diff)
        {
            victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true);
            std::list<Creature*> FriendlyList = DoFindAllFriendlyInGrid(100);
            std::vector<Creature*> Friendly;

            for(std::list<Creature*>::iterator i = FriendlyList.begin(); i != FriendlyList.end(); ++i)
                if((*i)->isInCombat() && (*i)->IsAIEnabled)
                    Friendly.push_back(*i);

            Unit* target = *(Friendly.begin() + rand()%Friendly.size());

            if (victim && target)
            {
                target->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                target->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
                victim->CastSpell(target, SPELL_WYRMCALLER_FIXATE_TRIGGER, true);
            }

            fixateTimer = 15000 + urand(0, 10000);
        }
        else
            fixateTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

/*#####
##  mob Illidari Fearbringer - id 22954
###########*/

struct TRINITY_DLL_DECL mob_illidari_fearbringerAI : public ScriptedAI
{
    mob_illidari_fearbringerAI(Creature *c) : ScriptedAI(c){}

    uint32 flamesTimer;
    uint32 rainTimer;
    uint32 stompTimer;
    uint32 checkTimer;

    void Reset()
    {
        checkTimer = 2000;
        flamesTimer = 5000 + urand(0, 10000);
        rainTimer = 15000 + urand(0, 10000);
        stompTimer = 10000 + urand(0, 10000);
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
    }

    void EnterCombat(Unit *who) { DoZoneInCombat(); }

    void JustDied(Unit *victim){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(checkTimer < diff)
        {
            m_creature->SetSpeed(MOVE_RUN, 2.5);
            checkTimer = 2000;
        }
        else
            checkTimer -= diff;
        if (flamesTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_FEARBRINGER_ILLIDARI_FLAMES);
            flamesTimer = 10000 + urand(0, 10000);
        }
        else
            flamesTimer -= diff;

        if (rainTimer < diff)
        {
            AddSpellToCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 50, true), SPELL_FEARBRINGER_RAIN_OF_CHAOS);
            rainTimer = 20000 + urand(0, 10000);
        }
        else
            rainTimer -= diff;

        if (stompTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_FEARBRINGER_WAR_STOMP);

            stompTimer = 15000 + urand(0, 10000);
        }
        else
            stompTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/*####
##  GetAI's
###########*/

CreatureAI* GetAI_mob_bonechewer_taskmaster(Creature *_Creature)
{
    return new mob_bonechewer_taskmasterAI (_Creature);
}

CreatureAI* GetAI_mob_bonechewer_worker(Creature *_Creature)
{
    return new mob_bonechewer_workerAI (_Creature);
}

CreatureAI* GetAI_mob_dragonmaw_skystalker(Creature *_Creature)
{
    return new mob_dragonmaw_skystalkerAI (_Creature);
}

CreatureAI* GetAI_mob_dragonmaw_windreaver(Creature *_Creature)
{
    return new mob_dragonmaw_windreaverAI (_Creature);
}

CreatureAI* GetAI_mob_dragonmaw_wyrmcaller(Creature *_Creature)
{
    return new mob_dragonmaw_wyrmcallerAI (_Creature);
}

CreatureAI* GetAI_mob_illidari_fearbringer(Creature *_Creature)
{
    return new mob_illidari_fearbringerAI (_Creature);
}


/* ============================
*
*      SHADE  OF  AKAMA - scripted in Shade of Akama script
*
* ============================*/

/* Content Data
    * Ashtongue Channeler
    * Ashtongue Defender
    * Ashtongue Elementalist
    * Ashtongue Rogue
    * Ashtongue Sorcerer
    * Ashtongue Spiritbinder
*/

/* ============================
*
*      SANCTUARY OF SHADOW (no boss here)
*
* ============================*/

/* Content Data
    * Ashtongue Battlelord
    * Ashtongue Feral Spirit
    * Ashtongue Mystic
    * Ashtongue Primalist
    * Ashtongue Stalker
    * Ashtongue Stormcaller
    * Illidari Boneslicer
    * Illidari Centurion
    * Illidari Defiler
    * Illidari Heartseeker
    * Illidari Nightlord
    * Storm Fury
*/

/****************
* Ashtongue Battlelord - id 22844
*****************/

#define SPELL_CLEAVE            15284
#define SPELL_CONCUSSION_BLOW   32588
#define SPELL_CONCUSSION_THROW  41182
#define SPELL_FRENZY            34970

struct TRINITY_DLL_DECL mob_ashtongue_battlelordAI : public ScriptedAI
{
    mob_ashtongue_battlelordAI(Creature *c) : ScriptedAI(c) {}

    uint32 Cleave;
    uint32 ConcussionBlow;
    uint32 ConcussionThrow;
    uint32 Frenzy;

    void Reset()
    {
        Cleave = urand(3000, 10000);
        ConcussionBlow = urand(10500, 25000);
        ConcussionThrow = urand(10500, 25000);
        Frenzy = 5000;
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Cleave < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave = 10000;
        }
        else
            Cleave -= diff;

        if(ConcussionBlow < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CONCUSSION_BLOW);
            ConcussionBlow = 25000;
        }
        else
            ConcussionBlow -= diff;

        if(ConcussionThrow < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1, 100, true))
            {
                AddSpellToCast(target, SPELL_CONCUSSION_THROW);
            }
            ConcussionThrow = 22000;
        }
        else
            ConcussionThrow -= diff;

        if(Frenzy < diff)
        {
            if(!m_creature->HasAura(SPELL_FRENZY, 0))
                AddSpellToCast(m_creature->getVictim(), SPELL_FRENZY);
            Frenzy = 30000;
        }
        else
            Frenzy -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_battlelord(Creature *_Creature)
{
    return new mob_ashtongue_battlelordAI (_Creature);
}

/****************
* Ashtongue Feral Spirit - id 22849
*****************/

#define SPELL_CHARGE_RAGE       39575
#define SPELL_SPIRIT_BOND       39578

#define NPC_ASHTONGUE_PRIMALIST 22847

struct TRINITY_DLL_DECL mob_ashtongue_feral_spiritAI : public ScriptedAI
{
    mob_ashtongue_feral_spiritAI(Creature *c) : ScriptedAI(c) {}

    uint32 ChargeRage;
    uint32 SpiritBond;

    void Reset()
    {
        ChargeRage = urand(10000, 30000);
        SpiritBond = urand(15000, 25000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }

    void MoveInLineOfSight(Unit *who)
    {
        if (!m_creature->isInCombat() && m_creature->IsWithinDistInMap(who, 65) && m_creature->IsHostileTo(who))
            AttackStart(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ChargeRage < diff)
        {
            AddSpellToCast(m_creature, SPELL_CHARGE_RAGE);
            ChargeRage = urand(20000, 30000);
        }
        else
            ChargeRage -= diff;

        if(SpiritBond < diff)
        {
            AddSpellToCast(m_creature, SPELL_SPIRIT_BOND);
            if(Creature* Primalist = GetClosestCreatureWithEntry(m_creature, NPC_ASHTONGUE_PRIMALIST, 40.0f))
                Primalist->CastSpell(Primalist, SPELL_SPIRIT_BOND, true);
            SpiritBond = 30000;
        }
        else
            SpiritBond -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_feral_spirit(Creature *_Creature)
{
    return new mob_ashtongue_feral_spiritAI (_Creature);
}

/****************
* Ashtongue Mystic - id 22844 & Ashtongue Mystic Totems - id 22894, 22896, 22897
*****************/

#define SPELL_FROST_SHOCK               41116
#define SPELL_FLAME_SHOCK               41115
#define SPELL_BLOODLUST                 41185
#define SPELL_CHAIN_HEAL                41114
#define SPELL_SEARING_TOTEM             39588
#define SPELL_SUMMON_WINDFURY_TOTEM     39586
#define SPELL_CYCLONE_TOTEM             39589
// Totems
#define NPC_CYCLONE_TOTEM           22894
#define NPC_ASHTONGUE_SEARING_TOTEM 22896
#define NPC_SUMMONED_WINDFURY_TOTEM 22897
#define SPELL_CYCLON                39594
#define SPELL_ATTACK                39593
#define SPELL_WINDFURY_WEAPON       33727   //rank 5

struct TRINITY_DLL_DECL totem_ashtongue_mysticAI : public Scripted_NoMovementAI
{
    totem_ashtongue_mysticAI(Creature *c) : Scripted_NoMovementAI(c) {}

    uint64 OwnerGUID;
    uint32 SpellTimer;

    void Reset()
    {
        DoZoneInCombat(m_creature);
        switch(m_creature->GetEntry())
        {
            case NPC_CYCLONE_TOTEM:
                m_creature->SetMaxHealth(urand(4100,4200));
                SpellTimer = urand(2000, 11000);
                break;
            case NPC_ASHTONGUE_SEARING_TOTEM:
                m_creature->SetMaxHealth(urand(4800,4900));
                SpellTimer = 1000;
                break;
            case NPC_SUMMONED_WINDFURY_TOTEM:
                m_creature->SetMaxHealth(urand(1800,1900));
                break;
        }
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }
    void JustDied(Unit* killer)
    {
        if(m_creature->GetEntry() == NPC_SUMMONED_WINDFURY_TOTEM)
        {
            if(Unit* Mystic = Unit::GetUnit(*me, OwnerGUID))
            {
                if(Mystic->HasAura(SPELL_WINDFURY_WEAPON, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_WINDFURY_WEAPON);
            }
        }
    }
    void UpdateAI(const uint32 diff)
    {
            if(SpellTimer < diff)
            {
                switch(m_creature->GetEntry())
                {
                    case NPC_CYCLONE_TOTEM:
                        if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
                            AddSpellToCast(target, SPELL_CYCLON);
                        SpellTimer = 11000;
                        break;
                    case NPC_ASHTONGUE_SEARING_TOTEM:
                        AddSpellToCast(m_creature, SPELL_ATTACK);
                        SpellTimer = 3000;
                        break;
                }
            }
            else
                SpellTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};

CreatureAI* GetAI_totem_ashtongue_mystic(Creature *_Creature)
{
    return new totem_ashtongue_mysticAI (_Creature);
}

struct TRINITY_DLL_DECL mob_ashtongue_mysticAI : public ScriptedAI
{
    mob_ashtongue_mysticAI(Creature *c) : ScriptedAI(c) {}

    uint32 FrostShock;
    uint32 FlameShock;
    uint32 ChainHeal;
    uint32 SearingTotem;
    uint32 WindfuryTotem;
    uint32 CycloneTotem;

    void Reset()
    {
        FrostShock = urand(5000, 22000);
        FlameShock = urand(10000, 30000);
        ChainHeal = 5000;
        SearingTotem = urand(500, 30000);
        WindfuryTotem = urand(500, 30000);
        CycloneTotem = urand(500, 30000);
    }
    void EnterCombat(Unit*) 
    {
        DoZoneInCombat();
        DoCast(m_creature, SPELL_BLOODLUST);
    }
    void JustSummoned(Creature* totem)  //some workaround about windfury totem
    {
        if(totem)
        {
            if(totem->GetEntry() == NPC_SUMMONED_WINDFURY_TOTEM)
            {
                ((totem_ashtongue_mysticAI*)totem)->OwnerGUID = m_creature->GetGUID();
                if(!m_creature->HasAura(SPELL_WINDFURY_WEAPON, 0))
                    m_creature->CastSpell(m_creature, SPELL_WINDFURY_WEAPON, true);
            }
            if(m_creature->getVictim())
                ((totem_ashtongue_mysticAI*)totem)->AttackStart(m_creature->getVictim());
        }
    }
    void SummonedCreatureDespawn(Creature* totem)
    {
        if(totem)
        {
            if(totem->GetEntry() == NPC_SUMMONED_WINDFURY_TOTEM)
            {
                if(m_creature->HasAura(SPELL_WINDFURY_WEAPON, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_WINDFURY_WEAPON);
            }
        }
    }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(FrostShock < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 20, true))
                AddSpellToCast(target, SPELL_FROST_SHOCK);
            FrostShock = 22000;
        }
        else
            FrostShock -= diff;

        if(FlameShock < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 20, true))
                AddSpellToCast(target, SPELL_FLAME_SHOCK);
            FlameShock = 30000;
        }
        else
            FlameShock -= diff;

        if(ChainHeal < diff)
        {
            if(m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 70)
            {
                ForceSpellCast(m_creature, SPELL_CHAIN_HEAL);
                ChainHeal = 20000;
            }
            else if(Unit* healTarget = DoSelectLowestHpFriendly(40, 15000))
            {
                ForceSpellCast(healTarget, SPELL_CHAIN_HEAL);
                ChainHeal = 20000;
            }
            else
                ChainHeal = 3000;
        }
        else
            ChainHeal -= diff;

        if(SearingTotem < diff)
        {
            AddSpellToCast(m_creature, SPELL_SEARING_TOTEM);
            SearingTotem = urand(30000, 40000);
        }
        else
            SearingTotem -= diff;

        if(WindfuryTotem < diff)
        {
            AddSpellToCast(m_creature, SPELL_SUMMON_WINDFURY_TOTEM);
            WindfuryTotem = urand(30000, 40000);
        }
        else
            WindfuryTotem -= diff;

        if(CycloneTotem < diff)
        {
            AddSpellToCast(m_creature, SPELL_CYCLONE_TOTEM);
            CycloneTotem = urand(30000, 40000);
        }
        else
            CycloneTotem -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_mystic(Creature *_Creature)
{
    return new mob_ashtongue_mysticAI (_Creature);
}

/****************
* Ashtongue Primalist - id 22847
*****************/

#define SPELL_MULTI_SHOT            41187
#define SPELL_SHOOT                 41188
#define SPELL_WYVERN_STING          41186
#define SPELL_SWEEPING_WING_CLIP    39584

struct TRINITY_DLL_DECL mob_ashtongue_primalistAI : public ScriptedAI
{
    mob_ashtongue_primalistAI(Creature *c) : ScriptedAI(c) {}

    uint32 MultiShot;
    uint32 Shoot;
    uint32 WyvernSting;
    uint32 SweepingWingClip;

    bool canShoot;

    void Reset()
    {
        canShoot = true;
        MultiShot = urand(20000, 40000);
        Shoot = 500;
        WyvernSting = urand(7000, 15000);
        SweepingWingClip = urand(20000, 37000);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(!m_creature->isInCombat() && m_creature->GetDistance(who) > 25.0)    //to avoid far pulls not from group formations
            return;

        if(m_creature->isInCombat() && m_creature->IsWithinDistInMap(who, 5.0) && m_creature->IsHostileTo(who))
            canShoot = false;
        else
            canShoot = true;
    }

    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(MultiShot < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true);
            if(target && m_creature->GetDistance(target) > 5.0f)
            {
                ForceSpellCast(target, SPELL_MULTI_SHOT);
                MultiShot = 40000;
            }
            else
                MultiShot = 3000;
        }
        else
            MultiShot -= diff;

        if(Shoot < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30.0f, true);
            if(canShoot && target && m_creature->GetDistance(target) > 5.0f)
                ForceSpellCast(target, SPELL_SHOOT);
            Shoot = 1500;
        }
        else
            Shoot -= diff;

        if(WyvernSting < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1, 35.0f, true);
            if(target && m_creature->GetDistance(target) > 5.0f)
            {
                ForceSpellCast(target, SPELL_WYVERN_STING);
                WyvernSting = 15000;
            }
            else
                WyvernSting = 2000;
        }
        else
            WyvernSting -= diff;

        if(SweepingWingClip < diff)
        {
            if(m_creature->IsWithinDistInMap(m_creature->getVictim(), 5.0))
            {
                AddSpellToCast(m_creature->getVictim(), SPELL_SWEEPING_WING_CLIP);
                m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()+urand(10, 15), m_creature->GetPositionY()+urand(3, 7), m_creature->GetPositionZ());
                SweepingWingClip = 37000;
            }
            else
                SweepingWingClip = 2500;
        }
        else
            SweepingWingClip -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_primalist(Creature *_Creature)
{
    return new mob_ashtongue_primalistAI (_Creature);
}

/****************
* Ashtongue Stalker - id 23374
*****************/

#define SPELL_BLIND                 34654
#define SPELL_INSTANT_POISON        41189
#define SPELL_MIND_NUMBING_POISON   41190
#define SPELL_STEALTH               34189
#define SPELL_DUAL_WIELD            29651

struct TRINITY_DLL_DECL mob_ashtongue_stalkerAI : public ScriptedAI
{
    mob_ashtongue_stalkerAI(Creature *c) : ScriptedAI(c) {}

    uint32 Blind;
    uint32 InstantPoison;
    uint32 MindNumbingPoison;

    void Reset()
    {
        DoCast(m_creature, SPELL_STEALTH);
        DoCast(m_creature, SPELL_DUAL_WIELD);
        Blind = urand(10000, 20000);
        InstantPoison = urand(5000, 10000);
        MindNumbingPoison = urand(5000, 10000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Blind < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_BLIND);
            Blind = 20000;
        }
        else
            Blind -= diff;

        if(InstantPoison < diff)
        {
            if(Unit *pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 60.0f, true, POWER_RAGE))
            {
                AddSpellToCast(pTarget, SPELL_INSTANT_POISON);
                InstantPoison = 10000;
            }
            else if(Unit *pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 60.0f, true, POWER_ENERGY))
            {
                AddSpellToCast(pTarget, SPELL_INSTANT_POISON);
                InstantPoison = 10000;
            }
        }
        else
            InstantPoison -= diff;

        if(MindNumbingPoison < diff)
        {
            if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 60.0f, true, POWER_MANA))
            {
                AddSpellToCast(pTarget, SPELL_MIND_NUMBING_POISON);
                MindNumbingPoison = 10000;
            }
        }
        else
            MindNumbingPoison -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_stalker(Creature *_Creature)
{
    return new mob_ashtongue_stalkerAI (_Creature);
}

/****************
* Ashtongue Stormcaller - id 22846
*****************/

#define SPELL_CHAIN_LIGHTNING           41183
#define SPELL_LIGHTNING_BOLT            41184
#define SPELL_LIGHTNING_SHIELD          41151

struct TRINITY_DLL_DECL mob_ashtongue_stormcallerAI : public ScriptedAI
{
    mob_ashtongue_stormcallerAI(Creature *c) : ScriptedAI(c) {}

    uint32 ChainLightning;
    uint32 LightningBolt;
    uint32 LightningShield;

    void Reset()
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        ChainLightning = urand(6000, 25000);
        LightningBolt = urand(1500, 3000);
        LightningShield = 25000;
    }
    void EnterCombat(Unit*)
    {
        DoZoneInCombat();
        DoCast(m_creature, SPELL_LIGHTNING_SHIELD);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ChainLightning < diff)
        {
            ForceSpellCast(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
            ChainLightning = 10000;
        }
        else
            ChainLightning -= diff;

        if(LightningBolt < diff)
        {
            if(Unit *pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 60.0f, true, 0, 8.0f))
            {
                DoCast(pTarget, SPELL_LIGHTNING_BOLT);
                LightningBolt = 5000;
            }
        }
        else
            LightningBolt -= diff;

        if(LightningShield < diff)
        {
            DoCast(m_creature, SPELL_LIGHTNING_SHIELD);
            LightningShield = 22000;
        }
        else
            LightningShield -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ashtongue_stormcaller(Creature *_Creature)
{
    return new mob_ashtongue_stormcallerAI (_Creature);
}

/****************
* Illidari Boneslicer - id 22869
*****************/

#define SPELL_CLOAK_OF_SHADOWS              39666
#define SPELL_GOUGE                         24698
#define SPELL_SHADOWSTEP                    41176
#define SPELL_WOUND_POISON                  39665

struct TRINITY_DLL_DECL mob_illidari_boneslicerAI : public ScriptedAI
{
    mob_illidari_boneslicerAI(Creature *c) : ScriptedAI(c) {}

    uint32 CloakOfShadows;
    uint32 Gouge;
    uint32 Shadowstep;
    uint32 WoundPoison;

    void Reset()
    {
        CloakOfShadows = 15000;
        Gouge = urand(1000, 10000);;
        Shadowstep = urand(5000, 15000);
        WoundPoison = urand(1000, 3000);
    }
    void EnterCombat(Unit*)
    {
        DoCast(m_creature, SPELL_CLOAK_OF_SHADOWS);
        DoZoneInCombat();
    }
    void DamageMade(Unit* target, uint32 & damage, bool direct_damage)
    {
        if(damage && direct_damage)
            ForceSpellCast(target, SPELL_WOUND_POISON);
    }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(CloakOfShadows < diff)
        {
            AddSpellToCast(m_creature, SPELL_CLOAK_OF_SHADOWS);
            CloakOfShadows = 15000;
        }
        else
            CloakOfShadows -= diff;

        if(Gouge < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_GOUGE);
            Gouge = 10000;
        }
        else
            Gouge -= diff;

        if(Shadowstep < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_SHADOWSTEP);
            Shadowstep = 15000;
        }
        else
            Shadowstep -= diff;

        if(WoundPoison < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_WOUND_POISON);
            WoundPoison = 3000;
        }
        else
            WoundPoison -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illidari_boneslicer(Creature *_Creature)
{
    return new mob_illidari_boneslicerAI (_Creature);
}

/****************
* Illidari Centurion - id 23337
*****************/

#define SPELL_CLEAVE                15284
#define SPELL_SONIC_STRIKE          41168

struct TRINITY_DLL_DECL mob_illidari_centurionAI : public ScriptedAI
{
    mob_illidari_centurionAI(Creature *c) : ScriptedAI(c) {}

    uint32 Cleave;
    uint32 SonicStrike;

    void Reset()
    {
        Cleave = urand(3000, 10000);
        SonicStrike = urand(5000, 15000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Cleave < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave = 10000;
        }
        else
            Cleave -= diff;

        if(SonicStrike < diff)  //in cone in front of a caster, should "in front" be changed randomly?
        {
            AddSpellToCast(m_creature, SPELL_SONIC_STRIKE);
            SonicStrike = 15000;
        }
        else
            SonicStrike -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illidari_centurion(Creature *_Creature)
{
    return new mob_illidari_centurionAI (_Creature);
}

/****************
* Illidari Defiler - id 22853
*****************/

#define SPELL_FEL_IMMOLATE                  39670
#define SPELL_CURSE_OF_AGONY                39672
#define SPELL_BANISH                        39674
#define SPELL_RAIN_OF_CHAOS                 39671

struct TRINITY_DLL_DECL mob_illidari_defilerAI : public ScriptedAI
{
    mob_illidari_defilerAI(Creature *c) : ScriptedAI(c) {}

    uint32 FelImmolate;
    uint32 CurseOfAgony;
    uint32 Banish;
    uint32 RainOfChaos;

    void Reset()
    {
        FelImmolate = urand(15000, 25000);
        CurseOfAgony = urand(20000, 35000);
        Banish = urand(10000, 30000);
        RainOfChaos = urand(3000, 15000);
    }
    void EnterCombat(Unit*)
    {
        DoZoneInCombat();
    }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(FelImmolate < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_FEL_IMMOLATE);
            FelImmolate = 25000;
        }
        else
            FelImmolate -= diff;

        if(CurseOfAgony < diff)
        {
            AddSpellToCast(m_creature, SPELL_CURSE_OF_AGONY);
            CurseOfAgony = 35000;
        }
        else
            CurseOfAgony -= diff;

        if(Banish < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                AddSpellToCast(target, SPELL_BANISH);
            Banish = 30000;
        }
        else
            Banish -= diff;

        if(RainOfChaos < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                AddSpellToCast(target, SPELL_RAIN_OF_CHAOS);
            RainOfChaos = 15000;
        }
        else
            RainOfChaos -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illidari_defiler(Creature *_Creature)
{
    return new mob_illidari_defilerAI (_Creature);
}

/****************
* Illidari Heartseeker - id 23339
*****************/

#define SPELL_RAPID_SHOT                41173
#define SPELL_SHOOT                     41169
#define SPELL_SKELETON_SHOT             41171
#define SPELL_CURSE_OF_THE_BLEAKHEART   6946  //41170 proper spell blocks 6947 proc

struct TRINITY_DLL_DECL mob_illidari_heartseekerAI : public ScriptedAI
{
    mob_illidari_heartseekerAI(Creature *c) : ScriptedAI(c) {}

    uint32 RapidShot;
    uint32 Shoot;
    uint32 SkeletonShot;
    uint32 Curse;

    void Reset()
    {
        RapidShot = urand(10000, 20000);
        Shoot = 500;
        SkeletonShot = urand(15000, 35000);
        Curse = urand(1000, 20000);
    }
    void EnterCombat(Unit* who)
    {
        DoZoneInCombat();
        AttackStart(who, false);
    }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Shoot < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                if(m_creature->GetDistance(target) > 30)
                {
                    m_creature->GetMotionMaster()->MoveChase(target, 20, 0);
                    m_creature->SetSpeed(MOVE_RUN, 1,5);
                }
                else if(!target->IsWithinDistInMap(m_creature, 5.0))
                {
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();
                    ForceSpellCast(target, SPELL_SHOOT);
                }
            }
            Shoot = 2000;
        }
        else
            Shoot -= diff;

        if(RapidShot < diff)
        {
            AddSpellToCast(m_creature, SPELL_RAPID_SHOT);
            RapidShot = 20000;
        }
        else
            RapidShot -= diff;

        if(SkeletonShot < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true))
            {
                if(target->IsWithinDistInMap(m_creature, 8.0))
                    SkeletonShot = 1000;
                else
                {
                    AddSpellToCast(target, SPELL_SKELETON_SHOT);
                    SkeletonShot = 35000;
                }
            }
        }
        else
            SkeletonShot -= diff;

        if(Curse < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true))    // workaround...
                target->CastSpell(target, SPELL_CURSE_OF_THE_BLEAKHEART, true, 0, 0, m_creature->GetGUID());
            Curse = 20000;
        }
        else
            Curse -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illidari_heartseeker(Creature *_Creature)
{
    return new mob_illidari_heartseekerAI (_Creature);
}

/****************
* Illidari Nightlord - id 22855
*****************/

#define SPELL_CURSE_OF_MENDING          39647
#define SPELL_FEAR                      41150
#define SPELL_SHADOW_INFERNO            39645
#define SPELL_SUMMON_SHADOWFIENDS       39649

struct TRINITY_DLL_DECL mob_illidari_nightlordAI : public ScriptedAI
{
    mob_illidari_nightlordAI(Creature *c) : ScriptedAI(c) {}

    uint32 CurseOfMending;
    uint32 Fear;
    uint32 ShadowInferno;
    uint32 Shadowfiends;

    void Reset()
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        CurseOfMending = urand(10000, 20000);
        Fear = urand(3000, 15000);
        ShadowInferno = 7000;
        Shadowfiends = urand(15000, 20000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(CurseOfMending < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 10, true))
                ForceSpellCast(target, SPELL_CURSE_OF_MENDING);
            CurseOfMending = 20000;
        }
        else
            CurseOfMending -= diff;

        if(Fear < diff)
        {
            ForceSpellCast(m_creature, SPELL_FEAR);
            Fear = urand(10000, 15000);
        }
        else
            Fear -= diff;

        if(ShadowInferno < diff)
        {
            if(Fear < 8200)
                Fear += 8000;
            if(CurseOfMending < 8200)
                CurseOfMending += 8000;
            if(Shadowfiends < 8200)
                Shadowfiends += 8000;
            ForceSpellCast(m_creature, SPELL_SHADOW_INFERNO);
            ShadowInferno = 20000;
        }
        else
            ShadowInferno -= diff;

        if(Shadowfiends < diff)
        {
            ForceSpellCast(m_creature, SPELL_SUMMON_SHADOWFIENDS);
            Shadowfiends = 25000;
        }
        else
            Shadowfiends -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_illidari_nightlord(Creature *_Creature)
{
    return new mob_illidari_nightlordAI (_Creature);
}

/****************
* Storm Fury - id 22848
*****************/

#define SPELL_STORM_BLINK           39581

struct TRINITY_DLL_DECL mob_storm_furyAI : public ScriptedAI
{
    mob_storm_furyAI(Creature *c) : ScriptedAI(c) {}

    uint32 StormBlink;

    void Reset()
    {
        StormBlink = urand(15000, 25000);
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(StormBlink < diff)
        {
            AddSpellToCast(m_creature, SPELL_STORM_BLINK);
            StormBlink = 25000;
        }
        else
            StormBlink -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_storm_fury(Creature *_Creature)
{
    return new mob_storm_furyAI (_Creature);
}

/* ============================
*
*      TERON  GOREFIEND
*
* ============================*/

/* Content Data
    * Hand of Gorefiend
    * Shadowmoon Blood Mage
    * Shadowmoon Champion
    * Shadowmoon Deathshaper
    * Shadowmoon Grunt - no special AI
    * Shadowmoon Houndmaster
    * Shadowmoon Reaver
    * Shadowmoon Riding Hound
    * Shadowmoon Soldier
    * Shadowmoon Weapon Master
    * Wrathbone Flayer
*/

/****************
* Hand of Gorefiend - id 23172
*****************/

#define SPELL_ENRAGE                38166

struct TRINITY_DLL_DECL mob_hand_of_gorefiendAI : public ScriptedAI
{
    mob_hand_of_gorefiendAI(Creature *c) : ScriptedAI(c) {}

    uint32 Enrage;

    void Reset()
    {
        Enrage = 10000;
    }
    void EnterCombat(Unit*) { DoZoneInCombat(); }
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Enrage < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            Enrage = 30000;
        }
        else
            Enrage -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hand_of_gorefiend(Creature *_Creature)
{
    return new mob_hand_of_gorefiendAI (_Creature);
}

/****************
* Shadowmoon Blood Mage - id 22945
*****************/

#define SPELL_BLOOD_SIPHON              41068
#define SPELL_BLOODBOLT                 41072

// zle id ale wyglada podobnie
#define SPELL_GREEN_BEAM        38909
#define MOB_SKELETON            22953

struct TRINITY_DLL_DECL mob_shadowmoon_blood_mageAI: public ScriptedAI
{
    mob_shadowmoon_blood_mageAI(Creature *c) : ScriptedAI(c) { }

    uint32 BloodSiphon;
    uint32 Bloodbolt;

    void Reset()
    {
        BloodSiphon = urand(3000, 20000);
        Bloodbolt = urand(5000, 15000);
    }

    void EnterCombat(Unit *)
    {
        m_creature->InterruptNonMeleeSpells(false, SPELL_GREEN_BEAM);
        DoZoneInCombat();
    }

    void SpellHitTarget(Unit* target, const SpellEntry*)    //workaround
    {
        if(target->HasAura(41229, 1))
            target->RemoveAura(41229, 1);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
        {
            if (!m_creature->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
            {
                if (Unit *skeleton = FindCreature(MOB_SKELETON, 20.0f, m_creature))
                {
                    if (skeleton->isAlive())
                        DoCast(skeleton, SPELL_GREEN_BEAM);
                }
            }
            return;
        }

        if(BloodSiphon < diff)
        {
            AddSpellToCast(m_creature, SPELL_BLOOD_SIPHON);
            BloodSiphon = 20000;
        }
        else
            BloodSiphon -= diff;

        if(Bloodbolt < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 40.0, true))
                ForceSpellCast(target, SPELL_BLOODBOLT);
            Bloodbolt = 15000;
        }
        else
            Bloodbolt -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_blood_mage(Creature *_Creature)
{
    return new mob_shadowmoon_blood_mageAI(_Creature);
}

/****************
* Shadowmoon Champion - id 22880 and Whirling Blade - id 23369
*****************/

#define SPELL_CHAOTIC_LIGHT                 41063
#define SPELL_WHIRLING_BLADE                41053
#define SPELL_WHIRLWIND                     41058

#define MOB_WHIRLING_BLADE                  23369

struct TRINITY_DLL_DECL mob_shadowmoon_championAI: public ScriptedAI
{
    mob_shadowmoon_championAI(Creature *c) : ScriptedAI(c) { }

    uint32 ChaoticLight;
    uint32 WhirlingBlade;

    void Reset()
    {
        ChaoticLight = urand(2000, 5000);
        WhirlingBlade = 0;
    }

    void EnterCombat(Unit *) { DoZoneInCombat(); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ChaoticLight < diff)
        {
            AddSpellToCast(m_creature, SPELL_CHAOTIC_LIGHT);
            ChaoticLight = urand(5000, 10000);
        }
        else
            ChaoticLight -= diff;

        if(WhirlingBlade < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 60.0, true))
                ForceSpellCast(target, SPELL_WHIRLING_BLADE);
            WhirlingBlade = 30000;
        }
        else
            WhirlingBlade -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_champion(Creature *_Creature)
{
    return new mob_shadowmoon_championAI(_Creature);
}

struct TRINITY_DLL_DECL mob_whirling_bladeAI: public Scripted_NoMovementAI
{
    mob_whirling_bladeAI(Creature *c) : Scripted_NoMovementAI(c) { }

    uint32 Whirl;
    uint32 DieTimer;

    void Reset()
    {
        DoCast(m_creature, SPELL_WHIRLWIND, true);
        Whirl = 6000;
        DieTimer = 14000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(Whirl < diff)
        {
            AddSpellToCast(m_creature, SPELL_WHIRLWIND);
            Whirl = 6000;
        }
        else
            Whirl -= diff;

        if(DieTimer < diff)
        {
            m_creature->Kill(m_creature, false);
            m_creature->RemoveCorpse();
            DieTimer = 2000;
        }
        else
            DieTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};

CreatureAI* GetAI_mob_whirling_blade(Creature *_Creature)
{
    return new mob_whirling_bladeAI(_Creature);
}

/****************
* Shadowmoon Deathshaper - id 22882
*****************/

#define SPELL_DEATH_COIL                41070
#define SPELL_DEMON_ARMOR               13787
#define SPELL_RAISE_DEAD                41071
#define SPELL_SHADOWBOLT                41069

struct TRINITY_DLL_DECL mob_shadowmoon_deathshaperAI: public ScriptedAI
{
    mob_shadowmoon_deathshaperAI(Creature *c) : ScriptedAI(c) { }

    uint32 Shadowbolt;
    uint32 DeathCoil;
    uint32 DemonArmor;
    uint32 RaiseDeadCheck;

    std::list<uint64> UsedCorpsesGUID;

    void Reset()
    {
        Shadowbolt = urand(500, 5000);
        DeathCoil = urand(5000, 20000);
        DemonArmor = 1800000;
        RaiseDeadCheck = 10000;

        UsedCorpsesGUID.clear();
    }

    void EnterCombat(Unit *)
    {
        m_creature->InterruptNonMeleeSpells(false, SPELL_GREEN_BEAM);
        DoCast(m_creature, SPELL_DEMON_ARMOR);
        DoZoneInCombat();
    }

    // THIS MAY CRASH, pls check it...
    uint64 SelectCorpseGUID()
    {
        std::list<Unit*> DeadList = DoFindAllDeadInRange(50);
        std::list<uint64> CorpseGUID;
        CorpseGUID.clear();

        // select all dead units GUIDs
        if(!DeadList.empty())
        {
            for(std::list<Unit*>::iterator i = DeadList.begin(); i != DeadList.end(); ++i)
            {
                CorpseGUID.push_back((*i)->GetGUID());  // this may be moved to searcher, but if no crashes
            }
        }
        // remove GUIDs of once used corpses
        if(!CorpseGUID.empty())
        {
            for(std::list<uint64>::iterator i = CorpseGUID.begin(); i != CorpseGUID.end(); ++i)
            {
                if(!UsedCorpsesGUID.empty())
                {
                    for(std::list<uint64>::iterator iter = UsedCorpsesGUID.begin(); iter != UsedCorpsesGUID.end(); ++iter)
                    {
                        if((*iter) == (*i))
                            CorpseGUID.remove((*iter));
                    }
                }
            }
        }
        // get victim GUID now (can be random later if no crashes)
        if(!CorpseGUID.empty())
            return (*CorpseGUID.begin());

        return NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        
        if(!UpdateVictim())
        {
            if (!m_creature->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
            {
                if (Unit *skeleton = FindCreature(MOB_SKELETON, 20.0f, m_creature))
                {
                    if (skeleton->isAlive())
                        DoCast(skeleton, SPELL_GREEN_BEAM);
                }
            }
            return;
        }

        if(Shadowbolt < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 40.0, true))
                AddSpellToCast(target, SPELL_SHADOWBOLT);
            Shadowbolt = 5000;
        }
        else
            Shadowbolt -= diff;

        if(DeathCoil < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30.0, true))
                AddSpellToCast(target, SPELL_DEATH_COIL);
            DeathCoil = 20000;
        }
        else
            DeathCoil -= diff;

        //THIS MAY CRASH!! check it pls
        /*
        if(RaiseDeadCheck < diff)
        {
            if(SelectCorpseGUID())
            {
                uint64 targetGUID = SelectCorpseGUID();
                if(Unit* target = Unit::GetUnit(*m_creature, targetGUID))
                {
                    AddSpellToCast(target, SPELL_RAISE_DEAD);
                    UsedCorpsesGUID.push_back(targetGUID);
                }
            }
            RaiseDeadCheck = 10000;
        }
        else
            RaiseDeadCheck -= diff;*/

        if(DemonArmor < diff)
        {
            ForceSpellCast(m_creature, SPELL_DEMON_ARMOR);
            DemonArmor = 1800000;
        }
        else
            DemonArmor -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_shadowmoon_deathshaper(Creature *_Creature)
{
    return new mob_shadowmoon_deathshaperAI(_Creature);
}

/****************
* Shadowmoon Houndmaster - id 23018
*****************/

#define SPELL_FREEZING_TRAP                 41085
#define SPELL_SHOOT_1                       41093
#define SPELL_SILENCING_SHOT                41084
#define SPELL_SUMMON_RIDING_WARHOUND        39906
#define SPELL_VOLLEY                        41091
#define SPELL_WING_CLIP                     32908
#define SPELL_FLARE                         41094

#define MOB_SHADOWMOON_RIDING_HOUND         23083

struct TRINITY_DLL_DECL mob_shadowmoon_houndmasterAI: public ScriptedAI
{
    mob_shadowmoon_houndmasterAI(Creature *c) : ScriptedAI(c) { }

    uint32 Shoot;
    uint32 FreezingTrap;
    uint32 SilencingShot;
    uint32 Volley;
    uint32 WingClip;
    uint32 Flare;

    void Reset()
    {
        
        if(Creature* Hound = GetClosestCreatureWithEntry(m_creature, MOB_SHADOWMOON_RIDING_HOUND, 80))
        {
            Hound->Kill(Hound, false);
            Hound->RemoveCorpse();
        }
        m_creature->Mount(14334);
        Shoot = 2000;
        FreezingTrap = 15000;
        SilencingShot = urand(5000, 15000);
        Volley = urand(10000, 25000);
        WingClip = urand(8000, 20000);
        Flare = urand(2000, 20000);
    }

    void EnterCombat(Unit *)
    {
        m_creature->Unmount();
        DoCast(m_creature, SPELL_SUMMON_RIDING_WARHOUND);
        DoCast(m_creature, SPELL_FREEZING_TRAP);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()+urand(10, 15), m_creature->GetPositionY()+urand(3, 7), m_creature->GetPositionZ());
        DoZoneInCombat();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(FreezingTrap < diff)
        {
            DoCast(m_creature, SPELL_FREEZING_TRAP);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()+urand(10, 15), m_creature->GetPositionY()+urand(3, 7), m_creature->GetPositionZ());
            FreezingTrap = 15000;
        }
        else
            FreezingTrap -= diff;

        if(Volley < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 40.0f, true);
            if(target && m_creature->GetDistance(target) > 10.0f)
            {
                ForceSpellCast(target, SPELL_VOLLEY);
                Volley = 25000;
            }
            else
                Volley = 3000;
        }
        else
            Volley -= diff;

        if(Shoot < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 100, true))
            {
                if(m_creature->GetDistance(target) > 30)
                {
                    m_creature->GetMotionMaster()->MoveChase(target, 20, 0);
                    m_creature->SetSpeed(MOVE_RUN, 1,5);
                }
                else if(!target->IsWithinDistInMap(m_creature, 5.0))
                {
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();
                    ForceSpellCast(target, SPELL_SHOOT_1);
                }
            }
            Shoot = 2000;
        }
        else
            Shoot -= diff;

        if(SilencingShot < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1, 35.0f, true);
            if(target && m_creature->GetDistance(target) > 8.0f)
            {
                ForceSpellCast(target, SPELL_SILENCING_SHOT);
                SilencingShot = 15000;
            }
            else
                SilencingShot = 4000;
        }
        else
            SilencingShot -= diff;

        if(WingClip < diff)
        {
            if(m_creature->IsWithinDistInMap(m_creature->getVictim(), 5.0))
            {
                AddSpellToCast(m_creature->getVictim(), SPELL_WING_CLIP);
                m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()+urand(10, 15), m_creature->GetPositionY()+urand(3, 7), m_creature->GetPositionZ());
                WingClip = 20000;
            }
            else
                WingClip = 2500;
        }
        else
            WingClip -= diff;

        if(Flare < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                AddSpellToCast(target, SPELL_FLARE);
            Flare = 20000;
        }
        else
            Flare -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_houndmaster(Creature *_Creature)
{
    return new mob_shadowmoon_houndmasterAI(_Creature);
}

/****************
* Shadowmoon Weapon Master - id 23049
*****************/

static float fieldPositions [8][2] =
{
    {448.26, 195.74},
    {448.66, 191.27},
    {449.34, 187.12},
    {449.50, 182.92},
    {442.48, 195.18},
    {442.90, 191.00},
    {443.34, 186.51},
    {443.84, 182.04}
};

#define DISTANCE_TO_MOVE   40.00f
#define _HEIGHT           163.98f

#define WEAPON_X          454.35f
#define WEAPON_Y          190.09f


struct TRINITY_DLL_DECL mob_shadowmoon_weapon_masterAI: public ScriptedAI
{
    mob_shadowmoon_weapon_masterAI(Creature *c) : ScriptedAI(c)
    {
        m_nextMove = 0;
        m_nextId = -1;
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    std::vector<uint64> soldiersList;

    uint32 m_nextMove;
    int32  m_nextId;

    void Reset()
    {
        if (!soldiersList.empty())
        {
            m_nextMove = 2000;
            m_nextId   = 1;
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 0:
            {
                m_nextMove = 2300;
                m_nextId   = 1;
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_POINT_NOSHEATHE);
            }
            break;
            case 1:
            {
                m_nextMove = 5000;
                m_nextId = 0;
            }
            break;
            default:   break;
        }
    }

    void EnterCombat(Unit *) { DoZoneInCombat(); }

    void UpdateAI(const uint32 diff)
    {
        if (soldiersList.empty())
        {
            if (pInstance)
            {
                if (pInstance->GetData(DATA_WEAPONMASTER_LIST_SIZE) == 8)
                {
                    for (int i = DATA_WEAPONMASTER_SOLDIER; i < DATA_WEAPONMASTER_SOLDIER+8; ++i)
                    {
                        soldiersList.push_back(pInstance->GetData64(i));
                        if (Creature *soldier = m_creature->GetMap()->GetCreature(pInstance->GetData64(i)))
                        {
                            soldier->Relocate(fieldPositions[i-30][0], fieldPositions[i-30][1], _HEIGHT, 0);
                            soldier->SetHomePosition(fieldPositions[i-30][0], fieldPositions[i-30][1], _HEIGHT, 0);
                        }
                    }
                    m_nextMove = 1000;
                    m_nextId   = 0;
                }
            }
        }

        if(!UpdateVictim())
        {
            if (m_nextId >= 0)
            {
                if (m_nextMove)
                {
                    if (m_nextMove <= diff)
                    {
                        m_nextMove = 0;
                        int id = m_nextId;
                        m_nextId = -1;

                        float x = 0;
                        float y = 0;

                        switch (id)
                        {
                            case 0:
                            {
                                for (int i = 0; i < 8; ++i)
                                {
                                    if (Unit *soldier = m_creature->GetUnit(*m_creature, soldiersList[i]))
                                    {
                                        x = soldier->GetPositionX() + DISTANCE_TO_MOVE * cos(0.0f);
                                        y = soldier->GetPositionY() + DISTANCE_TO_MOVE * sin(0.0f);
                                        soldier->GetMotionMaster()->MovePoint(0, x, y, _HEIGHT);
                                    }
                                }
                                x = m_creature->GetPositionX() + DISTANCE_TO_MOVE * cos(0.0f);
                                y = m_creature->GetPositionY() + DISTANCE_TO_MOVE * sin(0.0f);
                            }
                            break;
                            case 1:
                            {
                                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                                for (int i = 0; i < 8; ++i)
                                {
                                    if (Unit *soldier = m_creature->GetUnit(*m_creature, soldiersList[i]))
                                        soldier->GetMotionMaster()->MovePoint(1, fieldPositions[i][0], fieldPositions[i][1], _HEIGHT);
                                }
                                x = WEAPON_X;
                                y = WEAPON_Y;
                            }
                            break;
                        }
                        m_creature->GetMotionMaster()->MovePoint(id, x, y, _HEIGHT);
                    }
                    else m_nextMove -= diff;
                }
            }
            return;
        }
        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};


/****************
* Shadowmoon Soldier - id 23047
*****************/

#define SPELL_STRIKE        11976

struct TRINITY_DLL_DECL mob_shadowmoon_soldierAI: public ScriptedAI
{
    mob_shadowmoon_soldierAI(Creature *c) : ScriptedAI(c) { }

    void Reset()
    {
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 0:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            break;
        }
    }

    void EnterCombat(Unit *) { DoZoneInCombat(); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_weapon_master(Creature *_Creature)
{
    return new mob_shadowmoon_weapon_masterAI(_Creature);
}

CreatureAI* GetAI_mob_shadowmoon_soldier(Creature *_Creature)
{
    return new mob_shadowmoon_soldierAI(_Creature);
}

/* ============================
*
*      GURTOGG  BLOODBOIL
*
* ============================*/

/* Content Data
    * Bonechewer Behemoth
    * Bonechewer Shield Disciple
    * Bonechewer Blade Fury
    * Bonechewer Blood Prophet
    * Mutated Battle Wolf
    * Bonechewer Brawler
    * Bonechewer Combatant
    * Bonechewer Spectator
*/

/* ============================
*
*      RELIQUARY  OF  SOULS
*
* ============================*/

/* Content Data
    * Angered Soul Fragment
    * Hungering Soul Fragment
    * Suffering Soul Fragment
*/
/****************
* Angered Soul Fragment - id 23398
*****************/

#define SPELL_ANGER     41986

struct TRINITY_DLL_DECL mob_angered_soul_fragmentAI: public ScriptedAI
{
    mob_angered_soul_fragmentAI(Creature *c) : ScriptedAI(c) { }

    uint32 Anger;

    void Reset()
    {
        Anger = urand(1500, 6000);
    }

    void EnterCombat(Unit *)
    {
        DoZoneInCombat();
    }

    void JustDied(Unit* killer)
    {
        m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Anger < diff)
        {
            AddSpellToCast(m_creature, SPELL_ANGER);
            Anger = urand(6000, 16000);
        }
        else
            Anger -=diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_angered_soul_fragment(Creature *_Creature)
{
    return new mob_angered_soul_fragmentAI(_Creature);
}

/****************
* Hungering Soul Fragment - id 23401
*****************/

#define SPELL_CONSUMING_STRIKES     41248

struct TRINITY_DLL_DECL mob_hungering_soul_fragmentAI: public ScriptedAI
{
    mob_hungering_soul_fragmentAI(Creature *c) : ScriptedAI(c) { }

    void Reset()
    {
        DoCast(m_creature, SPELL_CONSUMING_STRIKES);
    }

    void EnterCombat(Unit *)
    {
        DoZoneInCombat();
    }

    void JustDied(Unit* killer)
    {
        m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hungering_soul_fragment(Creature *_Creature)
{
    return new mob_hungering_soul_fragmentAI(_Creature);
}

/****************
* Suffering Soul Fragment - id 23399
*****************/

#define SPELL_SOUL_BLAST        41245

struct TRINITY_DLL_DECL mob_suffering_soul_fragmentAI: public ScriptedAI
{
    mob_suffering_soul_fragmentAI(Creature *c) : ScriptedAI(c) { }

    uint32 SoulBlast;

    void Reset()
    {
        SoulBlast = urand(2000, 5000);
    }

    void EnterCombat(Unit *)
    {
        DoZoneInCombat();
    }

    void JustDied(Unit* killer)
    {
        m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(SoulBlast < diff)
        {
            AddSpellToCast(m_creature, SPELL_SOUL_BLAST);
            SoulBlast = urand(8000, 12000);
        }
        else
            SoulBlast -=diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_suffering_soul_fragment(Creature *_Creature)
{
    return new mob_suffering_soul_fragmentAI(_Creature);
}




/* ============================
*
*      MOTHER  SHAHRAZ
*
* ============================*/

/* Content Data
    * Priestess of Dementia
    * Priestess of Delight
    * Sister of Pain
    * Sister of Pleasure
    * Charming Courtesan
    * Spellbound Attendent
    * Enslaved Servant
    * Temple Concubine
*/

/* ============================
*
*      ILLIDARI  COUNCIL
*
* ============================*/

/* Content Data
    * Illidari Archon
    * Illidari Assassin
    * Illidari Battle-mage
    * Illidari Blood Lord
    * Promenade Sentinel
*/

void AddSC_black_temple_trash()
{
    Script *newscript;

    //Najentus
    newscript = new Script;
    newscript->Name = "mob_aqueous_lord";
    newscript->GetAI = &GetAI_mob_aqueous_lord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_aqueous_spawn";
    newscript->GetAI = &GetAI_mob_aqueous_spawn;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_general";
    newscript->GetAI = &GetAI_mob_coilskar_general;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_harpooner";
    newscript->GetAI = &GetAI_mob_coilskar_harpooner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_seacaller";
    newscript->GetAI = &GetAI_mob_coilskar_seacaller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_geyser";
    newscript->GetAI = &GetAI_mob_coilskar_geyser;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_soothsayer";
    newscript->GetAI = &GetAI_mob_coilskar_soothsayer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilskar_wrangler";
    newscript->GetAI = &GetAI_mob_coilskar_wrangler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dragon_turtle";
    newscript->GetAI = &GetAI_mob_dragon_turtle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_leviathan";
    newscript->GetAI = &GetAI_mob_leviathan;
    newscript->RegisterSelf();

    //Supremus
    newscript = new Script;
    newscript->Name = "mob_bonechewer_taskmaster";
    newscript->GetAI = &GetAI_mob_bonechewer_taskmaster;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_bonechewer_worker";
    newscript->GetAI = &GetAI_mob_bonechewer_worker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dragonmaw_skystalker";
    newscript->GetAI = &GetAI_mob_dragonmaw_skystalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dragonmaw_windreaver";
    newscript->GetAI = &GetAI_mob_dragonmaw_windreaver;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dragonmaw_wyrmcaller";
    newscript->GetAI = &GetAI_mob_dragonmaw_wyrmcaller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_fearbringer";
    newscript->GetAI = &GetAI_mob_illidari_fearbringer;
    newscript->RegisterSelf();

    // Shade Of Akama
    newscript = new Script;
    newscript->Name = "mob_ashtongue_battlelord";
    newscript->GetAI = &GetAI_mob_ashtongue_battlelord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_feral_spirit";
    newscript->GetAI = &GetAI_mob_ashtongue_feral_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_mystic";
    newscript->GetAI = &GetAI_mob_ashtongue_mystic;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "totem_ashtongue_mystic";
    newscript->GetAI = &GetAI_totem_ashtongue_mystic;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_primalist";
    newscript->GetAI = &GetAI_mob_ashtongue_primalist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_stalker";
    newscript->GetAI = &GetAI_mob_ashtongue_stalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_stormcaller";
    newscript->GetAI = &GetAI_mob_ashtongue_stormcaller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_boneslicer";
    newscript->GetAI = &GetAI_mob_illidari_boneslicer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_centurion";
    newscript->GetAI = &GetAI_mob_illidari_centurion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_defiler";
    newscript->GetAI = &GetAI_mob_illidari_defiler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_heartseeker";
    newscript->GetAI = &GetAI_mob_illidari_heartseeker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_illidari_nightlord";
    newscript->GetAI = &GetAI_mob_illidari_nightlord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_storm_fury";
    newscript->GetAI = &GetAI_mob_storm_fury;
    newscript->RegisterSelf();

    // Teron Gorefiend
    newscript = new Script;
    newscript->Name = "mob_hand_of_gorefiend";
    newscript->GetAI = &GetAI_mob_hand_of_gorefiend;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_blood_mage";
    newscript->GetAI = &GetAI_mob_shadowmoon_blood_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_champion";
    newscript->GetAI = &GetAI_mob_shadowmoon_champion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_whirling_blade";
    newscript->GetAI = &GetAI_mob_whirling_blade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_houndmaster";
    newscript->GetAI = &GetAI_mob_shadowmoon_houndmaster;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_weapon_master";
    newscript->GetAI = &GetAI_mob_shadowmoon_weapon_master;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_soldier";
    newscript->GetAI = &GetAI_mob_shadowmoon_soldier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_deathshaper";
    newscript->GetAI = &GetAI_mob_shadowmoon_deathshaper;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_angered_soul_fragment";
    newscript->GetAI = &GetAI_mob_angered_soul_fragment;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_hungering_soul_fragment";
    newscript->GetAI = &GetAI_mob_hungering_soul_fragment;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_suffering_soul_fragment";
    newscript->GetAI = &GetAI_mob_suffering_soul_fragment;
    newscript->RegisterSelf();
}
