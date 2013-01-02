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
SDName: Silithus
SD%Complete: 100
SDComment: Quest support: 7785, 8304.
SDCategory: Silithus
EndScriptData */

/* ContentData
npc_highlord_demitrian
npcs_rutgar_and_frankal
npc_cenarion_scout_jalia
npc_cenarion_scout_azenel
npc_cenarion_scout_landion
EndContentData */

#include "precompiled.h"

/*###
## npc_highlord_demitrian
###*/

#define GOSSIP_DEMITRIAN1 "What do you know of it?"
#define GOSSIP_DEMITRIAN2 "I am listening , Demitrian."
#define GOSSIP_DEMITRIAN3 "Continue, please."
#define GOSSIP_DEMITRIAN4 "A battle?"
#define GOSSIP_DEMITRIAN5 "<Nod>"
#define GOSSIP_DEMITRIAN6 "Caught unaware? How?"
#define GOSSIP_DEMITRIAN7 "So what did Ragnaros do next?"

bool GossipHello_npc_highlord_demitrian(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu(_Creature->GetGUID());

    if (player->GetQuestStatus(7785) == QUEST_STATUS_NONE &&
        (player->HasItemCount(18563,1,false) || player->HasItemCount(18564,1,false)))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(6812, _Creature->GetGUID());
        return true;
}

bool GossipSelect_npc_highlord_demitrian(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(6842, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        player->SEND_GOSSIP_MENU(6843, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
        player->SEND_GOSSIP_MENU(6844, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
        player->SEND_GOSSIP_MENU(6867, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
        player->SEND_GOSSIP_MENU(6868, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DEMITRIAN7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
        player->SEND_GOSSIP_MENU(6869, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        player->SEND_GOSSIP_MENU(6870, _Creature->GetGUID());

        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 19016, 1);
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 19016, true);
        break;
    }
    return true;
}

/*###
## npcs_rutgar_and_frankal
###*/

//gossip item text best guess
#define GOSSIP_ITEM1 "I seek information about Natalia"

#define GOSSIP_ITEM2 "That sounds dangerous!"
#define GOSSIP_ITEM3 "What did you do?"
#define GOSSIP_ITEM4 "Who?"
#define GOSSIP_ITEM5 "Women do that. What did she demand?"
#define GOSSIP_ITEM6 "What do you mean?"
#define GOSSIP_ITEM7 "What happened next?"

#define GOSSIP_ITEM11 "Yes, please continue"
#define GOSSIP_ITEM12 "What language?"
#define GOSSIP_ITEM13 "The Priestess attacked you?!"
#define GOSSIP_ITEM14 "I should ask the monkey about this"
#define GOSSIP_ITEM15 "Then what..."

//trigger creatures to kill
#define TRIGGER_RUTGAR 15222
#define TRIGGER_FRANKAL 15221

bool GossipHello_npcs_rutgar_and_frankal(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetQuestStatus(8304) == QUEST_STATUS_INCOMPLETE &&
        _Creature->GetEntry() == 15170 &&
        !player->GetReqKillOrCastCurrentCount(8304, TRIGGER_RUTGAR ))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    if (player->GetQuestStatus(8304) == QUEST_STATUS_INCOMPLETE &&
        _Creature->GetEntry() == 15171 &&
        player->GetReqKillOrCastCurrentCount(8304, TRIGGER_RUTGAR ))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);

    player->SEND_GOSSIP_MENU(7754, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npcs_rutgar_and_frankal(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(7755, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(7756, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(7757, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->SEND_GOSSIP_MENU(7758, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->SEND_GOSSIP_MENU(7759, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->SEND_GOSSIP_MENU(7760, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            player->SEND_GOSSIP_MENU(7761, _Creature->GetGUID());
                                                            //'kill' our trigger to update quest status
            player->KilledMonster( TRIGGER_RUTGAR, _Creature->GetGUID() );
            break;

        case GOSSIP_ACTION_INFO_DEF + 9:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->SEND_GOSSIP_MENU(7762, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM12, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->SEND_GOSSIP_MENU(7763, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM13, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->SEND_GOSSIP_MENU(7764, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM14, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->SEND_GOSSIP_MENU(7765, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM15, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            player->SEND_GOSSIP_MENU(7766, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            player->SEND_GOSSIP_MENU(7767, _Creature->GetGUID());
                                                            //'kill' our trigger to update quest status
            player->KilledMonster( TRIGGER_FRANKAL, _Creature->GetGUID() );
            break;
    }
    return true;
}

/*######
## npc_cenarion_scout_jalia
######*/

#define GOSSIP_ITEM_JALIA "Create Hive'Ashi Scout Report."

bool GossipHello_npc_cenarion_scout_jalia(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if( player->GetQuestStatus(8739) == QUEST_STATUS_INCOMPLETE )
    if(!player->HasItemCount(21161,1))    
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_JALIA, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO );

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_cenarion_scout_jalia(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if( action == GOSSIP_SENDER_INFO )
    {
        player->CastSpell( player, 25845, false);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## npc_cenarion_scout_azenel
######*/

#define GOSSIP_ITEM_AZENEL "Create Hive'Zora Scout Report."

bool GossipHello_npc_cenarion_scout_azenel(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if( player->GetQuestStatus(8534) == QUEST_STATUS_INCOMPLETE )
    if(!player->HasItemCount(21158,1))    
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_AZENEL, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO );

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_cenarion_scout_azenel(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if( action == GOSSIP_SENDER_INFO )
    {
        player->CastSpell( player, 25843, false);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## npc_cenarion_scout_landion
######*/

#define GOSSIP_ITEM_LANDION "Create Hive'Regal Scout Report."

bool GossipHello_npc_cenarion_scout_landion(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if( player->GetQuestStatus(8738) == QUEST_STATUS_INCOMPLETE )
       if(!player->HasItemCount(21160,1))    
          player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_LANDION, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO );

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_cenarion_scout_landion(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if( action == GOSSIP_SENDER_INFO )
    {
        player->CastSpell( player, 25847, false);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

///////
/// Wind Stone
///////

#define GOSSIP_WIND_STONE         "You will listen to this, ville duke! I am not your Twilight's Hammer lapdog! I am here to challenge you! Come! Come, and meet your death..."

#define DUKE_OF_SHARDS          15208
#define DUKE_OF_ZEPHYRS         15220
#define DUKE_OF_FATHOMS         15207
#define DUKE_OF_CYNDERS         15206

bool GossipHello_go_wind_stone(Player *player, GameObject* _GO)
{
    if(player->HasEquiped(20406) && player->HasEquiped(20408) && player->HasEquiped(20407) && player->HasEquiped(20422))
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_WIND_STONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    }

    player->SEND_GOSSIP_MENU(_GO->GetGOInfo()->questgiver.gossipID, _GO->GetGUID());
    return true;
}

void SendActionMenu_go_wind_stone(Player *player, GameObject* _GO, uint32 action)
{
    _GO->SetGoState(GO_STATE_ACTIVE);
    _GO->SetRespawnTime(600);
    player->CLOSE_GOSSIP_MENU();

    float x,y,z;
    player->GetClosePoint(x,y,z, 0.0f, 2.0f, frand(0, M_PI));

    switch(action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->CastSpell(player,24762,false);
        player->SummonCreature(RAND(DUKE_OF_CYNDERS, DUKE_OF_FATHOMS, DUKE_OF_SHARDS, DUKE_OF_ZEPHYRS), x,y,z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 600000);
        player->DestroyItemCount(20422, 1, true, true);
        break;
    }
}

bool GossipSelect_go_wind_stone(Player *player, GameObject* _GO, uint32 sender, uint32 action )
{
    switch(sender)
    {
        case GOSSIP_SENDER_MAIN:    SendActionMenu_go_wind_stone(player, _GO, action); break;
    }
    return true;
}

/*######
## go_silithyst_mound  (id 181597)
######*/
#define GOSSIP_TAKE_BUFF "[PH] Take the Silithyst"
#define SPELL_SILITHYST  29519

bool GOUse_go_silithyst_mound(Player* pPlayer, GameObject* pGO)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_TAKE_BUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(pGO->GetGOInfo()->questgiver.gossipID, pGO->GetGUID());
    return true;
}

bool GOGossipSelect_go_silithyst_mound(Player* pPlayer, GameObject* pGO, uint32 Sender, uint32 action)
{
    switch(action)
    {

    case GOSSIP_ACTION_INFO_DEF+1:
        if(!pPlayer->HasAura(SPELL_SILITHYST))
            pPlayer->CastSpell(pPlayer,SPELL_SILITHYST,false);  
        pGO->Delete();
        break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*###
##
####*/

void AddSC_silithus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_highlord_demitrian";
    newscript->pGossipHello =  &GossipHello_npc_highlord_demitrian;
    newscript->pGossipSelect = &GossipSelect_npc_highlord_demitrian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npcs_rutgar_and_frankal";
    newscript->pGossipHello =   &GossipHello_npcs_rutgar_and_frankal;
    newscript->pGossipSelect =  &GossipSelect_npcs_rutgar_and_frankal;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name="npc_cenarion_scout_jalia";
    newscript->pGossipHello = &GossipHello_npc_cenarion_scout_jalia;
    newscript->pGossipSelect = &GossipSelect_npc_cenarion_scout_jalia;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name="npc_cenarion_scout_azenel";
    newscript->pGossipHello = &GossipHello_npc_cenarion_scout_azenel;
    newscript->pGossipSelect = &GossipSelect_npc_cenarion_scout_azenel;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name="npc_cenarion_scout_landion";
    newscript->pGossipHello = &GossipHello_npc_cenarion_scout_landion;
    newscript->pGossipSelect = &GossipSelect_npc_cenarion_scout_landion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_wind_stone";
    newscript->pGOUse  = &GossipHello_go_wind_stone;
    newscript->pGossipSelectGO = &GossipSelect_go_wind_stone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_silithyst_mound";
    newscript->pGOUse =           &GOUse_go_silithyst_mound;
    newscript->pGossipSelectGO =  &GOGossipSelect_go_silithyst_mound;
    newscript->RegisterSelf();
}

