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

struct TRINITY_DLL_DECL npc_aeranasAI : public ScriptedAI
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

bool GOHello_go_haaleshi_altar(Player *player, GameObject* _GO)
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

struct TRINITY_DLL_DECL npc_wounded_blood_elfAI : public npc_escortAI
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

#define FINISHED_WHISPER "Thank you for allowing me to visit, $N. You have a very colorful soul, but it's a little brighter than I prefer... or I might have stayed longer!"

struct TRINITY_DLL_DECL npc_demoniac_scryerAI : public  Scripted_NoMovementAI
{
    npc_demoniac_scryerAI(Creature *c) :  Scripted_NoMovementAI(c) {}

    uint32 Lifespan_Timer;
    uint32 Imps_Timer;
    uint32 Warlock_Timer;
    std::list<uint64> PlayersWithQuestList;
    bool Spawns;

    void Reset()
    {
        Lifespan_Timer = 120000;
        Imps_Timer = 20000;
        Warlock_Timer = 10000;
        Spawns = true;
        PlayersWithQuestList.clear();

        std::list<Unit*> PlayerList;
        uint32 questDist = 60;                      // sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE);
        Trinity::AnyUnitInObjectRangeCheck  check(me, questDist);
        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck > searcher(PlayerList, check);
        Cell::VisitAllObjects(me, searcher, questDist);

        for(std::list<Unit*>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
        {
            if((*i)->GetTypeId() != TYPEID_PLAYER)
                continue;
            Player *player = (Player*)(*i);
            if(player->GetQuestStatus(10838) == QUEST_STATUS_INCOMPLETE)
                PlayersWithQuestList.push_back(player->GetGUID());
        }
    }

    void JustRespawned()
    {
        Reset();
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->getThreatManager().addThreat(me, 1);
    }

    void UpdateAI(const uint32 diff)
    {
        if(Spawns)
        {
            if(Imps_Timer < diff)
            {
                me->SummonCreature(22259, me->GetPositionX() - 3, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN , 120000);
                me->SummonCreature(22259, me->GetPositionX() + 3, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN , 120000);
                Imps_Timer = 30000;
            }
            else
                Imps_Timer -= diff;

            if(Warlock_Timer < diff)
            {
                me->SummonCreature(22273, me->GetPositionX(), me->GetPositionY() + 3, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                Warlock_Timer = 20000;
            }
            else
                Warlock_Timer -= diff;

            if(Lifespan_Timer < diff)
            {
                Spawns = false;
                for(std::list<uint64>::iterator i = PlayersWithQuestList.begin(); i != PlayersWithQuestList.end(); i++)
                {
                    if(Unit* player = Unit::GetUnit(*me, (*i)))
                    {
                        me->CastSpell(player, 38708, true);        
                        me->Whisper(FINISHED_WHISPER, (*i));  
                    }
                }
            }
            else
                Lifespan_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_demoniac_scryer(Creature *_Creature)
{
    CreatureAI* newAI = new npc_demoniac_scryerAI(_Creature);
    return newAI;
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

struct TRINITY_DLL_DECL npc_earthcaller_rygaAI : public npc_escortAI
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

struct TRINITY_DLL_DECL npc_ancestral_spirit_wolfAI : public npc_escortAI
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

struct TRINITY_DLL_DECL npc_living_flareAI : public FollowerAI
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

struct TRINITY_DLL_DECL npc_abyssal_shelf_questAI : public ScriptedAI
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

struct TRINITY_DLL_DECL npc_shattered_hand_berserkerAI : public ScriptedAI
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


CreatureAI* GetAI_npc_shattered_hand_berserker(Creature *_Creature)
{
    return new npc_shattered_hand_berserkerAI(_Creature);
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
    newscript->pGOHello = &GOHello_go_haaleshi_altar;
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
    newscript->pQuestAccept = &QuestAccept_npc_wounded_blood_elf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_demoniac_scryer";
    newscript->GetAI = &GetAI_npc_demoniac_scryer;
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
    newscript->Name="go_ice_stone";
    newscript->pGOHello  = &GossipHello_go_ice_stone;
    newscript->pGOSelect = &GossipSelect_go_ice_stone;
    newscript->RegisterSelf();
}
