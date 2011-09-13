/* Copyright (C) 2009 Trinity <http://www.trinitycore.org/>
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
SDName: Boss_Felmyst
SD%Complete: 80
SDComment: make felmyst faster during air phase/fog of corruption casting then adjust air timers, summon kalecgos after encounter is done
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum Quotes
{
    YELL_BIRTH      =       -1580036,
    YELL_KILL1      =       -1580037,
    YELL_KILL2      =       -1580038,
    YELL_BREATH     =       -1580039,
    YELL_TAKEOFF    =       -1580040,
    YELL_BERSERK    =       -1580041,
    YELL_DEATH      =       -1580042,
    YELL_KALECGOS   =       -1580043, //after felmyst's death spawned and say this
    EMOTE_BREATH    =       -1811004
};

enum Spells
{
    //Aura
    AURA_NOXIOUS_FUMES          =   47002,

    //Land phase
    SPELL_CLEAVE                =   19983,
    SPELL_CORROSION             =   45866,
    SPELL_GAS_NOVA              =   45855,
    SPELL_ENCAPSULATE_CHANNEL   =   45661,
    // SPELL_ENCAPSULATE_EFFECT    =   45665,
    // SPELL_ENCAPSULATE_AOE       =   45662,

    //Flight phase
    SPELL_VAPOR_SELECT          =   45391,   // fel to player, force cast 45392, 50000y selete target
    SPELL_VAPOR_SUMMON          =   45392,   // player summon vapor, radius around caster, 5y,
    SPELL_VAPOR_FORCE           =   45388,   // vapor to fel, force cast 45389
    SPELL_VAPOR_CHANNEL         =   45389,   // fel to vapor, green beam channel
    SPELL_VAPOR_TRIGGER         =   45411,   // linked to 45389, vapor to self, trigger 45410 and 46931
    SPELL_VAPOR_DAMAGE          =   46931,   // vapor damage, 4000
    SPELL_TRAIL_SUMMON          =   45410,   // vapor summon trail
    SPELL_TRAIL_TRIGGER         =   45399,   // trail to self, trigger 45402
    SPELL_TRAIL_DAMAGE          =   45402,   // trail damage, 2000 + 2000 dot
    SPELL_DEAD_SUMMON           =   45400,   // summon blazing dead, 5min
    SPELL_DEAD_PASSIVE          =   45415,
    SPELL_FOG_BREATH            =   45495,   // fel to self, speed burst
    SPELL_FOG_TRIGGER           =   45582,   // fog to self, trigger 45782
    SPELL_FOG_FORCE             =   45782,   // fog to player, force cast 45714
    SPELL_FOG_INFORM            =   45714,   // player let fel cast 45717, script effect
    SPELL_FOG_CHARM             =   45717,   // fel to player
    SPELL_FOG_CHARM2            =   45726,   // link to 45717

    SPELL_TRANSFORM_TRIGGER     =   44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_VISUAL      =   46350,   //46411stun?
    SPELL_TRANSFORM_FELMYST     =   45068,   // become fel
    SPELL_FELMYST_SUMMON        =   45069,

    //Other
    SPELL_BERSERK               =   45078,
    SPELL_CLOUD_VISUAL          =   45212,
    SPELL_CLOUD_SUMMON          =   45884
};

enum Creatures
{
    MOB_FELMYST        =   25038,
    //MOB_BRUTALLUS      =
    //MOB_KALECGOS       =
    MOB_UNYIELDING_DEAD  =   25268,
    MOB_MADRIGOSA      =   25160,
    MOB_FELMYST_VISUAL =   25041,
    MOB_FLIGHT_LEFT    =   25357,
    MOB_FLIGHT_RIGHT   =   25358,
    MOB_FOG_OF_CORRUPTION    =   25266,
    MOB_VAPOR          =   25265,
    MOB_VAPOR_TRAIL    =   25267,
    MOB_KALECGOS       =   24844        // dunno if it's right one - its not ;p
};

enum PhaseFelmyst
{
    PHASE_NULL  = 0,
    PHASE_GROUND = 1,
    PHASE_FLIGHT = 2,
};

enum EventFelmyst
{
    EVENT_NULL          =   0,
    EVENT_BERSERK       =   1,
    EVENT_CHECK         =   2,

    EVENT_CLEAVE        =   3,
    EVENT_CORROSION     =   4,
    EVENT_GAS_NOVA      =   5,
    EVENT_ENCAPSULATE   =   6,
    EVENT_FLIGHT        =   7,

    EVENT_FLIGHT_SEQUENCE   =   3,
    EVENT_SUMMON_DEAD       =   4,
    EVENT_SUMMON_FOG        =   5
};

static EventFelmyst MaxTimer[]=
{
    EVENT_NULL,
    EVENT_FLIGHT,
    EVENT_SUMMON_FOG,
};

float FlightNodes[2][2][3] = {
    { { 1493, 707, 60}, {1522, 504, 60} },
    { { 1466, 671, 60}, {1466, 517, 60} },
};

#define FELMYST_FLY_FLAGS (MOVEFLAG_LEVITATING)

struct TRINITY_DLL_DECL boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature *c) : ScriptedAI(c){
        pInstance = (c->GetInstanceData());

        // wait for core patch be accepted
        /*SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ENCAPSULATE_EFFECT);
        if(TempSpell->SpellIconID == 2294)
            TempSpell->SpellIconID = 2295;
        TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_VAPOR_TRIGGER);
        if((TempSpell->Attributes & SPELL_ATTR_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR_PASSIVE;
        TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_FOG_CHARM2);
        if((TempSpell->Attributes & SPELL_ATTR_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR_PASSIVE;*/
    }

    ScriptedInstance *pInstance;
    PhaseFelmyst Phase;
    EventFelmyst Event;
    uint32 Timer[EVENT_FLIGHT + 1];
    uint32 EvadeTimer;

    uint32 FlightCount;
    uint32 BreathCount;
    uint32 BreathPath;

    uint32 OutroPhase;
    uint32 OutroTimer;
    uint64 KalecgosGUID;

    float BreathX, BreathY;

    void Reset()
    {
        Phase = PHASE_NULL;
        Event = EVENT_NULL;
        Timer[EVENT_BERSERK] = 600000;
        Timer[EVENT_CHECK] = 1000;
        FlightCount = 0;
        EvadeTimer = 0;
        OutroPhase = 0;
        OutroTimer = 0;

        m_creature->AddUnitMovementFlag(FELMYST_FLY_FLAGS);
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);
        m_creature->setHover(true);

        DespawnSummons(MOB_VAPOR_TRAIL);
        m_creature->setActive(true);

        if(pInstance)
            pInstance->SetData(DATA_FELMYST_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        m_creature->setActive(true);
        DoZoneInCombat();
        m_creature->CastSpell(m_creature, AURA_NOXIOUS_FUMES, true);
        m_creature->RemoveUnitMovementFlag(FELMYST_FLY_FLAGS);
        EnterPhase(PHASE_GROUND);

        if(pInstance)
            pInstance->SetData(DATA_FELMYST_EVENT, IN_PROGRESS);
    }

    
    void AttackStart(Unit *who)
    {
        if(Phase != PHASE_FLIGHT)
            ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(Phase != PHASE_FLIGHT)
            ScriptedAI::MoveInLineOfSight(who);
    }


    void KilledUnit(Unit* victim)
    {
        DoScriptText(RAND(YELL_KILL1, YELL_KILL2), m_creature);
    }

    void JustRespawned()
    {
        DoScriptText(YELL_BIRTH, m_creature);
        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
        EvadeTimer = 2000;
    }

    void JustDied(Unit* Killer)
    {
        if(pInstance && Killer->GetTypeId() != TYPEID_UNIT)
        {
            DoScriptText(YELL_DEATH, m_creature);
            pInstance->SetData(DATA_FELMYST_EVENT, DONE);
            me->SummonCreature(MOB_KALECGOS, 1555, 737, 88, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
        }
    }

    void EnterEvadeMode()
    {
        CreatureAI::EnterEvadeMode();
        Map::PlayerList const &players = me->GetMap()->GetPlayers();
        for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            if(Player *p = i->getSource())
            {
                if(p->isAlive() && p->HasAura(SPELL_FOG_CHARM, 0))
                    me->Kill(p, false);
            }
    }


    void JustSummoned(Creature *summon)
    {
        if(summon->GetEntry() == MOB_UNYIELDING_DEAD)
        {
            summon->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            summon->CastSpell(summon, SPELL_DEAD_PASSIVE, true);
        } 
        else if(summon->GetEntry() == MOB_KALECGOS)
        {
            summon->setActive(true);
            summon->AddUnitMovementFlag(SPLINEFLAG_FLYINGING2 | MOVEFLAG_CAN_FLY);
            summon->GetMotionMaster()->MovePoint(0, 1471, 632, 37);
            KalecgosGUID = summon->GetGUID();
            OutroTimer = 20000;
        }
    }

    void MovementInform(uint32 Type, uint32 Id)
    {
        if(Type == HOME_MOTION_TYPE)
            m_creature->setHover(true);
        
        else if(Type == POINT_MOTION_TYPE)
        {        
            switch(Id)
            {
                case 0:
                    Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
                    me->setHover(true);
                    break;
                case 1:
                    DoScriptText(EMOTE_BREATH, m_creature);
                    me->setHover(true);
                    Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                    break;
                case 2:
                    m_creature->RemoveAurasDueToSpell(SPELL_FOG_BREATH);
                    me->setHover(true);
                    Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                    break;
                case 4:
                    Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                    break;

            }
        }
    }

    void DamageTaken(Unit*, uint32 &damage)
    {
        if(Phase != PHASE_GROUND && damage >= m_creature->GetHealth())
            damage = 0;
    }

    void EnterPhase(PhaseFelmyst NextPhase)
    {
        switch(NextPhase)
        {
        case PHASE_GROUND:
            Timer[EVENT_CLEAVE] = 5000 + rand()%5 * 1000;
            Timer[EVENT_CORROSION] = 10000 + rand()%10 * 1000;
            Timer[EVENT_GAS_NOVA] = 15000 + rand()%5 * 1000;
            Timer[EVENT_ENCAPSULATE] = 20000 + rand()%5 * 1000;
            Timer[EVENT_FLIGHT] = 60000;
            Timer[EVENT_CHECK] = 1000;
            break;
        case PHASE_FLIGHT:
            Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
            Timer[EVENT_SUMMON_DEAD] = 0;
            Timer[EVENT_SUMMON_FOG] = 0;
            Timer[EVENT_CHECK] = 1000;
            FlightCount = 0;
            BreathCount = 0;
            break;
        default:
            break;
        }
        Phase = NextPhase;
    }

    void DoOutro()
    {
        Unit *Kalecgos = me->GetUnit(KalecgosGUID);
        if(!Kalecgos)
            return;

        switch(OutroPhase)
        {
            case 0:
                DoScriptText(YELL_KALECGOS, Kalecgos);
                OutroTimer = 10000;
                break;
            case 1:
                Kalecgos->GetMotionMaster()->MovePoint(0, 1768, 598, 173);
                OutroTimer = 0;
                break;
        }

        OutroPhase++;
    }

    void HandleFlightSequence()
    {
        switch(FlightCount)
        {
        case 0:
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            m_creature->AddUnitMovementFlag(FELMYST_FLY_FLAGS);
            DoScriptText(YELL_TAKEOFF, m_creature);
            Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
            break;
        case 1:
            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX()+10, m_creature->GetPositionY(), m_creature->GetPositionZ()+20);
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        case 2:
        case 4:
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 150, true);
            if(target)
            {
                Creature* Vapor = m_creature->SummonCreature(MOB_VAPOR, target->GetPositionX()-5+rand()%10, target->GetPositionY()-5+rand()%10, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 9000);
                if(Vapor)
                {
                    Vapor->AI()->AttackStart(target);
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->CastSpell(Vapor, SPELL_VAPOR_CHANNEL, false); // core bug
                    Vapor->CastSpell(Vapor, SPELL_VAPOR_TRIGGER, true);
                }
            }
            else
            {
                EnterEvadeMode();
                return;
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
            break;
        }
        case 3:
        case 5:
            DespawnSummons(MOB_VAPOR_TRAIL);
            Timer[EVENT_FLIGHT_SEQUENCE] = 1;
            break;
        case 6:
        {
            BreathPath = urand(0, 1);
            float *pos = FlightNodes[BreathPath][BreathCount%2];
            m_creature->GetMotionMaster()->MovePoint(1, pos[0], pos[1], pos[2]);
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        }
        case 7:
        {
            float *pos = FlightNodes[BreathPath][(BreathCount+1)%2];
            m_creature->GetMotionMaster()->MovePoint(2, pos[0], pos[1], pos[2]);
            m_creature->CastSpell(m_creature, SPELL_FOG_BREATH, true);
            Timer[EVENT_SUMMON_FOG] = 1000;            
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        }
        case 8:
            BreathCount++;
            Timer[EVENT_SUMMON_FOG] = 0;
            Timer[EVENT_FLIGHT_SEQUENCE] = 1;
            if(BreathCount < 3) FlightCount = 5;
            break;
        case 9:
            if(Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0))
            {
                float x, y, z;
                target->GetPosition(x, y, z);
                m_creature->GetMotionMaster()->MovePoint(4, x, y, z);
            }
            else
            {
                EnterEvadeMode();
                return;
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        case 10:
            m_creature->RemoveUnitMovementFlag(FELMYST_FLY_FLAGS);
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            EnterPhase(PHASE_GROUND);
            AttackStart(m_creature->getVictim());
            DoStartMovement(m_creature->getVictim());
            break;
        default:
            break;
        }
        FlightCount++;
    }

    void UpdateAI(const uint32 diff)
    {
        if(EvadeTimer)
        {
            if(EvadeTimer <= diff)
            {
                m_creature->AddUnitMovementFlag(FELMYST_FLY_FLAGS);
                EnterEvadeMode();
                EvadeTimer = 0;
                return;
            } else
                EvadeTimer -= diff;
        }

        if(OutroTimer)
        {
            if(OutroTimer <= diff)
            {
                DoOutro();
            } else
                OutroTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        Event = EVENT_NULL;
        for(uint32 i = 1; i <= MaxTimer[Phase]; i++)
        {
            if(Timer[i])
                if(Timer[i] <= diff)
                {
                    if(!Event)
                        Event = (EventFelmyst)i;
                }else 
                    Timer[i] -= diff;
        }

        if(m_creature->IsNonMeleeSpellCasted(false))
            return;

        if(Phase == PHASE_GROUND)
        {
            switch(Event)
            {
            case EVENT_BERSERK:
                DoScriptText(YELL_BERSERK, m_creature);
                m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
                Timer[EVENT_BERSERK] = 10000;
                break;
            case EVENT_CLEAVE:
                m_creature->CastSpell(m_creature->getVictim(), SPELL_CLEAVE, false);
                Timer[EVENT_CLEAVE] = 5000 + rand()%5 * 1000;
                break;
            case EVENT_CORROSION:
                m_creature->CastSpell(m_creature->getVictim(), SPELL_CORROSION, false);
                Timer[EVENT_CORROSION] = 20000 + rand()%10 * 1000;
                break;
            case EVENT_GAS_NOVA:
                m_creature->CastSpell(m_creature, SPELL_GAS_NOVA, false);
                Timer[EVENT_GAS_NOVA] = 20000 + rand()%5 * 1000;
                break;
            case EVENT_ENCAPSULATE:
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 150, true))
                {
                    m_creature->CastSpell(target, SPELL_ENCAPSULATE_CHANNEL, false);
                    Timer[EVENT_ENCAPSULATE] = 25000 + rand()%5 * 1000;
                }
                break;
            case EVENT_FLIGHT:
                EnterPhase(PHASE_FLIGHT);
                break;
            case EVENT_CHECK:
                DoZoneInCombat();
                Timer[EVENT_CHECK]=1000;
                break;
            default:
                DoMeleeAttackIfReady();
                break;
            }
        }

        if(Phase == PHASE_FLIGHT)
        {
            switch(Event)
            {
            case EVENT_BERSERK:
                DoScriptText(YELL_BERSERK, m_creature);
                m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
                Timer[EVENT_BERSERK] = 0;
                break;
            case EVENT_FLIGHT_SEQUENCE:
                HandleFlightSequence();
                break;
            case EVENT_SUMMON_FOG:
            {
                float x, y, z;
                m_creature->SetSpeed(MOVE_WALK, 10.0f);
                m_creature->GetPosition(x, y, z);
                m_creature->UpdateGroundPositionZ(x, y, z);
                if(Creature *Fog = m_creature->SummonCreature(MOB_FOG_OF_CORRUPTION, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
                    Fog->CastSpell(Fog, SPELL_FOG_TRIGGER, true);
                
                Timer[EVENT_SUMMON_FOG] = 200;
                break;
            }
            case EVENT_CHECK:
                if(Timer[EVENT_FLIGHT_SEQUENCE])
                    m_creature->SetSpeed(MOVE_WALK, 5.0f);
                
                DoZoneInCombat();
                Timer[EVENT_CHECK] = 1000;
                break;
            default:
                break;
            }
        }
    }

    void DespawnSummons(uint32 entry)
    {
        std::list<Creature*> templist;
        float x, y, z;
        m_creature->GetPosition(x, y, z);

        Trinity::AllCreaturesOfEntryInRange check(m_creature, entry, 100);
        Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(templist, check);
        Cell::VisitGridObjects(me, searcher, 100.0f);

        for(std::list<Creature*>::iterator i = templist.begin(); i != templist.end(); ++i)
        {
            
            if(entry == MOB_VAPOR_TRAIL && Phase == PHASE_FLIGHT)
            {
                float x, y, z;
                (*i)->GetPosition(x, y, z);
                m_creature->SummonCreature(MOB_UNYIELDING_DEAD, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
            }
            (*i)->SetVisibility(VISIBILITY_OFF);
            (*i)->setDeathState(JUST_DIED);
            if((*i)->getDeathState() == CORPSE)
                (*i)->RemoveCorpse();
        }
    }
};


// AI for invisible mob that is following player while Felmyst is casting Demonic Vapor on him
struct TRINITY_DLL_DECL mob_felmyst_vaporAI : public ScriptedAI
{
    mob_felmyst_vaporAI(Creature *c) : ScriptedAI(c)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetSpeed(MOVE_RUN, 0.8);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 0.01);
    }
    void Reset() {}
    void EnterCombat(Unit* who)
    {
        DoZoneInCombat();
        //m_creature->CastSpell(m_creature, SPELL_VAPOR_FORCE, true); core bug
    }
    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->getVictim())
            AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
    }
};

// AI for invisible mob leaving on felmyst vapor trail (summoned by SPELL_VAPOR_FORCE)
struct TRINITY_DLL_DECL mob_felmyst_trailAI : public ScriptedAI
{
    mob_felmyst_trailAI(Creature *c) : ScriptedAI(c)
    {
        m_creature->CastSpell(m_creature, SPELL_TRAIL_TRIGGER, true);
        m_creature->SetSelection(m_creature->GetGUID());
        //m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.01);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void SpellHitTarget(Unit* target, const SpellEntry *entry)
    {
        if(entry->Id == SPELL_TRAIL_DAMAGE)
            me->SummonCreature(MOB_UNYIELDING_DEAD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
    }

    void Reset() {}
    void EnterCombat(Unit* who) {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff) {}
};

CreatureAI* GetAI_boss_felmyst(Creature *_Creature)
{
    return new boss_felmystAI(_Creature);
}

CreatureAI* GetAI_mob_felmyst_vapor(Creature *_Creature)
{
    return new mob_felmyst_vaporAI(_Creature);
}

CreatureAI* GetAI_mob_felmyst_trail(Creature *_Creature)
{
    return new mob_felmyst_trailAI(_Creature);
}

void AddSC_boss_felmyst()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_felmyst";
    newscript->GetAI = &GetAI_boss_felmyst;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_felmyst_vapor";
    newscript->GetAI = &GetAI_mob_felmyst_vapor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_felmyst_trail";
    newscript->GetAI = &GetAI_mob_felmyst_trail;
    newscript->RegisterSelf();
}
