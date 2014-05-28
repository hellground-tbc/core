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
SDName: Feralas
SD%Complete: 100
SDComment: Quest support: 3520, 2767. Special vendor Gregan Brewspewer
SDCategory: Feralas
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_gregan_brewspewer
######*/

#define GOSSIP_HELLO "Buy somethin', will ya?"

bool GossipHello_npc_gregan_brewspewer(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (creature->isVendor() && player->GetQuestStatus(3909) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(2433, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_gregan_brewspewer(Player* player, Creature* creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        player->SEND_GOSSIP_MENU(2434, creature->GetGUID());
    }
    if (action == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(creature->GetGUID());
    return true;
}

/*######
## npc_screecher_spirit
######*/

bool GossipHello_npc_screecher_spirit(Player* player, Creature* creature)
{
    player->SEND_GOSSIP_MENU(2039, creature->GetGUID());
    player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    return true;
}

/*######
## npc_oox22fe
######*/

enum oox22fe
{
    SAY_OOX_START         = -1060000,
    SAY_OOX_AGGRO1        = -1060001,
    SAY_OOX_AGGRO2        = -1060002,
    SAY_OOX_AMBUSH        = -1060003,
    SAY_OOX_END           = -1060005,

    NPC_YETI              = 7848,
    NPC_GORILLA           = 5260,
    NPC_WOODPAW_REAVER    = 5255,
    NPC_WOODPAW_BRUTE     = 5253,
    NPC_WOODPAW_ALPHA     = 5258,
    NPC_WOODPAW_MYSTIC    = 5254,

    QUEST_RESCUE_OOX22FE  = 2767
};

struct npc_oox22feAI : public npc_escortAI
{
    npc_oox22feAI(Creature* creature) : npc_escortAI(creature) {}

    void Reset() {}

    void EnterCombat(Unit* who)
    {
        switch (urand(0, 9))
        {
            case 0:
                DoScriptText(SAY_OOX_AGGRO1, me);
                break;
            case 1:
                DoScriptText(SAY_OOX_AGGRO2, me);
                break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void WaypointReached(uint32 i)
    {
        switch (i)
        {
            case 11:
                DoScriptText(SAY_OOX_AMBUSH, me);
                break;
            case 12:
                me->SummonCreature(NPC_YETI, -4850.91f, 1595.67f, 72.99f, 2.85f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_YETI, -4851.76f, 1599.37f, 73.85f, 2.85f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_YETI, -4852.81f, 1592.63f, 72.28f, 2.85f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 23:
                DoScriptText(SAY_OOX_AMBUSH, me);
                break;
            case 24:
                me->SummonCreature(NPC_GORILLA, -4609.56f, 1991.80f, 57.24f, 3.74f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_GORILLA, -4613.00f, 1994.45f, 57.2f, 3.78f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_GORILLA, -4619.37f, 1998.11f, 57.72f, 3.84f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 31:
                DoScriptText(SAY_OOX_AMBUSH, me);
                break;
            case 32:
                me->SummonCreature(NPC_WOODPAW_REAVER, -4425.14f, 2075.87f, 47.77f, 3.77f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_WOODPAW_BRUTE , -4426.68f, 2077.98f, 47.57f, 3.77f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_WOODPAW_MYSTIC, -4428.33f, 2080.24f, 47.43f, 3.87f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_WOODPAW_ALPHA , -4430.04f, 2075.54f, 46.83f, 3.81f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 40:
                DoScriptText(SAY_OOX_END, me);
                if (Player* player = GetPlayerForEscort())
                    player->GroupEventHappens(QUEST_RESCUE_OOX22FE, me);
                break;
        }
    }
};

CreatureAI* GetAI_npc_oox22fe(Creature* creature)
{
    return new npc_oox22feAI(creature);
}

bool QuestAccept_npc_oox22fe(Player* player, Creature* creature, const Quest* quest)
{
    if (quest->GetQuestId() == QUEST_RESCUE_OOX22FE)
    {
        creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoScriptText(SAY_OOX_START, creature);

        if (npc_escortAI* EscortAI = CAST_AI(npc_oox22feAI, creature->AI()))
            EscortAI->Start(true, true, player->GetGUID());
    }
    return true;
}

/*######
## AddSC
######*/

void AddSC_feralas()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_gregan_brewspewer";
    newscript->pGossipHello = &GossipHello_npc_gregan_brewspewer;
    newscript->pGossipSelect = &GossipSelect_npc_gregan_brewspewer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_screecher_spirit";
    newscript->pGossipHello = &GossipHello_npc_screecher_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oox22fe";
    newscript->GetAI = &GetAI_npc_oox22fe;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_oox22fe;
    newscript->RegisterSelf();
}

