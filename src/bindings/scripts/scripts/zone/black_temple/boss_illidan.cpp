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
SDName: boss_illidan_stormrage
SD%Complete: 90
SDComment: Somewhat of a workaround for Parasitic Shadowfiend, unable to summon GOs for Cage Trap.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

/************* Quotes and Sounds ***********************/
// Gossip for when a player clicks Akama
#define GOSSIP_ITEM           "We are ready to face Illidan"
#define GOSSIP_ITEM1          "We are not ready to face Illidan yet. "

// Yells for/by Akama
#define SAY_AKAMA_BEWARE      -1999988

// I think I'll fly now and let my subordinates take you on
#define SAY_SUMMONFLAMES      -1999994


// kk, I go big, dark and demon on you.
#define SAY_MORPH             

// I KILL!
#define SAY_ENRAGE            -1999997

#define SPELL_ENRAGE                    40683 // Increases damage by 50% and attack speed by 30%. 20 seconds, PHASE 5 ONLY

#define SPELL_SHADOW_PRISON             40647 // Illidan casts this spell to immobilize entire raid when he summons Maiev.
#define SPELL_DEATH                     41220 // This spell doesn't do anything except stun Illidan and set him on his knees.
#define SPELL_BERSERK                   45078 // Damage increased by 500%, attack speed by 150%

#define SPELL_SHADOWFIEND_PASSIVE       41913 // Passive aura for shadowfiends

// Other defines
#define CENTER_X            676.740
#define CENTER_Y            305.297
#define CENTER_Z            353.192

#define SPELL_CAGED                     40695 // Caged Trap triggers will cast this on Illidan if he is within 3 yards

struct Locations
{
    float x, y, z;
};

static Locations HoverPosition[]=
{
    {657, 340, 354.256f},
    {657, 275, 354.256f},
    {705, 275, 354.256f},
    {705, 340, 354.256f}
};

static Locations GlaivePosition[]=
{
    {676.226f, 325.230f, 354.319f},
    {678.059f, 285.220f, 354.319f}
};

static Locations EyeBlast[]=
{
    {651.156f, 257.067f, 354.319f},
    {638.005f, 306.560f, 354.319f},
    {638.110f, 306.876f, 354.319f},
    {710.786f, 266.433f, 354.319f},
    {711.203f, 343.015f, 354.319f}
};

enum IllidanPhase
{
    PHASE_NULL    = 0,
    PHASE_ONE     = 1,
    PHASE_TWO     = 2,
    PHASE_THREE   = 3,
    PHASE_FOUR    = 4,
    PHASE_FIVE    = 5,
    PHASE_MAIEV   = 6,
    PHASE_DEATH   = 7
};

enum IllidanSpell
{
    SPELL_ILLIDAN_DUAL_WIELD             = 42459,
    SPELL_ILLIDAN_KNEEL_INTRO            = 39656,

    // Phase 1 spells
    SPELL_ILLIDAN_SHEAR                  = 41032, // proper
    SPELL_ILLIDAN_DRAW_SOUL              = 40904, // need to implement scripteffect
    SPELL_ILLIDAN_FLAME_CRASH            = 40832,
    SPELL_ILLIDAN_PARASITIC_SHADOWFIEND  = 41917,

    // Phase 3 spells
    SPELL_ILLIDAN_AGONIZING_FLAMES       = 40932,

    // Phase 2 spells
    SPELL_ILLIDAN_THROW_GLAIVE           = 39635,
    SPELL_ILLIDAN_GLAIVE_RETURN          = 39873,
    SPELL_ILLIDAN_FIREBALL               = 40598,
    SPELL_ILLIDAN_DARK_BARRAGE           = 40585,
    SPELL_ILLIDAN_EYE_BLAST              = 39908,

    // Phase 4 spells
    SPELL_ILLIDAN_DEMON_TRANSFORM_1      = 40511,
    SPELL_ILLIDAN_DEMON_TRANSFORM_2      = 40398,
    SPELL_ILLIDAN_DEMON_TRANSFORM_3      = 40510,
    SPELL_ILLIDAN_DEMON_FORM             = 40506,
    SPELL_ILLIDAN_SHADOW_BLAST           = 41078,
    SPELL_ILLIDAN_FLAME_BURST            = 41126,
    SPELL_ILLIDAN_SHADOW_DEMON           = 41120
};

enum IllidanEvent
{
    EVENT_ILLIDAN_START                  = 1,
    EVENT_ILLIDAN_CHANGE_PHASE           = 2,
    EVENT_ILLIDAN_SUMMON_MINIONS         = 3,

    // Phase 1,3,5 events
    EVENT_ILLIDAN_SHEAR                  = 5,
    EVENT_ILLIDAN_DRAW_SOUL              = 6,
    EVENT_ILLIDAN_FLAME_CRASH            = 7,
    EVENT_ILLIDAN_PARASITIC_SHADOWFIEND  = 8,

    EVENT_ILLIDAN_AGONIZING_FLAMES       = 9,
    EVENT_ILLIDAN_SOFT_ENRAGE            = 10,

    // Phase 2 events
    EVENT_ILLIDAN_THROW_GLAIVE           = 11,
    EVENT_ILLIDAN_EYE_BLAST              = 12,
    EVENT_ILLIDAN_DARK_BARRAGE           = 13,
    EVENT_ILLIDAN_SUMMON_TEAR            = 14,
    EVENT_ILLIDAN_RETURN_GLAIVE          = 15,
    EVENT_ILLIDAN_LAND                   = 16,

    EVENT_ILLIDAN_TRANSFORM_NO1          = 17,
    EVENT_ILLIDAN_TRANSFORM_NO2          = 18,
    EVENT_ILLIDAN_TRANSFORM_NO3          = 19,
    EVENT_ILLIDAN_TRANSFORM_NO4          = 20,
    EVENT_ILLIDAN_TRANSFORM_NO5          = 21,
    EVENT_ILLIDAN_FLAME_BURST            = 22,
    EVENT_ILLIDAN_SHADOW_DEMON           = 23,
    EVENT_ILLIDAN_TRANSFORM_BACKNO1      = 24,
    EVENT_ILLIDAN_TRANSFORM_BACKNO2      = 25,
    EVENT_ILLIDAN_TRANSFORM_BACKNO3      = 26,
    EVENT_ILLIDAN_TRANSFORM_BACKNO4      = 27,

    // Phase: Maiev summon
    EVENT_ILLIDAN_SUMMON_MAIEV           = 28,
    EVENT_ILLIDAN_CAGE_TRAP              = 29,

    EVENT_ILLIDAN_RANDOM_YELL
};

enum IllidanTexts
{
    YELL_ILLIDAN_AGGRO                   = -1529002,
    YELL_ILLIDAN_SUMMON_MINIONS          = -1999989,

    YELL_ILLIDAN_SLAIN                   = -1999991,
    YELL_ILLIDAN_SLAIN2                  = -1999992,

    YELL_ILLIDAN_PHASE_TWO               = -1999993,
    YELL_ILLIDAN_EYE_BLAST               = -1999995,
    YELL_ILLIDAN_DEMON_FORM              = -1999996,

    YELL_ILLIDAN_TAUNT_NO1               = -1529016,
    YELL_ILLIDAN_TAUNT_NO2               = -1529017,
    YELL_ILLIDAN_TAUNT_NO3               = -1529018,
    YELL_ILLIDAN_TAUNT_NO4               = -1529019
};

enum CreatureEntries
{
    BLADE_OF_AZZINOTH       =   22996,
    FLAME_OF_AZZINOTH       =   22997,
    MAIEV_SHADOWSONG        =   23197,

    ILLIDARI_ELITE          =   23226,
    PARASITIC_SHADOWFIEND   =   23498,
    CAGE_TRAP_TRIGGER       =   23292
};

struct TRINITY_DLL_DECL boss_illidan_stormrageAI : public BossAI
{
    boss_illidan_stormrageAI(Creature* c) : BossAI(c, 1)
    {
        ForceSpellCast(me, SPELL_ILLIDAN_KNEEL_INTRO, INTERRUPT_AND_CAST_INSTANTLY);
    }

    uint64 m_maievGUID;
    uint32 m_hoverPoint;

    uint32 m_combatTimer;
    uint32 m_enrageTimer;

    bool b_maievPhase;

    IllidanPhase m_phase;

    void Reset()
    {
        events.Reset();
        ClearCastQueue();
        summons.DespawnAll();

        m_combatTimer = 1000;
        m_enrageTimer = 25*60000;

        m_maievGUID = 0;

        m_hoverPoint = 0;

        b_maievPhase = false;

        m_phase = PHASE_NULL;

        SetWarglaivesEquipped(false);

        me->SetReactState(REACT_AGGRESSIVE);
        me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void SetWarglaivesEquipped(bool equip)
    {
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, equip ? 45479 : 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, equip ? 45481 : 0);
        me->SetByteValue(UNIT_FIELD_BYTES_2, 0, equip ? SHEATH_STATE_MELEE : SHEATH_STATE_UNARMED);
    }

    void ChangePhase(IllidanPhase phase)
    {
        ClearCastQueue();
        events.CancelEventsByGCD(m_phase);

        switch (m_phase = phase)
        {
            case PHASE_FIVE:
            {
                events.ScheduleEvent(EVENT_ILLIDAN_SOFT_ENRAGE, 40000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_CAGE_TRAP, 30000, m_phase);
            }
            case PHASE_THREE:
            {
                events.ScheduleEvent(EVENT_ILLIDAN_AGONIZING_FLAMES, urand(28000, 35000), m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, m_phase == PHASE_FIVE ? 60000 : urand(40000, 55000), m_phase);
            }
            case PHASE_ONE:
            {
                events.ScheduleEvent(EVENT_ILLIDAN_SHEAR, 10000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_FLAME_CRASH, urand(25000, 35000), m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_DRAW_SOUL, urand(35000, 45000), m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_PARASITIC_SHADOWFIEND, 30000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_RANDOM_YELL, urand(32000, 35000), m_phase);

                SetWarglaivesEquipped(true);

                DoResetThreat();

                if (m_phase == PHASE_ONE)
                {
                    ForceSpellCast(me, SPELL_ILLIDAN_DUAL_WIELD);
                    events.ScheduleEvent(EVENT_ILLIDAN_SUMMON_MINIONS, 1000, m_phase);
                }
                break;
            }
            case PHASE_TWO:
            {
                DoScriptText(YELL_ILLIDAN_PHASE_TWO, me);

                me->RemoveAllAuras();
                me->InterruptNonMeleeSpells(false);

                me->AttackStop();
                me->SetReactState(REACT_PASSIVE);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                me->AddUnitMovementFlag(MOVEFLAG_LEVITATING);

                me->GetMotionMaster()->MovePoint(0, CENTER_X +5.0f, CENTER_Y, CENTER_Z);

                SetAutocast(SPELL_ILLIDAN_FIREBALL, 3000, false, AUTOCAST_RANDOM);

                events.ScheduleEvent(EVENT_ILLIDAN_THROW_GLAIVE, 15000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_EYE_BLAST, urand(30000, 40000), m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_DARK_BARRAGE, 80000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, 90000, m_phase); // DEBUG ONLY ! SET TO 20-30s
                break;
            }
            case PHASE_FOUR:
            {
                SetAutocast(SPELL_ILLIDAN_SHADOW_BLAST, 3000, false, AUTOCAST_TANK);
                events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_NO1, 0, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_FLAME_BURST, 20000, m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_SHADOW_DEMON, 30000, m_phase);
                break;
            }
            case PHASE_MAIEV:
            {
                break;
            }
        }
    }

    bool PhaseOneThreeFive()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ILLIDAN_SUMMON_MINIONS:
                {
                    if (HealthBelowPct(90.0f))
                    {
                        DoScriptText(YELL_ILLIDAN_SUMMON_MINIONS, me);

                        // Call back Akama to deal with minions
                        if (Creature *pAkama = instance->GetCreature(instance->GetData64(DATA_AKAMA)))
                        {
                            pAkama->AttackStop();
                            pAkama->DeleteThreatList();
                            pAkama->SetReactState(REACT_PASSIVE);

                            if (HostilReference* pRef = me->getThreatManager().getOnlineContainer().getReferenceByTarget(pAkama))
                                 pRef->removeReference();

                            pAkama->AI()->DoAction(8); // EVENT_AKAMA_MINIONS_FIGHT
                        }
                    }
                    else
                        events.ScheduleEvent(EVENT_ILLIDAN_SUMMON_MINIONS, 1000, PHASE_ONE);

                    break;
                }
                case EVENT_ILLIDAN_CHANGE_PHASE:
                {
                    ChangePhase(PHASE_FOUR);
                    return false;
                }
                case EVENT_ILLIDAN_RANDOM_YELL:
                {
                    DoScriptText(RAND(YELL_ILLIDAN_TAUNT_NO1, YELL_ILLIDAN_TAUNT_NO2, YELL_ILLIDAN_TAUNT_NO3, YELL_ILLIDAN_TAUNT_NO4), me);
                    events.ScheduleEvent(EVENT_ILLIDAN_RANDOM_YELL, urand(30000, 35000));
                    break;
                }
                case EVENT_ILLIDAN_SHEAR:
                {
                    AddSpellToCast(me->getVictim(), SPELL_ILLIDAN_SHEAR);
                    events.ScheduleEvent(EVENT_ILLIDAN_SHEAR, 10000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_FLAME_CRASH:
                {
                    AddSpellToCast(me->getVictim(), SPELL_ILLIDAN_FLAME_CRASH);
                    events.ScheduleEvent(EVENT_ILLIDAN_FLAME_CRASH, urand(25000, 30000), m_phase);
                    break;
                }
                case EVENT_ILLIDAN_DRAW_SOUL:
                {
                    AddSpellToCast(me, SPELL_ILLIDAN_DRAW_SOUL);
                    events.ScheduleEvent(EVENT_ILLIDAN_DRAW_SOUL, urand(35000, 45000), m_phase);
                    break;
                }
                case EVENT_ILLIDAN_PARASITIC_SHADOWFIEND:
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 150.0f, true, me->getVictimGUID()))
                        AddSpellToCast(pTarget, SPELL_ILLIDAN_PARASITIC_SHADOWFIEND);

                    events.ScheduleEvent(EVENT_ILLIDAN_PARASITIC_SHADOWFIEND, 30000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_AGONIZING_FLAMES:
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0 , 150.0f, true, 0, 15.0f))
                        AddSpellToCast(pTarget, SPELL_ILLIDAN_AGONIZING_FLAMES);

                    events.ScheduleEvent(EVENT_ILLIDAN_AGONIZING_FLAMES, urand(30000, 35000), m_phase);
                    break;
                }
            }
        }
        return true;
    }

    void CastEyeBlast()
    {
        Locations initial = EyeBlast[urand(0,4)];
        if (Creature* pTrigger = me->SummonTrigger(initial.x, initial.y, initial.z, 0, 13000))
        {
            if (Creature *pGlaive = GetClosestCreatureWithEntry(pTrigger, 23448, 70.0f))
            {
                WorldLocation final;
                pTrigger->GetClosePoint(final.coord_x, final.coord_y, final.coord_z, 80.0f, false, pTrigger->GetAngle(pGlaive));
                pTrigger->SetSpeed(MOVE_WALK, 2.5f);
                pTrigger->SetUnitMovementFlags(SPLINEFLAG_WALKMODE_MODE);
                pTrigger->GetMotionMaster()->MovePoint(0, final.coord_x, final.coord_y, final.coord_z);
                pTrigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                m_hoverPoint = urand(0,4);
                me->GetMotionMaster()->MovePoint(1, HoverPosition[m_hoverPoint].x, HoverPosition[m_hoverPoint].y, HoverPosition[m_hoverPoint].z);

                ForceSpellCastWithScriptText(pTrigger, SPELL_ILLIDAN_EYE_BLAST, YELL_ILLIDAN_EYE_BLAST, INTERRUPT_AND_CAST_INSTANTLY, false, true);
            }
        }
    }

    bool PhaseTwo()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ILLIDAN_CHANGE_PHASE:
                {
                    if (!FindCreature(FLAME_OF_AZZINOTH, 200.0f, me))
                    {
                        StopAutocast();
                        me->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, CENTER_Z);
                        events.ScheduleEvent(EVENT_ILLIDAN_RETURN_GLAIVE, 3000, m_phase);
                        break;
                    }
                    /*else
                    {
                        std::list<Creature*> tearsList = DoFindAllCreaturesWithEntry(FLAME_OF_AZZINOTH, 200.0f);
                        if (tearsList.size() < 2)
                        {
                            events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, 2000, m_phase);
                            break;
                        }

                        std::list<Unit*> listOne, listTwo;
                        Creature* tearOne = *tearsList.begin();
                        Creature* tearTwo = *(++tearsList.begin());

                        CAST_AI(ScriptedAI, tearOne->AI())->SelectUnitList(listOne, 25, SELECT_TARGET_FARTHEST, 100.0f, true, 25.0f);
                        CAST_AI(ScriptedAI, tearTwo->AI())->SelectUnitList(listTwo, 25, SELECT_TARGET_FARTHEST, 100.0f, true, 25.0f);

                        if (!listOne.empty() && !listTwo.empty())
                        {
                            for (std::list<Unit*>::iterator it = listOne.begin(); it != listOne.end(); ++it)
                            {
                                for (std::list<Unit*>::iterator it2 = listTwo.begin(); it2 != listTwo.end(); ++it2)
                                {
                                    if ((*it)->GetGUID() == (*it2)->GetGUID())
                                    {
                                        tearOne->CastSpell(tearOne, SPELL_FLAME_ENRAGE, true);
                                        tearTwo->CastSpell(tearTwo, SPELL_FLAME_ENRAGE, true);

                                        tearOne->CastSpell((*it), SPELL_CHARGE, true);
                                        tearTwo->CastSpell((*it2), SPELL_CHARGE, true);

                                        CAST_AI(ScriptedAI, tearOne->AI())->DoResetThreat();
                                        CAST_AI(ScriptedAI, tearTwo->AI())->DoResetThreat();
                                        events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, 10000, m_phase);
                                        return false;

                                    }
                                }
                            }
                        }
                    }*/
                    events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, 2000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_LAND:
                {
                    me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    ChangePhase(PHASE_THREE);
                    break;
                }
                case EVENT_ILLIDAN_RETURN_GLAIVE:
                {
                    // implement SPELL_ILLIDAN_GLAIVE_RETURN visual
                    summons.DespawnEntry(BLADE_OF_AZZINOTH);
                    events.ScheduleEvent(EVENT_ILLIDAN_LAND, 1000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_THROW_GLAIVE:
                {
                    AddSpellToCast((Unit*)NULL, SPELL_ILLIDAN_THROW_GLAIVE);
                    AddSpellToCast((Unit*)NULL, SPELL_ILLIDAN_THROW_GLAIVE);

                    SetWarglaivesEquipped(false);

                    events.ScheduleEvent(EVENT_ILLIDAN_SUMMON_TEAR, 2000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_SUMMON_TEAR:
                {
                    StartAutocast();
                    break;
                }
                case EVENT_ILLIDAN_EYE_BLAST:
                {
                    CastEyeBlast();
                    events.ScheduleEvent(EVENT_ILLIDAN_EYE_BLAST, urand(30000, 35000), m_phase);
                    break;
                }
                case EVENT_ILLIDAN_DARK_BARRAGE:
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        AddSpellToCast(pTarget, SPELL_ILLIDAN_DARK_BARRAGE);

                    events.ScheduleEvent(EVENT_ILLIDAN_DARK_BARRAGE, 44000, m_phase);
                    break;
                }
            }
        }
        return false;
    }

    bool PhaseFour()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ILLIDAN_TRANSFORM_NO1:
                {
                    me->GetMotionMaster()->Clear();

                    DoScriptText(YELL_ILLIDAN_DEMON_FORM, me);

                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_1, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_NO2, 1300, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_NO2:
                {
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_TRANSFORM_1);
                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_2, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_NO3, 4000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_NO3:
                {
                    DoResetThreat();
                    SetWarglaivesEquipped(false);

                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_FORM, INTERRUPT_AND_CAST_INSTANTLY, true);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_NO4, 3000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_NO4:
                {
                    StartAutocast();

                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_TRANSFORM_2);
                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_3, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_NO5, 3500, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_NO5:
                {
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_TRANSFORM_3);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_BACKNO1, 61000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_FLAME_BURST:
                {
                    AddSpellToCast(me, SPELL_ILLIDAN_FLAME_BURST);
                    events.ScheduleEvent(EVENT_ILLIDAN_FLAME_BURST, 20000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_SHADOW_DEMON:
                {
                    for (uint8 i = 0; i < 4; i++)
                    {
                        // Yes we can have multiple demons assigned to one person, Tank shouldn't be excluded from search :]
                        if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            pTarget->CastSpell(me, SPELL_ILLIDAN_SHADOW_DEMON, true);
                    }
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_BACKNO1:
                {
                    ClearCastQueue();
                    StopAutocast();
                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_1, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_BACKNO2, 1500, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_BACKNO2:
                {
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_TRANSFORM_1);
                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_2, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_BACKNO3, 4000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_BACKNO3:
                {
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_FORM);

                    SetWarglaivesEquipped(true);
                    events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_BACKNO4, 3000, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_TRANSFORM_BACKNO4:
                {
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_DEMON_TRANSFORM_2);
                    ForceSpellCast(me, SPELL_ILLIDAN_DEMON_TRANSFORM_3, INTERRUPT_AND_CAST_INSTANTLY);
                    events.ScheduleEvent(EVENT_ILLIDAN_CHANGE_PHASE, 3500, m_phase);
                    break;
                }
                case EVENT_ILLIDAN_CHANGE_PHASE:
                {
                    if (b_maievPhase)
                    {
                        ChangePhase(PHASE_MAIEV);
                        break;
                    }

                    if (HealthBelowPct(30.0f))
                        ChangePhase(PHASE_FIVE);
                    else
                        ChangePhase(PHASE_THREE);

                    me->GetMotionMaster()->MoveChase(me->getVictim());
                    break;
                }
            }
        }
        return false;
    }

    bool PhaseMaiev()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ILLIDAN_SUMMON_MAIEV:
                {
                    break;
                }
            }
        }
        return false;
    }

    bool PhaseDeath()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            }
        }
        return false;
    }

    bool HandlePhase(IllidanPhase m_phase)
    {
        switch(m_phase)
        {
            case PHASE_ONE:
            case PHASE_THREE:
            case PHASE_FIVE:
                return PhaseOneThreeFive();
            case PHASE_TWO:
                return PhaseTwo();
            case PHASE_FOUR:
                return PhaseFour();
            case PHASE_MAIEV:
                return PhaseMaiev();
            case PHASE_DEATH:
                return PhaseDeath();
            default:
                return true;
        }
    }

    void EnterCombat(Unit *pWho)
    {
        DoZoneInCombat();
    }

    void KilledUnit(Unit *pWho)
    {
        if (pWho == me)
            return;

        DoScriptText(RAND(YELL_ILLIDAN_SLAIN, YELL_ILLIDAN_SLAIN2), me);
    }

    void AttackStart(Unit *pWho)
    {
        if (m_phase == PHASE_TWO || m_phase == PHASE_NULL)
            return;

        if (m_phase == PHASE_FOUR)
            ScriptedAI::AttackStartNoMove(pWho);
        else
            ScriptedAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_phase == PHASE_TWO || m_phase == PHASE_NULL)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 MovementType, uint32 uiData)
    {
        if (MovementType == POINT_MOTION_TYPE)
            me->setHover(true);
    }

    void JustReachedHome()
    {
        ForceSpellCast(me, SPELL_ILLIDAN_KNEEL_INTRO, INTERRUPT_AND_CAST_INSTANTLY);

        if (Creature *pAkama = instance->GetCreature(instance->GetData64(DATA_AKAMA)))
        {
            pAkama->AI()->Reset();
            pAkama->AI()->EnterEvadeMode();
            pAkama->GetMotionMaster()->Clear(false); // need reset waypoint movegen, to test
            pAkama->GetMotionMaster()->MoveTargetedHome();
        }
    }

    void DoAction(const int32 action)
    {
        switch (action)
        {
            case EVENT_ILLIDAN_START:
            {
                DoScriptText(YELL_ILLIDAN_AGGRO, me);
                DoZoneInCombat();

                me->RemoveAurasDueToSpell(SPELL_ILLIDAN_KNEEL_INTRO);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                ChangePhase(PHASE_ONE);
                break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_combatTimer < diff)
        {
            DoZoneInCombat();
            m_combatTimer = 1000;
        }
        else
            m_combatTimer -= diff;

        if (m_enrageTimer < diff)
        {
        }
        else
            m_enrageTimer -= diff;

        if (m_phase == PHASE_ONE && HealthBelowPct(65.0f))
        {
            ChangePhase(PHASE_TWO);
            return;
        }

        if (!m_maievGUID && HealthBelowPct(30.0f))
        {
            if (m_phase == PHASE_FOUR)
            {
                events.CancelEventsByGCD(m_phase);
                events.ScheduleEvent(EVENT_ILLIDAN_TRANSFORM_BACKNO1, 0, m_phase);
                b_maievPhase = true;
            }
            else
                ChangePhase(PHASE_MAIEV);
        }

        events.Update(diff);
        if (HandlePhase(m_phase))
            DoMeleeAttackIfReady();

        CastNextSpellIfAnyAndReady(diff);
    }
};

static float SpiritSpawns[][4]=
{
    {23411, 755.5426, 309.9156, 312.2129},
    {23410, 755.5426, 298.7923, 312.0834}
};

//Akama spells
enum AkamaSpells  
{
    SPELL_AKAMA_DOOR_CAST_SUCCESS = 41268,
    SPELL_AKAMA_DOOR_CAST_FAIL    = 41271,
    SPELL_DEATHSWORN_DOOR_CHANNEL = 41269,
    SPELL_AKAMA_POTION            = 40535,
    SPELL_AKAMA_CHAIN_LIGHTNING   = 40536  // 6938 to 8062 for 5 targets
};

enum ConversationText
{
    SAY_ILLIDAN_NO1 = -1999998,
    SAY_AKAMA_NO1   = -1529099,
    SAY_ILLIDAN_NO2 = -1529000,
    SAY_AKAMA_NO2   = -1529001,
};

enum AkamaEvents
{
    EVENT_AKAMA_START             = 1,
    EVENT_AKAMA_TALK_SEQUENCE_NO1 = 2,
    EVENT_AKAMA_TALK_SEQUENCE_NO2 = 3,
    EVENT_AKAMA_TALK_SEQUENCE_NO3 = 4,
    EVENT_AKAMA_TALK_SEQUENCE_NO4 = 5,

    EVENT_AKAMA_SET_DOOR_EVENT    = 6,

    EVENT_AKAMA_ILLIDAN_FIGHT     = 7,
    EVENT_AKAMA_MINIONS_FIGHT     = 8,
    EVENT_AKAMA_SUMMON_ELITE,

    EVENT_AKAMA_DOOR_CAST_FAIL,
    EVENT_AKAMA_SUMMON_SPIRITS,
    EVENT_AKAMA_DOOR_CAST_SUCCESS,
    EVENT_AKAMA_DOOR_OPEN,
    EVENT_AKAMA_DOOR_MOVE_PATH
};

enum AkamaTexts
{
    SAY_AKAMA_DOOR_SPEECH_NO1    = -1309025,
    SAY_AKAMA_DOOR_SPEECH_NO2    = -1309024,
    SAY_AKAMA_DOOR_SPEECH_NO3    = -1309028,

    YELL_AKAMA_FIGHT_MINIONS     = -1999990,

    SAY_SPIRIT_DOOR_SPEECH_NO1   = -1309026,
    SAY_SPIRIT_DOOR_SPEECH_NO2   = -1309027,
};

enum AkamaPath
{
    PATH_AKAMA_MINION_EVENT       = 2111,
    PATH_AKAMA_DOOR_EVENT_AFTER   = 2109,
    PATH_AKAMA_DOOR_EVENT_BEFORE  = 2110
};

struct TRINITY_DLL_DECL boss_illidan_akamaAI : public BossAI
{
    boss_illidan_akamaAI(Creature* c) : BossAI(c, 2){}

    bool allowUpdate;
    bool doorEvent;

    uint32 m_pathId;

    void Reset()
    {
        ClearCastQueue();
        events.Reset();
        summons.DespawnAll();

        allowUpdate = false;
        doorEvent = false;

        m_pathId = 0;

        SetAutocast(SPELL_AKAMA_CHAIN_LIGHTNING, 10000, false, AUTOCAST_TANK);

        me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        if (instance->GetData(EVENT_ILLIDARIDOOR) == DONE)
            me->SetVisibility(VISIBILITY_ON);
        else
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->DestroyForNearbyPlayers();
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!me->GetDBTableGUIDLow())
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 type, uint32 data)
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            if (doorEvent)
            {
                if (data == 15 && m_pathId == PATH_AKAMA_DOOR_EVENT_BEFORE)
                {
                    m_pathId = 0;
                    events.ScheduleEvent(EVENT_AKAMA_DOOR_CAST_FAIL, 2000);
                }

                if (data == 17 && m_pathId == PATH_AKAMA_DOOR_EVENT_AFTER)
                {
                    if (Creature *pAkama = instance->GetCreature(instance->GetData64(DATA_AKAMA)))
                        pAkama->SetVisibility(VISIBILITY_ON);

                    m_pathId = 0;
                    me->DisappearAndDie();
                }
            }
            else
            {
                if (m_pathId == PATH_AKAMA_MINION_EVENT)
                {
                   if (data == 3)
                       DoScriptText(YELL_AKAMA_FIGHT_MINIONS, me);

                   if (data == 9)
                   {
                       m_pathId = 0;
                       events.ScheduleEvent(EVENT_AKAMA_SUMMON_ELITE, 1000);
                   }
                }
            }
        }

        if (type == POINT_MOTION_TYPE)
        {
            if (data == 0)
            {
                me->SetSelection(instance->GetData64(DATA_ILLIDANSTORMRAGE));
                events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO1, 500);
            }
        }
    }

    void HandleDoorEvent()
    {
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_AKAMA_DOOR_CAST_FAIL:
                {
                    AddSpellToCastWithScriptText((Unit*)NULL, SPELL_AKAMA_DOOR_CAST_FAIL, SAY_AKAMA_DOOR_SPEECH_NO1);
                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO1, 8500);
                    break;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO1:
                {
                    DoScriptText(SAY_AKAMA_DOOR_SPEECH_NO2, me);
                    events.ScheduleEvent(EVENT_AKAMA_SUMMON_SPIRITS, 10000);
                    break;
                }
                case EVENT_AKAMA_SUMMON_SPIRITS:
                {
                    for (uint8 i = 0; i < 2; i++)
                    {
                        if (Creature *pSpirit = me->SummonCreature(uint32(SpiritSpawns[i][0]), SpiritSpawns[i][1], SpiritSpawns[i][2], SpiritSpawns[i][3], me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 35000))
                            pSpirit->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    }

                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO2, 1000);
                    break;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO2:
                {
                    if (Creature *pCreature = GetClosestCreatureWithEntry(me, SpiritSpawns[0][0], 30.0f))
                        DoScriptText(SAY_SPIRIT_DOOR_SPEECH_NO1, pCreature);

                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO3, 3000);
                    break;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO3:
                {
                    if (Creature *pCreature = GetClosestCreatureWithEntry(me, SpiritSpawns[1][0], 30.0f))
                        DoScriptText(SAY_SPIRIT_DOOR_SPEECH_NO2, pCreature);

                    events.ScheduleEvent(EVENT_AKAMA_DOOR_CAST_SUCCESS, 8000);
                    break;
                }
                case EVENT_AKAMA_DOOR_CAST_SUCCESS:
                {
                    for (uint8 i = 0; i < 2; i++)
                    {
                        if (Creature *pSpirit = GetClosestCreatureWithEntry(me, uint32(SpiritSpawns[i][0]), 30.0f))
                            pSpirit->CastSpell((Unit*)NULL, SPELL_DEATHSWORN_DOOR_CHANNEL, false);
                    }

                    AddSpellToCast((Unit*)NULL, SPELL_AKAMA_DOOR_CAST_SUCCESS);
                    events.ScheduleEvent(EVENT_AKAMA_DOOR_OPEN, 15000);
                    break;
                }
                case EVENT_AKAMA_DOOR_OPEN:
                {
                    instance->SetData(EVENT_ILLIDARIDOOR, DONE);
                    instance->HandleGameObject(instance->GetData64(DATA_GAMEOBJECT_ILLIDAN_GATE), true);
                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO4, 3000);
                    break;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO4:
                {
                    DoScriptText(SAY_AKAMA_DOOR_SPEECH_NO3, me);
                    events.ScheduleEvent(EVENT_AKAMA_DOOR_MOVE_PATH, 10000);
                    break;
                }
                case EVENT_AKAMA_DOOR_MOVE_PATH:
                {
                    m_pathId = PATH_AKAMA_DOOR_EVENT_AFTER;
                    me->GetMotionMaster()->MovePath(PATH_AKAMA_DOOR_EVENT_AFTER, false);
                    break;
                }
            }
        }
        CastNextSpellIfAnyAndReady();
    }

    void EnterEvadeMode()
    {
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void DoAction(const int32 action)
    {
        switch (action)
        {
            case EVENT_AKAMA_SET_DOOR_EVENT:
            {
                doorEvent = true;
                me->SetVisibility(VISIBILITY_ON);
                break;
            }
            case EVENT_AKAMA_START:
            {
                allowUpdate = true;

                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (doorEvent)
                {
                    m_pathId = PATH_AKAMA_DOOR_EVENT_BEFORE;
                    me->GetMotionMaster()->MovePath(PATH_AKAMA_DOOR_EVENT_BEFORE, false);
                }
                else
                {
                    me->RemoveUnitMovementFlag(SPLINEFLAG_WALKMODE);
                    me->GetMotionMaster()->MovePoint(0, 728.379f, 314.462f, 352.996f);
                }
                break;
            }
            case EVENT_AKAMA_MINIONS_FIGHT:
            {
                StopAutocast();
                me->InterruptNonMeleeSpells(true);

                m_pathId = PATH_AKAMA_MINION_EVENT;

                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MovePath(PATH_AKAMA_MINION_EVENT, false);
                break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!allowUpdate && !UpdateVictim())
            return;

        events.Update(diff);

        if (doorEvent)
        {
            HandleDoorEvent();
            return;
        }

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_AKAMA_TALK_SEQUENCE_NO1:
                {
                    if (Creature *pIllidan = instance->GetCreature(instance->GetData64(DATA_ILLIDANSTORMRAGE)))
                    {
                        me->SetSelection(pIllidan->GetGUID());
                        pIllidan->SetSelection(me->GetGUID());
                        pIllidan->RemoveAurasDueToSpell(SPELL_ILLIDAN_KNEEL_INTRO);
                        DoScriptText(SAY_ILLIDAN_NO1, pIllidan);
                    }

                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO2, 11000);
                    return;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO2:
                {
                    DoScriptText(SAY_AKAMA_NO1, me);
                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO3, 10000);
                    return;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO3:
                {
                    if (Creature *pIllidan = instance->GetCreature(instance->GetData64(DATA_ILLIDANSTORMRAGE)))
                        DoScriptText(SAY_ILLIDAN_NO2, pIllidan);

                    events.ScheduleEvent(EVENT_AKAMA_TALK_SEQUENCE_NO4, 7000);
                    return;
                }
                case EVENT_AKAMA_TALK_SEQUENCE_NO4:
                {
                    DoScriptText(SAY_AKAMA_NO2, me);
                    events.ScheduleEvent(EVENT_AKAMA_ILLIDAN_FIGHT, 5000);
                    return;
                }
                case EVENT_AKAMA_ILLIDAN_FIGHT:
                {
                    if (Creature *pIllidan = instance->GetCreature(instance->GetData64(DATA_ILLIDANSTORMRAGE)))
                    {
                        pIllidan->AI()->DoAction(EVENT_ILLIDAN_START);
                        AttackStart(pIllidan);

                        StartAutocast();
                    }
                    break;
                }
                case EVENT_AKAMA_SUMMON_ELITE:
                {
                    break;
                }
            }
        }

        if (HealthBelowPct(20.0f))
        {
            ForceSpellCast(me, SPELL_AKAMA_POTION, INTERRUPT_AND_CAST_INSTANTLY);
            return;
        }

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

enum MaievTaunts
{
    MAIEV_TAUNT_NO1 = -1529020,
    MAIEV_TAUNT_NO2 = -1529021,
    MAIEV_TAUNT_NO3 = -1529022,
    MAIEV_TAUNT_NO4 = -1529023
};


//Maiev spells
#define SPELL_CAGE_TRAP_DUMMY           40761 // Put this in DB for cage trap GO.
#define SPELL_CAGE_TRAP_SUMMON          40694 // Summons a Cage Trap GO (bugged) on the ground along with a Cage Trap Disturb Trigger mob (working)
#define SPELL_CAGE_TRAP_BEAM            40713 // 8 Triggers on the ground in an octagon cast spells like this on Illidan 'caging him'
#define SPELL_TELEPORT_VISUAL           41232 // Teleport visual for Maiev
#define SPELL_SHADOW_STRIKE             40685 // 4375 to 5625 every 3 seconds for 12 seconds
#define SPELL_THROW_DAGGER              41152 // 5400 to 6600 damage, need dagger
#define SPELL_FAN_BLADES                39954 // bugged visual

struct TRINITY_DLL_DECL boss_illidan_maievAI : public BossAI
{
    boss_illidan_maievAI(Creature *c) : BossAI(c, 3){};

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
    }
};

enum GlaiveSpells
{
    SPELL_GLAIVE_SUMMON_TEAR      = 39855,
    SPELL_GLAIVE_CHANNEL          = 39857
};

struct TRINITY_DLL_DECL boss_illidan_glaiveAI : public Scripted_NoMovementAI
{
    boss_illidan_glaiveAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    uint32 m_summonTimer;
    uint32 m_checkTimer;

    uint64 m_tearGUID;

    void Reset()
    {
        m_tearGUID = 0;

        m_summonTimer = 2000;
        m_checkTimer = 5000;
    }

    void IsSummonedBy(Unit *pSummoner)
    {
        if (!pInstance)
            return;

        if (Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
            pIllidan->AI()->JustSummoned(me);
    }

    void JustSummoned(Creature *pWho)
    {
        m_tearGUID = pWho->GetGUID();
        ForceSpellCast(pWho, SPELL_GLAIVE_CHANNEL);

        if (Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
            pIllidan->AI()->JustSummoned(pWho);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_summonTimer)
        {
            if (m_summonTimer < diff)
            {
                AddSpellToCast(me, SPELL_GLAIVE_SUMMON_TEAR);
                m_summonTimer = 0;
            }
            else
                m_summonTimer -= diff;
        }

        CastNextSpellIfAnyAndReady();
    }
};

enum FlameEvents
{
    EVENT_FLAME_RANGE_CHECK = 1,
    EVENT_FLAME_FLAME_BLAST = 2,
    EVENT_FLAME_BLAZE       = 3
};

enum FlameSpells
{
    SPELL_FLAME_FLAME_BLAST = 40631,
    SPELL_FLAME_BLAZE       = 40637,
    SPELL_FLAME_CHARGE      = 40602,
    SPELL_FLAME_ENRAGE      = 45078
};

struct TRINITY_DLL_DECL boss_illidan_flameofazzinothAI : public ScriptedAI
{
    boss_illidan_flameofazzinothAI(Creature *c) : ScriptedAI(c), summons(me){}

    EventMap events;
    SummonList summons;

    void Reset()
    {
        events.Reset();
        summons.DespawnAll();
        ClearCastQueue();
    }

    void EnterCombat(Unit *pWho)
    {
        events.ScheduleEvent(EVENT_FLAME_RANGE_CHECK, 6000);
        events.ScheduleEvent(EVENT_FLAME_FLAME_BLAST, urand(25000, 30000));
        events.ScheduleEvent(EVENT_FLAME_BLAZE, urand(32000, 36000));
    }

    void JustDied(Unit *pKiller)
    {
        me->RemoveCorpse();
    }

    void JustSummoned(Creature *pWho)
    {
        summons.Summon(pWho);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while(uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FLAME_RANGE_CHECK:
                {
                    DoZoneInCombat();

                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_FARTHEST, 0, 200.0f, true, 0, 40.0f))
                    {
                        // wipe mode on :]
                        DoResetThreat();
                        ForceSpellCast(me, SPELL_FLAME_ENRAGE);
                        ForceSpellCast(pTarget, SPELL_FLAME_CHARGE);

                        me->AI()->AttackStart(pTarget);

                        events.ScheduleEvent(EVENT_FLAME_RANGE_CHECK, 15000);
                    }
                    else
                        events.ScheduleEvent(EVENT_FLAME_RANGE_CHECK, 2000);

                    break;
                }
                case EVENT_FLAME_FLAME_BLAST:
                {
                    AddSpellToCast(me->getVictim(), SPELL_FLAME_FLAME_BLAST);
                    events.ScheduleEvent(EVENT_FLAME_FLAME_BLAST, urand(25000, 30000));
                    break;
                }
                case EVENT_FLAME_BLAZE:
                {
                    AddSpellToCast(me, SPELL_FLAME_FLAME_BLAST);
                    events.ScheduleEvent(EVENT_FLAME_BLAZE, urand(32000, 36000));
                    break;
                }
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum ShadowDemonSpells
{
    SPELL_SHADOW_DEMON_PASSIVE      = 41079,
    SPELL_SHADOW_DEMON_CONSUME_SOUL = 41080,
    SPELL_SHADOW_DEMON_FOUND_TARGET = 41082
};

struct TRINITY_DLL_DECL boss_illidan_shadowdemonAI : public ScriptedAI
{
    boss_illidan_shadowdemonAI(Creature *c) : ScriptedAI(c){}

    uint64 m_targetGUID;

    uint32 m_checkTimer;

    void Reset()
    {
        me->SetReactState(REACT_PASSIVE);

        m_checkTimer = 2000;
        m_targetGUID = 0;
    }

    void MovementInform(uint32 type, uint32 data)
    {
        if (type != POINT_MOTION_TYPE || data)
            return;

        if (Unit *pTarget = me->GetUnit(m_targetGUID))
            ForceSpellCast(pTarget, SPELL_SHADOW_DEMON_CONSUME_SOUL, INTERRUPT_AND_CAST_INSTANTLY);

        if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true))
        {
            m_targetGUID = pTarget->GetGUID();
            ForceSpellCast(pTarget, SPELL_SHADOW_DEMON_FOUND_TARGET);
        }
    }

    void IsSummonedBy(Unit *pSummoner)
    {
        DoZoneInCombat();

        m_targetGUID = pSummoner->GetGUID();

        ForceSpellCast(pSummoner, SPELL_SHADOW_DEMON_FOUND_TARGET);
        ForceSpellCast(me, SPELL_SHADOW_DEMON_PASSIVE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_targetGUID)
        {
            if (m_checkTimer < diff)
            {
                Unit *pUnit = me->GetUnit(m_targetGUID);
                if (!pUnit)
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    {
                        m_targetGUID = pTarget->GetGUID();
                        ForceSpellCast(pTarget, SPELL_SHADOW_DEMON_FOUND_TARGET);
                    }
                }

                m_checkTimer = 2000;
            }
            else
                m_checkTimer -= diff;
        }

        CastNextSpellIfAnyAndReady();
    }
};

bool GossipSelect_boss_illidan_akama(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF) // Time to begin the Event
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->AI()->DoAction(EVENT_AKAMA_START);
    }
    else
        pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipHello_boss_illidan_akama(Player *player, Creature *_Creature)
{
    if (player->isGameMaster())
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    else
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10465, _Creature->GetGUID());
    return true;
}

CreatureAI* GetAI_boss_illidan_stormrage(Creature *_Creature)
{
    return new boss_illidan_stormrageAI(_Creature);
}

CreatureAI* GetAI_boss_illidan_akama(Creature *_Creature)
{
    return new boss_illidan_akamaAI(_Creature);
}

CreatureAI* GetAI_boss_illidan_maiev(Creature *_Creature)
{
    return new boss_illidan_maievAI(_Creature);
}

CreatureAI* GetAI_boss_illidan_glaive(Creature *_Creature)
{
    return new boss_illidan_glaiveAI(_Creature);
}

CreatureAI* GetAI_boss_illidan_shadowdemon(Creature *_Creature)
{
    return new boss_illidan_shadowdemonAI(_Creature);
}

CreatureAI* GetAI_boss_illidan_flameofazzinoth(Creature *_Creature)
{
    return new boss_illidan_flameofazzinothAI(_Creature);
}

void AddSC_boss_illidan()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_illidan_stormrage";
    newscript->GetAI = &GetAI_boss_illidan_stormrage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_illidan_akama";
    newscript->GetAI = &GetAI_boss_illidan_akama;
    newscript->pGossipHello = &GossipHello_boss_illidan_akama;
    newscript->pGossipSelect = &GossipSelect_boss_illidan_akama;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_illidan_maiev";
    newscript->GetAI = &GetAI_boss_illidan_maiev;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_illidan_glaive";
    newscript->GetAI = &GetAI_boss_illidan_glaive;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_illidan_shadowdemon";
    newscript->GetAI = &GetAI_boss_illidan_shadowdemon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_illidan_flameofazzinoth";
    newscript->GetAI = &GetAI_boss_illidan_flameofazzinoth;
    newscript->RegisterSelf();
}
/*
DELETE FROM `spell_script_target` WHERE `entry` = '39635';
INSERT INTO `spell_script_target` (`entry`,`type`,`targetEntry`) VALUES ('39635','1','23448');
UPDATE `creature_template` SET `ScriptName` = 'boss_illidan_glaive' WHERE `entry` ='22996';
UPDATE `creature_template` SET `ScriptName` = 'boss_illidan_akama' WHERE `entry` = '23089';
UPDATE `creature_template` SET `ScriptName` = 'boss_illidan_shadowdemon' WHERE `entry` = '23375';
UPDATE `creature` SET `spawntimesecs`='10' WHERE `id` = '23448';
UPDATE `script_texts` SET `type` = 0 WHERE entry IN(-1529099, -1529000, -1529001);
DELETE FROM `creature_template_addon` WHERE `entry` = '22917';
REPLACE INTO spell_script_target VALUES (41268, 1, 23412), (41269, 1, 23412), (41271, 1, 23412);

INSERT INTO `waypoint_data` VALUES ('2109', '1', '769.23', '305.157', '312.292', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '2', '775.453', '305.138', '316.401', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '3', '780.276', '305.124', '319.72', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '4', '785.867', '300.098', '319.76', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '5', '793.835', '292.918', '319.771', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '6', '796.945', '289.077', '319.836', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '7', '796.373', '284.29', '323.751', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '8', '795.95', '278.861', '328.191', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '9', '795.542', '273.43', '332.631', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '10', '795.153', '268.159', '336.941', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '11', '795.024', '262.407', '341.464', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '12', '791.342', '257.2', '341.464', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '13', '788.019', '252.503', '341.464', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '14', '783.752', '246.57', '341.724', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '15', '777.176', '244.22', '346.029', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '16', '772.464', '242.619', '349.101', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '17', '765.657', '240.153', '353.684', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2109', '18', '757.588', '239.638', '353.281', '0', '1', '0', '100', '0');

INSERT INTO `script_texts`(`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`)VALUES('-1309024','I cannot do this alone.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','274','Akama before door\r\n');
INSERT INTO `script_texts`(`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`)VALUES('-1309025','The door is all what stands between us and the Betrayer. Stand a side friends.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','Akama door cast');
INSERT INTO `script_texts`(`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`)VALUES('-1309026','You are not alone, Akama.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','Akama speech no 1 helper');
INSERT INTO `script_texts`(`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`)VALUES('-1309027','Your people will always be with you.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','0','Akama speech no 2 helper');
INSERT INTO `script_texts`(`entry`,`content_default`,`content_loc1`,`content_loc2`,`content_loc3`,`content_loc4`,`content_loc5`,`content_loc6`,`content_loc7`,`content_loc8`,`sound`,`type`,`language`,`emote`,`comment`)VALUES('-1309028','I thank you for your aid, brothers. Our people will be redeemed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,'0','0','0','66','Akama end speech');

INSERT INTO `waypoint_data` VALUES ('2110', '1', '669.205', '321.226', '271.69', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '2', '666.536', '337.008', '271.69', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '3', '670.742', '347.471', '271.69', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '4', '679.681', '364.894', '271.681', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '5', '692.234', '373.568', '271.68', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '6', '701.046', '379.66', '272.862', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '7', '714.898', '373.266', '278.119', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '8', '722.923', '367.704', '283.035', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '9', '730.117', '360.733', '289.848', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '10', '736.486', '352.951', '296.095', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '11', '740.103', '345.393', '300.454', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '12', '744.947', '334.724', '306.903', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '13', '747.264', '324.849', '309.56', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '14', '751.097', '308.212', '312.069', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '15', '751.998', '304.3', '312.065', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2110', '16', '759.091', '304.063', '312.274', '0', '1', '0', '100', '0');

INSERT INTO `waypoint_data` VALUES ('2111', '1', '735.911', '335.327', '352.996', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '2', '748.758', '361.503', '352.996', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '3', '766.342', '369.532', '353.674', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '4', '775.964', '366.737', '347.46', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '5', '784.779', '363.882', '341.741', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '6', '792.534', '357.53', '341.418', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '7', '795.411', '346.043', '341.211', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '8', '795.9', '333.309', '330.807', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '9', '795.997', '319.96', '319.897', '0', '1', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('2111', '10', '794.935', '304.499', '319.761', '0', '1', '0', '100', '0');

insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values(DEFAULT,'23089','564','1','0','1679','757.588','239.638','353.281','2.26385','300','0','0','960707','607000','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values(DEFAULT,'22917','564','1','0','442','701.94','307.019','354.27','0.7300','4294967295','0','0','6070400','7588','0','0');
UPDATE `creature_template` SET `ScriptName`='boss_illidan_flameofazzinoth', `flags_extra` = `flags_extra` | 262144 WHERE `entry`='22997';
*/
