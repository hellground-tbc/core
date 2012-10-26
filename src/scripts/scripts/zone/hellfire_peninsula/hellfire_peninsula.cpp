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
SDName: Hellfire_Peninsula
SD%Complete: 100
 SDComment: Quest support: 9375, 9410, 9418, 10129, 10146, 10162, 10163, 10340, 10346, 10347, 10382 (Special flight paths), 11516
SDCategory: Hellfire Peninsula
EndScriptData */

/* ContentData
npc_aeranas
go_haaleshi_altar
npc_wing_commander_dabiree
npc_gryphoneer_windbellow
npc_wing_commander_brack
npc_wounded_blood_elf
npc_earthcaller_ryga
npc_ancestral_spirit_wolf
npc_living_flare
npc_felblood_initiate
npc_emaciated_felblood
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"

/*######
## npc_aeranas
######*/

#define SAY_SUMMON                      -1000138
#define SAY_FREE                        -1000139

#define FACTION_HOSTILE                 16
#define FACTION_FRIENDLY                35

#define SPELL_ENVELOPING_WINDS          15535
#define SPELL_SHOCK                     12553

#define C_AERANAS                       17085

struct HELLGROUND_DLL_DECL npc_aeranasAI : public ScriptedAI
{
    npc_aeranasAI(Creature* c) : ScriptedAI(c) {}

    uint32 Faction_Timer;
    uint32 EnvelopingWinds_Timer;
    uint32 Shock_Timer;

    void Reset()
    {
        Faction_Timer = 8000;
        EnvelopingWinds_Timer = 9000;
        Shock_Timer = 5000;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->setFaction(FACTION_FRIENDLY);

        DoScriptText(SAY_SUMMON, m_creature);
    }

    void EnterCombat(Unit *who) {}

    void UpdateAI(const uint32 diff)
    {
        if (Faction_Timer)
        {
            if (Faction_Timer < diff)
            {
                m_creature->setFaction(FACTION_HOSTILE);
                Faction_Timer = 0;
            }else Faction_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        if ((m_creature->GetHealth()*100) / m_creature->GetMaxHealth() < 30)
        {
            m_creature->setFaction(FACTION_FRIENDLY);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            m_creature->RemoveAllAuras();
            m_creature->DeleteThreatList();
            m_creature->CombatStop();
            DoScriptText(SAY_FREE, m_creature);
            return;
        }

        if (Shock_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_SHOCK);
            Shock_Timer = 10000;
        }else Shock_Timer -= diff;

        if (EnvelopingWinds_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_ENVELOPING_WINDS);
            EnvelopingWinds_Timer = 25000;
        }else EnvelopingWinds_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aeranas(Creature *_Creature)
{
    return new npc_aeranasAI (_Creature);
}

/*######
## go_haaleshi_altar
######*/

bool GOUse_go_haaleshi_altar(Player *player, GameObject* _GO)
{
    _GO->SummonCreature(C_AERANAS,-1321.79, 4043.80, 116.24, 1.25, TEMPSUMMON_TIMED_DESPAWN, 180000);
    return false;
}

/*######
## npc_wing_commander_dabiree
######*/

#define GOSSIP_ITEM1_DAB "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_DAB "Fly me to Shatter Point"

bool GossipHello_npc_wing_commander_dabiree(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    //Mission: The Murketh and Shaadraz Gateways
    if (player->GetQuestStatus(10146) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    //Shatter Point
    if (!player->GetQuestRewardStatus(10340))
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_wing_commander_dabiree(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,33768,true);               //TaxiPath 585 (Gateways Murket and Shaadraz)
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,35069,true);               //TaxiPath 612 (Taxi - Hellfire Peninsula - Expedition Point to Shatter Point)
    }
    return true;
}

/*######
## npc_gryphoneer_windbellow
######*/

#define GOSSIP_ITEM1_WIN "Fly me to The Abyssal Shelf"
#define GOSSIP_ITEM2_WIN "Fly me to Honor Point"

bool GossipHello_npc_gryphoneer_windbellow(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
    if (player->GetQuestStatus(10163) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10346) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_WIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    //Go to the Front
    if (player->GetQuestStatus(10382) != QUEST_STATUS_NONE && !player->GetQuestRewardStatus(10382))
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_WIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_gryphoneer_windbellow(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,33899,true);               //TaxiPath 589 (Aerial Assault Flight (Alliance))
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,35065,true);               //TaxiPath 607 (Taxi - Hellfire Peninsula - Shatter Point to Beach Head)
    }
    return true;
}

/*######
## npc_wing_commander_brack
######*/

#define GOSSIP_ITEM1_BRA "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_BRA "Fly me to The Abyssal Shelf"
#define GOSSIP_ITEM3_BRA "Fly me to Spinebreaker Post"

bool GossipHello_npc_wing_commander_brack(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    //Mission: The Murketh and Shaadraz Gateways
    if (player->GetQuestStatus(10129) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
    if (player->GetQuestStatus(10162) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10347) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    //Spinebreaker Post
    if (player->GetQuestStatus(10242) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(10242))
        player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM3_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_wing_commander_brack(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    switch(action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,33659,true);               //TaxiPath 584 (Gateways Murket and Shaadraz)
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,33825,true);               //TaxiPath 587 (Aerial Assault Flight (Horde))
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,34578,true);               //TaxiPath 604 (Taxi - Reaver's Fall to Spinebreaker Ridge)
        break;
    }
        return true;
}

/*######
## npc_wounded_blood_elf
######*/

#define SAY_ELF_START               -1000117
#define SAY_ELF_SUMMON1             -1000118
#define SAY_ELF_RESTING             -1000119
#define SAY_ELF_SUMMON2             -1000120
#define SAY_ELF_COMPLETE            -1000121
#define SAY_ELF_AGGRO               -1000122

#define QUEST_ROAD_TO_FALCON_WATCH  9375

struct HELLGROUND_DLL_DECL npc_wounded_blood_elfAI : public npc_escortAI
{
    npc_wounded_blood_elfAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();
        if (!player)
            return;

        switch (i)
        {
        case 0:
            DoScriptText(SAY_ELF_START, m_creature, player);
            break;
        case 9:
            DoScriptText(SAY_ELF_SUMMON1, m_creature, player);
            // Spawn two Haal'eshi Talonguard
            DoSpawnCreature(16967, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            DoSpawnCreature(16967, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            break;
        case 13:
            DoScriptText(SAY_ELF_RESTING, m_creature, player);
            // make the NPC kneel
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
            break;
        case 14:
            DoScriptText(SAY_ELF_SUMMON2, m_creature, player);
            // Spawn two Haal'eshi Windwalker
            DoSpawnCreature(16966, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            DoSpawnCreature(16966, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            break;
        case 27:
            DoScriptText(SAY_ELF_COMPLETE, m_creature, player);
            // Award quest credit
            player->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH,m_creature);
            break;
        }
    }

    void Reset() { }

    void EnterCombat(Unit* who)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            DoScriptText(SAY_ELF_AGGRO, m_creature);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(m_creature);
    }
};

CreatureAI* GetAI_npc_wounded_blood_elf(Creature *_Creature)
{
    npc_wounded_blood_elfAI* welfAI = new npc_wounded_blood_elfAI(_Creature);

    welfAI->AddWaypoint(0, -1137.72, 4272.10, 14.04, 5000);
    welfAI->AddWaypoint(1, -1141.34, 4232.42, 14.63);
    welfAI->AddWaypoint(2, -1133.47, 4220.88, 11.78);
    welfAI->AddWaypoint(3, -1126.18, 4213.26, 13.51);
    welfAI->AddWaypoint(4, -1100.12, 4204.32, 16.41);
    welfAI->AddWaypoint(5, -1063.68, 4197.92, 15.51);
    welfAI->AddWaypoint(6, -1027.28, 4194.36, 15.52);
    welfAI->AddWaypoint(7, -995.68, 4189.59, 19.84);
    welfAI->AddWaypoint(8, -970.90, 4188.60, 24.61);
    welfAI->AddWaypoint(9, -961.93, 4193.34, 26.11, 15000); // Summon 1
    welfAI->AddWaypoint(10, -935.52, 4210.99, 31.98);
    welfAI->AddWaypoint(11, -913.42, 4218.27, 37.29);
    welfAI->AddWaypoint(12, -896.53, 4207.73, 43.23);
    welfAI->AddWaypoint(13, -868.49, 4194.77, 46.75, 30000);// Kneel and Rest Here
    welfAI->AddWaypoint(14, -852.83, 4198.29, 47.28, 15000);// Summon 2
    welfAI->AddWaypoint(15, -819.85, 4200.50, 46.37);
    welfAI->AddWaypoint(16, -791.92, 4201.96, 44.19);
    welfAI->AddWaypoint(17, -774.42, 4202.46, 47.41);
    welfAI->AddWaypoint(18, -762.90, 4202.17, 48.81);
    welfAI->AddWaypoint(19, -728.25, 4195.35, 50.68);
    welfAI->AddWaypoint(20, -713.58, 4192.07, 53.98);
    welfAI->AddWaypoint(21, -703.09, 4189.74, 56.96);
    welfAI->AddWaypoint(22, -693.70, 4185.43, 57.06);
    welfAI->AddWaypoint(23, -686.38, 4159.81, 60.26);
    welfAI->AddWaypoint(24, -679.88, 4147.04, 64.20);
    welfAI->AddWaypoint(25, -656.74, 4147.72, 64.11);
    welfAI->AddWaypoint(26, -652.22, 4137.50, 64.58);
    welfAI->AddWaypoint(27, -649.99, 4136.38, 64.63, 30000);// Award Quest Credit

    return (CreatureAI*)welfAI;
}

bool QuestAccept_npc_wounded_blood_elf(Player* player, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_wounded_blood_elfAI, pCreature->AI()))
            pEscortAI->Start(true, false, player->GetGUID());

        // Change faction so mobs attack
        pCreature->setFaction(775);
    }

    return true;
}


/*######
## npc_demoniac_scryer
######*/

#define GOSSIP_ITEM_ATTUNE    "Yes, Scryer. You may possess me."
#define FINISHED_WHISPER    "Thank you for allowing me to visit, $N. You have a very colorful soul, but it's a little brighter than I prefer... or I might have stayed longer!"

enum
{
    GOSSIP_TEXTID_PROTECT           = 10659,
    GOSSIP_TEXTID_ATTUNED           = 10643,

    QUEST_DEMONIAC                  = 10838,
    NPC_HELLFIRE_WARDLING           = 22259,
    NPC_ORC_HA                      = 22273,
    NPC_BUTTRESS                    = 22267,
    NPC_BUTTRESS_SPAWNER            = 22260,

    MAX_BUTTRESS                    = 4,
    TIME_TOTAL                      = MINUTE*10*IN_MILISECONDS,

    SPELL_SUMMONED                  = 7741,
    SPELL_DEMONIAC_VISITATION       = 38708,
    SPELL_BUTTRESS_APPERANCE        = 38719,
    SPELL_SUCKER_CHANNEL            = 38721,
    SPELL_SUCKER_DESPAWN_MOB        = 38691,
};

struct HELLGROUND_DLL_DECL npc_demoniac_scryerAI : public ScriptedAI
{
    npc_demoniac_scryerAI(Creature* c) : ScriptedAI(c) {}

    bool IfIsComplete;

    uint32 m_SpawnDemonTimer;
    uint32 m_SpawnOrcTimer;
    uint32 m_SpawnButtressTimer;
    uint32 m_EndTimer;
    uint32 m_ButtressCount;
    std::list<uint64> PlayersWithQuestList;

    void Reset()
	{
        IfIsComplete = false;
        m_SpawnDemonTimer = 15000;
        m_SpawnOrcTimer = 30000;
        m_SpawnButtressTimer = 45000;
        m_EndTimer = 262000;
        m_ButtressCount = 0;
        PlayersWithQuestList.clear();

        std::list<Unit*> PlayerList;
        uint32 questDist = 60;                      // sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE);
        Hellground::AnyUnitInObjectRangeCheck  check(me, questDist);
        Hellground::UnitListSearcher<Hellground::AnyUnitInObjectRangeCheck > searcher(PlayerList, check);
        Cell::VisitAllObjects(me, searcher, questDist);

        for(std::list<Unit*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
        {
            if((*i)->GetTypeId() != TYPEID_PLAYER)
                continue;
            Player *player = (Player*)(*i);
            if(player->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
                PlayersWithQuestList.push_back(player->GetGUID());
        }
	}

    void AttackedBy(Unit* pEnemy) {}
    void AttackStart(Unit* pEnemy) {}
 
    void DoSpawnButtress()
    {
        ++m_ButtressCount;

        float fAngle = 0.0f;

        switch(m_ButtressCount)
        {
            case 1: fAngle = 0.0f; break;
            case 2: fAngle = 4.6f; break;
            case 3: fAngle = 1.5f; break;
            case 4: fAngle = 3.1f; break;
        }

        float fX, fY, fZ;
        m_creature->GetNearPoint(m_creature, fX, fY, fZ, 0.0f, 5.0f, fAngle);

        uint32 m_Time = TIME_TOTAL - (m_SpawnButtressTimer * m_ButtressCount);
        m_creature->SummonCreature(NPC_BUTTRESS, fX, fY, fZ, m_creature->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, m_Time);
        m_creature->SummonCreature(NPC_BUTTRESS_SPAWNER, fX, fY, fZ, m_creature->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, m_Time);
    }

    void DoSpawnDemon()
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
        m_creature->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void DospawnOrc()
    {
        float fX, fY, fZ;
        m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
        m_creature->SummonCreature(NPC_ORC_HA, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
            pSummoned->AI()->AttackStart(m_creature);
        }
        if (pSummoned->GetEntry() == NPC_ORC_HA)
        {
            pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
            pSummoned->AI()->AttackStart(m_creature);
        }
        if (pSummoned->GetEntry() == NPC_BUTTRESS)
        {
            pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, false);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_BUTTRESS_SPAWNER)
            {
                pSummoned->CastSpell(m_creature, SPELL_SUCKER_CHANNEL, true);
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pTarget->GetEntry() == NPC_BUTTRESS && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
            ((Creature*)pTarget)->setDeathState(CORPSE);
        if (pTarget->GetEntry() == NPC_BUTTRESS_SPAWNER && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
            ((Creature*)pTarget)->setDeathState(CORPSE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_EndTimer <= diff)
        {
            m_creature->setDeathState(CORPSE);
            m_EndTimer = 262000;
        }
        else m_EndTimer -= diff;

        if (IfIsComplete)
            return;

        if (m_SpawnButtressTimer <= diff)
        {
            if (m_ButtressCount >= MAX_BUTTRESS)
            {
                DoCast(m_creature, SPELL_SUCKER_DESPAWN_MOB);
                for(std::list<uint64>::iterator i = PlayersWithQuestList.begin(); i != PlayersWithQuestList.end(); i++)
                {
                    if(Unit* player = Unit::GetUnit(*me, (*i)))
                    {      
                        me->Whisper(FINISHED_WHISPER, (*i));  
                    }
                }
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                IfIsComplete = true;
                return;
            }
            m_SpawnButtressTimer = 45000;
            DoSpawnButtress();
        }
        else m_SpawnButtressTimer -= diff;

        if (m_SpawnDemonTimer <= diff)
        {
            DoSpawnDemon();
            m_SpawnDemonTimer = 15000;
        }
        else m_SpawnDemonTimer -= diff;

        if (m_SpawnOrcTimer <= diff)
        {
            DospawnOrc();
            m_SpawnOrcTimer = 30000;
        }
        else m_SpawnOrcTimer -= diff;
    }
};

CreatureAI* GetAI_npc_demoniac_scryer(Creature* c)
{
    return new npc_demoniac_scryerAI(c);
}

bool GossipHello_npc_demoniac_scryer(Player* pPlayer, Creature* c)
{
    if (npc_demoniac_scryerAI* pScryerAI = dynamic_cast<npc_demoniac_scryerAI*>(c->AI()))
    {
        if (pScryerAI->IfIsComplete)
        {
            if (pPlayer->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ATTUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ATTUNED, c->GetObjectGuid());
            return true;
        }
    }
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_PROTECT, c->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_demoniac_scryer(Player* pPlayer, Creature* c, uint32 m_Sender, uint32 m_Action)
{
    if (m_Action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        c->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, false);
    }
    return true;
}

/*######
## npc_magic_sucker_device_spawner
######*/

enum
{
    SPELL_EFFECT    = 38724,
    NPC_SCRYER      = 22258,
    NPC_BUTTRES     = 22267
};

struct HELLGROUND_DLL_DECL npc_magic_sucker_device_spawnerAI : public ScriptedAI
{
    npc_magic_sucker_device_spawnerAI(Creature* c) : ScriptedAI(c) {}

    uint32 m_CastTimer;
    uint32 m_CheckTimer;

    void Reset()
    {
        m_CastTimer = 1800;
        m_CheckTimer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_CastTimer <= diff)
        {
            DoCast(m_creature, SPELL_EFFECT);
            m_CastTimer = 1800;
        }
        else m_CastTimer -= diff;

        if (m_CheckTimer <= diff)
        {
            if (Creature* pScr = GetClosestCreatureWithEntry(m_creature, NPC_SCRYER, 15.0f, false))
            {
                if (Creature* pBut = GetClosestCreatureWithEntry(m_creature, NPC_BUTTRES, 5.0f))
                {
                    pBut->setDeathState(CORPSE);
                    m_creature->setDeathState(CORPSE);
                }
            }

            m_CheckTimer = 5000;
        }
        else m_CheckTimer -= diff;
    }
};
CreatureAI* GetAI_npc_magic_sucker_device_spawner(Creature* c)
{
    return new npc_magic_sucker_device_spawnerAI(c);
}

/*######
## npc_ancestral_spirit_wolf & npc_earthcaller_ryga
######*/

enum AncestralSpiritWolf
{
    EMOTE_HEAD_UP                           = -1811000,
    EMOTE_HOWL                              = -1811001,
    EMOTE_RYGA                              = -1811002,
    SPELL_ANCESTRAL_SPIRIT_WOLF_BUFF_TIMER  = 29981,
};

struct HELLGROUND_DLL_DECL npc_earthcaller_rygaAI : public npc_escortAI
{
    npc_earthcaller_rygaAI(Creature *c) : npc_escortAI(c) {}

    void Reset() { }

    void WaypointReached(uint32 i)
    {
        switch (i)
        {
            case 1:
                DoScriptText(EMOTE_RYGA, me);
                break;
        }
    }
};

CreatureAI* GetAI_npc_earthcaller_ryga(Creature *_Creature)
{
    CreatureAI* newAI = new npc_earthcaller_rygaAI(_Creature);
    return newAI;
}

struct HELLGROUND_DLL_DECL npc_ancestral_spirit_wolfAI : public npc_escortAI
{
    npc_ancestral_spirit_wolfAI(Creature *c) : npc_escortAI(c) {}

    void Reset()
    {
        if(!me->HasAura(SPELL_ANCESTRAL_SPIRIT_WOLF_BUFF_TIMER, 0))
            me->AddAura(SPELL_ANCESTRAL_SPIRIT_WOLF_BUFF_TIMER, me);
        me->setFaction(7);
        if(Unit* owner = me->GetOwner())
        {
            if(owner->GetTypeId() == TYPEID_PLAYER)
            {
                Start(false, false, owner->GetGUID());
                SetMaxPlayerDistance(40.0f);
            }
        }
    }

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();
        if (!player)
            return;

        switch (i)
        {
            case 2:
                DoScriptText(EMOTE_HEAD_UP, me);
                break;
            case 6:
                SetRun();
                me->SetSpeed(MOVE_RUN, 0.75);
                DoScriptText(EMOTE_HOWL, me);
                break;
            case 36:
                if(Unit* Ryga = FindCreature(17123, 50, me))
                {
                    if (npc_escortAI* pEscortAI = CAST_AI(npc_earthcaller_rygaAI, ((Creature*)Ryga)->AI()))
                        pEscortAI->Start(false, false, 0, 0, true);
                }
                break;
        }
    }

    void EnterCombat(Unit* who) { return; }
};

CreatureAI* GetAI_npc_ancestral_spirit_wolf(Creature *_Creature)
{
    CreatureAI* newAI = new npc_ancestral_spirit_wolfAI(_Creature);
    return newAI;
}

/*######
## npc_living_flare
######*/

enum LivingFlare
{
    NPC_LIVING_FLARE                        = 24916,
    NPC_UNSTABLE_LIVING_FLARE               = 24958,
    NPC_GENERIC_TRIGGER                     = 24959,

    OBJECT_LARGE_FIRE                       = 187084,
    UNSTABLE_LIVING_FLARE_EXPLODE_EMOTE     = -1811010,

    SPELL_FEL_FLAREUP                       = 44944,
    SPELL_LIVING_FLARE_COSMETIC             = 44880,
    SPELL_LIVING_FLARE_MASTER               = 44877,
    SPELL_UNSTABLE_LIVING_FLARE_COSMETIC    = 46196,
    SPELL_LIVING_FLARE_TO_UNSTABLE          = 44943,

    QUEST_BLAST_THE_GATEWAY                 = 11516
};

float FirePos[3][3] = 
{
    {840.9, 2521.0, 293.4},
    {836.5, 2508.0, 292.0},
    {826.5, 2513.4, 291.7}
};

struct HELLGROUND_DLL_DECL npc_living_flareAI : public FollowerAI
{
    npc_living_flareAI(Creature *c) : FollowerAI(c) {}

    void Reset()
    {
        if(Unit* owner = me->GetOwner())
        {
            if(owner->GetTypeId() == TYPEID_PLAYER)
                StartFollow(((Player*)owner));
        }
        if(me->GetEntry() == NPC_UNSTABLE_LIVING_FLARE)
            return;

        if(!me->HasAura(SPELL_LIVING_FLARE_COSMETIC, 0))
            me->AddAura(SPELL_LIVING_FLARE_COSMETIC, me);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if(pWho->GetEntry() == NPC_GENERIC_TRIGGER && me->IsWithinDistInMap(pWho, 10.0f, true))
            Detonate();
    }

    void Detonate()
    {
        if(Unit* owner = me->GetOwner())
        {
            for(uint8 i=0;i<3;++i)
                owner->SummonGameObject(OBJECT_LARGE_FIRE, FirePos[i][0], FirePos[i][1], FirePos[i][2], owner->GetOrientation(), 0, 0, 0, 0, 30);
            if(owner->GetTypeId() == TYPEID_PLAYER)
                if(((Player*)owner)->GetQuestStatus(QUEST_BLAST_THE_GATEWAY) == QUEST_STATUS_INCOMPLETE)
                    ((Player*)owner)->AreaExploredOrEventHappens(QUEST_BLAST_THE_GATEWAY);
        }
        DoCast(me, SPELL_LIVING_FLARE_TO_UNSTABLE);
        DoScriptText(UNSTABLE_LIVING_FLARE_EXPLODE_EMOTE, me);
        me->SetVisibility(VISIBILITY_OFF);
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
    }

    void MorphToUnstable()
    {
        if(me->GetEntry() != NPC_UNSTABLE_LIVING_FLARE)
        {
            DoCast(me, SPELL_FEL_FLAREUP);
            me->UpdateEntry(NPC_UNSTABLE_LIVING_FLARE);
        }
        else
            return;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if(Unit* owner = me->GetOwner())
            me->setFaction(owner->getFaction());
        if(me->HasAura(SPELL_LIVING_FLARE_COSMETIC, 0))
            me->RemoveAurasDueToSpell(SPELL_LIVING_FLARE_COSMETIC);
        DoCast(me, SPELL_LIVING_FLARE_TO_UNSTABLE);
        DoCast(me, SPELL_UNSTABLE_LIVING_FLARE_COSMETIC);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if(spell->Id == SPELL_LIVING_FLARE_MASTER)
        {
            uint32 stack = 0;
            if(me->HasAura(SPELL_FEL_FLAREUP, 0))
            {
                if(Aura* Flare = me->GetAura(SPELL_FEL_FLAREUP, 0))
                    stack = Flare->GetStackAmount();
            }
            if(stack < 7)
                DoCast(me, SPELL_FEL_FLAREUP);
            else
                MorphToUnstable();
        }
    }

    void EnterCombat(Unit* who) { return; }
};

CreatureAI* GetAI_npc_living_flare(Creature *_Creature)
{
    CreatureAI* newAI = new npc_living_flareAI(_Creature);
    return newAI;
}

struct HELLGROUND_DLL_DECL npc_abyssal_shelf_questAI : public ScriptedAI
{
    npc_abyssal_shelf_questAI(Creature* c) : ScriptedAI(c) {}

    void UpdateAI(const uint32 diff)
    {
         if (!UpdateVictim())
             return;

         DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        m_creature->RemoveCorpse();
    }
};

CreatureAI* GetAI_npc_abyssal_shelf_quest(Creature *_Creature)
{
    CreatureAI* newAI = new npc_abyssal_shelf_questAI(_Creature);
    return newAI;
}

struct HELLGROUND_DLL_DECL npc_shattered_hand_berserkerAI : public ScriptedAI
{
    npc_shattered_hand_berserkerAI(Creature* c) : ScriptedAI(c) {}

    uint32 m_enrageTimer;
    uint32 m_chargeTimer;


    void Reset()
    {
        m_enrageTimer = 15000;
        m_chargeTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
         if (!UpdateVictim())
             return;

         if (m_chargeTimer < diff)
         {
             if (me->GetDistance2d(me->getVictim()) > 8.0f)
                 AddSpellToCast(me->getVictim(), 35570);

             m_chargeTimer = 15000;
         }
         else
             m_chargeTimer -= diff;

         if (m_enrageTimer < diff)
         {
             AddSpellToCast(me->getVictim(), 8599);
             m_enrageTimer = 120000;
         }
         else
             m_enrageTimer -= diff;

         CastNextSpellIfAnyAndReady();
         DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (GetClosestCreatureWithEntry(me, 22444, 25.0f))
        {
            if (Creature *pSpirit = me->SummonCreature(22454, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                pSpirit->AI()->AttackStart(pKiller);
        }
    }
};

CreatureAI* GetAI_npc_shattered_hand_berserker(Creature *_Creature)
{
    return new npc_shattered_hand_berserkerAI(_Creature);
}

/*######
## npc_felblood_initiate & npc_emaciated_felblood
######*/

#define SPELL_FELBLOOD_CHANNEL      44864
#define SPELL_BITTER_WITHDRAWAL     29098
#define SPELL_SINISTER_STRIKE       14873
#define SPELL_SPELLBREAKER          35871
#define SPELL_FEL_SIPHON_QUEST      44936
#define SPELL_SELF_STUN             45066

#define MOB_EMACIATED_FELBLOOD      24955

const char* YellChange[3] = 
{
    "No... no... NO!!!!!",
    "My power... is gone!",
    "What have you done!"
};
const char* YellSiphon[4] = 
{
    "More... more... MORE!!!",
    "I will soon be stronger than any elf! I will serve at Kil'jaeden's side!",
    "Unparalleled power... I... crave... more!",
    "Your life force is my nourishment, demon... Kil'jaeden's gift to us!"
};

struct HELLGROUND_DLL_DECL npc_felblood_initiateAI : public ScriptedAI
{
    npc_felblood_initiateAI(Creature *c) : ScriptedAI(c) { }

    uint32 Spellbreaker;
    uint32 ChangeTimer;
    uint32 OOCTimer;
    uint32 BitterWithdrawal;
    uint32 SinisterStrike;

    void Reset()
    {
        Spellbreaker = urand(6000, 10000);
        ChangeTimer = 0;
        OOCTimer = 5000;
        BitterWithdrawal = urand(10000, 15000);
        SinisterStrike = urand(5000, 15000);
    }

    void HandleOffCombatEffects()
    {
        DoCast((Unit*)NULL, SPELL_FELBLOOD_CHANNEL);
    }

    void EnterCombat(Unit* who)
    {
        if(me->IsNonMeleeSpellCasted(false))
            me->InterruptNonMeleeSpells(false);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if(spell->Id == SPELL_FEL_SIPHON_QUEST)
        {
            DoCast(me, SPELL_SELF_STUN);
            ChangeTimer = 3000;
        }
    }

    void UpdateAI(const uint32 diff)
    {
      if(!me->isInCombat())
      {
          if(OOCTimer < diff)
          {
              if(!me->IsNonMeleeSpellCasted(false))
                  HandleOffCombatEffects();
              if(roll_chance_i(3))
                  me->Yell(YellSiphon[urand(0,3)], 0, 0);
              OOCTimer = 60000;
          }
          else
              OOCTimer -= diff;
      }

      if(!UpdateVictim())
          return;

      if(ChangeTimer)
      {
          if(ChangeTimer <= diff)
          {
              me->UpdateEntry(MOB_EMACIATED_FELBLOOD);
              me->Yell(YellChange[urand(0,2)], 0, 0);
              me->RemoveAurasDueToSpell(SPELL_SELF_STUN);
              me->AI()->AttackStart(me->getVictim());
              ChangeTimer = 0;
          }
          else ChangeTimer -= diff;
      }

      if(me->GetEntry() == MOB_EMACIATED_FELBLOOD)
      {
          if(BitterWithdrawal < diff)
          {
              AddSpellToCast(SPELL_BITTER_WITHDRAWAL);
              BitterWithdrawal = urand(12000, 18000);
          }
          else
              BitterWithdrawal -= diff;

           if(SinisterStrike < diff)
           {
               AddSpellToCast(SPELL_SINISTER_STRIKE);
               SinisterStrike = urand(10000, 15000);
           }
           else
                SinisterStrike -= diff;
      }
      else
      {
          if(Spellbreaker < diff)
          {
              AddSpellToCast(SPELL_SPELLBREAKER);
              Spellbreaker = urand(8000, 12000);
          }
          else
                Spellbreaker -= diff;
      }

       CastNextSpellIfAnyAndReady();
       DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_felblood_initiate(Creature *_Creature)
{
    CreatureAI* newAI = new npc_felblood_initiateAI(_Creature);
    return newAI;
}

///////
/// Ice Stone
///////

#define GOSSIP_ICE_STONE        "Place your hands on stone"

#define NPC_GLACIAL_TEMPLAR         26216

bool GossipHello_go_ice_stone(Player *player, GameObject* _GO)
{
    if( player->GetQuestStatus(11954) == QUEST_STATUS_INCOMPLETE )
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ICE_STONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    }
    player->SEND_GOSSIP_MENU(_GO->GetGOInfo()->questgiver.gossipID, _GO->GetGUID());
    return true;
}

void SendActionMenu_go_ice_stone(Player *player, GameObject* _GO, uint32 action)
{
    _GO->SetGoState(GO_STATE_ACTIVE);
    _GO->SetRespawnTime(300);
    player->CLOSE_GOSSIP_MENU();

    float x,y,z;
    player->GetClosePoint(x,y,z, 0.0f, 2.0f, frand(0, M_PI));

    switch(action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->SummonCreature(NPC_GLACIAL_TEMPLAR, x,y,z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 600000);
        break;
    }
}

bool GossipSelect_go_ice_stone(Player *player, GameObject* _GO, uint32 sender, uint32 action )
{
    switch(sender)
    {
        case GOSSIP_SENDER_MAIN:    SendActionMenu_go_ice_stone(player, _GO, action); break;
    }
    return true;
}

/*######
## npc_hand_berserker
######*/

enum
{
    SPELL_SOUL_BURDEN       = 38879,
    SPELL_ENRAGE            = 8599,
    SPELL_CHARGE            = 35570,

    NPC_BUNNY               = 22444
};

struct HELLGROUND_DLL_DECL npc_hand_berserkerAI : public ScriptedAI
{
    npc_hand_berserkerAI(Creature* c) : ScriptedAI(c) {}

    void Reset() {}

    void AttackStart(Unit *pWho)
    {
        ScriptedAI::AttackStart(pWho);
    }

    void EnterCombat(Unit* pWho)
    {
        if (rand()%60)
        {
            DoCast(pWho, SPELL_CHARGE);
        }
    }   

    void DamageTaken(Unit* pDoneby, uint32 & Damage)
    {
        if (m_creature->HasAura(SPELL_ENRAGE))
            return;

        if (pDoneby->GetTypeId() == TYPEID_PLAYER && (m_creature->GetHealth()*100 - Damage) / m_creature->GetMaxHealth() < 30)
        {
            DoCast(m_creature, SPELL_ENRAGE);
        }
    }

    void JustDied(Unit* pWho)
    {
        if (Creature* Bunny = GetClosestCreatureWithEntry(m_creature, NPC_BUNNY, 17.5f))
        {
            Bunny->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoCast(Bunny, SPELL_SOUL_BURDEN);
        }
    }
};

CreatureAI* GetAI_npc_hand_berserker(Creature* c)
{
    return new npc_hand_berserkerAI(c);
}

/*######
## npc_anchorite_relic_bunny
######*/

enum
{
    NPC_HAND_BERSERKER      = 16878,
    NPC_FEL_SPIRIT          = 22454,
    SPELL_CHANNELS          = 39184,

    GO_RELIC                = 185298,
    SAY_SP                  = -1900130
};

struct HELLGROUND_DLL_DECL npc_anchorite_relic_bunnyAI : public ScriptedAI
{
    npc_anchorite_relic_bunnyAI(Creature* c) : ScriptedAI(c) {}

    uint32 m_ChTimer;
    uint32 m_EndTimer;

    void Reset()
    {
        m_ChTimer = 2000;
        m_EndTimer = 60000;
    }

    void AttackedBy(Unit* pEnemy) {}
    void AttackStart(Unit* pEnemy) {}

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_FEL_SPIRIT)
        {
            DoScriptText(SAY_SP, pSummoned);
            pSummoned->AI()->AttackStart(pSummoned->getVictim());
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_SOUL_BURDEN)
        {
            me->InterruptNonMeleeSpells(false);
            m_creature->SummonCreature(NPC_FEL_SPIRIT, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_ChTimer = 2000;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_ChTimer <= diff)
        {
            if (Creature* pBer = GetClosestCreatureWithEntry(m_creature, NPC_HAND_BERSERKER, 17.5f, true))
            {
                {
                    DoCast(pBer, SPELL_CHANNELS, false);
                    m_ChTimer = 95000;
                }
            }
            else m_creature->InterruptNonMeleeSpells(false);
        }
        else m_ChTimer -= diff;

        if (m_EndTimer <= diff)
        {
            if (GameObject* pRelic = GetClosestGameObjectWithEntry(m_creature, GO_RELIC, 5.0f))
            {
                pRelic->RemoveFromWorld();
                m_creature->setDeathState(JUST_DIED);
                m_creature->RemoveCorpse();
            }

            m_EndTimer = 60000;
        }
        else m_EndTimer -= diff;
    }
};

CreatureAI* GetAI_npc_anchorite_relic_bunny(Creature* c)
{
    return new npc_anchorite_relic_bunnyAI(c);
}

/*######
## npc_anchorite_barada
######*/

#define GOSSIP_ITEM_START      "I am ready Amchorite.Let us begin the exorcim."
#define SAY_BARADA1            -1900100
#define SAY_BARADA2            -1900101
#define SAY_BARADA3            -1900104
#define SAY_BARADA4            -1900105
#define SAY_BARADA5            -1900106
#define SAY_BARADA6            -1900107
#define SAY_BARADA7            -1900108
#define SAY_BARADA8            -1900109
#define SAY_COLONEL1           -1900110
#define SAY_COLONEL2           -1900111
#define SAY_COLONEL3           -1900112
#define SAY_COLONEL4           -1900113
#define SAY_COLONEL5           -1900114
#define SAY_COLONEL6           -1900115
#define SAY_COLONEL7           -1900116
#define SAY_COLONEL8           -1900117

enum
{
    QUEST_THE_EXORCIM          = 10935,
    NPC_COLONEL_JULES          = 22432,
    NPC_DARKNESS_RELEASED      = 22507,

    SPELL_EXORCIM              = 39277,
    SPELL_EXORCIM2             = 39278,
    SPELL_COLONEL1             = 39283,         
    SPELL_COLONEL2             = 39294,
    SPELL_COLONEL3             = 39284,
    SPELL_COLONEL4             = 39294,
    SPELL_COLONEL5             = 39295,
    SPELL_COLONEL7             = 39381,
    SPELL_COLONEL8             = 39380
};

struct Points
{
    float x, y, z;
};

static Points P[]=
{
    {-710.111f, 2754.346f, 102.367f},
    {-710.611f, 2753.435f, 103.774f},
    {-707.784f, 2749.562f, 103.774f},
    {-708.558f, 2744.923f, 103.774f},
    {-713.406f, 2744.240f, 103.774f},
    {-714.212f, 2750.606f, 103.774f},
    {-710.792f, 2750.693f, 103.774f},

    {-707.702f, 2749.038f, 101.590f},
    {-710.810f, 2748.376f, 101.590f},
    {-706.726f, 2751.632f, 101.591f},
    {-707.382f, 2753.994f, 101.591f},

    {-710.924f, 2754.683f, 105.0f}
};

struct HELLGROUND_DLL_DECL npc_anchorite_baradaAI : public ScriptedAI
{
    npc_anchorite_baradaAI(Creature* c) : ScriptedAI(c) {}

    bool Exorcim;

    uint32 m_StepsTimer;
    uint32 m_Steps;

    void Reset()
    {
        Exorcim = false;
        m_StepsTimer = 0;
        m_Steps = 0;
    }

    void AttackedBy(Unit* pWho) {}
    void AttackStart(Unit* pWho) {}

    void DoSpawnDarkness()
    {
        m_creature->SummonCreature(NPC_DARKNESS_RELEASED, P[11].x, P[11].y, P[11].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
    }

    uint32 NextStep(uint32 m_Steps)
    {
        Creature* pColonel = GetClosestCreatureWithEntry(m_creature, NPC_COLONEL_JULES, 15);

        switch(m_Steps)
        {
            case 1:m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                   m_creature->SetStandState(UNIT_STAND_STATE_STAND);return 2000;
            case 2:DoScriptText(SAY_BARADA1, m_creature,0);return 5000;
            case 3:DoScriptText(SAY_BARADA2, m_creature,0);return 3000;
            case 4:DoScriptText(SAY_COLONEL1, pColonel, 0);return 3000;
            case 5:m_creature->SetWalk(true);;return 3000;
            case 6:m_creature->GetMotionMaster()->MovePoint(0, P[7].x, P[7].y, P[7].z);return 2000;
            case 7:m_creature->GetMotionMaster()->MovePoint(0, P[8].x, P[8].y, P[8].z);return 2100;
            case 8:m_creature->SetFacingToObject(pColonel);return 2000;
            case 9:m_creature->CastSpell(m_creature, SPELL_EXORCIM , false);return 10000;
            case 10:DoScriptText(SAY_BARADA3, m_creature,0); return 10000;
            case 11:DoScriptText(SAY_COLONEL2, pColonel, 0);return 8000;
            case 12:m_creature->RemoveAllAuras();
            case 13:m_creature->CastSpell(m_creature, SPELL_EXORCIM2 , false);
            case 14:pColonel->CastSpell(pColonel, SPELL_COLONEL1, false);
            case 15:pColonel->SetSpeed(MOVE_FLIGHT, 0.15f);
                    pColonel->SetLevitate(true);
                    pColonel->GetMotionMaster()->MovePoint(0, P[1].x, P[1].y, P[1].z);
                    pColonel->CastSpell(pColonel, SPELL_COLONEL3, false);return 14000;
            case 16:DoScriptText(SAY_COLONEL3, pColonel, 0);
                    DoSpawnDarkness();
                    DoSpawnDarkness();return 14000;
            case 17:DoScriptText(SAY_BARADA4, m_creature, 0);
                    DoSpawnDarkness();
                    DoSpawnDarkness();return 14000;
            case 18:DoScriptText(SAY_COLONEL4, pColonel, 0);
                    DoSpawnDarkness();return 14000;
            case 19:DoScriptText(SAY_BARADA5, m_creature, 0); return 14000;
            case 20:pColonel->CastSpell(pColonel, SPELL_COLONEL4, false);
                    pColonel->CastSpell(pColonel, SPELL_COLONEL2, false);
                    DoSpawnDarkness();return 1500;
            case 21:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);return 7000;
            case 22:DoScriptText(SAY_COLONEL5, pColonel, 0);return 1000;
            case 23:pColonel->GetMotionMaster()->MovePoint(0, P[2].x, P[2].y, P[2].z);
                    DoSpawnDarkness();return 5000;
            case 24:pColonel->GetMotionMaster()->MovePoint(0, P[3].x, P[3].y, P[3].z);
                    pColonel->CastSpell(m_creature,SPELL_COLONEL5, false);return 3500;
            case 25:DoScriptText(SAY_BARADA6, m_creature, 0);
            case 26:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);
                    DoSpawnDarkness();return 2000;
            case 27:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[4].z);return 4000;
            case 28:pColonel->GetMotionMaster()->MovePoint(0, P[2].x, P[2].y, P[2].z);
                    DoScriptText(SAY_COLONEL6, pColonel, 0);
                    DoSpawnDarkness();return 4000;
            case 29:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[5].z);return 4000;
            case 30:pColonel->GetMotionMaster()->MovePoint(0, P[2].x, P[2].y, P[2].z);return 4000;
            case 31: DoScriptText(SAY_BARADA7, m_creature, 0); return 0;
            case 32:pColonel->GetMotionMaster()->MovePoint(0, P[3].x, P[3].y, P[3].z);
                    DoSpawnDarkness();return 4000;
            case 33:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);return 4000;
            case 34:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[5].z);
                    DoScriptText(SAY_COLONEL7, pColonel, 0);
                    DoSpawnDarkness();return 4000;
            case 35:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);return 4000;
            case 36:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[5].z);
                    DoSpawnDarkness();return 4000;
            case 37:DoScriptText(SAY_BARADA6, m_creature, 0);
            case 38:pColonel->GetMotionMaster()->MovePoint(0, P[2].x, P[2].y, P[2].z);return 2500;
            case 39:pColonel->GetMotionMaster()->MovePoint(0, P[3].x, P[3].y, P[3].z);return 4000;
            case 40:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);
                    DoScriptText(SAY_COLONEL8, pColonel, 0);return 4000;
            case 41:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[5].z);return 4000;
            case 42:pColonel->GetMotionMaster()->MovePoint(0, P[2].x, P[2].y, P[2].z);return 4000;
            case 43:DoScriptText(SAY_BARADA6, m_creature, 0); return 1000;
            case 44:pColonel->GetMotionMaster()->MovePoint(0, P[3].x, P[3].y, P[3].z);return 4000;
            case 45:pColonel->GetMotionMaster()->MovePoint(0, P[4].x, P[4].y, P[4].z);
                    pColonel->CastSpell(pColonel, SPELL_COLONEL8, false);return 4000;
            case 46:pColonel->GetMotionMaster()->MovePoint(0, P[5].x, P[5].y, P[5].z);
                    pColonel->CastSpell(pColonel, SPELL_COLONEL7, false);return 4000;
            case 47:pColonel->GetMotionMaster()->MovePoint(0, P[6].x, P[6].y, P[6].z);return 5000;
            case 48:DoScriptText(SAY_BARADA8, m_creature, 0); return 1000;
            case 49:pColonel->GetMotionMaster()->MovePoint(0, P[0].x, P[0].y, P[0].z);return 3000;
            case 50:pColonel->RemoveAllAuras();
            case 51:m_creature->RemoveAllAuras();return 2000;
            case 52:m_creature->SetWalk(true);return 2000;
            case 53:m_creature->GetMotionMaster()->MovePoint(0, P[9].x, P[9].y, P[9].z);return 2200;
            case 54:m_creature->GetMotionMaster()->MovePoint(0, P[10].x, P[10].y, P[10].z);return 7000;
            case 55:m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    m_creature->CombatStop();return 3000;
            case 56:pColonel->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);return 20000;
            case 57:m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            case 58:Reset();
        default: return 0;
        }
    }

    void JustDied(Unit* pWho)
    {
        if (Creature* pColonel = GetClosestCreatureWithEntry(m_creature, NPC_COLONEL_JULES, 15.0f))
        {
            pColonel->GetMotionMaster()->MovePoint(0, P[0].x, P[0].y, P[0].z);
            pColonel->RemoveAllAuras();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_StepsTimer <= diff)
        {
            if (Exorcim)
                m_StepsTimer = NextStep(++m_Steps);
        }
        else m_StepsTimer -= diff;
    }
};

CreatureAI* GetAI_npc_anchorite_barada(Creature* c)
{
    return new npc_anchorite_baradaAI(c);
}

bool GossipHello_npc_anchorite_barada(Player *pPlayer, Creature *c)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_EXORCIM) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(c->GetNpcTextId(), c->GetGUID());
    return true;
}

bool GossipSelect_npc_anchorite_barada(Player* pPlayer, Creature* c, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        ((npc_anchorite_baradaAI*)c->AI())->Exorcim = true;
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## npc_darkness_released
######*/

enum
{
    SPELL_AURA_ME     = 39303,
    SPELL_DARKNESS    = 39307,
    NPC_BARADA        = 22431
};

struct Move
{
    float x, y, z;
};

static Move M[]=
{
    {-714.212f, 2750.606f, 105.0f},
    {-713.406f, 2744.240f, 105.0f},
    {-707.784f, 2749.562f, 105.0f},
    {-708.558f, 2744.923f, 105.0f}
};

struct HELLGROUND_DLL_DECL npc_darkness_releasedAI : public ScriptedAI
{
    npc_darkness_releasedAI(Creature* c) : ScriptedAI(c) 
	{
        m_ChTimer = 5000;
        m_AtTimer = 10000;
        DoCast(m_creature, SPELL_AURA_ME);
        m_creature->SetLevitate(true);
        m_creature->SetSpeed(MOVE_FLIGHT, 0.08f);
        switch(urand(0,3))
        {
            case 0: m_creature->GetMotionMaster()->MovePoint(0, M[0].x, M[0].y, M[0].z); break;
            case 1: m_creature->GetMotionMaster()->MovePoint(0, M[1].x, M[1].y, M[1].z); break;
            case 2: m_creature->GetMotionMaster()->MovePoint(0, M[2].x, M[2].y, M[2].z); break;
            case 3: m_creature->GetMotionMaster()->MovePoint(0, M[3].x, M[3].y, M[3].z); break;
        }
    }

    uint32 m_AtTimer;
    uint32 m_ChTimer;

    void Reset() { }

    void AttackedBy(Unit* pWho) {}
    void AttackStart(Unit* pWho) {}

    void JustDied(Unit* pWho)
    {
        m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_AtTimer <= diff)
        {
            DoCast(m_creature, SPELL_DARKNESS);
            switch (urand(0,3))
            {
                case 0: m_creature->GetMotionMaster()->MovePoint(0, M[0].x, M[0].y, M[0].z); break;
                case 1: m_creature->GetMotionMaster()->MovePoint(0, M[1].x, M[1].y, M[1].z); break;
                case 2: m_creature->GetMotionMaster()->MovePoint(0, M[2].x, M[2].y, M[2].z); break;
                case 3: m_creature->GetMotionMaster()->MovePoint(0, M[3].x, M[3].y, M[3].z); break;
            }

        m_AtTimer = 10000;
        }
        else m_AtTimer -= diff;

        if (m_ChTimer <= diff)
        {
            if (Creature* pBar = GetClosestCreatureWithEntry(m_creature, NPC_BARADA, 15.0f, false))
            {
                m_creature->setDeathState(CORPSE);
            }

            if (Creature* pBara = GetClosestCreatureWithEntry(m_creature, NPC_BARADA, 15.0f))
            {
                if (!pBara->HasAura(SPELL_EXORCIM2))
                    m_creature->setDeathState(CORPSE);
            }

            m_ChTimer = 5000;
        }
        else m_ChTimer -= diff;
    }
};

CreatureAI* GetAI_npc_darkness_released(Creature* c)
{
    return new npc_darkness_releasedAI(c);
}

/*######
## npc_foul_purge
######*/

struct HELLGROUND_DLL_DECL npc_foul_purgeAI : public ScriptedAI
{
    npc_foul_purgeAI(Creature* c) : ScriptedAI(c) 
    {
        if (Creature* pBara = GetClosestCreatureWithEntry(m_creature, NPC_BARADA, 15.0f))
        {
            m_creature->GetMotionMaster()->MovePoint(0, pBara->GetPositionX(), pBara->GetPositionY(), pBara->GetPositionZ());
            AttackStart(pBara);
        }
        m_ChTimer = 4000;
    }

    uint32 m_ChTimer;

    void Reset() { }

    void JustDied(Unit* pWho)
    {
        m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_ChTimer <= diff)
        {
            if (Creature* pBar = GetClosestCreatureWithEntry(m_creature, NPC_BARADA, 15.0f, false))
            {
                m_creature->setDeathState(CORPSE);
            }

            if (Creature* pBara = GetClosestCreatureWithEntry(m_creature, NPC_BARADA, 15.0f))
            {
                if (!pBara->HasAura(SPELL_EXORCIM2))
                    m_creature->setDeathState(CORPSE);
            }

            m_ChTimer = 4000;
        }
        else m_ChTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_foul_purge(Creature* c)
{
    return new npc_foul_purgeAI(c);
}

/*######
## npc_sedai_quest_credit_marker
######*/

enum
{
    NPC_ESCORT1    = 17417,
    NPC_SEDAI      = 17404
};

struct HELLGROUND_DLL_DECL npc_sedai_quest_credit_markerAI : public ScriptedAI
{
    npc_sedai_quest_credit_markerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    void Reset()
    {
        DoSpawn();
    }

    void DoSpawn()
    {
        m_creature->SummonCreature(NPC_SEDAI, 225.908, 4124.034, 82.505, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 100000);
        m_creature->SummonCreature(NPC_ESCORT1, 229.257, 4125.271, 83.388, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ESCORT1)
        {
            pSummoned->SetWalk(true);
            pSummoned->GetMotionMaster()->MovePoint(0, 208.029f, 4134.618f, 77.763f);
        }
    }
};

CreatureAI* GetAI_npc_sedai_quest_credit_marker(Creature* pCreature)
{
    return new npc_sedai_quest_credit_markerAI(pCreature);
}

/*######
## npc_vindicator_sedai
######*/

#define SAY_MAG_ESSCORT    -1900125
#define SAY_SEDAI1         -1900126
#define SAY_SEDAI2         -1900127
#define SAY_KRUN           -1900128

enum
{
    NPC_ESCORT        = 17417,
    NPC_AMBUSHER      = 17418,
    NPC_KRUN          = 17405,

    SPELL_STUN        = 13005,
    SPELL_HOLYFIRE    = 17141
};

struct HELLGROUND_DLL_DECL npc_vindicator_sedaiAI : public ScriptedAI
{
    npc_vindicator_sedaiAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Vision;

    ObjectGuid m_PlayerGUID;
    uint32 m_StepsTimer;
    uint32 m_Steps;

    void Reset()
    {
        Vision = true;
        m_StepsTimer =0;
        m_Steps = 0;
        m_creature->SetWalk(true);
    }

    void DoSpawnEscort()
    {
        m_creature->SummonCreature(NPC_ESCORT, 227.188f, 4121.116f, 82.745f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
    }

    void DoSpawnAmbusher()
    {
        m_creature->SummonCreature(NPC_AMBUSHER, 223.408f, 4120.086f, 81.843f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);
    }

    void DoSpawnKrun()
    {
        m_creature->SummonCreature(NPC_KRUN, 192.872f, 4129.417f, 73.655f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 8000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ESCORT)
        {
            pSummoned->SetWalk(true);
            pSummoned->GetMotionMaster()->MovePoint(0, 205.660f, 4130.663f, 77.175f);
        }

        if (pSummoned->GetEntry() == NPC_AMBUSHER)
        {
            Creature* pEscort = GetClosestCreatureWithEntry(pSummoned, NPC_ESCORT, 15);
            pSummoned->AI()->AttackStart(pEscort);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_KRUN)
            {
                pSummoned->SetWalk(false);
                pSummoned->GetMotionMaster()->MovePoint(0, 194.739868f, 4143.145996f, 73.798088f);
                DoScriptText(SAY_KRUN, pSummoned,0);
                pSummoned->AI()->AttackStart(m_creature);
            }
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (((Player*)pWho)->GetQuestStatus(9545) == QUEST_STATUS_INCOMPLETE)
            {
                if (m_creature->IsWithinDistInMap(((Player *)pWho), 10))
                {
                    m_PlayerGUID = pWho->GetObjectGuid();
                }
            }
        }
    }

    uint32 NextStep(uint32 m_Steps)
    {
        switch(m_Steps)
        {
            case 1:DoSpawnEscort();
            case 2:m_creature->GetMotionMaster()->MovePoint(0, 204.877f, 4133.172f, 76.897f);return 2900;
            case 3:if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                   {
                       DoScriptText(SAY_MAG_ESSCORT, pEsc,0);
                   };
                   return 1000;
            case 4:if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                   {
                       pEsc->GetMotionMaster()->MovePoint(0, 229.257f, 4125.271f, 83.388f);
                   };
                   return 1500;
            case 5:if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                   {
                       pEsc->GetMotionMaster()->MovePoint(0, 227.188f, 4121.116f, 82.745f);
                   };
                   return 1000;
            case 6:DoScriptText(SAY_SEDAI1, m_creature,0);return 1000;
            case 7:DoSpawnAmbusher();return 3000;
            case 8:DoSpawnAmbusher();return 1000;
            case 9:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                   {
                       m_creature->AI()->AttackStart(pAmb);
                   };
                   return 2000;
            case 10:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        m_creature->CastSpell(pAmb, SPELL_STUN , false);
                    };
                    return 2000;
            case 11:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        pAmb->setDeathState(JUST_DIED);
                    };
                    return 1500;
            case 12:if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                    {
                        pEsc->setDeathState(JUST_DIED);
                    };
            case 13:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        m_creature->AI()->AttackStart(pAmb);
                    };
            case 14:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                        {
                            pEsc->AI()->AttackStart(pAmb);
                        }
                    };
                    return 1000;
            case 15:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        m_creature->CastSpell(pAmb, SPELL_HOLYFIRE , false);
                    };
                    return 6000;
            case 16:if (Creature* pAmb = GetClosestCreatureWithEntry(m_creature, NPC_AMBUSHER, 35))
                    {
                        pAmb->setDeathState(JUST_DIED);
                    };
                    return 1000;
            case 17:if (Creature* pEsc = GetClosestCreatureWithEntry(m_creature, NPC_ESCORT, 20))
                    {
                        pEsc->GetMotionMaster()->MovePoint(0, 235.063f, 4117.826f, 84.471f);
                    };
                    return 1000;
            case 18:m_creature->SetWalk(true);
                    m_creature->GetMotionMaster()->MovePoint(0, 199.706f, 4134.302f, 75.404f);return 6000;       
            case 19:m_creature->GetMotionMaster()->MovePoint(0, 193.524f, 4147.451f, 73.605f);return 7000;              
            case 21:m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_SEDAI2, m_creature,0);return 5000;
            case 22:DoSpawnKrun();return 1000;
            case 23:if (Creature* pKrun = GetClosestCreatureWithEntry(m_creature, NPC_KRUN, 20))
                    {
                        m_creature->CastSpell(pKrun, SPELL_HOLYFIRE, false);
                    };
                    return 3000;
            case 24:if (Creature * pCr = GetClosestCreatureWithEntry(m_creature, 17413, 6.0f))
                    {
                        if (Player* pPlayer = m_creature->GetPlayer(m_PlayerGUID))
                        {
                            float Radius = 10.0f;
                            if (m_creature->IsWithinDistInMap(pPlayer, Radius))
                            {
                                ((Player*)pPlayer)->KilledMonster(17413, pCr->GetObjectGuid());
                            }
                        }
                    };
                    return 1500;
            case 25:m_creature->setDeathState(JUST_DIED);
        default: return 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {

        if (m_StepsTimer <= diff)
        {
            if (Vision)
                m_StepsTimer = NextStep(++m_Steps);
        }
        else m_StepsTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_vindicator_sedai(Creature* pCreature)
{
    return new npc_vindicator_sedaiAI(pCreature);
}

/*######
## npc_pathaleon_image
######*/

enum
{
    SAY_PATHALEON1         = -1900165,
    SAY_PATHALEON2         = -1900166,
    SAY_PATHALEON3         = -1900167,
    SAY_PATHALEON4         = -1900168,

    SPELL_ROOTS            = 35468,
    SPELL_INSECT           = 35471,
    SPELL_LIGHTING         = 35487,
    SPELL_TELE             = 7741,

    NPC_TARGET_TRIGGER     = 20781,
    NPC_CRYSTAL_TRIGGER    = 20617,
    NPC_GOLIATHON          = 19305,
};

struct Pos
{
    float x, y, z;
};

static Pos S[]=
{
    {113.29f, 4858.19f, 74.37f},
    {81.20f, 4806.26f, 51.75f},
    {106.21f, 4834.39f, 79.56f},
    {124.98f, 4813.17f, 79.66f},
    {124.01f, 4778.61f, 77.86f},
    {46.37f, 4795.72f, 66.73f},
    {60.14f, 4830.46f, 77.83f}
};

struct HELLGROUND_DLL_DECL npc_pathaleon_imageAI : public ScriptedAI
{
    npc_pathaleon_imageAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Event;
    bool SummonTrigger;

    uint32 m_SumTimer;
    uint32 m_StepsTimer;
    uint32 m_Steps;

    void Reset()
	{
        m_SumTimer = 5000;
        m_StepsTimer = 0;
        m_Steps = 0;
        Event = true;
        SummonTrigger = false;
	}

    void DoSpawnGoliathon()
    {
        m_creature->SummonCreature(NPC_GOLIATHON, S[0].x, S[0].y, S[0].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
    }

    void DoSpawnTrigger()
    {
        m_creature->SummonCreature(NPC_TARGET_TRIGGER, S[1].x, S[1].y, S[1].z, 2.25f, TEMPSUMMON_TIMED_DESPAWN, 120000);
    }

    void DoSpawnCtrigger()
    {
        m_creature->SummonCreature(NPC_CRYSTAL_TRIGGER, S[2].x, S[2].y, S[2].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        m_creature->SummonCreature(NPC_CRYSTAL_TRIGGER, S[3].x, S[3].y, S[3].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        m_creature->SummonCreature(NPC_CRYSTAL_TRIGGER, S[4].x, S[4].y, S[4].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        m_creature->SummonCreature(NPC_CRYSTAL_TRIGGER, S[5].x, S[5].y, S[5].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        m_creature->SummonCreature(NPC_CRYSTAL_TRIGGER, S[6].x, S[6].y, S[6].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_GOLIATHON)
        {
            pSummoned->CastSpell(pSummoned, SPELL_TELE, false);
            pSummoned->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
        }
        if (pSummoned->GetEntry() == NPC_CRYSTAL_TRIGGER)
        {
            pSummoned->CastSpell(pSummoned, SPELL_INSECT, false);
            pSummoned->CastSpell(pSummoned, SPELL_LIGHTING, false);
        }
        else
        {
            if (pSummoned->GetEntry() == NPC_TARGET_TRIGGER)
            {
                pSummoned->CastSpell(pSummoned, SPELL_ROOTS, false);
            }
        }
    }

    int32 NextStep(uint32 m_Steps)
    {              
        switch (m_Steps)
        {
            case 1:
                return 10000;
            case 2:
                DoSpawnTrigger();
                SummonTrigger = true;
                return 2000;
            case 3:
                DoScriptText(SAY_PATHALEON1, m_creature, 0);
                return 15000;
            case 4:
                DoScriptText(SAY_PATHALEON2, m_creature, 0);
                return 15000;
            case 5:
                DoScriptText(SAY_PATHALEON3, m_creature, 0);
                return 15000;
            case 6:
                DoScriptText(SAY_PATHALEON4, m_creature, 0);
                return 5000;
            case 7:
                DoSpawnGoliathon();
                return 1000;
            case 8:
                DoCast(m_creature, SPELL_TELE);
                return 600;
            case 9:
                m_creature->SetVisibility(VISIBILITY_OFF);
                return 60000;
            case 10:
                m_creature->setDeathState(CORPSE);
            default: return 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_StepsTimer <= diff)
        {
            if (Event)
                m_StepsTimer = NextStep(++m_Steps);
        }
        else m_StepsTimer -= diff;

        if (SummonTrigger)
        {
            if (m_SumTimer <= diff)
            {
                DoSpawnCtrigger();
                m_SumTimer = 5000;
            }
            else m_SumTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_pathaleon_image(Creature* pCreature)
{
    return new npc_pathaleon_imageAI(pCreature);
}

/*######
## npc_viera
######*/

#define SAY_VIERA1                       -1900172
#define SAY_VIERA2                       -1900173

#define QUEST_LIVE_IS_FINER_PLEASURES    9483

#define NPC_CAT                          17230

struct HELLGROUND_DLL_DECL npc_vieraAI : public npc_escortAI
{
    npc_vieraAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint32 m_EndsTimer;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 0:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoSpawnCreature(NPC_CAT, 5, 5, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 85000);
                break;
            case 9:
                m_creature->SetFacingToObject(pPlayer);
                DoScriptText(SAY_VIERA1, m_creature, pPlayer);
                m_creature->SetStandState(UNIT_STAND_STATE_SIT);
                m_EndsTimer = 40000;
                SetEscortPaused(true);
                SetRun();
                break;
            case 10:
                if (Creature* pCat = GetClosestCreatureWithEntry(m_creature, NPC_CAT, 20))
                {
                    pCat->ForcedDespawn();
                }
                break;
        }
    }

    void Reset()
    {
        m_EndsTimer = 0;
    }

    void EnterCombat(Unit* who) {}

    void JustSummoned(Creature* summoned)
    {
        summoned->GetMotionMaster()->MoveFollow(m_creature, PET_FOLLOW_DIST,  summoned->GetFollowAngle());
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if(spell->Id == 30077)
        {
            DoScriptText(SAY_VIERA2, m_creature, 0);
            SetEscortPaused(false);
        }
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (m_EndsTimer <= diff)
        {
            SetEscortPaused(false);
        }
        else m_EndsTimer -= diff;
    }
};

bool QuestRewarded_npc_viera(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_LIVE_IS_FINER_PLEASURES)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_vieraAI, creature->AI()))
            pEscortAI->Start(false, false, player->GetGUID(), quest);
    }
    return true;
}

CreatureAI* GetAI_npc_viera(Creature* pCreature)
{
    return new npc_vieraAI(pCreature);
}

void AddSC_hellfire_peninsula()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_abyssal_shelf_quest";
    newscript->GetAI = &GetAI_npc_abyssal_shelf_quest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_aeranas";
    newscript->GetAI = &GetAI_npc_aeranas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_haaleshi_altar";
    newscript->pGOUse = &GOUse_go_haaleshi_altar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_wing_commander_dabiree";
    newscript->pGossipHello =   &GossipHello_npc_wing_commander_dabiree;
    newscript->pGossipSelect =  &GossipSelect_npc_wing_commander_dabiree;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_gryphoneer_windbellow";
    newscript->pGossipHello =   &GossipHello_npc_gryphoneer_windbellow;
    newscript->pGossipSelect =  &GossipSelect_npc_gryphoneer_windbellow;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_wing_commander_brack";
    newscript->pGossipHello =   &GossipHello_npc_wing_commander_brack;
    newscript->pGossipSelect =  &GossipSelect_npc_wing_commander_brack;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_wounded_blood_elf";
    newscript->GetAI = &GetAI_npc_wounded_blood_elf;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_wounded_blood_elf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_demoniac_scryer";
    newscript->pGossipHello =  &GossipHello_npc_demoniac_scryer;
    newscript->pGossipSelect = &GossipSelect_npc_demoniac_scryer;
    newscript->GetAI = &GetAI_npc_demoniac_scryer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_magic_sucker_device_spawner";
    newscript->GetAI = &GetAI_npc_magic_sucker_device_spawner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_ancestral_spirit_wolf";
    newscript->GetAI = &GetAI_npc_ancestral_spirit_wolf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_earthcaller_ryga";
    newscript->GetAI = &GetAI_npc_earthcaller_ryga;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_living_flare";
    newscript->GetAI = &GetAI_npc_living_flare;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_shattered_hand_berserker";
    newscript->GetAI = &GetAI_npc_shattered_hand_berserker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_felblood_initiate";
    newscript->GetAI = &GetAI_npc_felblood_initiate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_ice_stone";
    newscript->pGOUse  = &GossipHello_go_ice_stone;
    newscript->pGossipSelectGO = &GossipSelect_go_ice_stone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hand_berserker";
    newscript->GetAI = &GetAI_npc_hand_berserker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anchorite_relic_bunny";
    newscript->GetAI = &GetAI_npc_anchorite_relic_bunny;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anchorite_barada";
    newscript->pGossipHello =  &GossipHello_npc_anchorite_barada;
    newscript->pGossipSelect = &GossipSelect_npc_anchorite_barada;
    newscript->GetAI = &GetAI_npc_anchorite_barada;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_darkness_released";
    newscript->GetAI = &GetAI_npc_darkness_released;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_foul_purge";
    newscript->GetAI = &GetAI_npc_foul_purge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sedai_quest_credit_marker";
    newscript->GetAI = &GetAI_npc_sedai_quest_credit_marker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_vindicator_sedai";
    newscript->GetAI = &GetAI_npc_vindicator_sedai;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_pathaleon_image";
    newscript->GetAI = &GetAI_npc_pathaleon_image;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_viera";
    newscript->GetAI = &GetAI_npc_viera;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_viera;
    newscript->RegisterSelf();
}


