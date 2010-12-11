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
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
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

static uint32 HostageEntry[] = {23790, 23999, 24024, 24001};
static uint32 ChestEntry[] = {186648, 187021, 186672, 186667};

struct TRINITY_DLL_DECL npc_zulaman_hostageAI : public ScriptedAI
{
    npc_zulaman_hostageAI(Creature *c) : ScriptedAI(c) {IsLoot = false;}
    bool IsLoot;
    uint64 PlayerGUID;
    void Reset() {}
    void EnterCombat(Unit *who) {}
    void JustDied(Unit *)
    {
        //Player* player = Unit::GetPlayer(PlayerGUID);
        //if(player) player->SendLoot(m_creature->GetGUID(), LOOT_CORPSE);
    }
    void UpdateAI(const uint32 diff)
    {
        if(IsLoot) m_creature->CastSpell(m_creature, 7, false);
    }
};

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

    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
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
        }*/
    }
    return true;
}

CreatureAI* GetAI_npc_zulaman_hostage(Creature *_Creature)
{
    return new npc_zulaman_hostageAI(_Creature);
}

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
                m_pInstance->SetData(TYPE_EVENT_RUN,IN_PROGRESS);
                //TODO: Spawn group of Amani'shi Savage and make them run to entrance
                //TODO: Add, and modify reseting of the event, reseting quote is missing
                break;
        }
    }

    void Reset()
    {
        ResetTimer = 600000;    // 10min for players to make an event
        m_creature->RemoveAllAuras();
    }

    void StartEvent()
    {
        DoScriptText(SAY_START, m_creature);
        Start();
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);

        //Stop banging gong if still
        if (m_pInstance && m_pInstance->GetData(TYPE_EVENT_RUN) == SPECIAL && m_creature->HasAura(SPELL_BANGING_THE_GONG, 0))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_BANGING_THE_GONG);
            DoPlaySoundToSet(m_creature, SOUND_CELEBRATE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(HasEscortState(STATE_ESCORT_PAUSED))
        {
            if(ResetTimer < diff)
            {
                me->Kill(me, false);
                me->Respawn();
                ResetTimer = 600000;
            }
            else
                ResetTimer -= diff;
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
            pHarrisonAI->StartEvent();
        
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

void AddSC_zulaman()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_forest_frog";
    newscript->GetAI = &GetAI_npc_forest_frog;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_zulaman_hostage";
    newscript->GetAI = &GetAI_npc_zulaman_hostage;
    newscript->pGossipHello = &GossipHello_npc_zulaman_hostage;
    newscript->pGossipSelect = &GossipSelect_npc_zulaman_hostage;
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

