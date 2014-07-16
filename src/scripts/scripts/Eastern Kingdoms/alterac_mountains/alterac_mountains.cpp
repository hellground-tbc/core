/* 
 * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 * 
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
SDName: Alterac_Mountains
SD%Complete: 100
SDComment: Quest support: 6681, 1713
SDCategory: Alterac Mountains
EndScriptData */

/* ContentData
npc_ravenholdt
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_ravenholdt
######*/

struct npc_ravenholdtAI : public ScriptedAI
{
    npc_ravenholdtAI(Creature *creature) : ScriptedAI(creature) {}

    void Reset() { }

    void MoveInLineOfSight(Unit *who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            if (((Player*)who)->GetQuestStatus(6681) == QUEST_STATUS_INCOMPLETE)
                ((Player*)who)->KilledMonster(me->GetEntry(), me->GetGUID());
    }
};

CreatureAI* GetAI_npc_ravenholdt(Creature *creature)
{
    return new npc_ravenholdtAI (creature);
}

/*######
## npc_windwatcher
######*/

#define SAY_START          -1900233
#define SAY_SUMMONING      -1900234

#define QUEST_SUMMONING    1713
#define SPELL_SUMMON       8606

struct npc_windwatcherAI : public npc_escortAI
{
    npc_windwatcherAI(Creature* creature) : npc_escortAI(creature) {}

    void Reset() {}

    void EnterCombat(Unit* who){}

    void JustSummoned(Creature *summoned)
    {
        if (Player* player = GetPlayerForEscort())
            summoned->AI()->AttackStart(player);
    }

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();

        switch(i)
        {
            case 6:
                DoScriptText(SAY_SUMMONING, me, player);
                DoCast(me, SPELL_SUMMON);
                break;
            case 13:
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->SetFacingTo(1.39f);
                break;
        }
    }
};

CreatureAI* GetAI_npc_windwatcher(Creature* creature)
{
    return new npc_windwatcherAI(creature);
}

bool QuestAccept_npc_windwatcher(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_SUMMONING)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_windwatcherAI, creature->AI()))
        {
            pEscortAI->SetClearWaypoints(true);
            pEscortAI->SetDespawnAtEnd(false);
            pEscortAI->SetDespawnAtFar(false);
            pEscortAI->Start(false, false, player->GetGUID(), quest);
        }
        DoScriptText(SAY_START, creature, player);
    }
    return true;
}

/*######
## npc_av_trinket_questgiver
######*/

uint32 av_insignia[12] = {17690, 17905, 17906, 17907, 17908, 17909, 17691, 17900, 17901, 17902, 17903, 17904};
uint32 av_quests[12] = {7161, 7163, 7164, 7165, 7166, 7167, 7162, 7168, 7169, 7170, 7171, 7172};
#define GOSSIP_AV_TRINKET "Restore Insignia."

bool GossipHello_npc_av_trinket_questgiver(Player* player,Creature* creature)
{
    if( creature->isQuestGiver() )
        player->PrepareQuestMenu( creature->GetGUID() );

    bool hasTrinket = false;
    for(uint8 i=0;i<12;i++)
    {
        if(player->HasItemCount(av_insignia[i],1,true))
        {
            hasTrinket = true;
            break;
        }
    }

    if(!hasTrinket && (player->GetQuestStatus(7161) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(7162) == QUEST_STATUS_COMPLETE))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_AV_TRINKET, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO);

    player->SEND_GOSSIP_MENU(creature->GetNpcTextId(),creature->GetGUID());
    return true;
}

bool GossipSelect_npc_av_trinket_questgiver(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (action != GOSSIP_SENDER_INFO)
        return true;

    for(uint8 i=0;i<12;i++) // check again just in case
    {
        if(player->HasItemCount(av_insignia[i],1,true))
            return true;
    }

    uint8 trinketType = 255;
    for(uint8 i=0;i<12;i++)
    {
        if(player->GetQuestStatus(av_quests[i]) == QUEST_STATUS_COMPLETE)
            trinketType = i;
    }

    if(trinketType == 255) //just in case
        return true;

    ItemPosCountVec dest;
    uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, av_insignia[trinketType], 1);
    if (msg == EQUIP_ERR_OK)
    {
        Item* item = player->StoreNewItem(dest, av_insignia[trinketType], true);
        player->SendNewItem(item,1,true,false,true);
    }
    player->CLOSE_GOSSIP_MENU();

    return true;
}

void AddSC_alterac_mountains()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_ravenholdt";
    newscript->GetAI = &GetAI_npc_ravenholdt;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_windwatcher";
    newscript->GetAI = &GetAI_npc_windwatcher;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_windwatcher;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name="npc_av_trinket_questgiver";
    newscript->pGossipHello = &GossipHello_npc_av_trinket_questgiver;
    newscript->pGossipSelect = &GossipSelect_npc_av_trinket_questgiver;
    newscript->RegisterSelf();
}

