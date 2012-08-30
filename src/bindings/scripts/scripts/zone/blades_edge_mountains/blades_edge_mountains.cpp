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
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10503, 10504, 10556, 10609, 10682, 10980. Ogri'la->Skettis Flight. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_bladespire_ogre
mobs_nether_drake
npc_daranelle
npc_overseer_nuaar
npc_saikkal_the_elder
npc_skyguard_handler_irena
npc_bloodmaul_brutebane
npc_ogre_brute
npc_aether_ray
npc_wildlord_antelarion
npc_kolphis_darkscale
EndContentData */

#include "precompiled.h"

/*######
## mobs_nether_drake
######*/

#define SAY_NIHIL_1         -1000396
#define SAY_NIHIL_2         -1000397
#define SAY_NIHIL_3         -1000398
#define SAY_NIHIL_4         -1000399
#define SAY_NIHIL_INTERRUPT -1000400

#define ENTRY_WHELP                 20021
#define ENTRY_PROTO                 21821
#define ENTRY_ADOLE                 21817
#define ENTRY_MATUR                 21820
#define ENTRY_NIHIL                 21823

#define SPELL_T_PHASE_MODULATOR     37573

#define SPELL_ARCANE_BLAST          38881
#define SPELL_MANA_BURN             38884
#define SPELL_INTANGIBLE_PRESENCE   36513

struct HELLGROUND_DLL_DECL mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature *c) : ScriptedAI(c) {}

    bool IsNihil;
    uint32 NihilSpeech_Timer;
    uint32 NihilSpeech_Phase;

    uint32 ArcaneBlast_Timer;
    uint32 ManaBurn_Timer;
    uint32 IntangiblePresence_Timer;

    void Reset()
    {
        NihilSpeech_Timer = 2000;
        IsNihil = false;

        if( m_creature->GetEntry() == ENTRY_NIHIL )
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            IsNihil = true;
        }

        NihilSpeech_Phase = 1;

        ArcaneBlast_Timer = 7500;
        ManaBurn_Timer = 10000;
        IntangiblePresence_Timer = 15000;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if( spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPEID_PLAYER )
        {
            uint32 cEntry = 0;

            switch( m_creature->GetEntry() )
            {
                case ENTRY_WHELP:
                    cEntry = RAND(ENTRY_PROTO, ENTRY_ADOLE, ENTRY_MATUR, ENTRY_NIHIL);
                    break;
                case ENTRY_PROTO:
                    cEntry = RAND(ENTRY_ADOLE, ENTRY_MATUR, ENTRY_NIHIL);
                    break;
                case ENTRY_ADOLE:
                    cEntry = RAND(ENTRY_PROTO, ENTRY_MATUR, ENTRY_NIHIL);
                    break;
                case ENTRY_MATUR:
                    cEntry = RAND(ENTRY_PROTO, ENTRY_ADOLE, ENTRY_NIHIL);
                    break;
                case ENTRY_NIHIL:
                    if( NihilSpeech_Phase )
                    {
                        DoScriptText(SAY_NIHIL_INTERRUPT, m_creature);
                        IsNihil = false;
                        cEntry = RAND(ENTRY_PROTO, ENTRY_ADOLE, ENTRY_MATUR);
                    }
                    break;
            }

            if( cEntry )
            {
                m_creature->UpdateEntry(cEntry);

                if( cEntry == ENTRY_NIHIL )
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    m_creature->RemoveAllAuras();
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop(true);
                    Reset();
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if( IsNihil )
        {
            if( NihilSpeech_Phase )
            {
                if(NihilSpeech_Timer <= diff)
                {
                    switch( NihilSpeech_Phase )
                    {
                        case 1:
                            DoScriptText(SAY_NIHIL_1, m_creature);
                            ++NihilSpeech_Phase;
                            break;
                        case 2:
                            DoScriptText(SAY_NIHIL_2, m_creature);
                            ++NihilSpeech_Phase;
                            break;
                        case 3:
                            DoScriptText(SAY_NIHIL_3, m_creature);
                            ++NihilSpeech_Phase;
                            break;
                        case 4:
                            DoScriptText(SAY_NIHIL_4, m_creature);
                            ++NihilSpeech_Phase;
                            break;
                        case 5:
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->SetLevitate(true);
                            //then take off to random location. creature is initially summoned, so don't bother do anything else.
                            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX()+100, m_creature->GetPositionY(), m_creature->GetPositionZ()+100);
                            NihilSpeech_Phase = 0;
                            break;
                    }
                    NihilSpeech_Timer = 5000;
                }
                else
                    NihilSpeech_Timer -=diff;
            }
            return;                                         //anything below here is not interesting for Nihil, so skip it
        }

        if( !UpdateVictim() )
            return;

        if( IntangiblePresence_Timer <= diff )
        {
            DoCast(m_creature->getVictim(),SPELL_INTANGIBLE_PRESENCE);
            IntangiblePresence_Timer = 15000+rand()%15000;
        }
        else
            IntangiblePresence_Timer -= diff;

        if( ManaBurn_Timer <= diff )
        {
            Unit* target = m_creature->getVictim();
            if( target && target->getPowerType() == POWER_MANA )
                DoCast(target,SPELL_MANA_BURN);
            ManaBurn_Timer = 8000+rand()%8000;
        }
        else
            ManaBurn_Timer -= diff;

        if( ArcaneBlast_Timer <= diff )
        {
            DoCast(m_creature->getVictim(),SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = 2500+rand()%5000;
        }
        else
            ArcaneBlast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mobs_nether_drake(Creature *_Creature)
{
    return new mobs_nether_drakeAI (_Creature);
}

/*######
## npc_daranelle
######*/

#define SAY_DARANELLE -1000401

struct HELLGROUND_DLL_DECL npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if(who->HasAura(36904,0))
            {
                DoScriptText(SAY_DARANELLE, m_creature, who);
                //TODO: Move the below to updateAI and run if this statement == true
                ((Player*)who)->KilledMonster(21511, m_creature->GetGUID());
                ((Player*)who)->RemoveAurasDueToSpell(36904);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }
};

CreatureAI* GetAI_npc_daranelle(Creature *_Creature)
{
    return new npc_daranelleAI (_Creature);
}

/*######
## npc_overseer_nuaar
######*/

#define GOSSIP_HON "Overseer, I am here to negotiate on behalf of the Cenarion Expedition."

bool GossipHello_npc_overseer_nuaar(Player *player, Creature *_Creature)
{
    if (player->GetQuestStatus(10682) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10532, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_overseer_nuaar(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->SEND_GOSSIP_MENU(10533, _Creature->GetGUID());
        player->AreaExploredOrEventHappens(10682);
    }
    return true;
}

/*######
## npc_saikkal_the_elder
######*/

#define GOSSIP_HSTE "Yes... yes, it's me."
#define GOSSIP_SSTE "Yes elder. Tell me more of the book."

bool GossipHello_npc_saikkal_the_elder(Player *player, Creature *_Creature)
{
    if (player->GetQuestStatus(10980) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10794, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_saikkal_the_elder(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_SSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(10795, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->TalkedToCreature(_Creature->GetEntry(), _Creature->GetGUID());
            player->SEND_GOSSIP_MENU(10796, _Creature->GetGUID());
            break;
    }
    return true;
}

/*######
## npc_skyguard_handler_irena
######*/

#define GOSSIP_SKYGUARD "Fly me to Skettis please"

bool GossipHello_npc_skyguard_handler_irena(Player *player, Creature *_Creature )
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );

    if (player->GetReputationMgr().GetRank(1031) >= REP_HONORED)
        player->ADD_GOSSIP_ITEM( 2, GOSSIP_SKYGUARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_skyguard_handler_irena(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();

        std::vector<uint32> nodes;

        nodes.resize(2);
        nodes[0] = 172;                                     //from ogri'la
        nodes[1] = 171;                                     //end at skettis
        player->ActivateTaxiPathTo(nodes);                  //TaxiPath 706
    }
    return true;
}

/*######
## npc_bloodmaul_brutebane
######*/

enum eBloodmaul
{
    NPC_OGRE_BRUTE                       = 19995,
    NPC_QUEST_CREDIT                     = 21241,
    GO_KEG                               = 184315
};

struct npc_bloodmaul_brutebaneAI : public ScriptedAI
{
    npc_bloodmaul_brutebaneAI(Creature *c) : ScriptedAI(c){}

    void IsSummonedBy(Unit *pOwner)
    {
       if (Creature* pOgre = GetClosestCreatureWithEntry(me, NPC_OGRE_BRUTE, 50.0f))
       {
           pOgre->SetReactState(REACT_DEFENSIVE);
           pOgre->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()-1, m_creature->GetPositionY()+1, m_creature->GetPositionZ());

           if (Player *plOwner = pOwner->GetCharmerOrOwnerPlayerOrPlayerItself())
               plOwner->KilledMonster(NPC_QUEST_CREDIT, pOgre->GetGUID());
       }
    }

    void UpdateAI(const uint32 uiDiff){}
};

CreatureAI* GetAI_npc_bloodmaul_brutebane(Creature* pCreature)
{
    return new npc_bloodmaul_brutebaneAI (pCreature);
}

/*######
## npc_ogre_brute
######*/

struct npc_ogre_bruteAI : public ScriptedAI
{
    npc_ogre_bruteAI(Creature *c) : ScriptedAI(c){}

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || id != 1)
            return;

        if (GameObject* pKeg = FindGameObject(GO_KEG, 20.0, me))
            pKeg->Delete();

        me->HandleEmoteCommand(7);
        me->SetReactState(REACT_AGGRESSIVE);

        if (!me->getVictim())
            me->GetMotionMaster()->MoveTargetedHome();
        else
            me->GetMotionMaster()->MoveChase(me->getVictim());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ogre_brute(Creature* pCreature)
{
    return new npc_ogre_bruteAI(pCreature);
}

/*######
## npc_vim_bunny
######*/

#define SPELL_PENTAGRAM 39921
#define GO_FLAME_CIRCLE 185555
#define PENTAGRAM_TRIGGER 23040
#define MAIN_SPAWN 22911

struct HELLGROUND_DLL_DECL npc_vim_bunnyAI : public ScriptedAI
{
    npc_vim_bunnyAI(Creature *c) : ScriptedAI(c){}

    uint32 CheckTimer;

    void Reset()
    {
        CheckTimer = 1000;
    }

    bool GetPlayer()
    {
        Player *pPlayer = NULL;
        Hellground::AnyPlayerInObjectRangeCheck p_check(m_creature, 3.0f);
        Hellground::ObjectSearcher<Player, Hellground::AnyPlayerInObjectRangeCheck> searcher(pPlayer, p_check);

        Cell::VisitAllObjects(m_creature, searcher, 3.0f);
        return pPlayer;
    }

    void UpdateAI(const uint32 diff)
    {
        if(CheckTimer < diff)
        {
            if(me->GetDistance2d(3279.80f, 4639.76f) < 5.0)
            {
                if(GetClosestCreatureWithEntry(me, MAIN_SPAWN, 80.0f))
                {
                    CheckTimer = 20000;
                    return;
                }

                // WE NEED HERE TO BE SURE THAT SPAWN IS VALID !
                std::list<Creature*> triggers = FindAllCreaturesWithEntry(PENTAGRAM_TRIGGER, 50.0);
                if(triggers.size() >= 5)
                {
                    DoSpawnCreature(MAIN_SPAWN,0,0,0,0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                    CheckTimer = 20000;
                    return;
                }
                CheckTimer = 1000;
            }
            else
            {
                if(GetPlayer())
                {
                    Unit *temp = DoSpawnCreature(PENTAGRAM_TRIGGER,0,0,2.0,0, TEMPSUMMON_TIMED_DESPAWN, 15000);
                    temp->CastSpell(temp, SPELL_PENTAGRAM, false);
                    CheckTimer = 16000;
                    return;
                }
                CheckTimer = 2000;
            }
        }
        else
            CheckTimer -= diff;
    }
};

CreatureAI* GetAI_npc_vim_bunny(Creature *_Creature)
{
    return new npc_vim_bunnyAI (_Creature);
}

/*######
## Wrangle Some Aether Rays
######*/

// Spells
#define EMOTE_WEAK     "appears ready to be wrangled."
#define SPELL_SUMMON_WRANGLED   40917
#define SPELL_CHANNEL           40626

struct HELLGROUND_DLL_DECL mob_aetherrayAI : public ScriptedAI
{

    mob_aetherrayAI(Creature *c) : ScriptedAI(c) {}

    bool Weak;
    uint64 PlayerGUID;

    void Reset()
    {
        Weak = false;
    }

    void EnterCombat(Unit *who)
    {
        if(Player *player = who->GetCharmerOrOwnerPlayerOrPlayerItself())
            PlayerGUID = player->GetGUID();
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->GetMotionMaster()->MoveFollow(Unit::GetPlayer(PlayerGUID), PET_FOLLOW_DIST, m_creature->GetFollowAngle());
    }

    void UpdateAI(const uint32 diff)
    {

    if (!UpdateVictim())
            return;

    if(PlayerGUID) // start: support for quest 11066 and 11065
        {
            Player* target = Unit::GetPlayer(PlayerGUID);

            if(target && !Weak && m_creature->GetHealth() < (m_creature->GetMaxHealth() / 100 * 40)
                && (target->GetQuestStatus(11066) == QUEST_STATUS_INCOMPLETE || target->GetQuestStatus(11065) == QUEST_STATUS_INCOMPLETE))
            {
                me->MonsterTextEmote(EMOTE_WEAK, 0, false);
                Weak = true;
            }

            if(Weak && m_creature->HasAura(40856, 0))
            {
                me->CastSpell(target, SPELL_SUMMON_WRANGLED, false);
                target->KilledMonster(23343, m_creature->GetGUID());
                m_creature->AttackStop(); // delete the normal mob
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->RemoveCorpse();
            }
        }

        DoMeleeAttackIfReady();
    }


};

CreatureAI* GetAI_mob_aetherray(Creature *_Creature)
{
    return new mob_aetherrayAI (_Creature);
}

/*####
# npc_wildlord_antelarion
####*/

#define GOSSIP_ITEM_WILDLORD "Restore Felsworn Gas Mask."

bool GossipHello_npc_wildlord_antelarion(Player *player, Creature *_Creature)
{
    if (_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );
        if(player->GetQuestStatus(10819) || player->GetQuestStatus(10820) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(31366,1))
           player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_WILDLORD, GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO );
           player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_wildlord_antelarion(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if( action == GOSSIP_SENDER_INFO )
    {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 31366, 1);
            if (msg == EQUIP_ERR_OK)
            {
                 Item* item = player->StoreNewItem(dest, 31366, true);
                 player->SendNewItem(item,1,true,false,true);
            }
    player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*########
# npc_kolphis_darkscale
#########*/

#define GOSSIP_ITEM_KOLPHIS1 "I'm fine, thank you. You asked for me?"
#define GOSSIP_ITEM_KOLPHIS2 "Oh, it's not my fault. I can assure you."
#define GOSSIP_ITEM_KOLPHIS3 "Um, no...no, I don't want that at all."
#define GOSSIP_ITEM_KOLPHIS4 "Impressive. When do we attack?"
#define GOSSIP_ITEM_KOLPHIS5 "Absolutely!"

bool GossipHello_npc_kolphis_darkscale(Player *player, Creature *_Creature)
{
    if(_Creature->isQuestGiver())
        player->PrepareQuestMenu( _Creature->GetGUID() );
    if(player->GetQuestStatus(10722) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KOLPHIS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1 );
        player->SEND_GOSSIP_MENU(25019, _Creature->GetGUID());
return true;
}

bool GossipSelect_npc_kolphis_darkscale(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
  switch (action)
  {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KOLPHIS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(25020, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KOLPHIS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(25021, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KOLPHIS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(25022, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM_KOLPHIS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            player->SEND_GOSSIP_MENU(25023, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->CompleteQuest(10722);
            player->SEND_GOSSIP_MENU(25024, _Creature->GetGUID());
            break;
  }
return true;
}

/*#########
# npc_prophecy_trigger
#########*/

struct HELLGROUND_DLL_DECL npc_prophecy_triggerAI : public ScriptedAI
{
    npc_prophecy_triggerAI(Creature *c) : ScriptedAI(c)
    {
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (plWho->GetQuestStatus(10607) == QUEST_STATUS_INCOMPLETE && plWho->HasAura(37466,0) && plWho->GetDistance(me) < 20.0f)
            {
                switch(me->GetEntry())
                {
                    case 22798:
                        me->Whisper("From the darkest night shall rise again the raven, shall take flight in the shadows, shall reveal the nature of its kind. Prepare yourself for its coming, for the faithful shall be elevated to take flight with the raven, the rest be forgotten to walk upon the ground, clipped wings and shame.", plWho->GetGUID());
                        break;
                    case 22799:
                        me->Whisper("Steel your minds and guard your thoughts. The dark wings will cloud and consume the minds of the weak, a flock of thralls whose feet may never leave the ground.", plWho->GetGUID());
                        break;
                    case 22800:
                        me->Whisper("The old blood will flow once again with the coming of the raven, the return of the darkness in the skies. Scarlet night, and the rise of the old.", plWho->GetGUID());
                        break;
                    case 22801:
                        me->Whisper("The raven was struck down once for flying too high, unready. The eons have prepared the Dark Watcher for its ascent, to draw the dark cloak across the horizon.", plWho->GetGUID());
                        break;
                }

                plWho->KilledMonster(me->GetEntry(), me->GetGUID());
                me->DisappearAndDie();
            }
        }
    }
};

CreatureAI* GetAI_npc_prophecy_trigger(Creature *_Creature)
{
    return new npc_prophecy_triggerAI(_Creature);
}

/*#########
# go_thunderspike
# UPDATE `gameobject_template` SET `ScriptName` = "go_thunderspike" WHERE `entry` = 184729;
#########*/

#define Q_THE_THUNDERSPIKE 10526
#define GOR_GRIMGUT_ENTRY  21319

bool GOUse_go_thunderspike(Player *player, GameObject* _GO)
{
    if (player->GetQuestStatus(Q_THE_THUNDERSPIKE) == QUEST_STATUS_INCOMPLETE)
    {
        // to prevent spawn spam :)
        if (Creature *pGor = GetClosestCreatureWithEntry(player, GOR_GRIMGUT_ENTRY, 50.0f))
        {
            if (!pGor->getVictim() && pGor->isAlive())
                pGor->AI()->AttackStart(player);

            return false;
        }

        Position dest;
        player->GetValidPointInAngle(dest, 5.0f, frand(0.0f, 2*M_PI), true);

        if (Creature* pGor = player->SummonCreature(GOR_GRIMGUT_ENTRY, dest.x, dest.y, dest.z, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
            pGor->AI()->AttackStart(player);
    }

    return false;
}

/*######
## Simon Game - An Apexis Relic
######*/

enum SimonGame
{
    NPC_SIMON_BUNNY                 = 22923,
    NPC_APEXIS_GUARDIAN             = 22275,

    GO_APEXIS_RELIC                 = 185890,
    GO_APEXIS_MONUMENT              = 185944,
    GO_AURA_BLUE                    = 185872,
    GO_AURA_GREEN                   = 185873,
    GO_AURA_RED                     = 185874,
    GO_AURA_YELLOW                  = 185875,

    GO_BLUE_CLUSTER_DISPLAY         = 7369,
    GO_GREEN_CLUSTER_DISPLAY        = 7371,
    GO_RED_CLUSTER_DISPLAY          = 7373,
    GO_YELLOW_CLUSTER_DISPLAY       = 7375,
    GO_BLUE_CLUSTER_DISPLAY_LARGE   = 7364,
    GO_GREEN_CLUSTER_DISPLAY_LARGE  = 7365,
    GO_RED_CLUSTER_DISPLAY_LARGE    = 7366,
    GO_YELLOW_CLUSTER_DISPLAY_LARGE = 7367,

    SPELL_PRE_GAME_BLUE             = 40176,
    SPELL_PRE_GAME_GREEN            = 40177,
    SPELL_PRE_GAME_RED              = 40178,
    SPELL_PRE_GAME_YELLOW           = 40179,
    SPELL_VISUAL_BLUE               = 40244,
    SPELL_VISUAL_GREEN              = 40245,
    SPELL_VISUAL_RED                = 40246,
    SPELL_VISUAL_YELLOW             = 40247,

    SOUND_BLUE                      = 11588,
    SOUND_GREEN                     = 11589,
    SOUND_RED                       = 11590,
    SOUND_YELLOW                    = 11591,
    SOUND_DISABLE_NODE              = 11758,

    SPELL_AUDIBLE_GAME_TICK         = 40391,
    SPELL_VISUAL_START_PLAYER_LEVEL = 40436,
    SPELL_VISUAL_START_AI_LEVEL     = 40387,

    SPELL_BAD_PRESS_TRIGGER         = 41241,
    SPELL_BAD_PRESS_DAMAGE          = 40065,
    SPELL_REWARD_BUFF_1             = 40310,
    SPELL_REWARD_BUFF_2             = 40311,
    SPELL_REWARD_BUFF_3             = 40312,
};

enum SimonEvents
{
    EVENT_SIMON_SETUP_PRE_GAME         = 1,
    EVENT_SIMON_PLAY_SEQUENCE          = 2,
    EVENT_SIMON_RESET_CLUSTERS         = 3,
    EVENT_SIMON_PERIODIC_PLAYER_CHECK  = 4,
    EVENT_SIMON_TOO_LONG_TIME          = 5,
    EVENT_SIMON_GAME_TICK              = 6,
    EVENT_SIMON_ROUND_FINISHED         = 7,

    ACTION_SIMON_CORRECT_FULL_SEQUENCE = 8,
    ACTION_SIMON_WRONG_SEQUENCE        = 9,
    ACTION_SIMON_ROUND_FINISHED        = 10,
};

enum SimonColors
{
    SIMON_BLUE          = 0,
    SIMON_RED           = 1,
    SIMON_GREEN         = 2,
    SIMON_YELLOW        = 3,
    SIMON_MAX_COLORS    = 4,
};

struct npc_simon_bunnyAI : public ScriptedAI
{
    npc_simon_bunnyAI(Creature* creature) : ScriptedAI(creature) { }

    bool large;
    bool listening;
    uint8 gameLevel;
    uint8 fails;
    uint8 gameTicks;
    uint64 playerGUID;
    uint32 clusterIds[SIMON_MAX_COLORS];
    float zCoordCorrection;
    float searchDistance;
    EventMap _events;
    std::list<uint8> colorSequence, playableSequence, playerSequence;

    void UpdateAI(const uint32 diff)
    {
        _events.Update(diff);
        switch (_events.ExecuteEvent())
        {
            case EVENT_SIMON_PERIODIC_PLAYER_CHECK:
                if (!CheckPlayer())
                    ResetNode();
                else
                    _events.ScheduleEvent(EVENT_SIMON_PERIODIC_PLAYER_CHECK, 2000);
                break;
            case EVENT_SIMON_SETUP_PRE_GAME:
                SetUpPreGame();
                _events.CancelEvent(EVENT_SIMON_GAME_TICK);
                _events.ScheduleEvent(EVENT_SIMON_PLAY_SEQUENCE, 1000);
                break;
            case EVENT_SIMON_PLAY_SEQUENCE:
                if (!playableSequence.empty())
                {
                    PlayNextColor();
                    _events.ScheduleEvent(EVENT_SIMON_PLAY_SEQUENCE, 1500);
                }
                else
                {
                    listening = true;
                    AddSpellToCast(SPELL_VISUAL_START_PLAYER_LEVEL, CAST_NULL);
                    playerSequence.clear();
                    PrepareClusters();
                    gameTicks = 0;
                    _events.ScheduleEvent(EVENT_SIMON_GAME_TICK, 3000);
                }
                break;
            case EVENT_SIMON_GAME_TICK:
                AddSpellToCast(SPELL_AUDIBLE_GAME_TICK, CAST_NULL);
                
                if (gameTicks > gameLevel)
                    _events.ScheduleEvent(EVENT_SIMON_TOO_LONG_TIME, 500);
                else
                    _events.ScheduleEvent(EVENT_SIMON_GAME_TICK, 3000);
                gameTicks++;
                break;
            case EVENT_SIMON_RESET_CLUSTERS:
                PrepareClusters(true);
                break;
            case EVENT_SIMON_TOO_LONG_TIME:
                DoAction(ACTION_SIMON_WRONG_SEQUENCE);
                break;
            case EVENT_SIMON_ROUND_FINISHED:
                DoAction(ACTION_SIMON_ROUND_FINISHED);
                break;
        }
        CastNextSpellIfAnyAndReady();
    }

    void DoAction(const int32 action)
    {
        switch (action)
        {
            case ACTION_SIMON_ROUND_FINISHED:
                listening = false;
                AddSpellToCast(SPELL_VISUAL_START_AI_LEVEL, CAST_NULL);
                GiveRewardForLevel(gameLevel);
                _events.CancelEventsByGCD(0);
                if (gameLevel == 10)
                    ResetNode();
                else
                    _events.ScheduleEvent(EVENT_SIMON_SETUP_PRE_GAME, 1000);
                break;
            case ACTION_SIMON_CORRECT_FULL_SEQUENCE:
                gameLevel++;
                DoAction(ACTION_SIMON_ROUND_FINISHED);
                break;
            case ACTION_SIMON_WRONG_SEQUENCE:
                GivePunishment();
                DoAction(ACTION_SIMON_ROUND_FINISHED);
                break;
        }
    }

    // Called by color clusters script (go_simon_cluster) and used for knowing the button pressed by player
    void SetData(uint32 type, uint32 /*data*/)
    {
        if (!listening)
            return;

        uint8 pressedColor;

        if (type == clusterIds[SIMON_RED])
            pressedColor = SIMON_RED;
        else if (type == clusterIds[SIMON_BLUE])
            pressedColor = SIMON_BLUE;
        else if (type == clusterIds[SIMON_GREEN])
            pressedColor = SIMON_GREEN;
        else if (type == clusterIds[SIMON_YELLOW])
            pressedColor = SIMON_YELLOW;

        PlayColor(pressedColor);
        playerSequence.push_back(pressedColor);
        _events.ScheduleEvent(EVENT_SIMON_RESET_CLUSTERS, 500);
        CheckPlayerSequence();
    }

    // Used for getting involved player guid. Parameter id is used for defining if is a large(Monument) or small(Relic) node
    void SetGUID(uint64 guid, int32 id)
    {
        ClearCastQueue();

        me->SetLevitate(true);

        large = (bool)id;
        playerGUID = guid;
        StartGame();
    }

    /*
    Resets all variables and also find the ids of the four closests color clusters, since every simon
    node have diferent ids for clusters this is absolutely NECESSARY.
    */
    void StartGame()
    {
        listening = false;
        gameLevel = 0;
        fails = 0;
        gameTicks = 0;
        zCoordCorrection = large ? 8.0f : 2.75f;
        searchDistance = large ? 13.0f : 5.0f;
        colorSequence.clear();
        playableSequence.clear();
        playerSequence.clear();
        me->SetFloatValue(OBJECT_FIELD_SCALE_X, large ? 2 : 1);

        std::list<GameObject*> ClusterList;
        Hellground::AllGameObjectsInRange objects(me, searchDistance);
        Hellground::ObjectListSearcher<GameObject, Hellground::AllGameObjectsInRange> searcher(ClusterList, objects);
        Cell::VisitGridObjects(me, searcher, searchDistance);

        for (std::list<GameObject*>::const_iterator i = ClusterList.begin(); i != ClusterList.end(); ++i)
        {
            if (GameObject* go = (*i)->ToGameObject())
            {
                // We are checking for displayid because all simon nodes have 4 clusters with different entries
                if (large)
                {
                    switch (go->GetGOInfo()->displayId)
                    {
                        case GO_BLUE_CLUSTER_DISPLAY_LARGE: clusterIds[SIMON_BLUE] = go->GetEntry(); break;
                        case GO_RED_CLUSTER_DISPLAY_LARGE: clusterIds[SIMON_RED] = go->GetEntry(); break;
                        case GO_GREEN_CLUSTER_DISPLAY_LARGE: clusterIds[SIMON_GREEN] = go->GetEntry(); break;
                        case GO_YELLOW_CLUSTER_DISPLAY_LARGE: clusterIds[SIMON_YELLOW] = go->GetEntry(); break;
                    }
                }
                else
                {
                    switch (go->GetGOInfo()->displayId)
                    {
                        case GO_BLUE_CLUSTER_DISPLAY: clusterIds[SIMON_BLUE] = go->GetEntry(); break;
                        case GO_RED_CLUSTER_DISPLAY: clusterIds[SIMON_RED] = go->GetEntry(); break;
                        case GO_GREEN_CLUSTER_DISPLAY: clusterIds[SIMON_GREEN] = go->GetEntry(); break;
                        case GO_YELLOW_CLUSTER_DISPLAY: clusterIds[SIMON_YELLOW] = go->GetEntry(); break;
                    }
                }
            }
        }

        _events.Reset();
        _events.ScheduleEvent(EVENT_SIMON_ROUND_FINISHED, 1000);
        _events.ScheduleEvent(EVENT_SIMON_PERIODIC_PLAYER_CHECK, 2000);

        if (GameObject* relic = GetClosestGameObjectWithEntry(me, large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
            relic->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOTSELECTABLE);
    }

    // Called when despawning the bunny. Sets all the node GOs to their default states.
    void ResetNode()
    {
        DoPlaySoundToSet(me, SOUND_DISABLE_NODE);

        for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
            if (GameObject* cluster = GetClosestGameObjectWithEntry(me, clusterIds[clusterId], searchDistance))
                cluster->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOTSELECTABLE);

        for (uint32 auraId = GO_AURA_BLUE; auraId <= GO_AURA_YELLOW; auraId++)
            if (GameObject* auraGo = GetClosestGameObjectWithEntry(me, auraId, searchDistance))
                auraGo->RemoveFromWorld();

        if (GameObject* relic = GetClosestGameObjectWithEntry(me, large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
            relic->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOTSELECTABLE);

        me->ForcedDespawn(1000);
    }

    /*
    Called on every button click of player. Adds the clicked color to the player created sequence and
    checks if it corresponds to the AI created sequence. If so, incremente gameLevel and start a new
    round, if not, give punishment and restart current level.
    */
    void CheckPlayerSequence()
    {
        bool correct = true;
        if (playerSequence.size() <= colorSequence.size())
            for (std::list<uint8>::const_iterator i = playerSequence.begin(), j = colorSequence.begin(); i != playerSequence.end(); ++i, ++j)
                if ((*i) != (*j))
                    correct = false;

        if (correct && (playerSequence.size() == colorSequence.size()))
            DoAction(ACTION_SIMON_CORRECT_FULL_SEQUENCE);
        else if (!correct)
            DoAction(ACTION_SIMON_WRONG_SEQUENCE);
    }

    /*
    Generates a random sequence of colors depending on the gameLevel. We also copy this sequence to
    the playableSequence wich will be used when playing the sequence to the player.
    */
    void GenerateColorSequence()
    {
        colorSequence.clear();
        for (uint8 i = 0; i <= gameLevel; i++)
            colorSequence.push_back(RAND(SIMON_BLUE, SIMON_RED, SIMON_GREEN, SIMON_YELLOW));

        for (std::list<uint8>::const_iterator i = colorSequence.begin(); i != colorSequence.end(); ++i)
            playableSequence.push_back(*i);
    }


    // Remove any existant glowing auras over clusters and set clusters ready for interating with them.
    void PrepareClusters(bool clustersOnly = false)
    {
        for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
            if (GameObject* cluster = GetClosestGameObjectWithEntry(me, clusterIds[clusterId], searchDistance))
                cluster->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOTSELECTABLE);

        if (clustersOnly)
            return;

        for (uint32 auraId = GO_AURA_BLUE; auraId <= GO_AURA_YELLOW; auraId++)
            if (GameObject* auraGo = GetClosestGameObjectWithEntry(me, auraId, searchDistance))
                auraGo->RemoveFromWorld();
    }

    /*
    Called when AI is playing the sequence for player. We cast the visual spell and then remove the
    casted color from the casting sequence.
    */
    void PlayNextColor()
    {
        PlayColor(*playableSequence.begin());
        playableSequence.erase(playableSequence.begin());
    }

    // Casts a spell and plays a sound depending on parameter color.
    void PlayColor(uint8 color)
    {
        switch (color)
        {
            case SIMON_BLUE:
                AddSpellToCast(SPELL_VISUAL_BLUE, CAST_NULL);
                DoPlaySoundToSet(me, SOUND_BLUE);
                break;
            case SIMON_GREEN:
                AddSpellToCast(SPELL_VISUAL_GREEN, CAST_NULL);
                DoPlaySoundToSet(me, SOUND_GREEN);
                break;
            case SIMON_RED:
                AddSpellToCast(SPELL_VISUAL_RED, CAST_NULL);
                DoPlaySoundToSet(me, SOUND_RED);
                break;
            case SIMON_YELLOW:
                AddSpellToCast(SPELL_VISUAL_YELLOW, CAST_NULL);
                DoPlaySoundToSet(me, SOUND_YELLOW);
                break;
        }
    }

    /*
    Creates the transparent glowing auras on every cluster of this node.
    After calling this function bunny is teleported to the center of the node.
    */
    void SetUpPreGame()
    {
        for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
        {
            if (GameObject* cluster = GetClosestGameObjectWithEntry(me,clusterIds[clusterId], 2.0f*searchDistance))
            {
                cluster->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOTSELECTABLE);

                // break since we don't need glowing auras for large clusters
                if (large)
                    break;

                float x, y, z, o = 0.0f;
                cluster->GetPosition(x, y, z);
                me->NearTeleportTo(x, y, z, o);

                uint32 preGameSpellId;
                if (cluster->GetEntry() == clusterIds[SIMON_RED])
                    preGameSpellId = SPELL_PRE_GAME_RED;
                else if (cluster->GetEntry() == clusterIds[SIMON_BLUE])
                    preGameSpellId = SPELL_PRE_GAME_BLUE;
                else if (cluster->GetEntry() == clusterIds[SIMON_GREEN])
                    preGameSpellId = SPELL_PRE_GAME_GREEN;
                else if (cluster->GetEntry() == clusterIds[SIMON_YELLOW])
                    preGameSpellId = SPELL_PRE_GAME_YELLOW;
                else
                    break;

                me->CastSpell(cluster, preGameSpellId, true);
            }
        }

        if (GameObject* relic = GetClosestGameObjectWithEntry(me, large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
        {
            float x, y, z, o = 0.0f;
            relic->GetPosition(x, y, z);
            me->NearTeleportTo(x, y, z + zCoordCorrection, o);
        }

        GenerateColorSequence();
    }

    // Handles the spell rewards. The spells also have the QuestCompleteEffect, so quests credits are working.
    void GiveRewardForLevel(uint8 level)
    {
        uint32 rewSpell;
        switch (level)
        {
            case 6:
                if (large)
                    GivePunishment();
                else
                    rewSpell = SPELL_REWARD_BUFF_1;
                break;
            case 8:
                rewSpell = SPELL_REWARD_BUFF_2;
                break;
            case 10:
                rewSpell = SPELL_REWARD_BUFF_3;
                break;
            default:
                rewSpell = 0;
        }

        if (rewSpell)
            if (Player* player = me->GetPlayer(playerGUID))
                DoCast(player, rewSpell, true);
    }

    /*
    Depending on the number of failed pushes for player the damage of the spell scales, so we first
    cast the spell on the target that hits for 50 and shows the visual and then forces the player
    to cast the damaging spell on it self with the modified basepoints.
    4 fails = death.
    On large nodes punishment and reward are the same, summoning the Apexis Guardian.
    */
    void GivePunishment()
    {
        if (large)
        {
            if (Player* player = me->GetPlayer(playerGUID))
                if (Creature* guardian = me->SummonCreature(NPC_APEXIS_GUARDIAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - zCoordCorrection, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000))
                    guardian->AI()->AttackStart(player);

            ResetNode();
        }
        else
        {
            fails++;

            if (Player* player = me->GetPlayer(playerGUID))
                DoCast(player, SPELL_BAD_PRESS_TRIGGER, true);

            if (fails >= 4)
                ResetNode();
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell)
    {
        // Cast SPELL_BAD_PRESS_DAMAGE with scaled basepoints when the visual hits the target.
        // Need Fix: When SPELL_BAD_PRESS_TRIGGER hits target it triggers spell SPELL_BAD_PRESS_DAMAGE by itself
        // so player gets damage equal to calculated damage  dbc basepoints for SPELL_BAD_PRESS_DAMAGE (~50)
        if (spell->Id == SPELL_BAD_PRESS_TRIGGER)
        {
            int32 bp = (int32)((float)(fails)*0.33f*target->GetMaxHealth());
            target->CastCustomSpell(target, SPELL_BAD_PRESS_DAMAGE, &bp, NULL, NULL, true);
        }
    }

    // Checks if player has already die or has get too far from the current node
    bool CheckPlayer()
    {
        if (Player* player = me->GetPlayer(playerGUID))
        {
            if (player->isDead())
                return false;

            if (player->GetDistance2d(me) >= 2.0f*searchDistance)
            {
                GivePunishment();
                return false;
            }
        }
        else
            return false;

        return true;
    }
};

CreatureAI* GetAI(Creature* creature)
{
    return new npc_simon_bunnyAI(creature);
}

bool OnGossipHello_go_simon_cluster(Player* player, GameObject* go)
{
    if (Creature* bunny = GetClosestCreatureWithEntry(go, NPC_SIMON_BUNNY, 12.0f, true))
        bunny->AI()->SetData(go->GetEntry(), 0);

    player->CastSpell(player, go->GetGOInfo()->goober.spellId, true);
    go->AddUse();
    return true;
}

enum ApexisRelic
{
    QUEST_CRYSTALS            = 11025,
    GOSSIP_TEXT_ID            = 10948,

    ITEM_APEXIS_SHARD         = 32569,
    SPELL_TAKE_REAGENTS_SOLO  = 41145,
    SPELL_TAKE_REAGENTS_GROUP = 41146,
};

#define GOSSIP_ITEM_1 "Insert shard"
bool OnGossipHello_go_apexis_relic(Player* player, GameObject* go)
{
    bool large = (go->GetEntry() == GO_APEXIS_MONUMENT);

    if (player->HasItemCount(ITEM_APEXIS_SHARD, large ? 35 : 1))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->Kill(player, false);
    player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID, go->GetGUID());
    return true;
}

bool OnGossipSelect_go_apexis_relic(Player* player, GameObject* go, uint32 /*sender*/, uint32 /*action*/)
{
    player->CLOSE_GOSSIP_MENU();

    bool large = (go->GetEntry() == GO_APEXIS_MONUMENT);
    if (player->HasItemCount(ITEM_APEXIS_SHARD, large ? 35 : 1))
    {
        player->CastSpell(player, large ? SPELL_TAKE_REAGENTS_GROUP : SPELL_TAKE_REAGENTS_SOLO, false);

        if (Creature* bunny = player->SummonCreature(NPC_SIMON_BUNNY, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
             bunny->AI()->SetGUID(player->GetGUID(), large);
    }

    return true;
}

/*######
## AddSC
######*/

void AddSC_blades_edge_mountains()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="mobs_nether_drake";
    newscript->GetAI = &GetAI_mobs_nether_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_daranelle";
    newscript->GetAI = &GetAI_npc_daranelle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_overseer_nuaar";
    newscript->pGossipHello = &GossipHello_npc_overseer_nuaar;
    newscript->pGossipSelect = &GossipSelect_npc_overseer_nuaar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_saikkal_the_elder";
    newscript->pGossipHello = &GossipHello_npc_saikkal_the_elder;
    newscript->pGossipSelect = &GossipSelect_npc_saikkal_the_elder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_skyguard_handler_irena";
    newscript->pGossipHello =  &GossipHello_npc_skyguard_handler_irena;
    newscript->pGossipSelect = &GossipSelect_npc_skyguard_handler_irena;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bloodmaul_brutebane";
    newscript->GetAI = &GetAI_npc_bloodmaul_brutebane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ogre_brute";
    newscript->GetAI = &GetAI_npc_ogre_brute;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_vim_bunny";
    newscript->GetAI = &GetAI_npc_vim_bunny;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_aetherray";
    newscript->GetAI = &GetAI_mob_aetherray;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_wildlord_antelarion";
    newscript->pGossipHello = &GossipHello_npc_wildlord_antelarion;
    newscript->pGossipSelect = &GossipSelect_npc_wildlord_antelarion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_kolphis_darkscale";
    newscript->pGossipHello = &GossipHello_npc_kolphis_darkscale;
    newscript->pGossipSelect = &GossipSelect_npc_kolphis_darkscale;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_prophecy_trigger";
    newscript->GetAI = &GetAI_npc_prophecy_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_thunderspike";
    newscript->pGOUse = &GOUse_go_thunderspike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_apexis_relic";
    newscript->pGOUse = &OnGossipHello_go_apexis_relic;
    newscript->pGossipSelectGO = &OnGossipSelect_go_apexis_relic;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_simon_cluster";
    newscript->pGOUse = &OnGossipHello_go_simon_cluster;
    newscript->RegisterSelf();
}
