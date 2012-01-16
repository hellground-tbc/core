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

struct TRINITY_DLL_DECL boss_aranAI : public ScriptedAI
{
    boss_aranAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
        m_creature->GetPosition(wLoc);
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ARCMISSLE);
        if (TempSpell)
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

    float blizzardWaypoints[2][8];

    WorldLocation wLoc;

    uint32 DrinkInturruptTimer;
    uint32 DrinkingWaitTime;

    bool ElementalsSpawned;
    bool Drinking;
    bool DrinkInturrupted;
    bool PotionUsed;
    bool ArcaneCasting;

    void SetBlizzardWaypoints()
    {
        blizzardWaypoints[0][0] = -11154.3;    blizzardWaypoints[1][0] = -1903.3;
        blizzardWaypoints[0][1] = -11163.6;    blizzardWaypoints[1][1] = -1898.7;
        blizzardWaypoints[0][2] = -11173.6;    blizzardWaypoints[1][2] = -1901.2;
        blizzardWaypoints[0][3] = -11178.1;    blizzardWaypoints[1][3] = -1910.4;
        blizzardWaypoints[0][4] = -11175.4;    blizzardWaypoints[1][4] = -1920.6;
        blizzardWaypoints[0][5] = -11166.6;    blizzardWaypoints[1][5] = -1925.1;
        blizzardWaypoints[0][6] = -11156.5;    blizzardWaypoints[1][6] = -1922.8;
        blizzardWaypoints[0][7] = -11151.8;    blizzardWaypoints[1][7] = -1913.5;
    }

    void Reset()
    {
        ClearCastQueue();

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

        if (pInstance)
        {
            // Not in progress
            if (pInstance->GetData(DATA_SHADEOFARAN_EVENT) != DONE)
                pInstance->SetData(DATA_SHADEOFARAN_EVENT, NOT_STARTED);

            if (GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                Door->SetGoState(GO_STATE_ACTIVE);
        }

        if (m_creature->isAlive())
        {
            SetBlizzardWaypoints();
        }
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_KILL1, SAY_KILL2), m_creature);
    }

    void TeleportCenter()
    {
        m_creature->CastSpell(m_creature, SPELL_BLINK_CENTER, true);
        DoTeleportTo(wLoc.coord_x,wLoc.coord_y,wLoc.coord_z, 0);
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        pInstance->SetData(DATA_SHADEOFARAN_EVENT, DONE);

        if (GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
            Door->SetGoState(GO_STATE_ACTIVE);
    }

    bool PlayerHaveAtiesh()
    {
        Map::PlayerList const &PlayerList = ((InstanceMap*)m_creature->GetMap())->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl->HasEquiped(22632) ||
                i_pl->HasEquiped(22631) ||
                i_pl->HasEquiped(22630) ||
                i_pl->HasEquiped(22589))
                return true;
        }

        return false;
    }

    void EnterCombat(Unit *who)
    {
        if (PlayerHaveAtiesh())
            DoScriptText(SAY_ATIESH, m_creature);
        else
            DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), m_creature);

        if (pInstance)
        {
            pInstance->SetData(DATA_SHADEOFARAN_EVENT, IN_PROGRESS);
            if(GameObject* Door = GameObject::GetGameObject(*m_creature, pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                Door->SetGoState(GO_STATE_READY);
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
            if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
                targets.push_back(target);
        }

        //cut down to size if we have more than 3 targets
        while (targets.size() > 3)
            targets.erase(targets.begin()+rand()%targets.size());

        uint32 i = 0;
        for (std::vector<Unit*>::iterator itr = targets.begin(); itr!= targets.end(); ++itr)
        {
            if (*itr)
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
        float temp[2] = {0, 0};
        for (int16 i = 0; i < change; i++)
        {
            temp[0] = blizzardWaypoints[0][0];
            temp[1] = blizzardWaypoints[1][0];
            for (int16 j = 0; j < 7; j++)
            {
                blizzardWaypoints[0][j] = blizzardWaypoints[0][j + 1];
                blizzardWaypoints[1][j] = blizzardWaypoints[1][j + 1];
            }
            blizzardWaypoints[0][7] = temp[0];
            blizzardWaypoints[1][7] = temp[1];
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //Check_Timer
        if (CheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(&wLoc, 35.0f))
                EnterEvadeMode();
            else
                DoZoneInCombat();

            CheckTimer = 3000;
        }
        else
            CheckTimer -= diff;

        if (CloseDoorTimer)
        {
            if (CloseDoorTimer <= diff)
            {
                if (pInstance)
                {
                    if (GameObject* Door = m_creature->GetMap()->GetGameObject(pInstance->GetData64(DATA_GAMEOBJECT_LIBRARY_DOOR)))
                        Door->SetGoState(GO_STATE_READY);
                    CloseDoorTimer = 0;
                }
            }
            else
                CloseDoorTimer -= diff;
        }

        //Cooldowns for casts
        if (ArcaneCooldown)
        {
            if (ArcaneCooldown >= diff)
                ArcaneCooldown -= diff;
            else
                ArcaneCooldown = 0;
        }

        if (FireCooldown)
        {
            if (FireCooldown >= diff)
                FireCooldown -= diff;
            else
                FireCooldown = 0;
        }

        if (FrostCooldown)
        {
            if (FrostCooldown >= diff)
                FrostCooldown -= diff;
            else
                FrostCooldown = 0;
        }

        if (!Drinking && (m_creature->GetPower(POWER_MANA)*100 / m_creature->GetMaxPower(POWER_MANA)) < 20)
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
        if (NormalCastTimer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                Unit* target = NULL;
                target = SelectUnit(SELECT_TARGET_RANDOM, 0, 0.0f, true);

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
                    AddSpellToCast(target, CurrentNormalSpell, false, true);
                }
            }
            NormalCastTimer = 1000;
        }
        else
            NormalCastTimer -= diff;

        if (SecondarySpellTimer < diff)
        {
            switch (rand()%2)
            {
                case 0:
                    AddSpellToCast(m_creature, SPELL_AOE_CS);
                    break;
                case 1:
                    if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0, GetSpellMaxRange(SPELL_CHAINSOFICE), true))
                        AddSpellToCast(pUnit, SPELL_CHAINSOFICE);
                    break;
            }

            SecondarySpellTimer = urand(5000, 20000);
        }
        else
            SecondarySpellTimer -= diff;

        if (SuperCastTimer < diff)
        {
            if (ArcaneCasting)
            {
                ArcaneCasting = false;
                SuperCastTimer = urand(19000, 24000);
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
                    DoScriptText(RAND(SAY_EXPLOSION1, SAY_EXPLOSION2), m_creature);

                    TeleportCenter();
                    m_creature->CastSpell(m_creature, SPELL_PLAYERPULL, true);
                    m_creature->CastSpell(m_creature, SPELL_MASSSLOW, true);
                    m_creature->CastSpell(m_creature, SPELL_AEXPLOSION, false);
                    ArcaneCasting = true;
                    break;

                case SUPER_FLAME:
                    DoScriptText(RAND(SAY_FLAMEWREATH1, SAY_FLAMEWREATH2), m_creature);

                    FlameWreathTimer = 20000;
                    FlameWreathCheckTime = 500;

                    FlameWreathTarget[0] = 0;
                    FlameWreathTarget[1] = 0;
                    FlameWreathTarget[2] = 0;

                    FlameWreathEffect();
                    break;

                case SUPER_BLIZZARD:
                    DoScriptText(RAND(SAY_BLIZZARD1, SAY_BLIZZARD2), m_creature);

                    if  (Creature * blizzard = m_creature->GetMap()->GetCreature(pInstance->GetData64(DATA_BLIZZARD)))
                    {
                        ChangeBlizzardWaypointsOrder(urand(0, 7));
                        blizzard->CastSpell(blizzard, SPELL_CIRCULAR_BLIZZARD, true);
                    }

                    break;
            }

            if (ArcaneCasting)
                SuperCastTimer = 11000;
            else
                SuperCastTimer = urand(30000, 35000);
        }
        else
            SuperCastTimer -= diff;

        if (!ElementalsSpawned && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 40)
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

        if (BerserkTimer < diff)
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
        }
        else
            BerserkTimer -= diff;

        //Flame Wreath check
        if (FlameWreathTimer)
        {
            if (FlameWreathTimer >= diff)
                FlameWreathTimer -= diff;
            else
                FlameWreathTimer = 0;

            if (FlameWreathCheckTime < diff)
            {
                for (uint32 i = 0; i < 3; i++)
                {
                    if (!FlameWreathTarget[i])
                        continue;

                    Unit* pUnit = m_creature->GetMap()->GetUnit(FlameWreathTarget[i]);
                    if (pUnit && pUnit->GetDistance2d(FWTargPosX[i], FWTargPosY[i]) > 1)
                    {
                        pUnit->CastSpell(pUnit, 20476, true, 0, 0, m_creature->GetGUID());
                        pUnit->CastSpell(pUnit, 11027, true);
                        FlameWreathTarget[i] = 0;
                    }
                }
                FlameWreathCheckTime = 500;
            }
            else
                FlameWreathCheckTime -= diff;
        }

        CastNextSpellIfAnyAndReady();
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

        if (ArcaneCasting)
            return;

        //Inturrupt effect
        m_creature->InterruptNonMeleeSpells(false);

        //Normally we would set the cooldown equal to the spell duration
        //but we do not have access to the DurationStore

        switch (CurrentNormalSpell)
        {
            case SPELL_ARCMISSLE:
                ArcaneCooldown = 5000;
                break;
            case SPELL_FIREBALL:
                FireCooldown = 5000;
                break;
            case SPELL_FROSTBOLT:
                FrostCooldown = 5000;
                break;
            default:
                break;
        }
    }
};

struct TRINITY_DLL_DECL water_elementalAI : public ScriptedAI
{
    water_elementalAI(Creature *c) : ScriptedAI(c) {}

    uint32 CastTimer;

    void Reset()
    {
        ClearCastQueue();

        CastTimer = urand(2000, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CastTimer < diff)
        {
            //AddSpellToCast(m_creature->getVictim(), SPELL_WATERBOLT);
            DoCast(m_creature->getVictim(), SPELL_WATERBOLT);
            CastTimer = 2000 + (rand()%3000);
        }
        else
            CastTimer -= diff;

        CastNextSpellIfAnyAndReady();
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

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CastTimer < diff)
        {
            if (rand()%3)
            {
                m_creature->CastSpell(m_creature, SPELL_FROSTBOLT_VOLLEY, false);
                CastTimer = 5000;
            }
            else
            {
                m_creature->CastSpell(m_creature, SPELL_AMISSILE_VOLLEY, false);
                CastTimer = 20000;
            }
        }
        else
            CastTimer -= diff;
    }
};

struct TRINITY_DLL_DECL circular_blizzardAI : public ScriptedAI
{
    circular_blizzardAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint16 currentWaypoint;
    uint16 waypointTimer;
    WorldLocation wLoc;
    ScriptedInstance *pInstance;
    bool move;

    void Reset()
    {
        move = false;
        currentWaypoint = 0;
        waypointTimer = 0;
    }

    void JustDied(Unit* killer){}

    void SpellHit(Unit * caster, const SpellEntry * spell)
    {
        if (spell->Id == SPELL_CIRCULAR_BLIZZARD)
        {
            Creature *pAran = m_creature->GetMap()->GetCreature(pInstance->GetData64(DATA_ARAN));
            if (pAran)
            {
                wLoc.coord_x = ((boss_aranAI*)pAran->AI())->blizzardWaypoints[0][0];
                wLoc.coord_y = ((boss_aranAI*)pAran->AI())->blizzardWaypoints[1][0];
                wLoc.coord_z = pAran->GetPositionZ();
            }

            if (wLoc.coord_x || wLoc.coord_y || wLoc.coord_z)
                DoTeleportTo(wLoc.coord_x, wLoc.coord_y, wLoc.coord_z);

            currentWaypoint = 0;
            waypointTimer = 0;
            move = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!move)
            return;

        if (waypointTimer < diff)
        {
            if (currentWaypoint < 7)
                ++currentWaypoint;
            else
            {
                currentWaypoint = 0;
                move = false;
            }

            Creature *pAran = m_creature->GetMap()->GetCreature(pInstance->GetData64(DATA_ARAN));
            if(pAran)
            {
                wLoc.coord_x = ((boss_aranAI*)pAran->AI())->blizzardWaypoints[0][currentWaypoint];
                wLoc.coord_y = ((boss_aranAI*)pAran->AI())->blizzardWaypoints[1][currentWaypoint];
            }

            m_creature->GetMotionMaster()->MovePoint(currentWaypoint, wLoc.coord_x, wLoc.coord_y, wLoc.coord_z);
            waypointTimer = 3000;
        }
        else
            waypointTimer -= diff;
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

