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
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223. Receive emote General Marcus
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_dashel_stonefist
npc_general_marcus_jonathan
npc_lady_katrana_prestor
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

bool GossipHello_npc_archmage_malin(Player *player, Creature *_Creature)
{
    if(_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if(player->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(11223))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_archmage_malin(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if(action = GOSSIP_ACTION_INFO_DEF)
    {
        player->CLOSE_GOSSIP_MENU();
        _Creature->CastSpell(player, 42711, true);
    }

    return true;
}

/*######
## npc_bartleby
######*/

struct TRINITY_DLL_DECL npc_bartlebyAI : public ScriptedAI
{
    npc_bartlebyAI(Creature *c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void Reset()
    {
        m_creature->setFaction(11);
        m_creature->setEmoteState(7);

        PlayerGUID = 0;
    }

    void JustDied(Unit *who)
    {
        m_creature->setFaction(11);
    }

    void DamageTaken(Unit *done_by, uint32 & damage)
    {
        if(damage > m_creature->GetHealth() || ((m_creature->GetHealth() - damage)*100 / m_creature->GetMaxHealth() < 15))
        {
            //Take 0 damage
            damage = 0;

            if (done_by->GetTypeId() == TYPEID_PLAYER && done_by->GetGUID() == PlayerGUID)
            {
                ((Player*)done_by)->AttackStop();
                ((Player*)done_by)->AreaExploredOrEventHappens(1640);
            }
            m_creature->CombatStop();
            EnterEvadeMode();
        }
    }

    void EnterCombat(Unit *who) {}
};

bool QuestAccept_npc_bartleby(Player *player, Creature *_Creature, Quest const *_Quest)
{
    if(_Quest->GetQuestId() == 1640)
    {
        _Creature->setFaction(168);
        ((npc_bartlebyAI*)_Creature->AI())->PlayerGUID = player->GetGUID();
        ((npc_bartlebyAI*)_Creature->AI())->AttackStart(player);
    }
    return true;
}

CreatureAI* GetAI_npc_bartleby(Creature *_creature)
{
    return new npc_bartlebyAI(_creature);
}

/*######
## npc_dashel_stonefist
######*/

struct TRINITY_DLL_DECL npc_dashel_stonefistAI : public ScriptedAI
{
    npc_dashel_stonefistAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        m_creature->setFaction(11);
        m_creature->setEmoteState(7);
    }

    void DamageTaken(Unit *done_by, uint32 & damage)
    {
        if((damage > m_creature->GetHealth()) || (m_creature->GetHealth() - damage)*100 / m_creature->GetMaxHealth() < 15)
        {
            //Take 0 damage
            damage = 0;

            if (done_by->GetTypeId() == TYPEID_PLAYER)
            {
                ((Player*)done_by)->AttackStop();
                ((Player*)done_by)->AreaExploredOrEventHappens(1447);
            }
            //m_creature->CombatStop();
            EnterEvadeMode();
        }
    }

    void EnterCombat(Unit *who) {}
};

bool QuestAccept_npc_dashel_stonefist(Player *player, Creature *_Creature, Quest const *_Quest)
{
    if(_Quest->GetQuestId() == 1447)
    {
        _Creature->setFaction(168);
        ((npc_dashel_stonefistAI*)_Creature->AI())->AttackStart(player);
    }
    return true;
}

CreatureAI* GetAI_npc_dashel_stonefist(Creature *_creature)
{
    return new npc_dashel_stonefistAI(_creature);
}

/*######
## npc_general_marcus_jonathan
######*/

bool ReceiveEmote_npc_general_marcus_jonathan(Player *player, Creature *_Creature, uint32 emote)
{
    if(player->GetTeam() == ALLIANCE)
    {
        if (emote == TEXTEMOTE_SALUTE)
        {
            _Creature->SetOrientation(_Creature->GetAngle(player));
            _Creature->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
        }
        if (emote == TEXTEMOTE_WAVE)
        {
            _Creature->MonsterSay("Greetings citizen",LANG_COMMON,0);
        }
    }
    return true;
}

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

bool GossipHello_npc_lady_katrana_prestor(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(2693, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_lady_katrana_prestor(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(2694, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(2695, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(2696, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->CLOSE_GOSSIP_MENU();
            player->AreaExploredOrEventHappens(4185);
            break;
    }
    return true;
}
/*######
## npc_marzon_silent_blade
######*/

enum eLordGregorLescovar
{
    SAY_LESCOVAR_2 = -1000457,
    SAY_GUARD_2    = -1000458,
    SAY_LESCOVAR_3 = -1000459,
    SAY_MARZON_1   = -1000460,
    SAY_LESCOVAR_4 = -1000461,
    SAY_TYRION_2   = -1000462,
    SAY_LESCOVAR_5 = -1000463,
    SAY_MARZON_2   = -1000464,

    NPC_STORMWIND_ROYAL      = 1756,
    NPC_MARZON_BLADE         = 1755,
    NPC_LORD_GREGOR_LESCOVAR = 1754,
    NPC_TYRION               = 7766,

    QUEST_THE_ATTACK    = 434
};

/*######
## npc_lord_gregor_lescovar
######*/

struct npc_lord_gregor_lescovarAI : public npc_escortAI
{
    npc_lord_gregor_lescovarAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        pCreature->RestoreFaction();
    }

    uint32 uiTimer;
    uint32 uiPhase;

    uint64 MarzonGUID;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        uiTimer = 0;
        uiPhase = 0;

        MarzonGUID = 0;
    }

    void EnterEvadeMode()
    {
        me->DisappearAndDie();

        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
        {
            if (pMarzon->isAlive())
                pMarzon->DisappearAndDie();
        }
    }

    void EnterCombat(Unit* pWho)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoScriptText(SAY_LESCOVAR_5, me);
        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
        {
            if (pMarzon->isAlive() && !pMarzon->isInCombat())
                pMarzon->AI()->AttackStart(pWho);
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
                me->setFaction(35);
                break;
            case 14:
                SetEscortPaused(true);
                DoScriptText(SAY_LESCOVAR_2, me);
                uiTimer = 3000;
                uiPhase = 1;
                break;
            case 16:
                SetEscortPaused(true);
                if (Creature *pMarzon = me->SummonCreature(NPC_MARZON_BLADE,-8411.360352, 480.069733, 123.760895, 4.941504, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                {
                    pMarzon->GetMotionMaster()->MovePoint(0,-8408.000977, 468.611450, 123.759903);
                    MarzonGUID = pMarzon->GetGUID();
                }
                uiTimer = 2000;
                uiPhase = 4;
                break;
        }
    }
    //TO-DO: We don't have movemaps, also we can't make 2 npcs walks to one point propperly (and we can not use escort ai, because they are 2 different spawns and with same entry), because of it we make them, disappear.
    void DoGuardsDisappearAndDie()
    {
        std::list<Creature*> GuardList;
        GuardList = DoFindAllCreaturesWithEntry(NPC_STORMWIND_ROYAL,50.0f);
        if (!GuardList.empty())
        {
            for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
            {
                if (Creature* pGuard = *itr)
                    pGuard->DisappearAndDie();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch(uiPhase)
                {
                    case 1:
                        if (Creature* pGuard = GetClosestCreatureWithEntry(me, NPC_STORMWIND_ROYAL, 30.0f))
                            DoScriptText(SAY_GUARD_2, pGuard);
                        DoGuardsDisappearAndDie();
                        uiTimer = 3000;
                        uiPhase = 2;
                        break;
                    case 2:
                        me->RestoreFaction();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        uiTimer = 2000;
                        uiPhase = 3;
                        break;
                    case 3:
                        SetEscortPaused(false);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    case 4:
                        DoScriptText(SAY_LESCOVAR_3, me);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    case 5:
                        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
                            DoScriptText(SAY_MARZON_1, pMarzon);
                        uiTimer = 3000;
                        uiPhase = 6;
                        break;
                    case 6:
                        DoScriptText(SAY_LESCOVAR_4, me);
                        /*
                        if (Player* pPlayer = GetPlayerForEscort())
                            pPlayer->AreaExploredOrEventHappens(QUEST_THE_ATTACK);
                        */
                        uiTimer = 2000;
                        uiPhase = 7;
                        break;
                    case 7:
                        if (Creature* pTyrion = GetClosestCreatureWithEntry(me, NPC_TYRION, 20.0f))
                            DoScriptText(SAY_TYRION_2, pTyrion);
                        if (Creature *pMarzon = Unit::GetCreature(*me, MarzonGUID))
                            pMarzon->setFaction(14);
                        me->setFaction(14);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                }
            } else uiTimer -= uiDiff;
        }
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lord_gregor_lescovar(Creature* pCreature)
{
    return new npc_lord_gregor_lescovarAI(pCreature);
}

struct npc_marzon_silent_bladeAI : public ScriptedAI
{
    npc_marzon_silent_bladeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        me->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
    }

    void Reset()
    {
        me->RestoreFaction();
    }

    void EnterCombat(Unit* pWho)
    {
        DoScriptText(SAY_MARZON_2, me);

        if (Creature* pLord = GetClosestCreatureWithEntry(me, NPC_LORD_GREGOR_LESCOVAR, 30.0f))
        {
            if (pLord && pLord->isAlive() && !pLord->isInCombat())
                CAST_CRE(pLord)->AI()->AttackStart(pWho);
        }
    }

    void EnterEvadeMode()
    {
        me->DisappearAndDie();

        if (Creature* pLord = GetClosestCreatureWithEntry(me, NPC_LORD_GREGOR_LESCOVAR, 30.0f))
        {
            if (pLord && pLord->isAlive())
                CAST_CRE(pLord)->DisappearAndDie();
        }
    }

    void MovementInform(uint32 uiType, uint32 /*uiId*/)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (Creature* pLord = GetClosestCreatureWithEntry(me, NPC_LORD_GREGOR_LESCOVAR, 30.0f))
        {
            CAST_AI(npc_lord_gregor_lescovarAI, CAST_CRE(pLord)->AI())->uiTimer = 2000;
            CAST_AI(npc_lord_gregor_lescovarAI, CAST_CRE(pLord)->AI())->uiPhase = 5;
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_marzon_silent_blade(Creature* pCreature)
{
    return new npc_marzon_silent_bladeAI(pCreature);
}

/*######
## npc_tyrion_spybot
######*/

enum eTyrionSpybot
{
    SAY_QUEST_ACCEPT_ATTACK  = -1000499,
    SAY_TYRION_1             = -1000450,
    SAY_SPYBOT_1             = -1000451,
    SAY_GUARD_1              = -1000452,
    SAY_SPYBOT_2             = -1000453,
    SAY_SPYBOT_3             = -1000454,
    SAY_LESCOVAR_1           = -1000455,
    SAY_SPYBOT_4             = -1000456,

    NPC_PRIESTESS_TYRIONA    = 7779,
};

struct npc_tyrion_spybotAI : public npc_escortAI
{
    npc_tyrion_spybotAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint32 uiTimer;
    uint32 uiPhase;

    void Reset()
    {
        uiTimer = 0;
        uiPhase = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
                SetEscortPaused(true);
                uiTimer = 2000;
                uiPhase = 1;
                break;
            case 5:
                SetEscortPaused(true);
                DoScriptText(SAY_SPYBOT_1, me);
                uiTimer = 2000;
                uiPhase = 5;
                break;
            case 17:
                SetEscortPaused(true);
                DoScriptText(SAY_SPYBOT_3, me);
                uiTimer = 3000;
                uiPhase = 8;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch(uiPhase)
                {
                    case 1:
                        DoScriptText(SAY_QUEST_ACCEPT_ATTACK, me);
                        uiTimer = 3000;
                        uiPhase = 2;
                        break;
                    case 2:
                        if (Creature* pTyrion = GetClosestCreatureWithEntry(me, NPC_TYRION,10.0f))
                            DoScriptText(SAY_TYRION_1, pTyrion);
                        uiTimer = 3000;
                        uiPhase = 3;
                        break;
                    case 3:
                        me->UpdateEntry(NPC_PRIESTESS_TYRIONA, ALLIANCE);
                        uiTimer = 2000;
                        uiPhase = 4;
                        break;
                    case 4:
                       SetEscortPaused(false);
                       uiPhase = 0;
                       uiTimer = 0;
                       break;
                    case 5:
                        if (Creature* pGuard = GetClosestCreatureWithEntry(me, NPC_STORMWIND_ROYAL, 10.0f))
                            DoScriptText(SAY_GUARD_1, pGuard);
                        uiTimer = 3000;
                        uiPhase = 6;
                        break;
                    case 6:
                        DoScriptText(SAY_SPYBOT_2, me);
                        uiTimer = 3000;
                        uiPhase = 7;
                        break;
                    case 7:
                        SetEscortPaused(false);
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                    case 8:
                        if (Creature* pLescovar = GetClosestCreatureWithEntry(me, NPC_LORD_GREGOR_LESCOVAR,10.0f))
                            DoScriptText(SAY_LESCOVAR_1, pLescovar);
                        uiTimer = 3000;
                        uiPhase = 9;
                        break;
                    case 9:
                        DoScriptText(SAY_SPYBOT_4, me);
                        uiTimer = 3000;
                        uiPhase = 10;
                        break;
                    case 10:
                        if (Creature* pLescovar = GetClosestCreatureWithEntry(me, NPC_LORD_GREGOR_LESCOVAR,10.0f))
                        {
                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                CAST_AI(npc_lord_gregor_lescovarAI,pLescovar->AI())->Start(false, false, pPlayer->GetGUID());
                                CAST_AI(npc_lord_gregor_lescovarAI, pLescovar->AI())->SetMaxPlayerDistance(200.0f);
                            }
                        }
                        me->DisappearAndDie();
                        me->Respawn();
                        uiTimer = 0;
                        uiPhase = 0;
                        break;
                }
            } else uiTimer -= uiDiff;
        }
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tyrion_spybot(Creature* pCreature)
{
    return new npc_tyrion_spybotAI(pCreature);
}

/*######
## npc_tyrion
######*/

enum eTyrion
{
    NPC_TYRION_SPYBOT = 8856
};

bool QuestAccept_npc_tyrion(Player* pPlayer, Creature* pCreature, Quest const *pQuest)
{
    if (pQuest->GetQuestId() == QUEST_THE_ATTACK)
    {
        if (Creature* pSpybot = GetClosestCreatureWithEntry(pCreature, NPC_TYRION_SPYBOT, 5.0f))
        {
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->Start(false, false, pPlayer->GetGUID());
            CAST_AI(npc_tyrion_spybotAI,pSpybot->AI())->SetMaxPlayerDistance(200.0f);
        }
        return true;
    }
    return false;
}


void AddSC_stormwind_city()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_archmage_malin";
    newscript->pGossipHello = &GossipHello_npc_archmage_malin;
    newscript->pGossipSelect = &GossipSelect_npc_archmage_malin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bartleby";
    newscript->GetAI = &GetAI_npc_bartleby;
    newscript->pQuestAccept = &QuestAccept_npc_bartleby;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dashel_stonefist";
    newscript->GetAI = &GetAI_npc_dashel_stonefist;
    newscript->pQuestAccept = &QuestAccept_npc_dashel_stonefist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_general_marcus_jonathan";
    newscript->pReceiveEmote = &ReceiveEmote_npc_general_marcus_jonathan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_lady_katrana_prestor";
    newscript->pGossipHello = &GossipHello_npc_lady_katrana_prestor;
    newscript->pGossipSelect = &GossipSelect_npc_lady_katrana_prestor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion";
    newscript->pQuestAccept = &QuestAccept_npc_tyrion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tyrion_spybot";
    newscript->GetAI = &GetAI_npc_tyrion_spybot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lord_gregor_lescovar";
    newscript->GetAI = &GetAI_npc_lord_gregor_lescovar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_marzon_silent_blade";
    newscript->GetAI = &GetAI_npc_marzon_silent_blade;
    newscript->RegisterSelf();
}

