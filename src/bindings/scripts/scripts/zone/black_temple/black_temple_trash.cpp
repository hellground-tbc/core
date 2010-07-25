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
        SummonTimer = 10000;
        CrashingWave = 15000;
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);        //not tauntable
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);

    }
    void Aggro(Unit*) {}
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
            SummonTimer = 60000;
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
#define SPELL_MERGE_TRIGGERED   40105

#define NPC_AQUEOUS_LORD       22878

// TODO: Merge, needs casting targts verification, something is NOT GOOD...

struct TRINITY_DLL_DECL mob_aqueous_spawnAI : public ScriptedAI
{
    mob_aqueous_spawnAI(Creature *c) : ScriptedAI(c) {}

    uint32 SludgeNova;
    //uint32 MergeTimer;
    //bool merging;

    void Reset()
    {
        SludgeNova = 5000;
        //MergeTimer = urand(10000, 50000);
        //merging = false;

    }
    void Aggro(Unit*) {}
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(SludgeNova < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 30, true))
                    AddSpellToCast(target, SPELL_SLUDGE_NOVA);
            SludgeNova = urand(15000,18000);
        }
        else
            SludgeNova -= diff;

        /*
        if(!merging && MergeTimer < diff)
        {
            Unit* target = NULL;
            AddSpellToCast(target, SPELL_MERGE);
            merging = true;
        }
        else
            MergeTimer -= diff;*/

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

    void Aggro(Unit*) {}
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
            FreeFriend = urand(3000, 4000);
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

    void Reset()
    {
        SpearThrow = urand(1000, 5000);
        HookedNet = urand(15000, 20000);
        HarpoonersMark = urand(100, 5000);
    }

    bool CanCastMark()
    {
        Map::PlayerList const& pList = m_creature->GetMap()->GetPlayers();       // only one Harpooner's Mark on players at a time
        if(!pList.isEmpty())
        {
            for(Map::PlayerList::const_iterator i = pList.begin(); i != pList.end(); ++i)
            {
                Player* pl = i->getSource();
                if(pl && pl->HasAura(SPELL_HARPOONERS_MARK, 0))
                    return false;
            }
        }
        return true;
    }

    void Aggro(Unit*) {}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(HarpoonersMark < diff)
        {
            if(CanCastMark())
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 80, true))
                {
                    AttackStart(target);
                    m_creature->getThreatManager().addThreat(target, 5000);     //we attack target with a Mark, but in contrast to turtle, we can be overaggroed and/or taunted
                    HostilReference* aggroTarget = m_creature->getThreatManager().getOnlineContainer().getReferenceByTarget(target);
                    if(aggroTarget)
                        m_creature->getThreatManager().setCurrentVictim(aggroTarget);
                    AddSpellToCast(target, SPELL_HARPOONERS_MARK);
                }
            }
            else
                HarpoonersMark = urand(10000,30000);     //every 10-30 sec check if we can cast new mark
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
    void Aggro(Unit*) {}
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
            std::cout << "cast electric spura" << std::endl;
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
    void Aggro(Unit*) {}
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ShellShield < diff)
        {
            if(!CanBeShielded)
            {
                if(m_creature->GetHealth()/m_creature->GetMaxHealth()*100 < 50)
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
    void Aggro(Unit*) {}
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

    uint32 furyTimer;
    uint32 disgruntledTimer;
    bool furyCasted;
    bool disgruntledCasted;

    void Reset()
    {
        furyTimer = 20000;
        disgruntledTimer = 30000;
        furyCasted = false;
        disgruntledCasted = false;
    }

    void Aggro(Unit *who)
    {
        if (urand(0, 100) < 25)
        {
            m_creature->CastSpell(m_creature, SPELL_BONECHEWER_DISGRUNTLED, false);
            disgruntledCasted = true;
        }
    }

    void JustDied(Unit *victim){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (!furyCasted)
        {
            if (m_creature->GetHealth() < m_creature->GetMaxHealth() * 0.5 || furyTimer < diff)
            {
                m_creature->CastSpell(m_creature, SPELL_BONECHEWER_FURY, false);
                furyCasted = true;
            }
            else
                furyTimer -= diff;
        }

        if (!disgruntledCasted)
        {
            if (disgruntledTimer < diff)
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
        throwTimer = 15000 + urand(0, 10000);
    }

    void Aggro(Unit *who)
    {
        if (who)
        {
            if (urand(0, 100) < 20)
            {
                m_creature->CastSpell(who, SPELL_WORKER_THROW_PICK, false);
            }
        }
    }

    void JustDied(Unit *victim){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (throwTimer < diff)
        {
            Unit * victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 35, true);

            if (victim)
                m_creature->CastSpell(victim, SPELL_WORKER_THROW_PICK, false);

            throwTimer = 15000 + urand(0, 10000);
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

    uint32 shootTimer;
    uint32 immolationArrowTimer;

    void Reset()
    {
        shootTimer = 2000 + urand(0, 2000);
        immolationArrowTimer = 15000 + urand(0, 15000);
    }

    void Aggro(Unit *who){}

    void JustDied(Unit *victim){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        Unit * victim = m_creature->getVictim();

        if (!victim)
            return;

        if (m_creature->GetDistance(victim) > 37)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(victim, 25, 0);
        }

        if (shootTimer < diff)
        {
            AddSpellToCast(victim, SPELL_SKYSTALKER_SHOOT);
            shootTimer = 2000 + urand(0, 2000);
        }
        else
            shootTimer -= diff;

        if (immolationArrowTimer < diff)
        {
            ForceSpellCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true), SPELL_SKYSTALKER_IMMOLATION);
            immolationArrowTimer = 15000 + urand(0, 15000);
        }
        else
            immolationArrowTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};

/*#####
##  mob Dragonmaw Wind Reaver - id 23330
###########*/

struct TRINITY_DLL_DECL mob_dragonmaw_windreaverAI : public ScriptedAI
{
    mob_dragonmaw_windreaverAI(Creature *c) : ScriptedAI(c){}

    uint32 fireballTimer;
    uint32 doomBoltTimer;
    uint32 freezeTimer;

    void Reset()
    {
        fireballTimer = 2000 + urand(0, 2000);
        doomBoltTimer = 15000 + urand(0, 10000);
        freezeTimer = 20000 + urand(0, 15000);
    }

    void Aggro(Unit *who){}

    void JustDied(Unit *victim){}

    Unit * CheckMeleeRange()
    {
        Map::PlayerList const &PlayerList = ((InstanceMap*)m_creature->GetMap())->GetPlayers();
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl && i_pl->isAlive() && m_creature->GetDistance(i_pl) < 6)
                return (Unit*)i_pl;
        }

        return NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        Unit * victim = m_creature->getVictim();

        if (!victim)
            return;

        if (m_creature->GetDistance(victim) > 37)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(victim, 25, 0);
        }

        if (fireballTimer < diff)
        {
            AddSpellToCast(victim, SPELL_WINDREAVER_FIREBALL);
            fireballTimer = 3000 + urand(0, 2000);
        }
        else
            fireballTimer -= diff;

        if (doomBoltTimer < diff)
        {
            AddSpellToCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true), SPELL_WINDREAVER_DOOM_BOLT);
            doomBoltTimer = 15000 + urand(0, 10000);
        }
        else
            doomBoltTimer -= diff;

        if (freezeTimer < diff)
        {
            Unit * tmpTarget = CheckMeleeRange();
            if (tmpTarget)
            {
                ForceSpellCast(tmpTarget, SPELL_WINDREAVER_FREEZE);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(tmpTarget, 15, 0);
            }
            else
            {
                ForceSpellCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true), SPELL_WINDREAVER_FREEZE);
            }

            freezeTimer = 20000 + urand(0, 15000);
        }
        else
            freezeTimer -= diff;

        CastNextSpellIfAnyAndReady();
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
        cleaveTimer = 10000 + urand(0, 10000);
        fixateTimer = 20000 + urand(0, 10000);
        jabTimer = 5000 + urand(0, 15000);
    }

    void Aggro(Unit *who) {}

    void JustDied(Unit *victim) {}

    void OnAuraApply(Aura * aur, Unit * caster)
    {
        if (aur->GetId() == AURA_WYRMCALLER_FIXATED)
        {
            m_creature->AddThreat(caster, 1000000, SPELL_SCHOOL_MASK_NORMAL, NULL);
            m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
            m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        }
    }

    void OnAuraRemove(Aura * aur, Unit * caster)
    {
        if (aur->GetId() == AURA_WYRMCALLER_FIXATED)
        {
            m_creature->AddThreat(aur->GetCaster(), -1000000, SPELL_SCHOOL_MASK_NORMAL, NULL);
            m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
            m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
        }
    }

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
            cleaveTimer = 10000 + urand(0, 10000);
        }
        else
            cleaveTimer -= diff;

        if (jabTimer < diff)
        {
            m_creature->CastSpell(victim, SPELL_WYRMCALLER_JAB, false);
            jabTimer = 5000 + urand(0, 15000);
        }
        else
            jabTimer -= diff;

        if (fixateTimer < diff)
        {
            victim = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true);
            if (victim)
                victim->CastSpell(m_creature, SPELL_WYRMCALLER_FIXATE, false);

            fixateTimer = 20000 + urand(0, 10000);
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

    void Reset()
    {
        flamesTimer = 10000 + urand(0, 10000);
        rainTimer = 20000 + urand(0, 10000);
        stompTimer = 15000 + urand(0, 10000);
    }

    void Aggro(Unit *who){}

    void JustDied(Unit *victim){}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if (flamesTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_FEARBRINGER_ILLIDARI_FLAMES);
            flamesTimer = 10000 + urand(0, 10000);
        }
        else
            flamesTimer -= diff;

        if (rainTimer < diff)
        {
            AddSpellToCast(SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true), SPELL_FEARBRINGER_RAIN_OF_CHAOS);
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
*      SHADE  OF  AKAMA
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
    * Shadowmoon Grunt
    * Shadowmoon Houndmaster
    * Shadowmoon Reaver
    * Shadowmoon Riding Hound
    * Shadowmoon Soldier
    * Shadowmoon Weapon Master
    * Wrathbone Flayer
*/

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
}
