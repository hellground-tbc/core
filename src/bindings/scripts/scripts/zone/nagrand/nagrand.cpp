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
SDName: Nagrand
SD%Complete: 90
SDComment: Quest support: 9849, 9868, 9879, 9918, 9874, 9923, 9924, 9954, 9991, 10107, 10108, 10044, 10168, 10172, 10646, 10085, 10987. TextId's unknown for altruis_the_sufferer and greatmother_geyah (npc_text)
SDCategory: Nagrand
EndScriptData */

/* ContentData
mob_shattered_rumbler
mob_ancient_orc_ancestor
mob_lump
npc_altruis_the_sufferer
npc_greatmother_geyah
npc_lantresor_of_the_blade
npc_creditmarker_visit_with_ancestors
mob_sparrowhawk
npc_corki_capitive
go_corki_cage
npc_nagrand_captive (npc_maghar_captive and npc_kurenai_captive in 1)
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## mob_shattered_rumbler - this should be done with ACID
######*/

struct TRINITY_DLL_DECL mob_shattered_rumblerAI : public ScriptedAI
{
    bool Spawn;

    mob_shattered_rumblerAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        Spawn = false;
    }

    void SpellHit(Unit *Hitter, const SpellEntry *Spellkind)
    {
        if(Spellkind->Id == 32001 && !Spawn)
        {
            float x = m_creature->GetPositionX();
            float y = m_creature->GetPositionY();
            float z = m_creature->GetPositionZ();

            Hitter->SummonCreature(18181,x+(0.7 * (rand()%30)),y+(rand()%5),z,0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000);
            Hitter->SummonCreature(18181,x+(rand()%5),y-(rand()%5),z,0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000);
            Hitter->SummonCreature(18181,x-(rand()%5),y+(0.5 *(rand()%60)),z,0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000);
            m_creature->setDeathState(CORPSE);
            Spawn = true;
        }
        return;
    }
};
CreatureAI* GetAI_mob_shattered_rumbler(Creature *_Creature)
{
    return new mob_shattered_rumblerAI (_Creature);
}

/*######
## mob_ancient_orc_ancestor - this should be done with ACID also
######*/

struct TRINITY_DLL_DECL mob_ancient_orc_ancestorAI : public ScriptedAI
{
    bool Spawn;

    mob_ancient_orc_ancestorAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        Spawn = false;
    }

    void SpellHit(Unit *Hitter, const SpellEntry *Spellkind)
    {
        if(Spellkind->Id == 34063 && !Spawn)
        {
            float x = m_creature->GetPositionX();
            float y = m_creature->GetPositionY();
            float z = m_creature->GetPositionZ();
            Hitter->SummonCreature(19480,x,y,z,0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000);
            m_creature->setDeathState(CORPSE);
            Spawn = true;
        }
        return;
    }
};
CreatureAI* GetAI_mob_ancient_orc_ancestor(Creature *_Creature)
{
    return new mob_ancient_orc_ancestorAI (_Creature);
}

/*######
## mob_lump
######*/

#define SPELL_VISUAL_SLEEP  16093
#define SPELL_SPEAR_THROW   32248

#define LUMP_SAY0 -1000293
#define LUMP_SAY1 -1000294

#define LUMP_DEFEAT -1000295

#define GOSSIP_HL "I need answers, ogre!"
#define GOSSIP_SL1 "Why are Boulderfist out this far? You know that this is Kurenai territory."
#define GOSSIP_SL2 "And you think you can just eat anything you want? You're obviously trying to eat the Broken of Telaar."
#define GOSSIP_SL3 "This means war, Lump! War I say!"

struct TRINITY_DLL_DECL mob_lumpAI : public ScriptedAI
{
    mob_lumpAI(Creature *c) : ScriptedAI(c)
    {
        bReset = false;
    }

    uint32 Reset_Timer;
    uint32 Spear_Throw_Timer;
    bool bReset;

    void Reset()
    {
        Reset_Timer = 60000;
        Spear_Throw_Timer = 2000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DamageTaken(Unit *done_by, uint32 & damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER && (m_creature->GetHealth() - damage)*100 / m_creature->GetMaxHealth() < 30)
        {
            if (!bReset && ((Player*)done_by)->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
            {
                //Take 0 damage
                damage = 0;

                ((Player*)done_by)->AttackStop();
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop();
                m_creature->setFaction(1080);               //friendly
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, PLAYER_STATE_SIT);
                DoScriptText(LUMP_DEFEAT, m_creature);

                bReset = true;
            }
        }
    }

    void EnterCombat(Unit *who)
    {
        if (m_creature->HasAura(SPELL_VISUAL_SLEEP,0))
            m_creature->RemoveAura(SPELL_VISUAL_SLEEP,0);

        if (!m_creature->IsStandState())
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, PLAYER_STATE_NONE);

        DoScriptText(RAND(LUMP_SAY0, LUMP_SAY1), m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        //check if we waiting for a reset
        if (bReset)
        {
            if (Reset_Timer < diff)
            {
                EnterEvadeMode();
                bReset = false;
                m_creature->setFaction(1711);               //hostile
                return;
            }
            else Reset_Timer -= diff;
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Spear_Throw_Timer
        if (Spear_Throw_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_SPEAR_THROW);
            Spear_Throw_Timer = 20000;
        }else Spear_Throw_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lump(Creature *_creature)
{
    return new mob_lumpAI(_creature);
}

bool GossipHello_mob_lump(Player *player, Creature *_Creature)
{
    if (player->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9352, _Creature->GetGUID());

    return true;
}

bool GossipSelect_mob_lump(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(9353, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(9354, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SL3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(9355, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->SEND_GOSSIP_MENU(9356, _Creature->GetGUID());
            player->TalkedToCreature(18354, _Creature->GetGUID());
            break;
    }
    return true;
}

/*######
## npc_altruis_the_sufferer
######*/

#define GOSSIP_HATS1 "I see twisted steel and smell sundered earth."
#define GOSSIP_HATS2 "Well...?"
#define GOSSIP_HATS3 "[PH] Story about Illidan's Pupil"

#define GOSSIP_SATS1 "Legion?"
#define GOSSIP_SATS2 "And now?"
#define GOSSIP_SATS3 "How do you see them now?"
#define GOSSIP_SATS4 "Forge camps?"
#define GOSSIP_SATS5 "Ok."
#define GOSSIP_SATS6 "[PH] Story done"

bool GossipHello_npc_altruis_the_sufferer(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    //gossip before obtaining Survey the Land
    if ( player->GetQuestStatus(9991) == QUEST_STATUS_NONE )
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);

    //gossip when Survey the Land is incomplete (technically, after the flight)
    if (player->GetQuestStatus(9991) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+20);

    //wowwiki.com/Varedis
    if (player->GetQuestStatus(10646) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+30);

    player->SEND_GOSSIP_MENU(9419, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_altruis_the_sufferer(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+10:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->SEND_GOSSIP_MENU(9420, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+11:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->SEND_GOSSIP_MENU(9421, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+12:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->SEND_GOSSIP_MENU(9422, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+13:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            player->SEND_GOSSIP_MENU(9423, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+14:
            player->SEND_GOSSIP_MENU(9424, _Creature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+20:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
            player->SEND_GOSSIP_MENU(9427, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+21:
            player->CLOSE_GOSSIP_MENU();
            player->AreaExploredOrEventHappens(9991);
            break;

        case GOSSIP_ACTION_INFO_DEF+30:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SATS6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 31);
            player->SEND_GOSSIP_MENU(384, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+31:
            player->CLOSE_GOSSIP_MENU();
            player->AreaExploredOrEventHappens(10646);
            break;
    }
    return true;
}

bool QuestAccept_npc_altruis_the_sufferer(Player *player, Creature *creature, Quest const *quest )
{
    if ( !player->GetQuestRewardStatus(9991) )              //Survey the Land, q-id 9991
    {
        player->CLOSE_GOSSIP_MENU();

        std::vector<uint32> nodes;

        nodes.resize(2);
        nodes[0] = 113;                                     //from
        nodes[1] = 114;                                     //end at
        player->ActivateTaxiPathTo(nodes);                  //TaxiPath 532
    }
    return true;
}

/*######
## npc_greatmother_geyah
######*/

#define GOSSIP_HGG1 "Hello, Greatmother. Garrosh told me that you wanted to speak with me."
#define GOSSIP_HGG2 "Garrosh is beyond redemption, Greatmother. I fear that in helping the Mag'har, I have convinced Garrosh that he is unfit to lead."

#define GOSSIP_SGG1 "You raised all of the orcs here, Greatmother?"
#define GOSSIP_SGG2 "Do you believe that?"
#define GOSSIP_SGG3 "What can be done? I have tried many different things. I have done my best to help the people of Nagrand. Each time I have approached Garrosh, he has dismissed me."
#define GOSSIP_SGG4 "Left? How can you choose to leave?"
#define GOSSIP_SGG5 "What is this duty?"
#define GOSSIP_SGG6 "Is there anything I can do for you, Greatmother?"
#define GOSSIP_SGG7 "I have done all that I could, Greatmother. I thank you for your kind words."
#define GOSSIP_SGG8 "Greatmother, you are the mother of Durotan?"
#define GOSSIP_SGG9 "Greatmother, I never had the honor. Durotan died long before my time, but his heroics are known to all on my world. The orcs of Azeroth reside in a place known as Durotar, named after your son. And ... (You take a moment to breathe and think through what you are about to tell the Greatmother.)"
#define GOSSIP_SGG10 "It is my Warchief, Greatmother. The leader of my people. From my world. He ... He is the son of Durotan. He is your grandchild."
#define GOSSIP_SGG11 "I will return to Azeroth at once, Greatmother."

//all the textId's for the below is unknown, but i do believe the gossip item texts are proper.
bool GossipHello_npc_greatmother_geyah(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetQuestStatus(10044) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(),_Creature->GetGUID());
    }
    else if (player->GetQuestStatus(10172) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(),_Creature->GetGUID());
    }
    else

        player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(),_Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_greatmother_geyah(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            player->AreaExploredOrEventHappens(10044);
            player->CLOSE_GOSSIP_MENU();
            break;

        case GOSSIP_ACTION_INFO_DEF + 10:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SGG11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
            player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 15:
            player->AreaExploredOrEventHappens(10172);
            player->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

/*######
## npc_lantresor_of_the_blade
######*/

#define GOSSIP_HLB "I have killed many of your ogres, Lantresor. I have no fear."
#define GOSSIP_SLB1 "Should I know? You look like an orc to me."
#define GOSSIP_SLB2 "And the other half?"
#define GOSSIP_SLB3 "I have heard of your kind, but I never thought to see the day when I would meet a half-breed."
#define GOSSIP_SLB4 "My apologies. I did not mean to offend. I am here on behalf of my people."
#define GOSSIP_SLB5 "My people ask that you pull back your Boulderfist ogres and cease all attacks on our territories. In return, we will also pull back our forces."
#define GOSSIP_SLB6 "We will fight you until the end, then, Lantresor. We will not stand idly by as you pillage our towns and kill our people."
#define GOSSIP_SLB7 "What do I need to do?"

bool GossipHello_npc_lantresor_of_the_blade(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HLB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9361, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_lantresor_of_the_blade(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(9362, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(9363, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(9364, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->SEND_GOSSIP_MENU(9365, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->SEND_GOSSIP_MENU(9366, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            player->SEND_GOSSIP_MENU(9367, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SLB7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            player->SEND_GOSSIP_MENU(9368, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            player->SEND_GOSSIP_MENU(9369, _Creature->GetGUID());
            if (player->GetQuestStatus(10107) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(10107);
            if (player->GetQuestStatus(10108) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(10108);
            break;
    }
    return true;
}

/*######
## npc_creditmarker_visist_with_ancestors
######*/

struct TRINITY_DLL_DECL npc_creditmarker_visit_with_ancestorsAI : public ScriptedAI
{
    npc_creditmarker_visit_with_ancestorsAI(Creature* c) : ScriptedAI(c) {}

    void Reset() {}

    void MoveInLineOfSight(Unit *who)
    {
        if(!who)
            return;

        if(who->GetTypeId() == TYPEID_PLAYER)
        {
            if(((Player*)who)->GetQuestStatus(10085) == QUEST_STATUS_INCOMPLETE)
            {
                uint32 creditMarkerId = m_creature->GetEntry();
                if((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                {
                    // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                    if(!((Player*)who)->GetReqKillOrCastCurrentCount(10085, creditMarkerId))
                        ((Player*)who)->KilledMonster(creditMarkerId, m_creature->GetGUID());
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_creditmarker_visit_with_ancestors(Creature *_Creature)
{
    return new npc_creditmarker_visit_with_ancestorsAI (_Creature);
}

/*######
## mob_sparrowhawk
######*/

#define SPELL_SPARROWHAWK_NET 39810
#define SPELL_ITEM_CAPTIVE_SPARROWHAWK 39812

struct TRINITY_DLL_DECL mob_sparrowhawkAI : public ScriptedAI
{

    mob_sparrowhawkAI(Creature *c) : ScriptedAI(c) {}

    uint32 Check_Timer;
    uint64 PlayerGUID;
    bool fleeing;

    void Reset()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SPARROWHAWK_NET);
        Check_Timer = 1000;
        PlayerGUID = 0;
        fleeing = false;
    }
    void AttackStart(Unit *who)
    {
        if(PlayerGUID)
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(!who || PlayerGUID)
            return;

        if(!PlayerGUID && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(((Player *)who), 30) && ((Player *)who)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
        {
            PlayerGUID = who->GetGUID();
            return;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if(Check_Timer < diff)
        {
            if(PlayerGUID)
            {
                if(fleeing && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLEEING_MOTION_TYPE)
                    fleeing = false;

                Player *player = (Player *)Unit::GetUnit((*m_creature), PlayerGUID);
                if(player && m_creature->IsWithinDistInMap(player, 30))
                {
                    if(!fleeing)
                    {
                        m_creature->DeleteThreatList();
                        m_creature->GetMotionMaster()->MoveFleeing(player);
                        fleeing = true;
                    }
                }
                else if(fleeing)
                {
                    m_creature->GetMotionMaster()->MovementExpired(false);
                    PlayerGUID = 0;
                    fleeing = false;
                }
            }
            Check_Timer = 1000;
        } else Check_Timer -= diff;

        if (PlayerGUID)
            return;

        ScriptedAI::UpdateAI(diff);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
            if(spell->Id == SPELL_SPARROWHAWK_NET && ((Player*)caster)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
            {
                m_creature->CastSpell(caster, SPELL_ITEM_CAPTIVE_SPARROWHAWK, true);
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
        }
        return;
    }
};

CreatureAI* GetAI_mob_sparrowhawk(Creature *_Creature)
{
    return new mob_sparrowhawkAI (_Creature);
}

/*########
## Quests: HELP!, Corki's Gone Missing Again!, Cho'war the Pillager
########*/

enum CorkiCage
{
    QUEST_HELP1                          = 9923, // HELP!
    NPC_CORKI_CAPITIVE1                  = 18445,
    GO_CORKI_CAGE1                       = 182349,

    QUEST_HELP2                          = 9924, // Corki's Gone Missing Again!
    NPC_CORKI_CAPITIVE2                  = 20812,
    GO_CORKI_CAGE2                       = 182350,

    QUEST_HELP3                          = 9955, // Cho'war the Pillager
    NPC_CORKI_CAPITIVE3                  = 18369,
    GO_CORKI_CAGE3                       = 182521
};



struct npc_corki_capitiveAI : public ScriptedAI
{
    npc_corki_capitiveAI(Creature *c) : ScriptedAI(c){}

    uint32 FleeTimer;

  void Reset()
  {
    FleeTimer = 0;
    GameObject* cage = NULL;
    switch (me->GetEntry())
    {
      case NPC_CORKI_CAPITIVE1:
        cage = FindGameObject(GO_CORKI_CAGE1, 5.0f, me);
        break;
      case NPC_CORKI_CAPITIVE2:
        cage = FindGameObject(GO_CORKI_CAGE2, 5.0f, me);
        break;
      case NPC_CORKI_CAPITIVE3:
        cage = FindGameObject(GO_CORKI_CAGE3, 5.0f, me);
        break;
    }

     if(cage)
      cage->ResetDoorOrButton();
  }

    void UpdateAI(const uint32 diff)
    {
        if(FleeTimer)
        {
            if(FleeTimer <= diff)
                me->ForcedDespawn();
            else FleeTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_corki_capitiveAI(Creature* pCreature)
{
    return new npc_corki_capitiveAI(pCreature);
}

bool go_corki_cage(Player* pPlayer, GameObject* pGo)
{
   Creature* pCreature = NULL;
    switch(pGo->GetEntry())
    {
        case GO_CORKI_CAGE1:
            if(pPlayer->GetQuestStatus(QUEST_HELP1) == QUEST_STATUS_INCOMPLETE)
                pCreature = GetClosestCreatureWithEntry(pGo, NPC_CORKI_CAPITIVE1, 5.0f);
            break;
        case GO_CORKI_CAGE2:
            if(pPlayer->GetQuestStatus(QUEST_HELP2) == QUEST_STATUS_INCOMPLETE)
                pCreature = GetClosestCreatureWithEntry(pGo, NPC_CORKI_CAPITIVE2, 5.0f);
            break;
        case GO_CORKI_CAGE3:
            if(pPlayer->GetQuestStatus(QUEST_HELP3) == QUEST_STATUS_INCOMPLETE)
                pCreature = GetClosestCreatureWithEntry(pGo, NPC_CORKI_CAPITIVE3, 5.0f);
            break;
    }
    if(pCreature)
    {
        DoScriptText(-1230010-urand(0, 2), pCreature, pPlayer);
        pCreature->GetMotionMaster()->Clear();
        pCreature->GetMotionMaster()->MoveFleeing(pPlayer, 3500);
        pPlayer->KilledMonster(pCreature->GetEntry(), pCreature->GetGUID());
        CAST_AI(npc_corki_capitiveAI, pCreature->AI())->FleeTimer = 3500;
        return false;
    }
    return true;
}

/*#####
## npc_nagrand_captive
#####*/

enum eNagrandCaptive
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_NO_ESCAPE           = -1000483,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_MORE_REPLY          = -1000485,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,
    QUEST_TOTEM_KARDASH_A       = 9879,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202
};

static float m_afAmbushAH[]= {-1568.805786, 8533.873047, 1.958};
static float m_afAmbushAA[]= {-1512.240967, 8483.319336, -3.946};
static float m_afAmbushB[]= {-1491.554321, 8506.483398, 1.248};

struct npc_nagrand_captiveAI : public npc_escortAI
{
    npc_nagrand_captiveAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiChainLightningTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiFrostShockTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
        m_uiHealTimer = 0;
        m_uiFrostShockTimer = 6000;
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        DoCast(me, SPELL_EARTHBIND_TOTEM, false);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 7:
                DoScriptText(SAY_MAG_MORE, me);

                if (Creature* pTemp = me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0], m_afAmbushB[1], m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                    DoScriptText(SAY_MAG_MORE_REPLY, pTemp);

                me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0]-2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);

                me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]+2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                break;
            case 16:
                DoScriptText(SAY_MAG_COMPLETE, me);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(pPlayer->GetTeam() == ALLIANCE ? QUEST_TOTEM_KARDASH_A : QUEST_TOTEM_KARDASH_H, me);

                SetRun();
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_MURK_BRUTE)
            DoScriptText(SAY_MAG_NO_ESCAPE, pSummoned);

        if (pSummoned->isTotem())
            return;

        pSummoned->SetWalk(false);
        pSummoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        pSummoned->AI()->AttackStart(me);

    }

    void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CHAIN_LIGHTNING)
        {
            if (rand()%10)
                return;

            DoScriptText(SAY_MAG_LIGHTNING, me);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);
        if (!me->getVictim())
            return;

        if (m_uiChainLightningTimer <= uiDiff)
        {
            DoCast(me->getVictim(), SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (me->GetHealth()*100 < me->GetMaxHealth()*30)
        {
            if (m_uiHealTimer <= uiDiff)
            {
                DoCast(me, SPELL_HEALING_WAVE);
                m_uiHealTimer = 5000;
            }
            else
                m_uiHealTimer -= uiDiff;
        }

        if (m_uiFrostShockTimer <= uiDiff)
        {
            DoCast(me->getVictim(), SPELL_FROST_SHOCK);
            m_uiFrostShockTimer = urand(7500, 15000);
        }
        else
            m_uiFrostShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_nagrand_captive(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    switch (pQuest->GetQuestId())
    {
        case QUEST_TOTEM_KARDASH_H:
            if (npc_nagrand_captiveAI* pEscortAI = dynamic_cast<npc_nagrand_captiveAI*>(pCreature->AI()))
            {
                pCreature->SetStandState(UNIT_STAND_STATE_STAND);
                pCreature->setFaction(232);

                pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);

                DoScriptText(SAY_MAG_START, pCreature);

                pCreature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushAH[0]+2.5f, m_afAmbushAH[1]-2.5f, m_afAmbushAH[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                pCreature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushAH[0]-2.5f, m_afAmbushAH[1]+2.5f, m_afAmbushAH[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                pCreature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushAH[0], m_afAmbushAH[1], m_afAmbushAH[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            }
            break;
        case QUEST_TOTEM_KARDASH_A:
            if (npc_nagrand_captiveAI* pEscortAI = dynamic_cast<npc_nagrand_captiveAI*>(pCreature->AI()))
            {
                pCreature->SetStandState(UNIT_STAND_STATE_STAND);
                pCreature->setFaction(231);

                pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);

                DoScriptText(SAY_MAG_START, pCreature);

                pCreature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushAA[0]+2.5f, m_afAmbushAA[1]-2.5f, m_afAmbushAA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                pCreature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushAA[0]-2.5f, m_afAmbushAA[1]+2.5f, m_afAmbushAA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                pCreature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushAA[0], m_afAmbushAA[1], m_afAmbushAA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            }
            break;
        default:
            return false;
    }

    return true;
}

CreatureAI* GetAI_npc_nagrand_captive(Creature* pCreature)
{
    return new npc_nagrand_captiveAI(pCreature);
}

/*####
#  npc_multiphase_disurbanceAI
####*/

#define SPELL_TAKE_MULTIPHASE_READING   46281

struct npc_multiphase_disurbanceAI : public ScriptedAI
{
    npc_multiphase_disurbanceAI(Creature *c) : ScriptedAI(c){}

    uint32 despawnTimer;

    void Reset()
    {
        despawnTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (despawnTimer)
        {
            if (despawnTimer <= diff)
            {
                me->ForcedDespawn();
                despawnTimer = 0;
            }
            else
                despawnTimer -= diff;
        }
    }

    void SpellHit(Unit * /*caster*/, const SpellEntry * spell)
    {
        if (spell && spell->Id == SPELL_TAKE_MULTIPHASE_READING)
            despawnTimer = 2500;
    }
};

CreatureAI* GetAI_npc_multiphase_disturbance(Creature* pCreature)
{
    return new npc_multiphase_disurbanceAI(pCreature);
}

void AddSC_nagrand()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="mob_shattered_rumbler";
    newscript->GetAI = &GetAI_mob_shattered_rumbler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_ancient_orc_ancestor";
    newscript->GetAI = &GetAI_mob_ancient_orc_ancestor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_lump";
    newscript->GetAI = &GetAI_mob_lump;
    newscript->pGossipHello =  &GossipHello_mob_lump;
    newscript->pGossipSelect = &GossipSelect_mob_lump;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_altruis_the_sufferer";
    newscript->pGossipHello =  &GossipHello_npc_altruis_the_sufferer;
    newscript->pGossipSelect = &GossipSelect_npc_altruis_the_sufferer;
    newscript->pQuestAcceptNPC =  &QuestAccept_npc_altruis_the_sufferer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_greatmother_geyah";
    newscript->pGossipHello =  &GossipHello_npc_greatmother_geyah;
    newscript->pGossipSelect = &GossipSelect_npc_greatmother_geyah;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_lantresor_of_the_blade";
    newscript->pGossipHello =  &GossipHello_npc_lantresor_of_the_blade;
    newscript->pGossipSelect = &GossipSelect_npc_lantresor_of_the_blade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_creditmarker_visit_with_ancestors";
    newscript->GetAI = &GetAI_npc_creditmarker_visit_with_ancestors;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_sparrowhawk";
    newscript->GetAI = &GetAI_mob_sparrowhawk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_corki_capitive";
    newscript->GetAI = &GetAI_npc_corki_capitiveAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_corki_cage";
    newscript->pGOUse = &go_corki_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nagrand_captive";
    newscript->GetAI = &GetAI_npc_nagrand_captive;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_nagrand_captive;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_multiphase_disturbance";
    newscript->GetAI = &GetAI_npc_multiphase_disturbance;
    newscript->RegisterSelf();
}
