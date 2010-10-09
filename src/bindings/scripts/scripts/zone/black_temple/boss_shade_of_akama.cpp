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
SDName: Boss_Shade_of_Akama
SD%Complete: 90
SDComment: Seems to be complete.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

#define SAY_DEATH                   -1564013
#define SAY_LOW_HEALTH              -1564014

// Ending cinematic text
#define SAY_FREE                    -1564015
#define SAY_BROKEN_FREE_01          -1564016
#define SAY_BROKEN_FREE_02          -1564017

#define CHANNELERS_COUNT 6
#define CHANNELERS_Z 118.538f

#define MAX_BROKEN 8

static float BrokenPositions[8][2] =
{
    {505.113, 355.168},
    {513.113, 364.402},
    {517.816, 376.878},
    {517.841, 396.493},
    {516.314, 407.241},
    {514.607, 426.291},
    {514.937, 437.125},
    {508.137, 445.109}
};

static float BrokenMoveTo[8][2] =
{
    {482.526, 367.918},
    {491.073, 371.607},
    {488.4, 383.502},
    {495.84, 394.657},
    {489.34, 410.812},
    {494.039, 424.049},
    {495.574, 436.208},
    {485.928, 443.373}
};

#define GOSSIP_ITEM                 "We are ready to fight alongside you, Akama"

struct Location
{
    float x, y, o, z;
};

#define SPAWN_LOCATION_SOUTH 0
#define SPAWN_LOCATION_NORTH 1

#define SPAWN_Z 112.87f

static float moveTo[2][3] =
{
    {470.0, 400.80, CHANNELERS_Z},
    {482.45, 401.02, SPAWN_Z}
};

static float SpawnLocations[2][2]=
{
    {499.30, 469.37},
    {499.06, 331.73}
};


// Locations
#define AGGRO_X         482.793182
#define AGGRO_Y         401.270172
#define AGGRO_Z         112.783928

/*#define AKAMA_X 488.25
#define AKAMA_Y 401.22
#define AKAMA_Z         112.78
*/

#define AKAMA_X         514.78
#define AKAMA_Y         400.79
#define AKAMA_Z         112.78

// Spells
#define SPELL_VERTEX_SHADE_BLACK    39833

#define SPELL_SHADE_SOUL_CHANNEL    40401
#define SPELL_SHADE_SOUL_CHANNEL_2  40520

#define SPELL_DESTRUCTIVE_POISON    40874
#define SPELL_CHAIN_LIGHTNING       40536

#define SPELL_AKAMA_SOUL_CHANNEL    40447
#define SPELL_AKAMA_SOUL_RETRIEVE   40902

#define AKAMA_SOUL_EXPEL            40855
#define SPELL_DEBILITATIG_STRIKE    41178
#define SPELL_SHIELD_BASH           41180
#define SPELL_HEROIC_STRIKE         41975

// Channeler entry
#define CREATURE_CHANNELER          23421
#define CREATURE_SORCERER           23215
#define CREATURE_DEFENDER           23216
#define CREATURE_BROKEN             23319

static const uint32 spawnEntries[3]= {23523, 23318, 23524};

struct TRINITY_DLL_DECL mob_ashtongue_channelerAI : public ScriptedAI
{
    mob_ashtongue_channelerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance *)c->GetInstanceData();
        ShadeGUID = 0;
    }

    ScriptedInstance *pInstance;
    uint64 ShadeGUID;
    bool m_channel;

    void Reset()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SHADE_SOUL_CHANNEL);
        m_channel = true;
    }

    void OnAuraRemove(Aura *aura, bool stackRemove);
    void EnterCombat(Unit* who) {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!m_channel)
            return;

        if (ShadeGUID)
        {
            if (!m_creature->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
            {
                m_creature->RemoveAurasDueToSpell(SPELL_SHADE_SOUL_CHANNEL);
                if (Unit *shade = m_creature->GetUnit(*m_creature, ShadeGUID))
                {
                    if (shade->isAlive())
                        DoCast(shade, SPELL_SHADE_SOUL_CHANNEL);
                }
            }
        }
        else
        {
            if (pInstance)
                ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
        }
    }
};

struct TRINITY_DLL_DECL mob_ashtongue_defenderAI : public ScriptedAI
{
    mob_ashtongue_defenderAI(Creature* c) : ScriptedAI(c)
    { 
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 m_debilStrikeTimer;
    uint32 m_shieldBashTimer;
    uint32 m_checkTimer;
    bool m_attack;

    void Reset()
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);

        m_debilStrikeTimer = 10000;
        m_shieldBashTimer = 1000;
        m_checkTimer = 10000;
        m_attack = false;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;
        
        if (!m_attack)
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
            m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_attack = true;
        }

        if(pInstance)
        {
            if(Creature *pAkama = pInstance->GetCreature(pInstance->GetData64(DATA_AKAMA_SHADE)))
            {   
                m_creature->AddThreat(pAkama, 100000.0f);
                AttackStart(pAkama);
            }
        }
    }

    void DoMeleeAttackIfReady()
    {
        if(me->hasUnitState(UNIT_STAT_CASTING))
            return;

        //Make sure our attack is ready and we aren't currently casting before checking distance
        if (me->isAttackReady())
        {
            //If we are within range melee the target
            if (me->IsWithinMeleeRange(me->getVictim()))
            {
                me->AttackerStateUpdate(me->getVictim());
                me->resetAttackTimer();
                
                if(IS_CREATURE_GUID(me->getVictimGUID()))
                    DoCast(me->getVictim(), SPELL_HEROIC_STRIKE, true);
            }
        }

        if (me->haveOffhandWeapon() && me->isAttackReady(OFF_ATTACK))
        {
            //If we are within range melee the target
            if (me->IsWithinMeleeRange(me->getVictim()))
            {
                me->AttackerStateUpdate(me->getVictim(), OFF_ATTACK);
                me->resetAttackTimer(OFF_ATTACK);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_attack || !UpdateVictim())
            return;

        if (m_debilStrikeTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_DEBILITATIG_STRIKE, false);
            m_debilStrikeTimer = 20000;
        }
        else
            m_debilStrikeTimer -= diff;

        if(m_shieldBashTimer < diff)
        {
            if (m_creature->getVictim()->hasUnitState(UNIT_STAT_CASTING))
            {
                DoCast(m_creature->getVictim(), SPELL_SHIELD_BASH, false);
                m_shieldBashTimer = 10000;
            }
        }
        else
            m_shieldBashTimer -= diff;

        if(m_checkTimer < diff)
        {
            if(!pInstance)
                return;

            if(Creature *pAkama = m_creature->GetCreature(*m_creature, pInstance->GetData64(DATA_SHADEOFAKAMA)))
            {
                if(!pAkama->isAlive())
                {
                    m_creature->Kill(m_creature, false);
                    m_creature->RemoveCorpse();
                }
            }
            m_checkTimer = 5000;
        }
        else
            m_checkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

enum spiritbinderSpells
{
    SPELL_CHAIN_HEAL  = 42027,
    SPELL_SPIRIT_HEAL = 42317,
    SPELL_SPIRIT_MEND = 42025
};

struct TRINITY_DLL_DECL mob_ashtongue_spiritbinderAI : public ScriptedAI
{
    mob_ashtongue_spiritbinderAI(Creature* c) : ScriptedAI(c)
    { 
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 m_chainHealTimer;
    uint32 m_spiritHealTimer;
    uint32 m_spiritMendTimer;
    uint32 m_checkTimer;

    void Reset()
    {
        m_chainHealTimer  = urand(10000, 15000);
        m_spiritHealTimer = urand(7000, 10000);
        m_spiritMendTimer = urand(14000, 20000);
        m_checkTimer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_chainHealTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_CHAIN_HEAL, false);
            m_chainHealTimer = 20000;
        }
        else
            m_chainHealTimer -= diff;

        if(m_spiritMendTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_SPIRIT_MEND, false);
            m_spiritMendTimer = 20000;
        }
        else
            m_spiritMendTimer -= diff;

        if(m_spiritHealTimer < diff)
        {
            if(Unit *pFriend = DoSelectLowestHpFriendly(40.0f, 1000))
            {
                AddSpellToCast(pFriend, SPELL_SPIRIT_HEAL, false);
                m_spiritHealTimer = 10000;
            }
        }
        else
            m_spiritHealTimer -= diff;

        if(m_checkTimer < diff)
        {
            if(!pInstance)
                return;

            if(Creature *pAkama = m_creature->GetCreature(*m_creature, pInstance->GetData64(DATA_SHADEOFAKAMA)))
            {
                if(!pAkama->isAlive())
                {
                    m_creature->Kill(m_creature, false);
                    m_creature->RemoveCorpse();
                }
            }
            m_checkTimer = 5000;
        }
        else
            m_checkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum elementalistSpells
{
    SPELL_RAIN_OF_FIRE = 42023,
    SPELL_LIGHTNING_BOLT = 42024 
};

struct TRINITY_DLL_DECL mob_ashtongue_elementalistAI : public ScriptedAI
{
    mob_ashtongue_elementalistAI(Creature* c) : ScriptedAI(c)
    { 
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 m_rainofFireTimer;
    uint32 m_lightningBoltTimer;
    uint32 m_checkTimer;

    void Reset()
    {
        m_rainofFireTimer  = urand(5000, 18000);
        m_lightningBoltTimer = urand(2000, 4000);
        m_checkTimer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_lightningBoltTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_LIGHTNING_BOLT, false);
            m_lightningBoltTimer = 10000;
        }
        else
            m_lightningBoltTimer -= diff;

        if(m_rainofFireTimer < diff)
        {
            DoZoneInCombat();
            if(Unit *pEnemy = SelectUnit(SELECT_TARGET_RANDOM, 0, 40.0f, true))
            {
                AddSpellToCast(pEnemy, SPELL_RAIN_OF_FIRE, false);
                m_rainofFireTimer = 15000;
            }
        }
        else
            m_rainofFireTimer -= diff;

        if(m_checkTimer < diff)
        {
            if(!pInstance)
                return;

            if(Creature *pAkama = m_creature->GetCreature(*m_creature, pInstance->GetData64(DATA_SHADEOFAKAMA)))
            {
                if(!pAkama->isAlive())
                {
                    m_creature->Kill(m_creature, false);
                    m_creature->RemoveCorpse();
                }
            }
            m_checkTimer = 5000;
        }
        else
            m_checkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum rogueSpells
{
    SPELL_DEBILITATING_POISON = 41978,
    SPELL_EVISCERATE = 41177
};

struct TRINITY_DLL_DECL mob_ashtongue_rogueAI : public ScriptedAI
{
    mob_ashtongue_rogueAI(Creature* c) : ScriptedAI(c)
    { 
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 m_debilPoisonTimer;
    uint32 m_eviscerateTimer;
    uint32 m_checkTimer;

    void Reset()
    {
        m_debilPoisonTimer  = urand(5000, 15000);
        m_eviscerateTimer = urand(2000, 7000);
        m_checkTimer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_debilPoisonTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_DEBILITATING_POISON, false);
            m_debilPoisonTimer = 15000;
        }
        else
            m_debilPoisonTimer -= diff;

        if(m_eviscerateTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_EVISCERATE, false);
            m_eviscerateTimer = 10000;
        }
        else
            m_eviscerateTimer -= diff;

        if(m_checkTimer < diff)
        {
            if(!pInstance)
                return;

            if(Creature *pAkama = m_creature->GetCreature(*m_creature, pInstance->GetData64(DATA_SHADEOFAKAMA)))
            {
                if(!pAkama->isAlive())
                {
                    m_creature->Kill(m_creature, false);
                    m_creature->RemoveCorpse();
                }
            }
            m_checkTimer = 5000;
        }
        else
            m_checkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL mob_ashtongue_sorcererAI : public ScriptedAI
{
    mob_ashtongue_sorcererAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance *)c->GetInstanceData();
        ShadeGUID = 0;
    }

    ScriptedInstance *pInstance;

    uint64 ShadeGUID;
    bool m_channel;
    uint32 m_checkTimer;

    void Reset()
    {
        m_checkTimer = 1000;
        m_channel = true;
    }

    void OnAuraRemove(Aura *aura, bool stackRemove);
    void EnterCombat(Unit* who) {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!m_channel)
            return;

        if (ShadeGUID)
        {
            if (!m_creature->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
            {
                if (m_checkTimer < diff)
                {
                    if (Unit *shade = m_creature->GetUnit(*m_creature, ShadeGUID))
                    {
                        if (m_creature->GetDistance2d(shade) <= 10.0f +urand(0,5))
                        {
                            m_creature->StopMoving();
                            m_creature->RemoveAurasDueToSpell(SPELL_SHADE_SOUL_CHANNEL);
                            if (shade->isAlive())
                                DoCast(shade, SPELL_SHADE_SOUL_CHANNEL);
                            else
                                m_channel = false;
                        }
                    }
                    m_checkTimer = 1000;
                }
                else
                    m_checkTimer -= diff;
            }
        }
        else
        {
            if (pInstance)
                ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
        }
    }
};

struct TRINITY_DLL_DECL boss_shade_of_akamaAI : public ScriptedAI
{
    boss_shade_of_akamaAI(Creature* c) : ScriptedAI(c), m_summons(c)
    {
        m_creature->setActive(true);
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        AkamaGUID = pInstance ? pInstance->GetData64(DATA_AKAMA_SHADE) : 0;

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);

        m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
        m_creature->GetPosition(wLoc);
    }

    void EnterEvadeMode()
    {
        if(!_EnterEvadeMode())
            return;

        DespawnChannelersAndSorcerers();

        if(Unit *owner = me->GetCharmerOrOwner())
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, m_creature->GetFollowAngle(), MOTION_SLOT_ACTIVE);
            Reset();
        }
        else
             me->GetMotionMaster()->MoveTargetedHome();
    }

    ScriptedInstance* pInstance;

    std::list<uint64> m_channelers;
    std::list<uint64> m_sorcerers;
    SummonList m_summons;

    uint64 AkamaGUID;

    uint32 m_damageTimer;

    uint32 m_waveTimer;
    uint32 m_guardTimer;
    uint32 m_sorcTimer;

    uint32 m_freeSlot;

    uint32 m_checkTimer;

    uint16 event_phase;

    WorldLocation wLoc;

    void Reset()
    {
        m_freeSlot = 0;
        m_checkTimer = 3000;
        event_phase = 0;
        m_summons.DespawnAll();
        SpawnChannelers();

        m_creature->SetSpeed(MOVE_WALK, 1);
        m_creature->SetSpeed(MOVE_RUN, 1);

        if (Creature *akama = m_creature->GetCreature(*m_creature, AkamaGUID))
        {
            if (akama->isDead())
                akama->Respawn();
            akama->AI()->EnterEvadeMode();
            akama->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        m_damageTimer = 10000;
        m_waveTimer = 7000;
        m_guardTimer = 9000;
        m_sorcTimer = 9000;

        if (pInstance)
            pInstance->SetData(DATA_SHADEOFAKAMAEVENT, NOT_STARTED);
    }

    void ProcessSpawning(const uint32 diff)
    {
        if (m_waveTimer < diff)
        {
            Creature *akama = m_creature->GetCreature(*m_creature, AkamaGUID);
            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    Creature *mob = m_creature->SummonCreature(spawnEntries[j], SpawnLocations[i][0], SpawnLocations[i][1], SPAWN_Z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                    if (mob)
                    {
                        m_summons.Summon(mob);
                        if (akama)
                            mob->AI()->AttackStart(akama);
                    }
                }
            }
            m_waveTimer = 35000;
        }
        else
            m_waveTimer -= diff;

        if (m_guardTimer < diff)
        {
            Creature *mob = m_creature->SummonCreature(CREATURE_DEFENDER, SpawnLocations[0][0], SpawnLocations[0][1], SPAWN_Z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
            if (mob)
            {
                m_summons.Summon(mob);
                if(Creature *pAkama = m_creature->GetCreature(*m_creature, AkamaGUID))
                    m_creature->AddThreat(pAkama, 100000.0f);

                mob->GetMotionMaster()->MovePoint(0, AKAMA_X, AKAMA_Y, AKAMA_Z);
                mob->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            m_guardTimer = 30000;
        }
        else
            m_guardTimer -= diff;

        if (m_sorcTimer < diff)
        {
            if (!m_freeSlot)
            {
                m_sorcTimer = 5000;
                return;
            }

            Creature *mob = m_creature->SummonCreature(CREATURE_SORCERER, SpawnLocations[1][0], SpawnLocations[1][1], SPAWN_Z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
            if (mob)
            {
                m_sorcerers.push_back(mob->GetGUID());
                mob->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            }
            m_sorcTimer = 30000;
        }
        else
            m_sorcTimer -= diff;
    }

    void KilledUnit(Unit *) {}
    void JustDied(Unit* killer);

    void SummonedCreatureDespawn(Creature *summon)
    {
        if (summon->GetEntry() == CREATURE_CHANNELER || summon->GetEntry() == CREATURE_SORCERER)
            return;
        m_summons.Despawn(summon);
    }

    void MoveInLineOfSight(Unit *who) {}

    void AttackStart(Unit* who)
    {
        if (!who || event_phase != 6)
            return;

        if (m_creature->Attack(who, true))
            DoStartMovement(who);
    }

    void OnAuraApply(Aura *aura, Unit *, bool stackApply)
    {
        if (aura->GetSpellProto()->Id == SPELL_SHADE_SOUL_CHANNEL_2)
            SetBanish(true);
        if (aura->GetSpellProto()->Id == SPELL_AKAMA_SOUL_CHANNEL)
            event_phase = 1;
    }

    void TurnOffChanneling()
    {
        for (std::list<uint64>::const_iterator itr = m_channelers.begin(); itr != m_channelers.end(); ++itr)
        {
            if (Creature *channeler = m_creature->GetCreature(*m_creature, *itr))
            {
                ((mob_ashtongue_channelerAI *)channeler->AI())->m_channel = false;
                channeler->InterruptNonMeleeSpells(false);
            }
        }
        for (std::list<uint64>::const_iterator itr = m_sorcerers.begin(); itr != m_sorcerers.end(); ++itr)
        {
            if (Creature *sorc = m_creature->GetCreature(*m_creature, *itr))
            {
                ((mob_ashtongue_sorcererAI *)sorc->AI())->m_channel = false;
                sorc->InterruptNonMeleeSpells(false);
            }
        }
        m_freeSlot = 0;
    }

    void DamageMade(Unit *target, uint32 &damage, bool direct_damage)
    {
        if (target->GetGUID() == AkamaGUID)
        {
            if (event_phase == 6)
            {
                SetBanish(false);
                event_phase = 7;
                TurnOffChanneling();

                target->InterruptNonMeleeSpells(false);
                ((Creature *)target)->AI()->AttackStart(m_creature);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type == HOME_MOTION_TYPE)
        {
            Reset();
            return;
        }

        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 0)
            event_phase = 4;
        else
        {
            if (id == 1)
            {
                event_phase = 6;
                if (Creature *akama = m_creature->GetCreature(*m_creature, AkamaGUID))
                {
                    m_creature->AddThreat(akama, 1000000.0f);
                    m_creature->AI()->AttackStart(akama);
                }
            }
        }
    }

    void OnAuraRemove(Aura *aura, bool stackRemove)
    {
        if (aura->GetSpellProto()->Id == SPELL_SHADE_SOUL_CHANNEL_2)
        {
            ++m_freeSlot;
            if (!stackRemove)
                SetBanish(false);
            if (event_phase == 3 || event_phase == 5)
                --event_phase;
        }
    }

    void SetBanish(bool set)
    {
        if (set)
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        else
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void DespawnChannelersAndSorcerers()
    {
        for (std::list<uint64>::const_iterator itr = m_channelers.begin(); itr != m_channelers.end(); ++itr)
        {
            Creature *channeler = m_creature->GetCreature(*m_creature, *itr);
            if (channeler)
            {
                channeler->setDeathState(JUST_DIED);
                channeler->RemoveCorpse();
            }
        }
        m_channelers.clear();
        for (std::list<uint64>::const_iterator itr = m_sorcerers.begin(); itr != m_sorcerers.end(); ++itr)
        {
            Creature *sorc = m_creature->GetCreature(*m_creature, *itr);
            if (sorc)
            {
                sorc->setDeathState(JUST_DIED);
                sorc->RemoveCorpse();;
            }
        }
        m_sorcerers.clear();
    }

    void SpawnChannelers()
    {
        if (m_channelers.empty())
        {
            float pos_x = m_creature->GetPositionX();
            float pos_y = m_creature->GetPositionY();
            for (int i = 0; i < CHANNELERS_COUNT; ++i)
            {
                float x = pos_x + 15.0f * cos(M_PI/3 * i);
                float y = pos_y + 15.0f * sin(M_PI/3 * i);
                Creature *channeler = m_creature->SummonCreature(CREATURE_CHANNELER, x, y, CHANNELERS_Z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                if (channeler)
                {
                    m_channelers.push_back(channeler->GetGUID());
                    channeler->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    channeler->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }
        }
        else
        {
            for (std::list<uint64>::const_iterator itr = m_channelers.begin(); itr != m_channelers.end(); ++itr)
            {
                Creature *channeler = m_creature->GetCreature(*m_creature, *itr);
                if (channeler)
                {
                    m_creature->DealDamage(channeler, channeler->GetMaxHealth());
                    channeler->Respawn();
                    channeler->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }
        }

        // despawn sorcerers here
        for (std::list<uint64>::const_iterator itr = m_sorcerers.begin(); itr != m_sorcerers.end(); ++itr)
        {
            Creature *sorc = m_creature->GetCreature(*m_creature, *itr);
            if (sorc)
            {
                sorc->setDeathState(JUST_DIED);
                sorc->RemoveCorpse();
            }
        }
        m_sorcerers.clear();
    }

    void SetAkamaGUID(uint64 guid) { AkamaGUID = guid; }

    void UpdateAI(const uint32 diff)
    {
        if (!event_phase)
        {
            if (!AkamaGUID && pInstance)
                AkamaGUID = pInstance->GetData64(DATA_AKAMA_SHADE);
        }
        else
        {
            if (event_phase == 1)
            {
                for (std::list<uint64>::const_iterator itr = m_channelers.begin(); itr != m_channelers.end(); ++itr)
                {
                    if (Creature *channeler = m_creature->GetCreature(*m_creature, *itr))
                        channeler->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                DoZoneInCombat();
                ++event_phase;
            }
            else
            {
                if (event_phase == 2 || event_phase == 4)
                {
                    if (m_creature->GetTotalAuraModifier(SPELL_AURA_MOD_DECREASE_SPEED) > -100)
                    {
                        int i = 0;
                        if (event_phase == 4)
                            i = 1;
                        ++event_phase;

                        if (m_creature->GetUnitMovementFlags() & MOVEMENTFLAG_WALK_MODE)
                            m_creature->RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);

                        m_creature->GetMotionMaster()->MovePoint(i, moveTo[i][0], moveTo[i][1], moveTo[i][2]);
                    }
                }

                if(m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    ProcessSpawning(diff);

                if (m_checkTimer < diff)
                {
                    if (!m_creature->IsWithinDistInMap(&wLoc, 100))
                        EnterEvadeMode();
                    else
                        DoZoneInCombat();
                    m_checkTimer = 3000;
                }
                else
                    m_checkTimer -= diff;

                if (event_phase >= 7)
                {
                    if (m_damageTimer < diff)
                    {
                        if (AkamaGUID)
                        {
                            Creature *akama = m_creature->GetCreature(*m_creature, AkamaGUID);
                            if (akama && akama->isAlive())
                            {
                                int damage = akama->GetMaxHealth()/12;
                                if (event_phase == 12) // after 60s deal damage equal to hp
                                    damage = akama->GetHealth();
                                m_creature->DealDamage(akama, damage, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                                m_damageTimer = 10000;
                                ++event_phase;
                            }
                            else
                                EnterEvadeMode();
                        }
                    }
                    else
                        m_damageTimer -= diff;
                }
            }
        }
        DoMeleeAttackIfReady();
    }
};

void mob_ashtongue_channelerAI::OnAuraRemove(Aura *aura, bool stackRemove)
{
    if (aura->GetSpellProto()->Id == SPELL_SHADE_SOUL_CHANNEL)
    {
        if (Unit *shade = m_creature->GetUnit(*m_creature, ShadeGUID))
            shade->RemoveSingleAuraFromStack(SPELL_SHADE_SOUL_CHANNEL_2, 0);
    }
}

void mob_ashtongue_sorcererAI::OnAuraRemove(Aura *aura, bool stackRemove)
{
    if (aura->GetSpellProto()->Id == SPELL_SHADE_SOUL_CHANNEL)
    {
        if (Unit *shade = m_creature->GetUnit(*m_creature, ShadeGUID))
            shade->RemoveSingleAuraFromStack(SPELL_SHADE_SOUL_CHANNEL_2, 0);
    }
}

struct TRINITY_DLL_DECL npc_akamaAI : public ScriptedAI
{
    npc_akamaAI(Creature* c) : ScriptedAI(c), m_summons(m_creature)
    {
        pInstance = (ScriptedInstance *)c->GetInstanceData();
        ShadeGUID = pInstance ? pInstance->GetData64(DATA_SHADEOFAKAMA) : 0;
        m_creature->setActive(true);
    }

    void ShadeKilled()
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_talk = 0;
        m_talkTimer = 3000;
        if (pInstance)
            pInstance->SetData(DATA_SHADEOFAKAMAEVENT, DONE);
    }

    ScriptedInstance* pInstance;

    uint64 ShadeGUID;

    uint32 m_destructivePTimer;
    uint32 m_lightningBoltTimer;

    uint32 m_talk;
    uint32 m_talkTimer;

    bool m_yell;

    SummonList m_summons;

    void Reset()
    {
        m_destructivePTimer = 5000;
        m_lightningBoltTimer = 8000;
        m_yell = false;
        m_talk = 0;
        m_talkTimer = 0;

        if (pInstance)
        {
            if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT) == NOT_STARTED)
            {
                m_creature->SetUInt32Value(UNIT_NPC_FLAGS, 0);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_PL_SPELL_TARGET);
            }
        }
        m_summons.DespawnAll();
    }

    void SummonedCreatureDespawn(Creature *summon)
    {
        m_summons.Despawn(summon);
    }

    void BeginEvent(Player* pl)
    {
        pInstance->SetData(DATA_SHADEOFAKAMAEVENT, IN_PROGRESS);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->GetMotionMaster()->MovePoint(0, AKAMA_X, AKAMA_Y, AKAMA_Z);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 0:
            {
                if (!pInstance)
                {
                    EnterEvadeMode();
                    return;
                }

                ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
                if (!ShadeGUID)
                {
                    EnterEvadeMode();
                    return;
                }

                Creature *shade = m_creature->GetCreature(*m_creature, ShadeGUID);
                if (shade)
                {
                    DoCast(shade, SPELL_AKAMA_SOUL_CHANNEL);
                    m_creature->SetSelection(ShadeGUID);
                }
            }
            break;
            case 1:
            {
                m_talkTimer  = 500;
                m_talk = 1;
            }
            break;
            case 2:
            {
                // upstairs
                m_talkTimer = 500;
                m_talk = 2;
            }
            break;
            case 3:
            {
                m_talkTimer = 500;
                m_talk = 3;
            }
            break;
            case 4:
            {
                m_talkTimer = 500;
                m_talk = 4;
            }
            break;
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        Creature *shade = m_creature->GetCreature(*m_creature, ShadeGUID);
        if (shade && shade->isAlive())
            shade->AI()->EnterEvadeMode();
        m_summons.DespawnAll();
    }

    inline bool UpdateVictim()
    {
        if (pInstance && pInstance->GetData(DATA_SHADEOFAKAMAEVENT) == IN_PROGRESS)
        {
            if (!me->isInCombat())
                return false;

            if (Creature *shade = m_creature->GetCreature(*m_creature, ShadeGUID))
            {
                AttackStart(shade);
                return me->getVictim();
            }
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance)
            return;

        if (m_talkTimer)
        {
            if (m_talkTimer <= diff)
            {
                m_talkTimer = 0;
                switch (m_talk)
                {
                    case 0:
                        m_creature->GetMotionMaster()->MovePoint(1, moveTo[1][0], moveTo[1][1], moveTo[1][2]);
                    break;
                    case 1:
                        m_creature->GetMotionMaster()->MovePoint(2, moveTo[0][0], moveTo[0][1], moveTo[0][2]);
                    break;
                    case 2:
                        m_creature->GetMotionMaster()->MovePoint(3, moveTo[0][0]-5.0f, moveTo[0][1], moveTo[0][2]);
                    break;
                    case 3:
                        m_creature->GetMotionMaster()->MovePoint(4, moveTo[0][0]-4.0f, moveTo[0][1], moveTo[0][2]);
                    break;
                    case 4:
                    {
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        ++m_talk;
                        m_talkTimer = 1500;
                    }
                    break;
                    case 5:
                    {
                        DoScriptText(SAY_FREE, m_creature);
                        DoCast(m_creature, 40927, true);
                        if (Creature *shade = m_creature->GetCreature(*m_creature, ShadeGUID))
                            DoCast(shade, SPELL_AKAMA_SOUL_RETRIEVE);

                        ++m_talk;
                        m_talkTimer = 60000;
                        for (int i = 0; i < MAX_BROKEN; ++i)
                        {
                            Creature *broken = m_creature->SummonCreature(CREATURE_BROKEN, BrokenPositions[i][0], BrokenPositions[i][1], AKAMA_Z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 45000);
                            if (broken)
                            {
                                m_summons.Summon(broken);
                                broken->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                broken->GetMotionMaster()->MovePoint(0, BrokenMoveTo[i][0], BrokenMoveTo[i][1], SPAWN_Z);
                                broken->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_KNEEL);
                            }
                        }
                    }
                    break;
                    default:
                        m_creature->AI()->EnterEvadeMode();
                    break;
                }
            }
            else
                m_talkTimer -= diff;
        }

        if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT) != IN_PROGRESS)
            return;

        if (!m_yell && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 15)
        {
            DoScriptText(SAY_LOW_HEALTH, m_creature);
            m_yell = true;
        }

        if (!UpdateVictim())
            return;

        if(m_creature->getVictim()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            return;

        if (m_destructivePTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_DESTRUCTIVE_POISON, true);
            m_destructivePTimer = 5000;
        }
        else
            m_destructivePTimer -= diff;

        if (m_lightningBoltTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
            m_lightningBoltTimer = 8000;
        }
        else
            m_lightningBoltTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

void boss_shade_of_akamaAI::JustDied(Unit *)
{
    DespawnChannelersAndSorcerers();
    m_summons.DespawnAll();
    if (Creature *akama = m_creature->GetCreature(*m_creature, AkamaGUID))
        ((npc_akamaAI *)akama->AI())->ShadeKilled();
}

CreatureAI* GetAI_boss_shade_of_akama(Creature *_Creature)
{
    return new boss_shade_of_akamaAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_channeler(Creature *_Creature)
{
    return new mob_ashtongue_channelerAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_sorcerer(Creature *_Creature)
{
    return new mob_ashtongue_sorcererAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_defender(Creature *_Creature)
{
    return new mob_ashtongue_defenderAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_spiritbinder(Creature *_Creature)
{
    return new mob_ashtongue_spiritbinderAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_elementalist(Creature *_Creature)
{
    return new mob_ashtongue_elementalistAI (_Creature);
}

CreatureAI* GetAI_mob_ashtongue_rogue(Creature *_Creature)
{
    return new mob_ashtongue_rogueAI (_Creature);
}

CreatureAI* GetAI_npc_akama_shade(Creature *_Creature)
{
    return new npc_akamaAI (_Creature);
}

bool GossipSelect_npc_akama(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (ScriptedInstance *pInstance = (ScriptedInstance *)_Creature->GetInstanceData())
    {
        if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT) != NOT_STARTED)
        {
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        if (action == GOSSIP_ACTION_INFO_DEF + 1)               //Fight time
        {
            player->CLOSE_GOSSIP_MENU();
            ((npc_akamaAI*)_Creature->AI())->BeginEvent(player);
        }
    }
    return true;
}

bool GossipHello_npc_akama(Player *player, Creature *_Creature)
{
    if (ScriptedInstance *pInstance = (ScriptedInstance *)_Creature->GetInstanceData())
    {
        if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT) == NOT_STARTED)
        {
            if (player->isAlive())
            {
                player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(907, _Creature->GetGUID());
            }
        }
    }
    return true;
}

void AddSC_boss_shade_of_akama()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_shade_of_akama";
    newscript->GetAI = &GetAI_boss_shade_of_akama;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_channeler";
    newscript->GetAI = &GetAI_mob_ashtongue_channeler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_defender";
    newscript->GetAI = &GetAI_mob_ashtongue_defender;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_sorcerer";
    newscript->GetAI = &GetAI_mob_ashtongue_sorcerer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_spiritbinder";
    newscript->GetAI = &GetAI_mob_ashtongue_spiritbinder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_elementalist";
    newscript->GetAI = &GetAI_mob_ashtongue_elementalist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ashtongue_rogue";
    newscript->GetAI = &GetAI_mob_ashtongue_rogue;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_akama_shade";
    newscript->GetAI = &GetAI_npc_akama_shade;
    newscript->pGossipHello = &GossipHello_npc_akama;
    newscript->pGossipSelect = &GossipSelect_npc_akama;
    newscript->RegisterSelf();
}
