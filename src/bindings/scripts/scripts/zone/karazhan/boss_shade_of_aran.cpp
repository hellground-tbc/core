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
SDName: Boss_Shade_of_Aran
SD%Complete: 95
SDComment: Flame wreath missing cast animation, mods won't triggere.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "../../creature/simple_ai.h"
#include "def_karazhan.h"
#include "GameObject.h"

#define SAY_AGGRO1                  -1532073
#define SAY_AGGRO2                  -1532074
#define SAY_AGGRO3                  -1532075
#define SAY_FLAMEWREATH1            -1532076
#define SAY_FLAMEWREATH2            -1532077
#define SAY_BLIZZARD1               -1532078
#define SAY_BLIZZARD2               -1532079
#define SAY_EXPLOSION1              -1532080
#define SAY_EXPLOSION2              -1532081
#define SAY_DRINK                   -1532082                //Low Mana / AoE Pyroblast
#define SAY_ELEMENTALS              -1532083
#define SAY_KILL1                   -1532084
#define SAY_KILL2                   -1532085
#define SAY_TIMEOVER                -1532086
#define SAY_DEATH                   -1532087
#define SAY_ATIESH                  -1532088                //Atiesh is equipped by a raid member

//Spells
#define SPELL_FROSTBOLT     29954
#define SPELL_FIREBALL      29953
#define SPELL_ARCMISSLE     29955
#define SPELL_CHAINSOFICE   29991
#define SPELL_DRAGONSBREATH 29964
#define SPELL_MASSSLOW      30035
#define SPELL_FLAME_WREATH  29946
#define SPELL_AOE_CS        29961
#define SPELL_PLAYERPULL    32265
#define SPELL_AEXPLOSION    29973
#define SPELL_MASS_POLY     29963
#define SPELL_BLINK_CENTER  29967
#define SPELL_ELEMENTALS    29962
#define SPELL_CONJURE       29975
#define SPELL_DRINK         30024
#define SPELL_POTION        32453
#define SPELL_AOE_PYROBLAST 29978

//Creature Spells
#define SPELL_CIRCULAR_BLIZZARD     29952
#define SPELL_WATERBOLT             31012
#define SPELL_SHADOW_PYRO           29978
#define SPELL_FROSTBOLT_VOLLEY      38837
#define SPELL_AMISSILE_VOLLEY       29960

//Creatures
#define CREATURE_WATER_ELEMENTAL         17167
#define CREATURE_SHADOW_OF_ARAN          18254
#define CREATURE_ARAN_BLIZZARD           17161
#define CREATURE_ARAN_TELEPORT           17170      //waypoint's for blizzard

enum SuperSpell
{
    SUPER_FLAME = 0,
    SUPER_BLIZZARD,
    SUPER_AE,
};

float ElementalSpawnPoints[2][4] = {
    {-11143.5, -11167.6, -11186.8, -11162.6},   // X coord
    {-1914.26, -1933.8,  -1909.7,  -1895.4}     // Y coord
};

float shadowOfAranSpawnPoints[2][8] = {
    {-11143.5, -11152.1, -11167.6, -11181.3, -11186.8, -11178,  -11162.6, -11148.6},// X coord
    {-1914.26, -1928.2,  -1933.8,  -1925.05, -1909.7,  -1895.7, -1895.4,  -1899}    // Y coord
};
float blizzardWaypointsSpawnPoints[2][8] = {
    {-11156.6, -11163.9, -11171.6, -11176.1, -11173.5, -11166.4, -11158.4, -11154.2},  // X coord
    {-1905.2,  -1901.1,  -1903.6,  -1910.8,  -1918.7,  -1922.9,  -1920.4,  -1913.1}    // Y coord
};

struct TRINITY_DLL_DECL boss_aranAI : public ScriptedAI
{
    boss_aranAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_creature->GetPosition(wLoc);
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ARCMISSLE);
        if(TempSpell)
        {
            TempSpell->EffectImplicitTargetA[0] = TARGET_UNIT_CASTER;
            TempSpell->EffectImplicitTargetA[1] = TARGET_UNIT_CASTER;
            TempSpell->EffectImplicitTargetA[2] = TARGET_UNIT_TARGET_ENEMY;
        }
    }

    ScriptedInstance* pInstance;

    uint32 SecondarySpellTimer;
    uint32 NormalCastTimer;
    uint32 SuperCastTimer;
    uint32 BerserkTimer;
    uint32 CloseDoorTimer;                                  // Don't close the door right on aggro in case some people are still entering.

    uint8 LastSuperSpell;

    uint32 FlameWreathTimer;
    uint32 FlameWreathCheckTime;
    uint64 FlameWreathTarget[3];
    float FWTargPosX[3];
    float FWTargPosY[3];

    uint32 CurrentNormalSpell;
    uint32 ArcaneCooldown;
    uint32 FireCooldown;
    uint32 FrostCooldown;
    uint32 CheckTimer;

    uint64 shadeOfAranTeleportCreatures[8];

    WorldLocation wLoc;

    uint32 DrinkInturruptTimer;
    uint32 DrinkingWaitTime;

    bool ElementalsSpawned;
    bool Drinking;
    bool DrinkInturrupted;
    bool PotionUsed;
    bool ArcaneCasting;

    void Reset()
    {
        SecondarySpellTimer = 5000;
        NormalCastTimer     = 0;
        SuperCastTimer      = 30000;
        BerserkTimer        = 720000;
        CloseDoorTimer      = 15000;
        CheckTimer          = 3000;

        LastSuperSpell = rand()%3;

        FlameWreathTimer     = 0;
        FlameWreathCheckTime = 0;

        CurrentNormalSpell = 0;
        ArcaneCooldown     = 0;
        FireCooldown       = 0;
        FrostCooldown      = 0;

        DrinkInturruptTimer = 10000;
        DrinkingWaitTime    = 1000;

        ElementalsSpawned       = false;
        Drinking                = false;
        DrinkInturrupted        = false;
        PotionUsed              = false;
        ArcaneCasting           = false;

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);

        if(pInstance)
        {
            // Not in progress
            pInstance->SetData(DATA_SHADEOFARAN_EVENT, NOT_STARTED);

            if(GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                Door->SetGoState(0);
        }

        if(m_creature->isAlive() && !shadeOfAranTeleportCreatures[0])
        {
            Creature *pCreature;
            for (uint16 i = 0; i < 8; i++)
            {
                pCreature = m_creature->SummonCreature(CREATURE_ARAN_TELEPORT, blizzardWaypointsSpawnPoints[0][i], blizzardWaypointsSpawnPoints[1][i], m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                if(pCreature)
                    shadeOfAranTeleportCreatures[i] = pCreature->GetGUID();
            }
        }
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_KILL1, m_creature); break;
        case 1: DoScriptText(SAY_KILL2, m_creature); break;
        }
    }

    void TeleportCenter()
    {
        m_creature->CastSpell(m_creature, SPELL_BLINK_CENTER, true);
        m_creature->Relocate(wLoc.x,wLoc.y,wLoc.z);
        m_creature->SendMonsterMove(wLoc.x,wLoc.y,wLoc.z, 0);
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
        {
            pInstance->SetData(DATA_SHADEOFARAN_EVENT, DONE);

            if(GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                Door->SetGoState(0);
        }

        if (shadeOfAranTeleportCreatures[0])
        {
            Creature *pCreature;
            for (int16 i = 0; i < 8; i++)
            {
                pCreature = Unit::GetCreature(*m_creature, shadeOfAranTeleportCreatures[i]);
                if(pCreature)
                    m_creature->Kill(pCreature, false);
                shadeOfAranTeleportCreatures[i] = 0;
            }
        }
    }

    void Aggro(Unit *who)
    {
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
        case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
        case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if(pInstance)
        {
            pInstance->SetData(DATA_SHADEOFARAN_EVENT, IN_PROGRESS);
            if(GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                Door->SetGoState(1);
        }
    }

    void FlameWreathEffect()
    {
        std::vector<Unit*> targets;
        std::list<HostilReference *> t_list = m_creature->getThreatManager().getThreatList();

        if(!t_list.size())
            return;

        //store the threat list in a different container
        for(std::list<HostilReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
        {
            Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
            //only on alive players
            if(target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER )
                targets.push_back( target);
        }

        //cut down to size if we have more than 3 targets
        while(targets.size() > 3)
            targets.erase(targets.begin()+rand()%targets.size());

        uint32 i = 0;
        for(std::vector<Unit*>::iterator itr = targets.begin(); itr!= targets.end(); ++itr)
        {
            if(*itr)
            {
                FlameWreathTarget[i] = (*itr)->GetGUID();
                FWTargPosX[i] = (*itr)->GetPositionX();
                FWTargPosY[i] = (*itr)->GetPositionY();
                m_creature->CastSpell((*itr), SPELL_FLAME_WREATH, true);
                i++;
            }
        }
    }

    void ChangeBlizzardWaypointsOrder(uint16 change)
    {
        uint64 temp = 0;
        for (int16 i = 0; i < change; i++)
        {
            temp = shadeOfAranTeleportCreatures[0];
            for (int16 j = 0; j < 7; j++)
                shadeOfAranTeleportCreatures[j] = shadeOfAranTeleportCreatures[j + 1];
            shadeOfAranTeleportCreatures[7] = temp;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() )
            return;

        //Check_Timer
        if(CheckTimer < diff)
        {
            if(m_creature->GetDistance(wLoc.x,wLoc.y,wLoc.z) > 35.0f)
                EnterEvadeMode();
            else
                DoZoneInCombat();
            
            CheckTimer = 3000;
        }else CheckTimer -= diff;

        if(CloseDoorTimer)
        {
            if(CloseDoorTimer <= diff)
            {
                if(pInstance)
                {
                    if(GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                        Door->SetGoState(1);
                    CloseDoorTimer = 0;
                }
            }else CloseDoorTimer -= diff;
        }

        //Cooldowns for casts
        if (ArcaneCooldown)
        {
            if (ArcaneCooldown >= diff)
                ArcaneCooldown -= diff;
        else ArcaneCooldown = 0;
        }

        if (FireCooldown)
        {
            if (FireCooldown >= diff)
                FireCooldown -= diff;
        else FireCooldown = 0;
        }

        if (FrostCooldown)
        {
            if (FrostCooldown >= diff)
                FrostCooldown -= diff;
        else FrostCooldown = 0;
        }

        if(!Drinking && (m_creature->GetPower(POWER_MANA)*100 / m_creature->GetMaxPower(POWER_MANA)) < 20)
        {
            if (DrinkingWaitTime > diff)            // wait 1 sec to prevent broking mass polymorph by last normal spell
            {
                DrinkingWaitTime -= diff;
                return;
            }

            Drinking = true;
            m_creature->InterruptNonMeleeSpells(false);

            DoScriptText(SAY_DRINK, m_creature);

            if (!DrinkInturrupted)
            {
                m_creature->RemoveAllAuras();
                m_creature->CastSpell(m_creature, SPELL_MASS_POLY, true);
                m_creature->CastSpell(m_creature, SPELL_CONJURE, false);
                m_creature->CastSpell(m_creature, SPELL_DRINK, false);
                                                            //Sitting down
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 1);
                DrinkInturruptTimer = 10000;
                PotionUsed = false;
            }
        }

        //Drinking check
        if (Drinking)
        {
            if (DrinkInturrupted && !PotionUsed)
            {
                DrinkInturruptTimer = 1000;                            //to prevent interrupting SPELL_POTION
                PotionUsed = true;
                m_creature->CastSpell(m_creature, SPELL_POTION, false);
            }
            
            if (DrinkInturruptTimer < diff)
            {
                Drinking = false;
                DrinkInturrupted = false;
                m_creature->RemoveAurasDueToSpell(SPELL_DRINK);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                m_creature->CastSpell(m_creature, SPELL_AOE_PYROBLAST, false);
                DrinkingWaitTime = 1000;
            }
            else
                DrinkInturruptTimer -= diff;

            return;
        }

        //Normal casts
        if(NormalCastTimer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                Unit* target = NULL;
                target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (!target)
                    return;

                uint32 Spells[3];
                uint8 AvailableSpells = 0;

                //Check for what spells are not on cooldown
                if (!ArcaneCooldown)
                {
                    Spells[AvailableSpells] = SPELL_ARCMISSLE;
                    AvailableSpells++;
                }
                if (!FireCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FIREBALL;
                    AvailableSpells++;
                }
                if (!FrostCooldown)
                {
                    Spells[AvailableSpells] = SPELL_FROSTBOLT;
                    AvailableSpells++;
                }

                //If no available spells wait 1 second and try again
                if (AvailableSpells)
                {
                    CurrentNormalSpell = Spells[rand() % AvailableSpells];
                    m_creature->CastSpell(target, CurrentNormalSpell,false);
                }
            }
            NormalCastTimer = 1000;
        }else NormalCastTimer -= diff;

        if(SecondarySpellTimer < diff)
        {
            switch (rand()%2)
            {

                case 0:
                    DoCast(m_creature, SPELL_AOE_CS);
                    break;
                case 1:
                    if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        DoCast(pUnit, SPELL_CHAINSOFICE);
                    break;
            }
            SecondarySpellTimer = 5000 + (rand()%15000);
        }else SecondarySpellTimer -= diff;

        if(SuperCastTimer < diff)
        {
            if(ArcaneCasting)
            {
                ArcaneCasting = false;
                SuperCastTimer = 19000+rand()%5000;
                return;
            }
            uint8 Available[2];

            switch (LastSuperSpell)
            {
                case SUPER_AE:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_FLAME:
                    Available[0] = SUPER_AE;
                    Available[1] = SUPER_BLIZZARD;
                    break;
                case SUPER_BLIZZARD:
                    Available[0] = SUPER_FLAME;
                    Available[1] = SUPER_AE;
                    break;
            }

            LastSuperSpell = Available[rand()%2];

            switch (LastSuperSpell)
            {
                case SUPER_AE:

                    if (rand()%2)
                        DoScriptText(SAY_EXPLOSION1, m_creature);
                    else
                        DoScriptText(SAY_EXPLOSION2, m_creature);

                    TeleportCenter();
                    m_creature->CastSpell(m_creature, SPELL_PLAYERPULL, true);
                    m_creature->CastSpell(m_creature, SPELL_MASSSLOW, true);
                    m_creature->CastSpell(m_creature, SPELL_AEXPLOSION, false);
                    ArcaneCasting = true;
                    break;

                case SUPER_FLAME:
                    if (rand()%2)
                        DoScriptText(SAY_FLAMEWREATH1, m_creature);
                    else
                        DoScriptText(SAY_FLAMEWREATH2, m_creature);

                    FlameWreathTimer = 20000;
                    FlameWreathCheckTime = 500;

                    FlameWreathTarget[0] = 0;
                    FlameWreathTarget[1] = 0;
                    FlameWreathTarget[2] = 0;

                    FlameWreathEffect();
                    break;

                case SUPER_BLIZZARD:

                    if (rand()%2)
                        DoScriptText(SAY_BLIZZARD1, m_creature);
                    else
                        DoScriptText(SAY_BLIZZARD2, m_creature);

                    Creature* Spawn = NULL;
                    ChangeBlizzardWaypointsOrder(rand()%8);
                    Spawn = DoSpawnCreature(CREATURE_ARAN_BLIZZARD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 25000);
                    if (Spawn)
                    {
                        Spawn->CastSpell(Spawn, SPELL_CIRCULAR_BLIZZARD, false);
                    }
                    break;
            }

            if(ArcaneCasting)
                SuperCastTimer = 11000;
            else
                SuperCastTimer = 30000 + (rand()%5000);
        }else SuperCastTimer -= diff;

        if(!ElementalsSpawned && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 40)
        {
            ElementalsSpawned = true;
            TeleportCenter();

            for (uint32 i = 0; i < 4; i++)
            {
                Creature* pUnit = m_creature->SummonCreature(CREATURE_WATER_ELEMENTAL, ElementalSpawnPoints[0][i], ElementalSpawnPoints[1][i], m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 90000);
                if (pUnit)
                {
                    pUnit->Attack(m_creature->getVictim(), true);
                    pUnit->setFaction(m_creature->getFaction());
                }
            }

            DoScriptText(SAY_ELEMENTALS, m_creature);
        }

        if(BerserkTimer < diff)
        {
            for (uint32 i = 0; i < 8; i++)
            {
                Creature* pUnit = m_creature->SummonCreature(CREATURE_SHADOW_OF_ARAN, shadowOfAranSpawnPoints[0][i], shadowOfAranSpawnPoints[1][i], m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                if (pUnit)
                {
                    pUnit->Attack(m_creature->getVictim(), true);
                    pUnit->setFaction(m_creature->getFaction());
                }
            }

            DoScriptText(SAY_TIMEOVER, m_creature);

            BerserkTimer = 60000;
        }else BerserkTimer -= diff;

        //Flame Wreath check
        if (FlameWreathTimer)
        {
            if (FlameWreathTimer >= diff)
                FlameWreathTimer -= diff;
            else FlameWreathTimer = 0;

            if (FlameWreathCheckTime < diff)
            {
                for (uint32 i = 0; i < 3; i++)
                {
                    if (!FlameWreathTarget[i])
                        continue;

                    Unit* pUnit = Unit::GetUnit(*m_creature, FlameWreathTarget[i]);
                    if (pUnit && pUnit->GetDistance2d(FWTargPosX[i], FWTargPosY[i]) > 3)
                    {
                        pUnit->CastSpell(pUnit, 20476, true, 0, 0, m_creature->GetGUID());
                        pUnit->CastSpell(pUnit, 11027, true);
                        FlameWreathTarget[i] = 0;
                    }
                }
                FlameWreathCheckTime = 500;
            }else FlameWreathCheckTime -= diff;
        }

        if (ArcaneCooldown && FireCooldown && FrostCooldown)
            DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pAttacker, uint32 &damage)
    {
        if (!DrinkInturrupted && Drinking && damage)
            DrinkInturrupted = true;
    }

    void SpellHit(Unit* pAttacker, const SpellEntry* Spell)
    {
        //We only care about inturrupt effects and only if they are durring a spell currently being casted
        if ((Spell->Effect[0] != SPELL_EFFECT_INTERRUPT_CAST &&
            Spell->Effect[1] != SPELL_EFFECT_INTERRUPT_CAST &&
            Spell->Effect[2] != SPELL_EFFECT_INTERRUPT_CAST) || !m_creature->IsNonMeleeSpellCasted(false))
            return;

        if(ArcaneCasting)
        {
            for(uint8 i = 0; i<3; i++)
                if(Spell->Effect[i] == SPELL_EFFECT_INTERRUPT_CAST)
                    return;
        }

        //Inturrupt effect
        m_creature->InterruptNonMeleeSpells(false);

        //Normally we would set the cooldown equal to the spell duration
        //but we do not have access to the DurationStore

        switch (CurrentNormalSpell)
        {
            case SPELL_ARCMISSLE: ArcaneCooldown = 5000; break;
            case SPELL_FIREBALL: FireCooldown = 5000; break;
            case SPELL_FROSTBOLT: FrostCooldown = 5000; break;
        }
    }
};

struct TRINITY_DLL_DECL water_elementalAI : public ScriptedAI
{
    water_elementalAI(Creature *c) : ScriptedAI(c) {}

    uint32 CastTimer;

    void Reset()
    {
        CastTimer = 2000 + (rand()%3000);
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() )
            return;

        if(CastTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_WATERBOLT);
            CastTimer = 2000 + (rand()%3000);
        }else CastTimer -= diff;
    }
};

struct TRINITY_DLL_DECL shadow_of_aranAI : public ScriptedAI
{
    shadow_of_aranAI(Creature *c) : ScriptedAI(c) {}

    uint32 CastTimer;

    void Reset()
    {
        CastTimer = 2000;
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() )
            return;

        if(CastTimer < diff)
        {
            switch(rand()%2)
            {
            case 0:
                m_creature->CastSpell(m_creature, SPELL_FROSTBOLT_VOLLEY, false);
                CastTimer = 5000;
                break;
            case 1:
                m_creature->CastSpell(m_creature, SPELL_AMISSILE_VOLLEY, false);
                CastTimer = 20000;
                break;
            default:
                break;
            }
        }else CastTimer -= diff;
    }
};

struct TRINITY_DLL_DECL circular_blizzardAI : public ScriptedAI
{
    circular_blizzardAI(Creature *c) : ScriptedAI(c) 
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        if(pInstance)
            AranGUID = pInstance->GetData64(DATA_ARAN);
    }

    uint16 currentWaypoint;
    uint16 waypointTimer; 
    uint64 AranGUID;
    WorldLocation wLoc;
    ScriptedInstance *pInstance;

    void Reset()
    {
        if(AranGUID)
        {
            Creature *pAran = Unit::GetCreature(*m_creature, AranGUID);
            if(pAran)
            {
                Creature *pCreature = Unit::GetCreature(*m_creature, ((boss_aranAI *)pAran->AI())->shadeOfAranTeleportCreatures[0]);
                if(pCreature)
                    pCreature->GetPosition(wLoc);
            }
        }
        if(wLoc.x || wLoc.y || wLoc.z)
            DoTeleportTo(wLoc.x, wLoc.y, wLoc.z);

        currentWaypoint = 0;
        waypointTimer = 0;
    }

    void Aggro(Unit* who){}

    void JustDied(Unit* killer){}

    void UpdateAI(const uint32 diff)
    {
        if (waypointTimer < diff)
        {
            if (currentWaypoint < 7)
                ++currentWaypoint;
            else
                currentWaypoint = 0;

            if(AranGUID)
            {
                Creature *pAran = Unit::GetCreature(*m_creature, AranGUID);
                if(pAran)
                {
                    Creature *pCreature = Unit::GetCreature(*m_creature, ((boss_aranAI *)pAran->AI())->shadeOfAranTeleportCreatures[currentWaypoint]);
                    if(pCreature)
                        pCreature->GetPosition(wLoc);
                    else
                        m_creature->Kill(m_creature, false);
                }
            }
            m_creature->GetMotionMaster()->MovePoint(currentWaypoint, wLoc.x, wLoc.y, wLoc.z);
            waypointTimer = 3000;
        }
        else waypointTimer -= diff;
    }
};

CreatureAI* GetAI_boss_aran(Creature *_Creature)
{
    return new boss_aranAI (_Creature);
}

CreatureAI* GetAI_water_elemental(Creature *_Creature)
{
    return new water_elementalAI (_Creature);
}

CreatureAI* GetAI_shadow_of_aran(Creature *_Creature)
{
    shadow_of_aranAI* shadowAI = new shadow_of_aranAI(_Creature);

    return (CreatureAI*)shadowAI;
}

CreatureAI* GetAI_circular_blizzard(Creature *_Creature)
{
    circular_blizzardAI* blizzardAI = new circular_blizzardAI(_Creature);

    return (CreatureAI*)blizzardAI;
}

void AddSC_boss_shade_of_aran()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_shade_of_aran";
    newscript->GetAI = &GetAI_boss_aran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_shadow_of_aran";
    newscript->GetAI = &GetAI_shadow_of_aran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_aran_elemental";
    newscript->GetAI = &GetAI_water_elemental;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_aran_blizzard";
    newscript->GetAI = &GetAI_circular_blizzard;
    newscript->RegisterSelf();
}

