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
SDName: Boss_Felblood_Kaelthas
SD%Complete: 80
SDComment: 
SDCategory: Magisters' Terrace
EndScriptData */

#include "precompiled.h"
#include "def_magisters_terrace.h"
#include "WorldPacket.h"

#define SAY_AGGRO                   -1585023                //This yell should be done when the room is cleared. For now, set it as aggro yell.
#define SAY_PHOENIX                 -1585024
#define SAY_FLAMESTRIKE             -1585025
#define SAY_GRAVITY_LAPSE           -1585026
#define SAY_TIRED                   -1585027
#define SAY_RECAST_GRAVITY          -1585028
#define SAY_DEATH                   -1585029

/*** Spells ***/

// Phase 1 spells

#define SPELL_FIREBALL                (HeroicMode?46164:44189)

#define SPELL_PHOENIX                 44194
#define SPELL_REBIRTH_PHOENIX         44196
#define SPELL_REBIRTH_EGG             44200
#define SPELL_PHOENIX_BURN            44197 //triggers 44198 each 2 sec
#define SPELL_PHOENIX_FIREBALL        (HeroicMode?44237:44202)
#define SPELL_EMBER_BLAST             44199

#define SPELL_TELEPORT_PLAYER         20477 // currently not used, should prevent phoenix kiting

#define SPELL_SUMMON_FLAMESTRIKE      44192
#define SPELL_FLAMESTRIKE_VISUAL      44191
#define SPELL_FLAMESTRIKE            (HeroicMode?46163:44190)

#define SPELL_SHOCK_BARRIER           46165
#define SPELL_PYROBLAST               36819

// Phase 2 spells

#define SPELL_TELEPORT_CENTER         44218 // Teleports boss to the center. 44219-44223 teleports player around Kael, managed by Script Effect
#define SPELL_GRAVITY_LAPSE           44224 // Cast at the beginning of every Gravity Lapse
#define SPELL_GRAVITY_LAPSE_DOT       (HeroicMode?44226:49887)  // managed by Script Effect
#define SPELL_GRAVITY_LAPSE_FLY       44227 // managed by Script Effect

#define SPELL_SUMMON_ARCANE_SPHERE    44265
#define SPELL_ARCANE_SPHERE_PASSIVE   44263 // Passive auras on Arcane Spheres

#define SPELL_POWER_FEEDBACK          (HeroicMode?47109:44233)

// Miscalenous
#define SPELL_ESCAPE_TO_IOQD          46841

/*** Creatures ***/
#define CREATURE_PHOENIX              24674
#define CREATURE_PHOENIX_EGG          24675
#define CREATURE_ARCANE_SPHERE        24708

/** Locations **/
/*float KaelLocations[6][2]=
{
    {148.744659, 181.377426},//center
    {140.823883, 195.403046},//phoenixpos1
    {156.574188, 195.650482},//phoenixpos2
    {149.813, 160.917},//spherepos1
    {167.223, 173.594},//spherepos2
    {130.68, 173.007},//spherepos3
};*/
//#define LOCATION_Z      -16.727455

struct TRINITY_DLL_DECL boss_felblood_kaelthasAI : public Scripted_NoMovementAI
{
    boss_felblood_kaelthasAI(Creature* c) : Scripted_NoMovementAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint32 PhoenixTimer;
    uint32 FlameStrikeTimer;
    uint32 CheckTimer;
    uint32 OutroTimer;
    bool Outro;

    //Heroic only
    uint32 PyroblastTimer;

    uint32 GravityLapseTimer;
    uint32 GravityLapsePhase;

    uint8 Phase;
    // 0 = Not started
    // 1 = Fireball; Summon Phoenix; Flamestrike
    // 2 = Gravity Lapses

    void Reset()
    {
        // TODO: Timers
        PhoenixTimer = 30000;
        FlameStrikeTimer = 25000;
        CheckTimer = 2000;
        OutroTimer = 10000;
        Outro = false;

        PyroblastTimer = 60000;

        GravityLapseTimer = 0;
        GravityLapsePhase = 0;

        Phase = 0;
        SetAutocast(SPELL_FIREBALL, 2000, true);

        if(pInstance)
            pInstance->SetData(DATA_KAELTHAS_EVENT, NOT_STARTED);
        ResetStatues(true);
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if(damage > m_creature->GetHealth() && done_by->GetGUID() != me->GetGUID())
        {
            damage = 0;
            DoScriptText(SAY_DEATH, m_creature);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            if(pInstance)
                pInstance->SetData(DATA_KAELTHAS_EVENT, DONE);
            if(!Outro)
                Outro = true;
        }
    }

    void EnterCombat(Unit *who)
    {
        //DoScriptText(SAY_AGGRO, m_creature);  // move to when trash killed
        DoZoneInCombat();
        if(pInstance)
            pInstance->SetData(DATA_KAELTHAS_EVENT, IN_PROGRESS);
    }

    /*void EnterEvadeMode()
    {
        RemoveGravityLapse();
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreaturesAddon();

        if( m_creature->isAlive() )
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void TeleportPlayersToSelf()
    {
        float x,y,z;
        m_creature->Relocate(KaelLocations[0][0], KaelLocations[0][1], LOCATION_Z, 0);
        Map *map = m_creature->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
                if(i_pl->isAlive())
                {
                    i_pl->CastSpell(i_pl, SPELL_TELEPORT_CENTER, true);
                    m_creature->GetNearPoint(m_creature,x,y,z,5,5,0);
                    i_pl->TeleportTo(m_creature->GetMapId(),x,y,LOCATION_Z,i_pl->GetOrientation());
                }
        }
        DoCast(m_creature, SPELL_TELEPORT_CENTER, true);
    }

    void CastGravityLapseKnockUp()
    {
        Map *map = m_creature->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
                if(i_pl->isAlive())
                // Knockback into the air
                    i_pl->CastSpell(i_pl, SPELL_GRAVITY_LAPSE_DOT, true, 0, 0, m_creature->GetGUID());
        }
    }

    void CastGravityLapseFly()                              // Use Fly Packet hack for now as players can't cast "fly" spells unless in map 530. Has to be done a while after they get knocked into the air...
    {
        Map *map = m_creature->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        Map::PlayerList::const_iterator i;
        for (i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (Player* i_pl = i->getSource())
            {
                if(i_pl->isAlive())
                {
                    // Also needs an exception in spell system.
                    i_pl->CastSpell(i_pl, SPELL_GRAVITY_LAPSE_FLY, true, 0, 0, m_creature->GetGUID());
                    // Use packet hack
                    /*WorldPacket data(12);
                    data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
                    data.append(i_pl->GetPackGUID());
                    data << uint32(0);
                    i_pl->SendMessageToSet(&data, true);
                    i_pl->SetSpeed(MOVE_FLIGHT, 2.0f);
                }
            }
        }
    }*/

    void RemoveGravityLapse()
    {
        Map *map = m_creature->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if(Player* player = i->getSource())
            {
                player->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
                player->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);
            }
        }
    }

    void ResetStatues(bool deactivate)
    {
        if(pInstance)
        {
            if(GameObject* LeftStatue = me->GetMap()->GetGameObject(pInstance->GetData64(DATA_KAEL_STATUE_LEFT)))
                LeftStatue->SetGoState(GOState(deactivate));
            if(GameObject* RightStatue = me->GetMap()->GetGameObject(pInstance->GetData64(DATA_KAEL_STATUE_RIGHT)))
                RightStatue->SetGoState(GOState(deactivate));
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(Outro)
        {
            if(OutroTimer < diff)
                me->DealDamage(me, me->GetHealth());
            OutroTimer -= diff;
            return;
        }

        if(!UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            if(pInstance)
            {
                if(pInstance->GetData(DATA_KAELTHAS_EVENT) == IN_PROGRESS)
                    DoZoneInCombat();
            }
            CheckTimer = 2000;
        }
        else
            CheckTimer -= diff;

        switch(Phase)
        {
            case 0:
            {
                if(HeroicMode)
                {
                    if(PyroblastTimer < diff)
                    {
                        AddSpellToCast(SPELL_SHOCK_BARRIER, CAST_SELF);
                        AddSpellToCast(SPELL_PYROBLAST, CAST_TANK);
                        PyroblastTimer = 60000;
                    }
                    else
                        PyroblastTimer -= diff;
                }

                if(PhoenixTimer < diff)
                {
                    AddSpellToCastWithScriptText(SPELL_PHOENIX, CAST_SELF, SAY_PHOENIX);
                    /*uint32 random = rand()%2 + 1;
                    float x = KaelLocations[random][0];
                    float y = KaelLocations[random][1];
                    Creature* Phoenix = m_creature->SummonCreature(CREATURE_PHOENIX, x, y, LOCATION_Z, 0, TEMPSUMMON_CORPSE_DESPAWN, 60000);
                    if(Phoenix)
                    {
                        Phoenix->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                        SetThreatList(Phoenix);
                        Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1);
                        if(target)
                        {
                            Phoenix->AddThreat(target,1000);
                            Phoenix->Attack(target,true);
                            Phoenix->GetMotionMaster()->MoveChase(target);
                        }
                        else
                        {
                            Phoenix->AddThreat(m_creature->getVictim(),1000);
                            Phoenix->Attack(m_creature->getVictim(),true);
                            Phoenix->GetMotionMaster()->MoveChase(m_creature->getVictim());
                        }
                    }

                    DoScriptText(SAY_PHOENIX, m_creature);*/

                    PhoenixTimer = 40000;
                }
                else
                    PhoenixTimer -= diff;

                if(FlameStrikeTimer < diff)
                {
                    AddSpellToCast(SPELL_SUMMON_FLAMESTRIKE, CAST_RANDOM);
                    if(roll_chance_f(40))
                        DoScriptText(SAY_FLAMESTRIKE, me);
                    FlameStrikeTimer = urand(20000, 25000);
                }
                else
                    FlameStrikeTimer -= diff;

                // Below 50%
                if(HealthBelowPct(50))
                {
                    ResetStatues(false);    // shatter statues
                    ClearCastQueue();
                    StopAutocast();
                    me->SetSelection(0);
                    ForceSpellCastWithScriptText(SPELL_TELEPORT_CENTER, CAST_SELF, SAY_GRAVITY_LAPSE, INTERRUPT_AND_CAST_INSTANTLY);
                    m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    m_creature->StopMoving();
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveIdle();
                    GravityLapseTimer = 0;
                    GravityLapsePhase = 1;
                    Phase = 1;
                }
                DoMeleeAttackIfReady();
            }
            break;

            case 1:
            {
                if(GravityLapseTimer < diff)
                {
                    switch(GravityLapsePhase)
                    {
                        case 0:
                            DoScriptText(SAY_RECAST_GRAVITY, m_creature);
                            GravityLapsePhase++;
                            GravityLapseTimer = 500;
                            break;
                        case 1:
                            AddSpellToCast(m_creature, SPELL_GRAVITY_LAPSE);
                            GravityLapseTimer = 3000;// Don't interrupt the visual spell
                            GravityLapsePhase++;
                            break;
                        /*case 1:
                            TeleportPlayersToSelf();
                            GravityLapseTimer = 1000;
                            GravityLapsePhase = 2;
                            break;*/
                        case 2:
                            //CastGravityLapseKnockUp();
                            for(uint8 i = 0; i < 3; ++i)
                            {
                                AddSpellToCast(SPELL_SUMMON_ARCANE_SPHERE, CAST_SELF);
                            }
                            GravityLapseTimer = 29000;
                            GravityLapsePhase++;
                            break;
                        case 3:
                            me->InterruptNonMeleeSpells(false);
                            AddSpellToCastWithScriptText(SPELL_POWER_FEEDBACK, CAST_SELF, SAY_TIRED);
                            GravityLapseTimer = 10000;
                            GravityLapsePhase = 0;
                            /*CastGravityLapseFly();
                            GravityLapseTimer = 30000;
                            GravityLapsePhase = 4;
                            for(uint8 i = 0; i < 3; ++i)
                            {
                                Creature* Orb = m_creature->SummonCreature(CREATURE_ARCANE_SPHERE,KaelLocations[3+i][0],KaelLocations[3+i][1],LOCATION_Z,0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,30000);
                                if(Orb)
                                {
                                    //SetThreatList(Orb);
                                    Unit *target = SelectUnit(SELECT_TARGET_BOTTOMAGGRO,i);
                                    if(target)
                                    {
                                        Orb->AddThreat(target,1000);
                                        Orb->Attack(target,true);
                                        Orb->GetMotionMaster()->MoveChase(target);
                                    }
                                    else
                                    {
                                        Unit *ntarget = SelectUnit(SELECT_TARGET_RANDOM,0);
                                        if(ntarget)
                                        {
                                            Orb->AddThreat(ntarget,1000);
                                            Orb->Attack(ntarget,true);
                                            Orb->GetMotionMaster()->MoveChase(ntarget);
                                        }
                                    }
                                }
                            }*/
                            //DoCast(m_creature, SPELL_GRAVITY_LAPSE_CHANNEL);
                            break;
/*
                        case 4:
                            m_creature->InterruptNonMeleeSpells(false);
                            DoScriptText(SAY_TIRED, m_creature);
                            DoCast(m_creature, SPELL_POWER_FEEDBACK);
                            RemoveGravityLapse();
                            GravityLapseTimer = 10000;
                            GravityLapsePhase = 0;
                            break;*/
                    }
                }
                else
                    GravityLapseTimer -= diff;
            }
            break;
        }
        CastNextSpellIfAnyAndReady(diff);
    }
};

struct TRINITY_DLL_DECL mob_felkael_flamestrikeAI : public Scripted_NoMovementAI
{
    mob_felkael_flamestrikeAI(Creature *c) : Scripted_NoMovementAI(c) { }

    uint32 FlameStrikeTimer;

    void Reset()
    {
        FlameStrikeTimer = 5000;
        DoCast(m_creature, SPELL_FLAMESTRIKE_VISUAL, true);
    }
    void UpdateAI(const uint32 diff)
    {
        if(FlameStrikeTimer < diff)
        {
            DoCast(me, SPELL_FLAMESTRIKE, true);
            me->Kill(me, false);
        }
        else
            FlameStrikeTimer -= diff;
    }
};

struct TRINITY_DLL_DECL mob_felkael_phoenix_eggAI : public Scripted_NoMovementAI
{
    mob_felkael_phoenix_eggAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = (c->GetInstanceData());
    }
    uint32 HatchTimer;
    ScriptedInstance* pInstance;

    void Reset()
    {
        HatchTimer = 15000;
    }

    void JustDied(Unit* slayer)
    {
        // kill own phoenix when killed by player
        if(slayer->GetGUID() != me->GetGUID())
        {
            std::list<Creature*> ClosePhoenixList = FindAllCreaturesWithEntry(CREATURE_PHOENIX, 20);

            if(ClosePhoenixList.empty())
                me->Kill(me, false);

            for(std::list<Creature*>::iterator i = ClosePhoenixList.begin(); i != ClosePhoenixList.end(); ++i)
            {
                if((*i)->GetAuraByCasterSpell(SPELL_EMBER_BLAST, me->GetGUID()))
                {
                    (*i)->Kill((*i), false);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(HatchTimer < diff)
        {
            std::list<Creature*> ClosePhoenixList = FindAllCreaturesWithEntry(CREATURE_PHOENIX, 20);

            if(ClosePhoenixList.empty())
            {
                me->Kill(me, false);
                return;
            }

            for(std::list<Creature*>::iterator i = ClosePhoenixList.begin(); i != ClosePhoenixList.end(); ++i)
            {
                if((*i)->GetAuraByCasterSpell(SPELL_EMBER_BLAST, me->GetGUID()))
                {
                    (*i)->RemoveAurasDueToSpell(SPELL_EMBER_BLAST);
                }
            }
            me->Kill(me, false);
        }
        else
            HatchTimer -= diff;
    }
};

struct TRINITY_DLL_DECL mob_felkael_phoenixAI : public ScriptedAI
{
    mob_felkael_phoenixAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }
    uint8 phase;
    uint32 CheckTimer;
    uint64 EggGUID;
    ScriptedInstance* pInstance;

    void Reset()
    {
        DoZoneInCombat();
        me->SetWalk(false);
        me->SetSpeed(MOVE_RUN, 0.6f);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        AddSpellToCast(SPELL_REBIRTH_PHOENIX, CAST_NULL);
        AddSpellToCast(SPELL_PHOENIX_BURN, CAST_SELF);
        CheckTimer = 1000;
        phase = 0;
        EggGUID = 0;
    }

    void OnAuraRemove(Aura* Aur, bool)
    {
        if(Aur->GetId() == SPELL_EMBER_BLAST)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            DoZoneInCombat();
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true))
                AttackStart(target);
            AddSpellToCast(SPELL_REBIRTH_EGG, CAST_SELF);
            AddSpellToCast(SPELL_PHOENIX_BURN, CAST_SELF);
        }
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if(damage > m_creature->GetHealth())
        {
            damage = 0;
            me->RemoveAurasDueToSpell(SPELL_PHOENIX_BURN);
            ForceSpellCast(SPELL_EMBER_BLAST, CAST_SELF, INTERRUPT_AND_CAST_INSTANTLY);
            me->GetMap()->CreatureRelocation(me, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            Creature* egg = DoSpawnCreature(CREATURE_PHOENIX_EGG, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
            if(egg)
            {
                EggGUID = egg->GetGUID();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(CheckTimer < diff)
        {
            if (pInstance)
            {
                if(pInstance->GetData(DATA_KAELTHAS_EVENT) != IN_PROGRESS)
                {
                    me->Kill(me, false);
                    if(EggGUID)
                    {
                        if(Unit* egg = me->GetMap()->GetUnit(EggGUID))
                            egg->Kill(egg, false);
                    }
                }
                if(phase < 2)
                {
                    if(Unit* Kael = me->GetMap()->GetUnit(pInstance->GetData64(DATA_KAEL)))
                        phase = ((boss_felblood_kaelthasAI*)Kael)->Phase;
                }
            }
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if (!UpdateVictim())
            return;

        if(phase == 2)
        {
            SetAutocast(SPELL_PHOENIX_FIREBALL, 2000, true, CAST_RANDOM, 200, true);
            phase++;    //set autocast only once
        }

        CastNextSpellIfAnyAndReady();
    }
};

struct TRINITY_DLL_DECL mob_arcane_sphereAI : public ScriptedAI
{
    mob_arcane_sphereAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }
    uint32 DespawnTimer;
    uint32 ChangeTargetTimer;
    uint32 CheckTimer;

    ScriptedInstance* pInstance;

    void Reset()
    {
        DoZoneInCombat();
        m_creature->SetLevitate(true);
        m_creature->SetSpeed(MOVE_FLIGHT, 0.8);    // to be tested
        DoCast(m_creature, SPELL_ARCANE_SPHERE_PASSIVE, true);
        if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true))
        {
            AttackStart(target);
            me->AddThreat(target, 50000.0);
        }
        DespawnTimer = 29000;
        ChangeTargetTimer = 5000;
        CheckTimer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            ChangeTargetTimer = 0;

        if(CheckTimer < diff)
        {
            if (pInstance)
            {
                Creature *boss = Unit::GetCreature((*m_creature),pInstance->GetData64(DATA_KAEL));
                if(boss)
                {
                    if(!((boss_felblood_kaelthasAI*)boss->AI())->Phase || boss->isDead())
                        DespawnTimer = 0;
                }else DespawnTimer = 0;
            }
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if(DespawnTimer < diff)
            me->Kill(me, false);
        else
            DespawnTimer -= diff;

        if(ChangeTargetTimer < diff)
        {
            DoResetThreat();
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true, me->getVictimGUID()))
            {
                AttackStart(target);
                me->AddThreat(target, 50000.0);
            }
            ChangeTargetTimer = 7000;   // to be tested
        }
        else
            ChangeTargetTimer -= diff;
    }
};

bool GOUse_go_kael_orb(Player *player, GameObject* _GO)
{
    ScriptedInstance* pInst = (ScriptedInstance*)_GO->GetInstanceData();
    if (pInst)
    {
        if(pInst->GetData(DATA_KAELTHAS_EVENT) == DONE)
            player->CastSpell(player, SPELL_ESCAPE_TO_IOQD, true);
    }
    return true;
}

bool GOUse_go_movie_orb(Player *player, GameObject* _GO)
{
    if (player)
    {
        WorldPacket data(SMSG_TRIGGER_CINEMATIC, 4);
        data << (uint32)164;
        player->GetSession()->SendPacket(&data);

        if (player->GetQuestStatus(11490) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonster(25042, 0);
            /*Unit *qUnit = player->SummonCreature(25042,player->GetPositionX(),player->GetPositionY(),player->GetPositionZ()-10,0,TEMPSUMMON_CORPSE_DESPAWN,0);
            if(qUnit)
                player->DealDamage(qUnit, qUnit->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);*/
        }
    }
    return true;
}

CreatureAI* GetAI_boss_felblood_kaelthas(Creature* c)
{
    return new boss_felblood_kaelthasAI(c);
}

CreatureAI* GetAI_mob_arcane_sphere(Creature* c)
{
    return new mob_arcane_sphereAI(c);
}

CreatureAI* GetAI_mob_felkael_phoenix(Creature* c)
{
    return new mob_felkael_phoenixAI(c);
}

CreatureAI* GetAI_mob_felkael_phoenix_egg(Creature* c)
{
    return new mob_felkael_phoenix_eggAI(c);
}

CreatureAI* GetAI_mob_felkael_flamestrike(Creature* c)
{
    return new mob_felkael_flamestrikeAI(c);
}

void AddSC_boss_felblood_kaelthas()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_felblood_kaelthas";
    newscript->GetAI = &GetAI_boss_felblood_kaelthas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_arcane_sphere";
    newscript->GetAI = &GetAI_mob_arcane_sphere;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_felkael_phoenix";
    newscript->GetAI = &GetAI_mob_felkael_phoenix;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_felkael_phoenix_egg";
    newscript->GetAI = &GetAI_mob_felkael_phoenix_egg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_felkael_flamestrike";
    newscript->GetAI = &GetAI_mob_felkael_flamestrike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_kael_orb";
    newscript->pGOUse = &GOUse_go_kael_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_movie_orb";
    newscript->pGOUse = &GOUse_go_movie_orb;
    newscript->RegisterSelf();
}

