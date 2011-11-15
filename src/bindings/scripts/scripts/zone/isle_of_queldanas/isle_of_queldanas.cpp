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
SDName: Isle_of_Queldanas
SD%Complete: 100
SDComment: Quest support: 11524, 11525, 11532, 11533, 11542, 11543, 11541
SDCategory: Isle Of Quel'Danas
EndScriptData */

/* ContentData
npc_ayren_cloudbreaker
npc_converted_sentry
npc_unrestrained_dragonhawk
npc_greengill_slave
EndContentData */

#include "precompiled.h"

/*######
## npc_ayren_cloudbreaker
######*/

#define GOSSIP_FLY1 "Speaking of action, I've been ordered to undertake an air strike."
#define GOSSIP_FLY2 "I need to intercept the Dawnblade reinforcements."
bool GossipHello_npc_ayren_cloudbreaker(Player *player, Creature *_Creature)
{
    if( player->GetQuestStatus(11532) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(11533) == QUEST_STATUS_INCOMPLETE ||
        (player->GetQuestStatus(11532) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(11532)) || (player->GetQuestStatus(11533) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(11533)))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_FLY1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    if( player->GetQuestStatus(11542) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(11543) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_FLY2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    player->SEND_GOSSIP_MENU(12252,_Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_ayren_cloudbreaker(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,45071,true);               //TaxiPath 779
    }
    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,45113,true);               //TaxiPath 784
    }
    return true;
}

/*######
## npc_shattered_sun_bombardier
######*/

#define BOMBARDIER_FLY_PATH     1776

const char* BombardierYell[6] = 
{
    "Fall into formation! We're approaching the Dead Scar.",
    "Keep your eye on the demons. We're not concerned with killing Scourge today.",
    "Be quick with those charges. Some of those demons are going to take more than one direct hit to bring down.",
    "Move up and hit them from above. Let's try to get $n some cover.",
    "It's show time! Blast them hard, blast them fast!",
    "We've got your back, $n"
};

struct TRINITY_DLL_DECL npc_shattered_sun_bombardierAI : public ScriptedAI
{
    npc_shattered_sun_bombardierAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;
    uint32 yell_timer;
    uint8 yell;
    bool PathFly;

    void Reset()
    {
        me->SetVisibility(VISIBILITY_OFF);
        me->setActive(true);
        PlayerGUID = 0;
        yell_timer = 60000000;
        PathFly = false;
        yell = 0;
        me->GetMotionMaster()->MoveIdle();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (who->IsTaxiFlying() && who->IsWithinDistInMap(me, 40) && !PathFly)
        {
            PlayerGUID = who->GetGUID();
            me->GetMotionMaster()->Clear(false);
            m_creature->SetLevitate(true);
            m_creature->GetMotionMaster()->MovePath(BOMBARDIER_FLY_PATH, false);
            me->SetSpeed(MOVE_WALK, 1.4*who->GetSpeed(MOVE_FLIGHT));
            me->SetVisibility(VISIBILITY_ON);
            yell_timer = 5000;
            PathFly = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (yell_timer < diff)
        {
            switch(yell)
            {
                case 0:
                    if(me->GetGUIDLow() == 85370)
                        me->Yell(BombardierYell[rand()%3], 0, PlayerGUID);
                    me->SetSpeed(MOVE_WALK, 1.01*me->GetSpeed(MOVE_WALK));
                    yell++;
                    yell_timer = 7000;
                    break;
                case 1:
                    if(me->GetGUIDLow() == 85370)
                        me->Yell(BombardierYell[3+rand()%3], 0, PlayerGUID);
                    yell++;
                    yell_timer = 7000;
                    break;
                case 2:
                    me->DisappearAndDie();
                    me->Respawn();
                    break;
                default:
                    break;
            }
        }
        else
            yell_timer -= diff;
    }
};

CreatureAI* GetAI_npc_shattered_sun_bombardierAI(Creature* _Creature)
{
    return new npc_shattered_sun_bombardierAI(_Creature);
}

/*######
## npc_unrestrained_dragonhawk
######*/

#define GOSSIP_UD "<Ride the dragonhawk to Sun's Reach>"

bool GossipHello_npc_unrestrained_dragonhawk(Player *player, Creature *_Creature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_UD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    player->SEND_GOSSIP_MENU(12371,_Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_unrestrained_dragonhawk(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player,45353,true);               //TaxiPath 788
    }
    return true;
}

/*######
## npc_greengill_slave
######*/

#define ENRAGE  45111
#define ORB     45109
#define QUESTG  11541
#define DM      25060

struct TRINITY_DLL_DECL npc_greengill_slaveAI : public ScriptedAI
{
    npc_greengill_slaveAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void Reset()
    {
        PlayerGUID = 0;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if(!caster)
            return;

        if(caster->GetTypeId() == TYPEID_PLAYER && spell->Id == ORB && !m_creature->HasAura(ENRAGE, 0))
        {
            PlayerGUID = caster->GetGUID();
            if(PlayerGUID)
            {
                Player* plr = Unit::GetPlayer(PlayerGUID);
                if(plr && plr->GetQuestStatus(QUESTG) == QUEST_STATUS_INCOMPLETE)
                    plr->KilledMonster(25086, m_creature->GetGUID());
            }
            DoCast(m_creature, ENRAGE);
            Unit* Myrmidon = FindCreature(DM, 70, m_creature);
            if(Myrmidon)
            {
                m_creature->AddThreat(Myrmidon, 100000.0f);
                AttackStart(Myrmidon);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_greengill_slaveAI(Creature* _Creature)
{
    return new npc_greengill_slaveAI(_Creature);
}

void AddSC_isle_of_queldanas()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_ayren_cloudbreaker";
    newscript->pGossipHello = &GossipHello_npc_ayren_cloudbreaker;
    newscript->pGossipSelect = &GossipSelect_npc_ayren_cloudbreaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_shattered_sun_bombardier";
    newscript->GetAI = &GetAI_npc_shattered_sun_bombardierAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_unrestrained_dragonhawk";
    newscript->pGossipHello = &GossipHello_npc_unrestrained_dragonhawk;
    newscript->pGossipSelect = &GossipSelect_npc_unrestrained_dragonhawk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_greengill_slave";
    newscript->GetAI = &GetAI_npc_greengill_slaveAI;
    newscript->RegisterSelf();
}

