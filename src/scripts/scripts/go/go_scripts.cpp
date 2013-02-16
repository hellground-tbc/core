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
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 4285,4287,4288(crystal pylons), 4296. Field_Repair_Bot->Teaches spell 22704. Barov_journal->Teaches spell 26089
SDCategory: Game Objects
EndScriptData */

/* ContentData
go_cat_figurine
go_northern_crystal_pylon
go_eastern_crystal_pylon
go_western_crystal_pylon
go_barov_journal
go_gilded_brazier - implemented at ghostlands.cpp
go_orb_of_command
go_tablet_of_madness
go_tablet_of_the_seven
go_jump_a_trone
go_ethereum_prison - implemented at netherstorm.cpp
go_blood_filled_orb
go_ethereum_stasis
matrix_punchograph
go_resonite_cask
go_sacred_fire_of_life
go_field_repair_bot_74A
go_teleporter
go_hive_pod
go_ethereum_transponder_zeta
EndContentData */

#include "precompiled.h"

/*######
## go_cat_figurine  UPDATE `gameobject_template` SET `ScriptName`='go_cat_figurine' WHERE `entry`=13873;
######*/

#define SPELL_SUMMON_GHOST_SABER    5968

bool GOUse_go_cat_figurine(Player *player, GameObject* _GO)
{
    player->CastSpell(player,SPELL_SUMMON_GHOST_SABER,true);
    return false;
}

/*######
## go_crystal_pylons (3x)
######*/

bool GOUse_go_northern_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4285) == QUEST_STATUS_INCOMPLETE)
        player->AreaExploredOrEventHappens(4285);

    return true;
}

bool GOUse_go_eastern_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4287) == QUEST_STATUS_INCOMPLETE)
        player->AreaExploredOrEventHappens(4287);

    return true;
}

bool GOUse_go_western_crystal_pylon(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        player->PrepareQuestMenu(_GO->GetGUID());
        player->SendPreparedQuest(_GO->GetGUID());
    }

    if (player->GetQuestStatus(4288) == QUEST_STATUS_INCOMPLETE)
        player->AreaExploredOrEventHappens(4288);

    return true;
}

/*######
## go_barov_journal
######*/

bool GOUse_go_barov_journal(Player *player, GameObject* _GO)
{
    if(player->HasSkill(SKILL_TAILORING) && player->GetBaseSkillValue(SKILL_TAILORING) >= 280 && !player->HasSpell(26086))
    {
        player->CastSpell(player,26095,false);
    }
    return true;
}

/*######
## go_field_repair_bot_74A
######*/

bool GOUse_go_field_repair_bot_74A(Player *player, GameObject* _GO)
{
    if(player->HasSkill(SKILL_ENGINERING) && player->GetBaseSkillValue(SKILL_ENGINERING) >= 300 && !player->HasSpell(22704))
    {
        player->CastSpell(player,22864,false);
    }
    return true;
}

/*######
## go_orb_of_command
######*/

#define GOSSIP_ORB_OF_COMMAND       "<Place my hand on the orb.>"
#define QUEST_BLACKHANDS_COMMAND    7761

bool GOUse_go_orb_of_command(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_BLACKHANDS_COMMAND))
    {
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ORB_OF_COMMAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7155, pGO->GetGUID());
    }

    return true;
}

bool GOGossipSelect_go_orb_of_command(Player* pPlayer, GameObject* pGO, uint32 Sender, uint32 action)
{
    switch(action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CastSpell(pPlayer, 23460, true);
            break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## go_tablet_of_madness
######*/

bool GOUse_go_tablet_of_madness(Player *player, GameObject* _GO)
{
    if (player->HasSkill(SKILL_ALCHEMY) && player->GetSkillValue(SKILL_ALCHEMY) >= 300 && !player->HasSpell(24266))
    {
        player->CastSpell(player,24267,false);
    }
    return true;
}

/*######
## go_tablet_of_the_seven
######*/

//TODO: use gossip option ("Transcript the Tablet") instead, if Trinity adds support.
bool GOUse_go_tablet_of_the_seven(Player *player, GameObject* _GO)
{
    if (_GO->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
        return true;

    if (player->GetQuestStatus(4296) == QUEST_STATUS_INCOMPLETE)
        player->CastSpell(player,15065,false);

    return true;
}

/*#####
## go_jump_a_tron
######*/

bool GOUse_go_jump_a_tron(Player *player, GameObject* _GO)
{
    if (player->GetQuestStatus(10111) == QUEST_STATUS_INCOMPLETE)
     player->CastSpell(player,33382,true);

    return true;
}

/*######
## go_sacred_fire_of_life
######*/

#define NPC_ARIKARA  10882

bool GOUse_go_sacred_fire_of_life(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ARIKARA, -5008.338, -2118.894, 83.657, 0.874, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return true;
}

/*######
## go_crystalforge
######*/

#define ITEM_APEXIS_SHARD 32569
#define GOSSIP_ITEM_BEAST_1 "Purchase 1 Unstable Flask of the Beast for the cost of 10 Apexis Shards"
#define GOSSIP_ITEM_BEAST_5 "Purchase 5 Unstable Flask of the Beast for the cost of 50 Apexis Shards"
#define GOSSIP_ITEM_SORCERER_1 "Purchase 1 Unstable Flask of the Sorcerer for the cost of 10 Apexis Shards"
#define GOSSIP_ITEM_SORCERER_5 "Purchase 5 Unstable Flask of the Sorcerer for the cost of 50 Apexis Shards"

enum FELFORGE
{
    SPELL_CREATE_1_FLASK_OF_BEAST   = 40964,
    SPELL_CREATE_5_FLASK_OF_BEAST   = 40965,
};

enum BASHIRFORGE
{
    SPELL_CREATE_1_FLASK_OF_SORCERER   = 40968,
    SPELL_CREATE_5_FLASK_OF_SORCERER   = 40970,
};

bool GOUse_go_crystalforge(Player* pPlayer, GameObject* pGO)
{
    switch(pGO->GetEntry())
    {
        case 185919: // Fel Crystalforge
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BEAST_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BEAST_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        break;

        case 185921: // Bashir Crystalforge
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SORCERER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SORCERER_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        break;
    }
    pPlayer->SEND_GOSSIP_MENU(pGO->GetGOInfo()->questgiver.gossipID, pGO->GetGUID());
    return true;
}

bool GOGossipSelect_go_crystalforge(Player* pPlayer, GameObject* pGO, uint32 Sender, uint32 action)
{
    switch(action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            if (pPlayer->HasItemCount(ITEM_APEXIS_SHARD, 10))
            {
                pPlayer->CastSpell(pPlayer,(pGO->GetEntry() == 185919)
                               ? uint32(SPELL_CREATE_1_FLASK_OF_BEAST)
                               : uint32(SPELL_CREATE_1_FLASK_OF_SORCERER)
                               , false);
            }
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            if (pPlayer->HasItemCount(ITEM_APEXIS_SHARD, 50))
            {
                pPlayer->CastSpell(pPlayer,(pGO->GetEntry() == 185919)
                               ? uint32(SPELL_CREATE_5_FLASK_OF_BEAST)
                               : uint32(SPELL_CREATE_5_FLASK_OF_SORCERER),
                               false);
            }
            break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## matrix_punchograph
######*/

enum eMatrixPunchograph
{
    ITEM_WHITE_PUNCH_CARD = 9279,
    ITEM_YELLOW_PUNCH_CARD = 9280,
    ITEM_BLUE_PUNCH_CARD = 9282,
    ITEM_RED_PUNCH_CARD = 9281,
    ITEM_PRISMATIC_PUNCH_CARD = 9316,
    SPELL_YELLOW_PUNCH_CARD = 11512,
    SPELL_BLUE_PUNCH_CARD = 11525,
    SPELL_RED_PUNCH_CARD = 11528,
    SPELL_PRISMATIC_PUNCH_CARD = 11545,
    MATRIX_PUNCHOGRAPH_3005_A = 142345,
    MATRIX_PUNCHOGRAPH_3005_B = 142475,
    MATRIX_PUNCHOGRAPH_3005_C = 142476,
    MATRIX_PUNCHOGRAPH_3005_D = 142696,
};

bool GOUse_go_matrix_punchograph(Player *pPlayer, GameObject *pGO)
{
    switch(pGO->GetEntry())
    {
        case MATRIX_PUNCHOGRAPH_3005_A:
            if (pPlayer->HasItemCount(ITEM_WHITE_PUNCH_CARD, 1))
            {
                pPlayer->CastSpell(pPlayer,SPELL_YELLOW_PUNCH_CARD,true);
                pPlayer->DestroyItemCount(ITEM_WHITE_PUNCH_CARD, 1, true);
            }
            break;
        case MATRIX_PUNCHOGRAPH_3005_B:
            if (pPlayer->HasItemCount(ITEM_YELLOW_PUNCH_CARD, 1))
            {
                pPlayer->CastSpell(pPlayer,SPELL_BLUE_PUNCH_CARD,true);
                pPlayer->DestroyItemCount(ITEM_YELLOW_PUNCH_CARD, 1, true);
            }
            break;
        case MATRIX_PUNCHOGRAPH_3005_C:
            if (pPlayer->HasItemCount(ITEM_BLUE_PUNCH_CARD, 1))
            {
                pPlayer->CastSpell(pPlayer,SPELL_RED_PUNCH_CARD,true);
                pPlayer->DestroyItemCount(ITEM_BLUE_PUNCH_CARD, 1, true);
            }
            break;
        case MATRIX_PUNCHOGRAPH_3005_D:
            if (pPlayer->HasItemCount(ITEM_RED_PUNCH_CARD, 1))
            {
                pPlayer->CastSpell(pPlayer, SPELL_PRISMATIC_PUNCH_CARD, true);
                pPlayer->DestroyItemCount(ITEM_RED_PUNCH_CARD, 1, true);
            }
            break;
        default:
            break;
    }
    return false;
}

/*######
## go_blood_filled_orb
######*/

#define NPC_ZELEMAR  17830

bool GOUse_go_blood_filled_orb(Player *pPlayer, GameObject *pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ZELEMAR, -369.746, 166.759, -21.50, 5.235, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return false;
}


/*######
## go_ethereum_stasis
######*/

const uint32 NpcStasisEntry[] =
{
    22825, 20888, 22827, 22826, 22828
};

bool GOUse_go_ethereum_stasis(Player* pPlayer, GameObject* pGo)
{
    pGo->SetGoState(GO_STATE_ACTIVE);
    pPlayer->SummonCreature(NpcStasisEntry[rand()%5],pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetAngle(pPlayer), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
    pGo->SetRespawnTime(120);
    return false;
}

/*######
## go_resonite_cask
######*/

enum eResoniteCask
{
    NPC_GOGGEROC    = 11920
};

bool GOUse_go_resonite_cask(Player *pPlayer, GameObject *pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
    {
        Unit* Goggeroc = FindCreature(NPC_GOGGEROC, 10.0, pPlayer); //prevent multiple summoning
        if(!Goggeroc)
            pGO->SummonCreature(NPC_GOGGEROC, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);
    }

    return false;
}

/*######
## go_darkmoon_cannon
######*/

#define SPELL_WINGS 42867

bool GOUse_go_darkmoon_cannon(Player *player, GameObject* _GO)
{
    // player->Relocate(    //przeniesc gracza na czubek armaty :]
    player->CastSpell(player,SPELL_WINGS,true);
    return false;
}

/*######
## go_hive_pod (Quest 1126: Hive in the Tower)
######*/

enum eHives
{
    QUEST_HIVE_IN_THE_TOWER                       = 1126,
    NPC_HIVE_AMBUSHER                             = 13301
};

bool GOUse_go_hive_pod(Player *pPlayer, GameObject *pGO)
{
    pPlayer->SendLoot(pGO->GetGUID(), LOOT_CORPSE);
    pGO->SummonCreature(NPC_HIVE_AMBUSHER,pGO->GetPositionX()-1,pGO->GetPositionY(),pGO->GetPositionZ(),pGO->GetAngle(pPlayer),TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    pGO->SummonCreature(NPC_HIVE_AMBUSHER,pGO->GetPositionX(),pGO->GetPositionY()-1,pGO->GetPositionZ(),pGO->GetAngle(pPlayer),TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);

    return false;
}

/*######
## go_DISCO
######*/

#define SPELL_LISTEN_TO_THE_MUSIC   50493

bool GOUse_go_DISCO(Player* pPlayer, GameObject* pGO)
{
    pPlayer->CastSpell(pPlayer, SPELL_LISTEN_TO_THE_MUSIC, true);

    return false;
}

/*######
## go_ethereum_transponder_zeta
######*/

#define NPC_AMEER  20482

bool GOUse_go_ethereum_transponder_zeta(Player* pPlayer, GameObject* pGO)
{
    pGO->UseDoorOrButton(60);
    pPlayer->SummonCreature(NPC_AMEER, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+1, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
	
    return true;
}

/*######
## go_ethereal_teleport_pad
######*/

#define NPC_MARID  20518

bool GOUse_go_ethereal_teleport_pad(Player* pPlayer, GameObject* pGO)
{
    pGO->UseDoorOrButton(60);
    pPlayer->SummonCreature(NPC_MARID, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+1, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
	
    return true;
}

/*######
## go_fel_crystal_prism
######*/

#define GOSSIP_ITEM_1 "Insert 35 Apexis Shards!"

enum
{
    NPC_BRAXXUS        = 23353,
    NPC_INCINERATOR    = 23354,
    NPC_GALVANOTH      = 22281,
    NPC_ZARCSIN        = 23355,

    ITEM_APEX_SHARD    = 32569
};

bool GOUse_go_fel_crystal_prism(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->HasItemCount(ITEM_APEX_SHARD,  35) && pPlayer->GetQuestStatus(11079) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pGO->GetGOInfo()->questgiver.gossipID, pGO->GetGUID());
    return true;
}

bool GOGossipSelect_go_fel_crystal_prism(Player* pPlayer, GameObject* pGO, uint32 Sender, uint32 action)
{
    switch (urand(0,3))
    {
        case 0:
            pGO->SummonCreature(NPC_BRAXXUS, pGO->GetPositionX()+(rand()%4), pGO->GetPositionY()-(rand()%4), pGO->GetPositionZ()-(rand()%4), pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 1: 
            pGO->SummonCreature(NPC_INCINERATOR, pGO->GetPositionX()+(rand()%4), pGO->GetPositionY()-(rand()%4), pGO->GetPositionZ()-(rand()%4), pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 2:
            pGO->SummonCreature(NPC_GALVANOTH, pGO->GetPositionX()+(rand()%4), pGO->GetPositionY()-(rand()%4), pGO->GetPositionZ()-(rand()%4), pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 3:
            pGO->SummonCreature(NPC_ZARCSIN, pGO->GetPositionX()+(rand()%4), pGO->GetPositionY()-(rand()%4), pGO->GetPositionZ()-(rand()%4), pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
    }

    pPlayer->DestroyItemCount(ITEM_APEX_SHARD, 35, true);
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## go_rule_skies
######*/

enum
{
    NPC_RIVENDARK    = 23061,
    NPC_OBSIDIA      = 23282,
    NPC_FURYWING     = 23261,
    NPC_INSIDION     = 23281
};

bool GOUse_go_rule_skies(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->GetQuestStatus(11078) == QUEST_STATUS_INCOMPLETE)
    {
        switch(pGO->GetEntry())
        {
            case 185936:
                pGO->SummonCreature(NPC_RIVENDARK, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+10.0f, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 185932:
                pGO->SummonCreature(NPC_OBSIDIA, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+10.0f, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 185937:
                pGO->SummonCreature(NPC_FURYWING, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+10.0f, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 185938:
                pGO->SummonCreature(NPC_INSIDION, pGO->GetPositionX(), pGO->GetPositionY(), pGO->GetPositionZ()+10.0f, pGO->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
        }
        pGO->SetGoState(GO_STATE_ACTIVE);
        pGO->SetRespawnTime(300);
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## go_beer_keg
######*/

enum
{
    NPC_CREDIT_FERMENTED    = 22368,
    NPC_CREDIT_GREEN        = 22356,
    NPC_CREDIT_MOONSHINE    = 22367
};

bool GOUse_go_beer_keg(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->GetQuestStatus(10720) == QUEST_STATUS_INCOMPLETE)
    {
        switch(pGO->GetEntry())
        {
            case 185214:
                pPlayer->KilledMonster(NPC_CREDIT_FERMENTED, pGO->GetGUID());
                break;
            case 185206:
                pPlayer->KilledMonster(NPC_CREDIT_GREEN, pGO->GetGUID());
                break;
            case 185213:
                pPlayer->KilledMonster(NPC_CREDIT_MOONSHINE, pGO->GetGUID());
                break;
        }
    }

    return true;
}

void AddSC_go_scripts()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="go_northern_crystal_pylon";
    newscript->pGOUse =           &GOUse_go_northern_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_eastern_crystal_pylon";
    newscript->pGOUse =           &GOUse_go_eastern_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_western_crystal_pylon";
    newscript->pGOUse =           &GOUse_go_western_crystal_pylon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_barov_journal";
    newscript->pGOUse =           &GOUse_go_barov_journal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_field_repair_bot_74A";
    newscript->pGOUse =           &GOUse_go_field_repair_bot_74A;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_orb_of_command";
    newscript->pGOUse =           &GOUse_go_orb_of_command;
    newscript->pGossipSelectGO =  &GOGossipSelect_go_orb_of_command;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_tablet_of_madness";
    newscript->pGOUse =           &GOUse_go_tablet_of_madness;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_tablet_of_the_seven";
    newscript->pGOUse =           &GOUse_go_tablet_of_the_seven;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_jump_a_tron";
    newscript->pGOUse =           &GOUse_go_jump_a_tron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_sacred_fire_of_life";
    newscript->pGOUse =           &GOUse_go_sacred_fire_of_life;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_crystalforge";
    newscript->pGOUse =           &GOUse_go_crystalforge;
    newscript->pGossipSelectGO =  &GOGossipSelect_go_crystalforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_matrix_punchograph";
    newscript->pGOUse =           &GOUse_go_matrix_punchograph;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_cat_figurine";
    newscript->pGOUse =           &GOUse_go_cat_figurine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_blood_filled_orb";
    newscript->pGOUse =           &GOUse_go_blood_filled_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ethereum_stasis";
    newscript->pGOUse =           &GOUse_go_ethereum_stasis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_resonite_cask";
    newscript->pGOUse =           &GOUse_go_resonite_cask;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_darkmoon_cannon";
    newscript->pGOUse =           &GOUse_go_darkmoon_cannon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_hive_pod";
    newscript->pGOUse = &GOUse_go_hive_pod;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_DISCO";
    newscript->pGOUse = &GOUse_go_DISCO;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ethereum_transponder_zeta";
    newscript->pGOUse = &GOUse_go_ethereum_transponder_zeta;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ethereal_teleport_pad";
    newscript->pGOUse = &GOUse_go_ethereal_teleport_pad;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_fel_crystal_prism";
    newscript->pGOUse = &GOUse_go_fel_crystal_prism;
    newscript->pGossipSelectGO = &GOGossipSelect_go_fel_crystal_prism;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_rule_skies";
    newscript->pGOUse = &GOUse_go_rule_skies;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_beer_keg";
    newscript->pGOUse = &GOUse_go_beer_keg;
    newscript->RegisterSelf();
}

