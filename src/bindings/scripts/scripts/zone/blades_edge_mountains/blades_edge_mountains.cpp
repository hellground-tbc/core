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

struct TRINITY_DLL_DECL mobs_nether_drakeAI : public ScriptedAI
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

struct TRINITY_DLL_DECL npc_daranelleAI : public ScriptedAI
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

struct TRINITY_DLL_DECL npc_vim_bunnyAI : public ScriptedAI
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
        Trinity::AnyPlayerInObjectRangeCheck p_check(m_creature, 3.0f);
        Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(pPlayer, p_check);

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

struct TRINITY_DLL_DECL mob_aetherrayAI : public ScriptedAI
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
        summoned->GetMotionMaster()->MoveFollow(Unit::GetPlayer(PlayerGUID), PET_FOLLOW_DIST, m_creature->GetFollowAngle(), MOTION_SLOT_ACTIVE);
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

struct TRINITY_DLL_DECL npc_prophecy_triggerAI : public ScriptedAI
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

enum usedSpells
{
    SPELL_SG_SWITCHED_ON    = 40494,
    SPELL_SG_SWITCHED_OF    = 40495,

    SPELL_SG_SWITCHED_ON_DISABLE_OFF = 40512,
    SPELL_SG_SWITCHED_ON_ENABLE_OFF  = 40499,

    SPELL_SG_SUPPRESS_MOBS_TEST = 41154,

    SPELL_SG_START_GAME_SOLO           = 41137, // trigger: 41145
    SPELL_SG_START_GAME_SOLO_REAGENTS  = 41145, // 1 shard + trigger: 39993, 41153

    SPELL_SG_START_GAME_PARTY          = 41139, // trigger: 41139
    SPELL_SG_START_GAME_PARTY_REAGENTS = 41146, // 35 shards + trigger: 39993, 41153

    SPELL_SG_REWARD_OFFSET = 41259,             // Reward all in spell range ?
    SPELL_SG_GROUP_REWARD_PRE_SPELL = 41952,    // triggers: 41124

    SPELL_SG_AUDIBLE_OFFSET = 41407,

    SPELL_SG_PRE_GAME_YELLOW_AURA      = 40179, // activate GOs, trigger: 40289
    SPELL_SG_PRE_GAME_S_YELLOW_A_SMALL = 40289, // summons: yellow aura GO
    SPELL_SG_PRE_GAME_S_YELLOW_A_LARGE = 41110, // activate GOs, summons: large yellow aura GO?

    SPELL_SG_PRE_GAME_RED_AURA      = 40178, // activate GOs, trigger: 40288
    SPELL_SG_PRE_GAME_S_RED_A_SMALL = 40288, // summons: red aura GO
    SPELL_SG_PRE_GAME_S_RED_A_LARGE = 41111, // activate GOs, summons: large red aura GO?

    SPELL_SG_PRE_GAME_GREEN_AURA      = 40177, // activate GOs, trigger: 40287
    SPELL_SG_PRE_GAME_S_GREEN_A_SMALL = 40287, // summons: green aura GO
    SPELL_SG_PRE_GAME_S_GREEN_A_LARGE = 41112, // activate GOs, summons: large green aura GO?

    SPELL_SG_PRE_GAME_BLUE_AURA      = 40176, // activate GOs, trigger: 40281
    SPELL_SG_PRE_GAME_S_BLUE_A_SMALL = 40281, // summons: green aura GO
    SPELL_SG_PRE_GAME_S_BLUE_A_LARGE = 41113, // activate GOs, summons: large green aura GO?

    SPELL_SG_START_YELLOW = 40172,
    SPELL_SG_START_RED = 40169,
    SPELL_SG_START_GREEN = 40171,
    SPELL_SG_START_BLUE = 40170,

    SPELL_SG_START_TIMER = 39993, // should trigger: 40362?
    SPELL_SG_START_TIMER_FORCE_PRIODIC = 40362,

    SPELL_SG_PRE_GAME_TIMER = 40041,

    SPELL_SG_END_YELLOW = 40286,
    SPELL_SG_END_RED = 40285,
    SPELL_SG_END_GREEN = 40284,
    SPELL_SG_END_BLUE = 40283,

    SPELL_SG_END_TIMER = 40359,

    SPELL_SG_BUTTON_PRESSED = 39999,

    SPELL_SG_VISUAL_LEVEL_START_OFFSET = 41235,
    SPELL_SG_VISUAL_LEVEL_START = 40436,

    SPELL_SG_VISUAL_GAME_TICK = 40391,
    SPELL_SG_VISUAL_GAME_TICK_L = 42019,

    SPELL_SG_VISUAL_GAME_START_TEST = 41300,
    SPELL_SG_VISUAL_GAME_START = 40387,

    SPELL_SG_VISUAL_GAME_FAILED_L = 41257,
    SPELL_SG_VISUAL_GAME_FAILED = 40437,

    SPELL_SG_VISUAL_BUTTON_PRESS_YELLOW = 40247,
    SPELL_SG_VISUAL_BUTTON_PRESS_RED = 40246,
    SPELL_SG_VISUAL_BUTTON_PRESS_GREEN = 40245,
    SPELL_SG_VISUAL_BUTTON_PRESS_BLUE = 40244,

    SPELL_SG_CREATE_BUNNY = 40502,
    SPELL_SG_PORT_BUNNY_LARGE = 41090,

    SPELL_SG_GOOD_PRESS = 40063,
    SPELL_SG_BAD_PRESS = 41241, // trigers: 40065

    SPELL_SG_AUDIBLE_BUTTON_PUSH_BLUE = 41398,

    SPELL_APEXIS_VIBRATIONS_QUEST = 40310, // triggers: 40623
};

/*

ONEDAY THERE WILL BE A SCRIPT

*/

enum uNPC
{
    SG_BUNNY = 22923,
    SG_BUNNY_LARGE = 23378,
    SG_SUMMON_TARGET = 23382,
    SIMON_UNIT = 23385,
};

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
}
