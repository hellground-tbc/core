/* Copyright (C) 2008 - 2010 HellgroundDev <http://gamefreedom.pl/>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* ScriptData
SDName: Black_Temple_Trash
SD%Complete: 0
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
* Aqueous Lord
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

CreatureAI* GetAI_mob_aqueous_lord(Creature *_Creature)
{
    return new mob_aqueous_lordAI(_Creature);
}

/****************
* Aqueous Spawn
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

        if(!merging && MergeTimer < diff)
        {
            std::cout << "merge" << std::endl;
            Unit* Lord = FindCreature(NPC_AQUEOUS_LORD, 80.0, m_creature);
                if(Lord)
                    AddSpellToCast(Lord, SPELL_MERGE);
            //AddSpellToCast(m_creature, SPELL_MERGE);
                merging = true;
        }
        else
            MergeTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_aqueous_spawn(Creature *_Creature)
{
    return new mob_aqueous_spawnAI(_Creature);
}

/****************
* Coilscar General
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

CreatureAI* GetAI_mob_coilskar_general(Creature *_Creature)
{
    return new mob_coilskar_generalAI(_Creature);
}

/****************
* Coilscar Harpooner
*****************/


struct TRINITY_DLL_DECL mob_coilskar_harpoonerAI : public ScriptedAI
{
    mob_coilskar_harpoonerAI(Creature *c) : ScriptedAI(c) {}

    void Aggro(Unit*) {}
    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;


        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_coilskar_wranglerAI : public ScriptedAI
{
    mob_coilskar_wranglerAI(Creature *c) : ScriptedAI(c) {}

    void Aggro(Unit*) {}
    void UpdateAI(const uint32 diff)
    {
        DoMeleeAttackIfReady();
    }
};

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
}
