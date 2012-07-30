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
go_warmaul_prison
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

#define GOSSIP_ITEM_BEAST_1 "[PH] Create 1 x Flask of Beast"
#define GOSSIP_ITEM_BEAST_5 "[PH] Create 5 x Flask of Beast"

#define GOSSIP_ITEM_SORCERER_1 "[PH] Create 1 x Flask of Sorcerer"
#define GOSSIP_ITEM_SORCERER_5 "[PH] Create 5 x Flask of Sorcerer"


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
            pPlayer->CastSpell(pPlayer,(pGO->GetEntry() == 185919)
                               ? uint32(SPELL_CREATE_1_FLASK_OF_BEAST)
                               : uint32(SPELL_CREATE_1_FLASK_OF_SORCERER)
                               , false);
        break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CastSpell(pPlayer,(pGO->GetEntry() == 185919)
                               ? uint32(SPELL_CREATE_5_FLASK_OF_BEAST)
                               : uint32(SPELL_CREATE_5_FLASK_OF_SORCERER),
                               false);
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
## go_warmaul_prison
######*/

bool GOUse_go_warmaul_prison(Player* pPlayer, GameObject* pGO)
{
    //Working, but still count while the cage is empty. Any ideas?
    Unit *Prisoner = FindCreature(18428, 4.0f, pPlayer);
    if(!Prisoner)
        return true;

    if (pGO->GetGoType() == GAMEOBJECT_TYPE_DOOR)
    {
        DoScriptText(-1600000-urand(0, 3), Prisoner, pPlayer);
        pPlayer->CastedCreatureOrGO(18428, Prisoner->GetGUID(), 32347);
        Prisoner->Kill(Prisoner, false);
        ((Creature*)Prisoner)->RemoveCorpse();
    }

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
    newscript->Name = "go_warmaul_prison";
    newscript->pGOUse = &GOUse_go_warmaul_prison;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_DISCO";
    newscript->pGOUse = &GOUse_go_DISCO;
    newscript->RegisterSelf();
}

