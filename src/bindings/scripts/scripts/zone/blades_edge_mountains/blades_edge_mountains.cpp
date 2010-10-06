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
                                                            // | MOVEMENTFLAG_LEVITATING
                            m_creature->AddUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
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

    if (player->GetReputationRank(1031) >= REP_HONORED)
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
    QUEST_GETTING_THE_BLADESPIRE_TANKED  = 10512,
    NPC_OGRE_BRUTE                       = 19995,
    NPC_QUEST_CREDIT                     = 21241,
    GO_KEG                               = 184315
};

struct npc_bloodmaul_brutebaneAI : public ScriptedAI
{
    npc_bloodmaul_brutebaneAI(Creature *c) : ScriptedAI(c)
    {
       if(Unit* Ogre = FindCreature(NPC_OGRE_BRUTE, 50, m_creature))
       {
           ((Creature*)Ogre)->SetReactState(REACT_DEFENSIVE);
           Ogre->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX()-1, m_creature->GetPositionY()+1, m_creature->GetPositionZ());
       }
    }

    uint64 OgreGUID;

    void Reset()
    {
        OgreGUID = 0;
    }

    void UpdateAI(const uint32 uiDiff) {}
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
    npc_ogre_bruteAI(Creature *c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void Reset()
    {
        PlayerGUID = 0;
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || (!who->isAlive())) return;

        if (m_creature->IsWithinDistInMap(who, 50.0f) && (who->GetTypeId() == TYPEID_PLAYER) && ((Player*)who)->GetQuestStatus(QUEST_GETTING_THE_BLADESPIRE_TANKED) == QUEST_STATUS_INCOMPLETE)
        {
            PlayerGUID = who->GetGUID();
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        Player* player = Unit::GetPlayer(PlayerGUID);
        if(id == 1)
        {
            GameObject* Keg = FindGameObject(GO_KEG, 20.0, m_creature);
            if(Keg)
                Keg->Delete();
            m_creature->HandleEmoteCommand(7);
            m_creature->SetReactState(REACT_AGGRESSIVE);
            m_creature->GetMotionMaster()->MoveTargetedHome();
            Unit* Credit = FindCreature(NPC_QUEST_CREDIT, 50, m_creature);
            if(player && Credit)
                player->KilledMonster(NPC_QUEST_CREDIT, Credit->GetGUID());
        }
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
        me->VisitNearbyObject(3.0f, searcher);
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
                std::list<Creature*> triggers = DoFindAllCreaturesWithEntry(PENTAGRAM_TRIGGER, 50.0);
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
    
}