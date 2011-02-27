/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

/* ContentData
npc_forest_frog
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "def_zulaman.h"

/*######
## npc_forest_frog
######*/

#define SPELL_REMOVE_AMANI_CURSE    43732
#define SPELL_PUSH_MOJO             43923
#define ENTRY_FOREST_FROG           24396

enum npc
{
    NPC_KRAZ = 24024,
    NPC_MANNUTH = 24397,
    NPC_DEEZ = 24403,
    NPC_GALATHRYN = 24404,
    NPC_ADARRAH = 24405,
    NPC_FUDGERICK = 24406,
    NPC_DARWEN = 24407,
    NPC_MITZI = 24445,
    NPC_CHRISTIAN = 24448,
    NPC_BRENNAN = 24453,
    NPC_HOLLEE = 24455,
};

struct TRINITY_DLL_DECL npc_forest_frogAI : public ScriptedAI
{
    npc_forest_frogAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance *pInstance;

    void Reset() { }

    void EnterCombat(Unit *who) { }

    void DoSpawnRandom()
    {
        if( pInstance )
        {
            uint32 cEntry = RAND(NPC_KRAZ, NPC_MANNUTH, NPC_DEEZ, NPC_GALATHRYN, NPC_ADARRAH, NPC_FUDGERICK, NPC_DARWEN, NPC_MITZI, NPC_CHRISTIAN, NPC_BRENNAN, NPC_HOLLEE);

            if( !pInstance->GetData(TYPE_RAND_VENDOR_1) )
                if(rand()%10 == 1) cEntry = 24408;      //Gunter
            if( !pInstance->GetData(TYPE_RAND_VENDOR_2) )
                if(rand()%10 == 1) cEntry = 24409;      //Kyren

            if( cEntry ) m_creature->UpdateEntry(cEntry);

            if( cEntry == 24408) pInstance->SetData(TYPE_RAND_VENDOR_1,DONE);
            if( cEntry == 24409) pInstance->SetData(TYPE_RAND_VENDOR_2,DONE);
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if( spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && m_creature->GetEntry() == ENTRY_FOREST_FROG )
        {
            //increase or decrease chance of mojo?
            if( rand()%99 == 50 ) DoCast(caster,SPELL_PUSH_MOJO,true);
            else DoSpawnRandom();
        }
    }
};
CreatureAI* GetAI_npc_forest_frog(Creature *_Creature)
{
    return new npc_forest_frogAI (_Creature);
}

/*######
## npc_zulaman_hostage
######*/

#define GOSSIP_HOSTAGE1        "I am glad to help you."

#define SPELL_SMASH             18944   // most probably wrong spell, but has cool visual
#define GO_LOOT_BOX             186622
#define GO_EXPLOSION_CHARGE     183410  // probably wrong go, but looks ok
#define GO_ASHLI_VASE           186671
#define SPELL_EXPLOSION         43418   // also not sure about this
#define SPELL_ASHLI_FIREBALL    43525
#define SPELL_FIREBALL_VISUAL   29473   // also wrong spell

struct TRINITY_DLL_DECL npc_hostageAI : public ScriptedAI
{
    npc_hostageAI(Creature *c) : ScriptedAI(c) 
    {
       pInstance = (ScriptedInstance*)c->GetInstanceData();
    }


    ScriptedInstance *pInstance;
    uint32 CheckTimer;
    float dist;
    float angle;
    bool EventStarted;

    void Reset() 
    {
        CheckTimer = 10000;
        EventStarted = false;
    }
    void EnterCombat(Unit *who) {}
    void JustDied(Unit *) {}
    virtual bool RewardReached(GameObject* reward) { return true; }

    void MovementInform(uint32 Type, uint32 Id)
    {
        if (Type != POINT_MOTION_TYPE)
            return;

        if(Id == 1)
        {   
            uint8 i = GetHostageIndex(me->GetEntry());
            GameObject *target = m_creature->GetMap()->GetGameObject(pInstance->GetData64(DATA_CHEST_0 + i));
            if(target && RewardReached(target))
            {
                me->SetFacingToObject(target);
                target->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                pInstance->SetData(DATA_HOSTAGE_0_STATE + i, HOSTAGE_CHEST_UNLOCKED);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!EventStarted && pInstance)
        {
            if(CheckTimer < diff)
            {
                uint8 i = GetHostageIndex(me->GetEntry());
                if(pInstance->GetData(DATA_HOSTAGE_0_STATE + i) & HOSTAGE_FREED)
                {
                    GameObject *target = m_creature->GetMap()->GetGameObject(pInstance->GetData64(DATA_CHEST_0 + i));
                    if(target)
                    {
                        float x, y;
                        target->GetNearPoint2D(x, y, dist, angle);
                        me->GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ()); 
                    }
                    EventStarted = true;
                }
                CheckTimer = 2000;
            } else 
                CheckTimer -= diff;
        }
    }
};

struct TRINITY_DLL_DECL npc_tanzarAI : public npc_hostageAI
{
    npc_tanzarAI(Creature *c) : npc_hostageAI(c)
    {
        dist = 0.9;
        angle = 3.1415;
    }

    bool RewardReached(GameObject *reward)
    {
        me->CastSpell(me, 1804, false); // CHECKME: should do some emote that he is opening chest
        return true;
    } 
};

CreatureAI* GetAI_npc_tanzar(Creature *_Creature)
{
    return new npc_tanzarAI(_Creature);
}

struct TRINITY_DLL_DECL npc_harkorAI : public npc_hostageAI
{
    npc_harkorAI(Creature *c) : npc_hostageAI(c)    
    {
        dist = 3;
        angle = 3.1415;
    }

    bool RewardReached(GameObject *reward)
    {
        GameObject *target = FindGameObject(GO_LOOT_BOX, 10, me);
        if(target)
        {
            me->CastSpell(me, SPELL_SMASH, false); // CHECKME: should do some emote that he is opening chest
            target->Delete();
        }
        return true;
    } 
};

CreatureAI* GetAI_npc_harkor(Creature *_Creature)
{
    return new npc_harkorAI(_Creature);
}

struct TRINITY_DLL_DECL npc_krazAI : public npc_hostageAI
{
    npc_krazAI(Creature *c) : npc_hostageAI(c)
    {
        dist = 0.1;
        angle = 0;
    }

    uint64 ExplosionChargeGUID;
    bool Move;

    void Reset()
    {
        npc_hostageAI::Reset();
        
        ExplosionChargeGUID = 0;
        Move = false;
    }

    bool RewardReached(GameObject *reward)
    {
        GameObject *go = me->SummonGameObject(GO_EXPLOSION_CHARGE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, 0, 0, 0, 0, 0);
        if(go)
        {
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
            ExplosionChargeGUID = go->GetGUID();
            Move = true;
        }
        return false;
    } 

    void MovementInform(uint32 Type, uint32 Id)
    {
        npc_hostageAI::MovementInform(Type, Id);
        if (Type != POINT_MOTION_TYPE)
            return;

        if(Id == 2 && ExplosionChargeGUID)
        {
            GameObject *go = me->GetMap()->GetGameObject(ExplosionChargeGUID);
            if(go)
            {
                go->CastSpell(go, SPELL_EXPLOSION);
                go->Delete();
            }
            ExplosionChargeGUID = 0;

            go = m_creature->GetMap()->GetGameObject(pInstance->GetData64(DATA_CHEST_KRAZ));
            if(go)
            {
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                pInstance->SetData(DATA_HOSTAGE_KRAZ_STATE, HOSTAGE_CHEST_UNLOCKED);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(Move)
        {
            m_creature->GetMotionMaster()->MovePoint(2, HostageInfo[2].x, HostageInfo[2].y, HostageInfo[2].z);
            Move = false;
        }
        npc_hostageAI::UpdateAI(diff);
    }
};


CreatureAI* GetAI_npc_kraz(Creature *_Creature)
{
    return new npc_krazAI(_Creature);
}

float AshliWP[][3] = {
    {409, 1146, 5},
    {360, 1090, 7},
    {334, 1089, 6},
    {332, 1145, 6},
    {385, 1089, 6},
    {403, 1089, 6},
    {374, 1087, 6}
};

struct TRINITY_DLL_DECL npc_ashliAI : public ScriptedAI
{
    npc_ashliAI(Creature *c) : ScriptedAI(c) 
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ASHLI_FIREBALL);
        if(TempSpell)
            TempSpell->EffectImplicitTargetA[0] = TARGET_GAMEOBJECT;
    }

    ScriptedInstance *pInstance;
    uint32 CheckTimer;
    bool EventStarted;
    bool Move;
    bool Fire;
    bool FillTargets;
    uint32 MovePoint;
    std::list<uint64> targets;
    uint64 LastTarget;

    void Reset() 
    {
        CheckTimer = 10000;
        EventStarted = false;
        MovePoint = 0;
        Move = false;
        Fire = false;
        LastTarget = 0;
        targets.clear();
    }

    void EnterCombat(Unit *who) {}
    void JustDied(Unit *) {}

    void MovementInform(uint32 Type, uint32 Id)
    {
        if (Type != POINT_MOTION_TYPE)
            return;

        if(Id >= 6)
            return;

        Fire = true;
        targets.clear();
        std::list<Creature*> t = DoFindAllCreaturesWithEntry(23746, 20);
        for(std::list<Creature*>::iterator i = t.begin(); i != t.end(); ++i)
            targets.push_back((*i)->GetGUID());
        
    }

    void SpellHitTarget(Unit *target, const SpellEntry *entry)
    {
        GameObject *go = FindGameObject(GO_ASHLI_VASE, 5, target);
        if(go)
            go->Delete();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!EventStarted && pInstance)
        {
            if(CheckTimer < diff)
            {
                if(pInstance->GetData(DATA_HOSTAGE_ASHLI_STATE) & HOSTAGE_FREED)
                {
                    Move = true;
                    EventStarted = true;
                }
                CheckTimer = 2000;
            } else 
                CheckTimer -= diff;
        }

        if(Fire && !m_creature->hasUnitState(UNIT_STAT_CASTING))
        {
            if(LastTarget)
            {
                if(Unit *unit = me->GetUnit(LastTarget))
                    me->CastSpell(unit, SPELL_FIREBALL_VISUAL, true);
                LastTarget = 0;
            }

            Unit *target = NULL;
            if(!targets.empty())
            {
                target = me->GetUnit(targets.front());
                targets.pop_front();
            }
            if(target)
            {
                me->CastSpell(target, SPELL_ASHLI_FIREBALL, false);
                LastTarget = target->GetGUID();
            } 
            else
            {
                Fire = false;
                Move = true;
                MovePoint++;
                if(MovePoint == 6)
                {
                    GameObject *target = m_creature->GetMap()->GetGameObject(pInstance->GetData64(DATA_CHEST_ASHLI));
                    if(target)
                    {
                        target->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        pInstance->SetData(DATA_HOSTAGE_ASHLI_STATE, HOSTAGE_CHEST_UNLOCKED);
                    }
                }
            }
        }

        if(Move)
        {
            me->GetMotionMaster()->MovePoint(MovePoint, AshliWP[MovePoint][0], AshliWP[MovePoint][1], AshliWP[MovePoint][2]);
            Move = false;
        }
    }
};

CreatureAI* GetAI_npc_ashli(Creature *_Creature)
{
    return new npc_ashliAI(_Creature);
}

bool GOHello_go_zulaman_cage(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
    if(!pInstance)
        return true;

    int i = GetHostageIndex(pGo->GetEntry());
    if(pInstance->GetData(DATA_HOSTAGE_0_STATE + i) & HOSTAGE_SAVED)
    {
        pInstance->SetData(DATA_HOSTAGE_0_STATE + i, HOSTAGE_FREED);
        pGo->SetGoState(GO_STATE_ACTIVE);
        return false;
    }
    return true;  
}


bool GOHello_go_zulaman_timed_event_chest(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
    if(!pInstance)
        return true;

    int i = GetHostageIndex(pGo->GetEntry());
    if(pInstance->GetData(DATA_HOSTAGE_0_STATE + i) & HOSTAGE_CHEST_UNLOCKED)
    {
        pInstance->SetData(DATA_HOSTAGE_0_STATE + i, HOSTAGE_CHEST_LOOTED);
        return false;
    }
    return true;  
}

/*
bool GossipHello_npc_zulaman_hostage(Player* player, Creature* _Creature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_HOSTAGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
    return true;
}


bool GossipSelect_npc_zulaman_hostage(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if(action == GOSSIP_ACTION_INFO_DEF + 1)
        player->CLOSE_GOSSIP_MENU();

    if(!_Creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
        return true;
    _Creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    ScriptedInstance* pInstance = (_Creature->GetInstanceData());
    if(pInstance)
    {
        //uint8 progress = pInstance->GetData(DATA_CHESTLOOTED);
        pInstance->SetData(DATA_CHESTLOOTED, 0);
        float x, y, z;
        _Creature->GetPosition(x, y, z);
        uint32 entry = _Creature->GetEntry();
        for(uint8 i = 0; i < 4; ++i)
        {
            if(HostageEntry[i] == entry)
            {
                _Creature->SummonGameObject(ChestEntry[i], x-2, y, z, 0, 0, 0, 0, 0, 0);
                break;
            }
        }
        /*Creature* summon = _Creature->SummonCreature(HostageInfo[progress], x-2, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        if(summon)
        {
            ((npc_zulaman_hostageAI*)summon->AI())->PlayerGUID = player->GetGUID();
            ((npc_zulaman_hostageAI*)summon->AI())->IsLoot = true;
            summon->SetDisplayId(10056);
            summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            summon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }*
    }
    return true;
}
*/


/*######
## npc_harrison_jones_za
######*/

enum
{
SAY_START = -1568079,
SAY_AT_GONG = -1568080,
SAY_OPENING_ENTRANCE = -1568081,
SAY_OPEN_GATE = -1568082,

SPELL_BANGING_THE_GONG = 45222,

SOUND_GONG = 12204,
SOUND_CELEBRATE = 12135
};

#define GOSSIP_ITEM_BEGIN "Thanks for the concern, but we intend to explore Zul'Aman."

struct TRINITY_DLL_DECL npc_harrison_jones_zaAI : public npc_escortAI
{
    npc_harrison_jones_zaAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 ResetTimer;

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        // if hitted by Amani'shi Guardian, killed immediately
        if(done_by->GetTypeId() == TYPEID_UNIT && damage)
        {
            damage = m_creature->GetMaxHealth();
            done_by->GetMotionMaster()->MoveChase(GetPlayerForEscort());
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (!m_pInstance)
            return;

        switch(uiPointId)
        {
            case 2:
                DoScriptText(SAY_AT_GONG, m_creature);

                if (GameObject* pEntranceDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_GONG)))
                    pEntranceDoor->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);

                //Start bang gong for 10min
                m_creature->CastSpell(m_creature, SPELL_BANGING_THE_GONG, false);
                SetEscortPaused(true);
                break;
            case 3:
                DoScriptText(SAY_OPENING_ENTRANCE, m_creature);
                break;
           case 4:
                DoScriptText(SAY_OPEN_GATE, m_creature);
                break;
           case 5:
               m_pInstance->SetData(TYPE_EVENT_RUN,SPECIAL);
               if(Unit* Guardian = FindCreature(23597, 40, me))
                   ((Creature*)Guardian)->GetMotionMaster()->MoveChase(me);
                break;
                //TODO: Spawn group of Amani'shi Savage and make them run to entrance
                //TODO: Add, and modify reseting of the event, reseting quote is missing
        }
    }

    void Reset()
    {
        ResetTimer = 600000;    // 10min for players to make an event
        m_creature->RemoveAllAuras();
    }

    void StartEvent(Player* pPlayer)
    {
        DoScriptText(SAY_START, m_creature);
        Start(true, false, pPlayer->GetGUID(), 0, false, true);
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);

        //Stop banging gong if still
        if (m_creature->HasAura(SPELL_BANGING_THE_GONG, 0))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_BANGING_THE_GONG);
            DoPlaySoundToSet(m_creature, SOUND_CELEBRATE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(HasEscortState(STATE_ESCORT_PAUSED))
        {/*
            if(ResetTimer < diff)
            {
                me->Kill(me, false);
                me->Respawn();
                ResetTimer = 600000;
            }
            else
                ResetTimer -= diff;*/
        }
        npc_escortAI::UpdateAI(diff);
    }
};

bool GossipHello_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pInstance && pInstance->GetData(TYPE_EVENT_RUN) == NOT_STARTED)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEGIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_harrison_jones_za(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
            pHarrisonAI->StartEvent(pPlayer);
        
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

CreatureAI* GetAI_npc_harrison_jones_za(Creature* pCreature)
{
    return new npc_harrison_jones_zaAI(pCreature);
}

/*######
## go_strange_gong
######*/

bool GOHello_go_strange_gong(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance || pPlayer->HasAura(SPELL_BANGING_THE_GONG, 0))
        return false;

    pPlayer->CastSpell(pPlayer, SPELL_BANGING_THE_GONG, false);
    return false;
}

struct TRINITY_DLL_DECL npc_zulaman_door_triggerAI : public Scripted_NoMovementAI
{
    npc_zulaman_door_triggerAI(Creature *c) : Scripted_NoMovementAI(c) 
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    uint32 CheckTimer;
    uint32 StoperTime;

    uint32 CountChannelingPlayers()
    {
        uint32 count = 0;
        Map::PlayerList const& players = me->GetMap()->GetPlayers();
        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                {
                    if(plr->HasAura(SPELL_BANGING_THE_GONG, 0))
                        count++;
                }
            }
        }
        return count;
    }

    void StopBanging()
    {
        Map::PlayerList const& players = me->GetMap()->GetPlayers();
        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                {
                    if(plr->HasAura(SPELL_BANGING_THE_GONG, 0))
                        plr->InterruptNonMeleeSpells(false);
                }
            }
        }
    }

    void Reset()
    {
        StoperTime = 0;
        CheckTimer = 2000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(CheckTimer < diff)
        {
            if(CountChannelingPlayers() >= 5)
                StoperTime += (2000+diff);
            CheckTimer = 2000;
        }
        else
            CheckTimer -= diff;

        if(StoperTime >= 30000) // to be verified
        {
            StopBanging();
            if(Creature* pCreature = me->GetMap()->GetCreature(pInstance->GetData64(DATA_HARRISON)))
            {
                if (npc_harrison_jones_zaAI* pHarrisonAI = dynamic_cast<npc_harrison_jones_zaAI*>(pCreature->AI()))
                    pHarrisonAI->SetHoldState(false);
            }
            if(GameObject* pGo = me->GetMap()->GetGameObject(pInstance->GetData64(DATA_GO_GONG)))
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
            StoperTime = 0;
        }
    }
};

CreatureAI* GetAI_npc_zulaman_door_trigger(Creature *_Creature)
{
    return new npc_zulaman_door_triggerAI(_Creature);
}

/*####################
# Akilzon Gauntlet event
#######################*/

#define AKILZON_GAUNTLET_NOT_STARTED        0
#define AKILZON_GAUNTLET_IN_PROGRESS        10
#define AKILZON_GAUNTLET_TEMPEST_ENGAGED    11
#define AKILZON_GAUNTLET_TEMPEST_DEAD       12

#define NPC_AMANISHI_WARRIOR        24225
#define NPC_AMANISHI_EAGLE          24159
#define SPELL_TALON                 43517
#define SPELL_CHARGE                43519
#define SPELL_KICK                  43518
#define SAY_GAUNTLET_START          -1568025

int32 GauntletWP[][3] =
{
    { 226, 1492, 26 },
    { 227, 1439, 26 },
    { 227, 1369, 48 },
    { 284, 1379, 49 },
    { 301, 1385, 58 },
};

struct TRINITY_DLL_DECL npc_amanishi_lookoutAI : public ScriptedAI
{
    npc_amanishi_lookoutAI(Creature *c) : ScriptedAI(c), Summons(c)
    {
        pInstance = c->GetInstanceData();
        m_creature->setActive(true);
    }

    ScriptedInstance *pInstance;

    bool EventStarted;
    bool Move;
    uint8 MovePoint;
    SummonList Summons;

    uint32 warriorsTimer;
    uint32 eaglesTimer;

    void Reset()
    {
    //    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    //    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetVisibility(VISIBILITY_ON);
        EventStarted = false;
        warriorsTimer = 40000;
        eaglesTimer = 1000;
        Move = false;

        if(pInstance)
            pInstance->SetData(DATA_AKILZONGAUNTLET, AKILZON_GAUNTLET_NOT_STARTED);
    }

    void StartEvent()
    {
        m_creature->GetMotionMaster()->MovePoint(0, 226, 1461, 26);
        EventStarted = true;
        DoZoneInCombat();
        DoGlobalScriptText(SAY_GAUNTLET_START, AKILZON, me->GetMap());
    }

    void EnterCombat(Unit *who)
    {
    }

    void JustDied(Unit* Killer)
    {
        if(Killer != m_creature)
        {
            m_creature->Respawn();
            m_creature->AI()->EnterEvadeMode();
        }
    }

    
    void JustSummoned(Creature* summoned)
    {
        Summons.Summon(summoned);
    }


    void SummonedCreatureDespawn(Creature *summon)
    {
        Summons.Despawn(summon);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(!EventStarted && m_creature->IsHostileTo( who ) && m_creature->IsWithinDistInMap(who, 50))
        {
            StartEvent();
            if(pInstance)
                pInstance->SetData(DATA_AKILZONGAUNTLET, AKILZON_GAUNTLET_IN_PROGRESS);
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(type == POINT_MOTION_TYPE)
        {
            if(id > 3)
            {
                m_creature->SetVisibility(VISIBILITY_OFF);
            }
            else
            {
                Move = true;
                MovePoint = id + 1;
            }
            
        }
    }

    void UpdateAI(const uint32 diff)
    {   
        // Event started by entering combat with gauntlet mob
        if(!EventStarted && pInstance && pInstance->GetData(DATA_AKILZONGAUNTLET) != AKILZON_GAUNTLET_NOT_STARTED)
        {
            StartEvent();
        }

        if(Move)
        {
            m_creature->GetMotionMaster()->MovePoint(MovePoint, GauntletWP[MovePoint][0], GauntletWP[MovePoint][1], GauntletWP[MovePoint][2]);
            Move = false;
        }

        if(me->getThreatManager().isThreatListEmpty() && EventStarted)
        {
            EnterEvadeMode();
            EventStarted = false;
            Summons.DespawnAll();
        }

        else if (pInstance && pInstance->GetData(DATA_AKILZONGAUNTLET) == AKILZON_GAUNTLET_IN_PROGRESS)
        {
            if(warriorsTimer < diff)
            {
                for(uint8 i = 0; i < 2; i++)
                    m_creature->SummonCreature(NPC_AMANISHI_WARRIOR, GauntletWP[0][0] + 2*i, GauntletWP[0][1] + 2*i, GauntletWP[0][2], 3.1415f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                warriorsTimer = 40000;
            }
            else
                warriorsTimer -= diff;

            if(eaglesTimer < diff)
            {
                uint8 maxEagles = RAND(5, 6);
                for(uint8 i = 0; i < maxEagles; i++)
                    m_creature->SummonCreature(NPC_AMANISHI_EAGLE, GauntletWP[4][0] + 2*(i%2)-4, GauntletWP[4][1] + 2*(i/2) - 4, GauntletWP[4][2], 3.1415f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                eaglesTimer = 25000; 
            }
            else
                eaglesTimer -= diff;
        }

        else if(pInstance && pInstance->GetData(DATA_AKILZONGAUNTLET) == AKILZON_GAUNTLET_TEMPEST_DEAD)
        {
            Reset();
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth());
        }   
    }
};

CreatureAI* GetAI_npc_amanishi_lookout(Creature *_Creature)
{
    return new npc_amanishi_lookoutAI (_Creature);
}

struct TRINITY_DLL_DECL npc_amanishi_warriorAI : public npc_escortAI
{
    npc_amanishi_warriorAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = c->GetInstanceData();
        m_creature->setActive(true);
        SetDespawnAtEnd(false);
        Reset();
    }

    ScriptedInstance *pInstance;

    uint32 KickTimer;
    uint32 ChargeTimer;

    void Reset()
    {
        Start(true, true);
        KickTimer = 9000;
        ChargeTimer = 2000;
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(KickTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_KICK);
            KickTimer = 9000;
        } else
            KickTimer -= diff;

        if(ChargeTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CHARGE);
            ChargeTimer = 5000;
        } else
            ChargeTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void WaypointReached(uint32 )
    {
    }
};

CreatureAI* GetAI_npc_amanishi_warrior(Creature *_Creature)
{
    npc_escortAI* ai = new npc_amanishi_warriorAI (_Creature);
    ai->AddWaypoint(0, GauntletWP[1][0], GauntletWP[1][1], GauntletWP[1][2]);
    ai->AddWaypoint(1, GauntletWP[2][0], GauntletWP[2][1], GauntletWP[2][2]);
    ai->AddWaypoint(2, GauntletWP[3][0], GauntletWP[3][1], GauntletWP[3][2]);
    ai->AddWaypoint(3, GauntletWP[4][0], GauntletWP[4][1], GauntletWP[4][2]);
    return ai;
}

struct TRINITY_DLL_DECL npc_amani_eagleAI : public npc_escortAI
{
    npc_amani_eagleAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = c->GetInstanceData();
        m_creature->setActive(true);
        SetDespawnAtEnd(false);
        Reset();
    }

    ScriptedInstance *pInstance;
    uint32 TalonTimer;

    void Reset()
    {
        Start(true, true);
        TalonTimer = 10000;
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(TalonTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_TALON);
            TalonTimer = 10000;
        } else
            TalonTimer -= diff;

        DoMeleeAttackIfReady();
    }
    void WaypointReached(uint32 )
    {
    }
};

CreatureAI* GetAI_npc_amani_eagle(Creature *_Creature)
{
    npc_escortAI* ai = new npc_amani_eagleAI (_Creature);
    ai->AddWaypoint(0, GauntletWP[3][0], GauntletWP[3][1], GauntletWP[3][2]);
    ai->AddWaypoint(1, GauntletWP[2][0], GauntletWP[2][1], GauntletWP[2][2]);
    ai->AddWaypoint(2, GauntletWP[1][0], GauntletWP[1][1], GauntletWP[1][2]);
    ai->AddWaypoint(3, GauntletWP[0][0], GauntletWP[0][1], GauntletWP[0][2]);
    return ai;
}

void AddSC_zulaman()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_amanishi_warrior";
    newscript->GetAI = &GetAI_npc_amanishi_warrior;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_amani_eagle";
    newscript->GetAI = &GetAI_npc_amani_eagle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_amanishi_lookout";
    newscript->GetAI = &GetAI_npc_amanishi_lookout;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_forest_frog";
    newscript->GetAI = &GetAI_npc_forest_frog;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ashli";
    newscript->GetAI = &GetAI_npc_ashli;
//    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
//    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kraz";
    newscript->GetAI = &GetAI_npc_kraz;
//    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
//    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_harkor";
    newscript->GetAI = &GetAI_npc_harkor;
//    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
//    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tanzar";
    newscript->GetAI = &GetAI_npc_tanzar;
//    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
//    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_zulaman_cage";
    newscript->pGOHello = &GOHello_go_zulaman_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_zulaman_timed_event_chest";
    newscript->pGOHello = &GOHello_go_zulaman_timed_event_chest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_harrison_jones_za";
    newscript->GetAI = &GetAI_npc_harrison_jones_za;
    newscript->pGossipHello = &GossipHello_npc_harrison_jones_za;
    newscript->pGossipSelect = &GossipSelect_npc_harrison_jones_za;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_strange_gong";
    newscript->pGOHello = &GOHello_go_strange_gong;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_zulaman_door_trigger";
    newscript->GetAI = &GetAI_npc_zulaman_door_trigger;
    newscript->RegisterSelf();
}

