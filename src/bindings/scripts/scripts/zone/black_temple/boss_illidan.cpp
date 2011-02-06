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

#define GETGO(obj, guid)      GameObject* obj = GameObject::GetGameObject(*me, guid)
#define GETUNIT(unit, guid)   Unit* unit = Unit::GetUnit(*me, guid)
#define GETCRE(cre, guid)     Creature* cre = Unit::GetCreature(*me, guid)
#define HPPCT(unit)           unit->GetHealth()*100 / unit->GetMaxHealth()

/************* Quotes and Sounds ***********************/
// Gossip for when a player clicks Akama
#define GOSSIP_ITEM           "We are ready to face Illidan"

// Yells for/by Akama
#define SAY_AKAMA_BEWARE      -1999988
#define SAY_AKAMA_MINION      -1999989
#define SAY_AKAMA_LEAVE       -1999990

// Self explanatory
#define SAY_KILL1             -1999991
#define SAY_KILL2             -1999992

// I think I'll fly now and let my subordinates take you on
#define SAY_TAKEOFF           -1999993
#define SAY_SUMMONFLAMES      -1999994

// When casting Eye Blast. Demon Fire will be appear on places that he casts this
#define SAY_EYE_BLAST         -1999995

// kk, I go big, dark and demon on you.
#define SAY_MORPH             -1999996

// I KILL!
#define SAY_ENRAGE            -1999997

/************** Spells *************/
// Normal Form
#define SPELL_SHEAR                     41032 // 41032 is bugged, cannot be block/dodge/parry// Reduces Max. Health by 60% for 7 seconds. Can stack 19 times. 1.5 second cast
#define SPELL_FLAME_CRASH               40832 // Summons an invis/unselect passive mob that has an aura of flame in a circle around him.
#define SPELL_DRAW_SOUL                 40904 // 5k Shadow Damage in front of him. Heals Illidan for 100k health (script effect)
#define SPELL_PARASITIC_SHADOWFIEND     41917 // DoT of 3k Shadow every 2 seconds. Lasts 10 seconds. (Script effect: Summon 2 parasites once the debuff has ticked off)
#define SPELL_PARASITIC_SHADOWFIEND2    41914 // Used by Parasitic
#define SPELL_SUMMON_PARASITICS         41915 // Summons 2 Parasitic Shadowfiends on the target. It's supposed to be cast as soon as the Parasitic Shadowfiend debuff is gone, but the spells aren't linked :(
#define SPELL_AGONIZING_FLAMES          40932 // 4k fire damage initial to target and anyone w/i 5 yards. PHASE 3 ONLY
#define SPELL_ENRAGE                    40683 // Increases damage by 50% and attack speed by 30%. 20 seconds, PHASE 5 ONLY

// Flying (Phase 2)
#define SPELL_THROW_GLAIVE              39635 // Throws a glaive on the ground
#define SPELL_THROW_GLAIVE2             39849 // Animation for the above spell
#define SPELL_GLAIVE_RETURNS            39873 // Glaive flies back to Illidan
#define SPELL_FIREBALL                  40598 // 2.5k-3.5k damage in 10 yard radius. 2 second cast time.
#define SPELL_DARK_BARRAGE              40585 // 10 second channeled spell, 3k shadow damage per second.

// Demon Form
#define SPELL_DEMON_TRANSFORM_1         40511 // First phase of animations for transforming into Dark Illidan (fall to ground)
#define SPELL_DEMON_TRANSFORM_2         40398 // Second phase of animations (kneel)
#define SPELL_DEMON_TRANSFORM_3         40510 // Final phase of animations (stand up and roar)
#define SPELL_DEMON_FORM                40506 // Transforms into Demon Illidan. Has an Aura of Dread on him.
#define SPELL_SHADOW_BLAST              41078 // 8k - 11k Shadow Damage. Targets highest threat. Has a splash effect, damaging anyone in 20 yards of the target.
#define SPELL_FLAME_BURST               41126 // Hurls fire at entire raid for ~3.5k damage every 10 seconds. Resistable. (Does not work: Script effect)
#define SPELL_FLAME_BURST_EFFECT        41131 // The actual damage. Have each player cast it on itself (workaround)

// Other Illidan spells
#define SPELL_KNEEL                     39656 // Before beginning encounter, this is how he appears (talking to skully).
#define SPELL_SHADOW_PRISON             40647 // Illidan casts this spell to immobilize entire raid when he summons Maiev.
#define SPELL_DEATH                     41220 // This spell doesn't do anything except stun Illidan and set him on his knees.
#define SPELL_BERSERK                   45078 // Damage increased by 500%, attack speed by 150%
#define SPELL_DUAL_WIELD                42459

//Phase Normal spells
#define SPELL_FLAME_CRASH_EFFECT        40836 // Firey blue ring of circle that the other flame crash summons
#define SPELL_SUMMON_SHADOWDEMON        41117 // Summon four shadowfiends
#define SPELL_SHADOWFIEND_PASSIVE       41913 // Passive aura for shadowfiends

//Phase Flight spells
#define SPELL_AZZINOTH_CHANNEL          39857 // Glaives cast it on Flames. Not sure if this is the right spell.
#define SPELL_EYE_BLAST_TRIGGER         40017 // This summons Demon Form every few seconds and deals ~20k damage in its radius
#define SPELL_EYE_BLAST                 39908 // This does the blue flamey animation.
#define SPELL_BLAZE_EFFECT              40610 // Green flame on the ground, triggers damage (5k) every few seconds
#define SPELL_BLAZE_SUMMON              40637 // Summons the Blaze creature
#define SPELL_DEMON_FIRE                40029 // Blue fire trail left by Eye Blast. Deals 2k per second if players stand on it.
#define SPELL_FLAME_BLAST               40631 // Flames of Azzinoth use this. Frontal cone AoE 7k-9k damage.
#define SPELL_CHARGE                    41581 //40602 // Flames of Azzinoth charges whoever is too far from them. They enrage after this. For simplicity, we'll use the same enrage as Illidan.
#define SPELL_FLAME_ENRAGE              45078

// Other defines
#define CENTER_X            676.740
#define CENTER_Y            305.297
#define CENTER_Z            353.192

#define SPELL_CAGED                     40695 // Caged Trap triggers will cast this on Illidan if he is within 3 yards

#define FLAME_ENRAGE_DISTANCE   30
#define FLAME_CHARGE_DISTANCE   50

/**** Creature Summon and Recognition IDs ****/
enum CreatureEntry
{
    EMPTY                   =       0,
    AKAMA                   =   22990,
    ILLIDAN_STORMRAGE       =   22917,
    BLADE_OF_AZZINOTH       =   22996,
    FLAME_OF_AZZINOTH       =   22997,
    MAIEV_SHADOWSONG        =   23197,
    SHADOW_DEMON            =   23375,
    DEMON_FIRE              =   23069,
    FLAME_CRASH             =   23336,
    ILLIDAN_DOOR_TRIGGER    =   23412,
    SPIRIT_OF_OLUM          =   23411,
    SPIRIT_OF_UDALO         =   23410,
    ILLIDARI_ELITE          =   23226,
    PARASITIC_SHADOWFIEND   =   23498,
    CAGE_TRAP_TRIGGER       =   23292,
};

struct Yells
{
    int32 entry;
    uint32 creature, timer;
};

static Yells Conversation[]=
{
    { -1999998, ILLIDAN_STORMRAGE, 8000},
    { -1529012, ILLIDAN_STORMRAGE, 5000},
    { -1999999, AKAMA, 7000},
    { -1529013, AKAMA, 5000},
    { -1529000, ILLIDAN_STORMRAGE, 8000},
    { -1529001, AKAMA, 3000},
    { -1529014, AKAMA, 2000},
    { -1529002, ILLIDAN_STORMRAGE, 3000},
    {        0, EMPTY, 1000},
    {        0, EMPTY, 0},//9
    { -1529003, ILLIDAN_STORMRAGE, 8000},
    { -1529004, MAIEV_SHADOWSONG, 8000},
    { -1529005, ILLIDAN_STORMRAGE, 5000},
    { -1529006, MAIEV_SHADOWSONG, 8000},
    { -1529007, ILLIDAN_STORMRAGE, 1000},//14
    { -1529008, MAIEV_SHADOWSONG, 6000},//15
    { -1529009, ILLIDAN_STORMRAGE, 30000}, // Emote dead for now. Kill him later
    { -1529010, MAIEV_SHADOWSONG, 9000},
    { -1529015, MAIEV_SHADOWSONG, 5000},
    {        0, EMPTY, 1000},//19 Maiev disappear
    { -1529011, AKAMA, 8000},
    {        0, EMPTY, 1000}//21
};

struct Locations
{
    float x, y, z;
};

static Locations HoverPosition[]=
{
    { 657, 340, 355 },
    { 657, 275, 355 },
    { 705, 275, 355 },
    { 705, 340, 355 }
};

static Locations GlaivePosition[]=
{
    { 695.105, 305.303, 354.256 },
    { 659.338, 305.303, 354.256 }, //the distance between two glaives is 36
    { 700.105, 305.303, 354.256 },
    { 664.338, 305.303, 354.256 }
};

static Locations EyeBlast[]=
{
    { 677, 350, 354 },          //start point, pass through glaive point
    { 677, 260, 354 }
};


struct Animation // For the demon transformation
{
    uint32 aura, unaura, timer, size, displayid, phase;
    bool equip;
};

static Animation DemonTransformation[]=
{
    {SPELL_DEMON_TRANSFORM_1, 0, 1000, 0, 0, 6, true},
    {SPELL_DEMON_TRANSFORM_2, SPELL_DEMON_TRANSFORM_1, 4000, 0, 0, 6, true},
    {0, 0, 3000, 1073741824, 21322, 6, false},//stunned, cannot cast demon form
    {SPELL_DEMON_TRANSFORM_3, SPELL_DEMON_TRANSFORM_2, 3500, 0, 0, 6, false},
    {SPELL_DEMON_FORM, SPELL_DEMON_TRANSFORM_3, 0, 0, 0, 4, false},
    {SPELL_DEMON_TRANSFORM_1, 0, 1000, 0, 0, 6, false},
    {SPELL_DEMON_TRANSFORM_2, SPELL_DEMON_TRANSFORM_1, 4000, 0, 0, 6, false},
    {0, SPELL_DEMON_FORM, 3000, 1069547520, 21135, 6, false},
    {SPELL_DEMON_TRANSFORM_3, SPELL_DEMON_TRANSFORM_2, 3500, 0, 0, 6, true},
    {0, SPELL_DEMON_TRANSFORM_3, 0, 0, 0, 8, true}
};

enum IllidanTaunts
{
    ILLIDAN_TAUNT_NO1 = -1529016,
    ILLIDAN_TAUNT_NO2 = -1529017,
    ILLIDAN_TAUNT_NO3 = -1529018,
    ILLIDAN_TAUNT_NO4 = -1529019
};

/*** Phase Names ***/
enum IllidanPhase
{
    PHASE_ILLIDAN_NULL          =   0,
    PHASE_NORMAL                =   1,
    PHASE_FLIGHT                =   2,
    PHASE_NORMAL_2              =   3,
    PHASE_DEMON                 =   4,
    PHASE_NORMAL_MAIEV          =   5,
    PHASE_TALK_SEQUENCE         =   6,
    PHASE_FLIGHT_SEQUENCE       =   7,
    PHASE_TRANSFORM_SEQUENCE    =   8,
    PHASE_ILLIDAN_MAX           =   9,
};

enum IllidanEvent
{
    EVENT_NULL                  =   0,
    EVENT_BERSERK               =   1,
    //normal phase
    EVENT_TAUNT                 =   2,
    EVENT_SHEAR                 =   3,
    EVENT_FLAME_CRASH           =   4,
    EVENT_PARASITIC_SHADOWFIEND =   5,
    EVENT_PARASITE_CHECK        =   6,
    EVENT_DRAW_SOUL             =   7,
    EVENT_AGONIZING_FLAMES      =   8,
    EVENT_TRANSFORM_NORMAL      =   9,
    EVENT_ENRAGE                =   10,

    //flight phase
    EVENT_FIREBALL              =   2,
    EVENT_DARK_BARRAGE          =   3,
    EVENT_EYE_BLAST             =   4,
    EVENT_MOVE_POINT            =   5,

    //demon phase
    EVENT_SHADOW_BLAST          =   2,
    EVENT_FLAME_BURST           =   3,
    EVENT_SHADOWDEMON           =   4,
    EVENT_TRANSFORM_DEMON       =   5,

    //sequence phase
    EVENT_TALK_SEQUENCE         =   2,
    EVENT_FLIGHT_SEQUENCE       =   2,
    EVENT_TRANSFORM_SEQUENCE    =   2,
};

static IllidanEvent MaxTimer[]=
{
    EVENT_NULL,
    EVENT_DRAW_SOUL,
    EVENT_MOVE_POINT,
    EVENT_TRANSFORM_NORMAL,
    EVENT_TRANSFORM_DEMON,
    EVENT_ENRAGE,
    EVENT_TALK_SEQUENCE,
    EVENT_FLIGHT_SEQUENCE,
    EVENT_TRANSFORM_SEQUENCE
};
/************************************** Illidan's AI ***************************************/
struct TRINITY_DLL_DECL boss_illidan_stormrageAI : public ScriptedAI
{
    boss_illidan_stormrageAI(Creature* c) : ScriptedAI(c), Summons(me)
    {
        pInstance = (c->GetInstanceData());
        me->CastSpell(me, SPELL_DUAL_WIELD, true);
    }

    ScriptedInstance* pInstance;

    IllidanPhase m_phase;
    IllidanEvent m_event;
    uint32 Timer[EVENT_ENRAGE + 1];

    uint32 TalkCount;
    uint32 TransformCount;
    uint32 FlightCount;

    uint32 HoverPoint;

    uint64 AkamaGUID;
    uint64 MaievGUID;
    uint64 GlaiveGUID[2];

    SummonList Summons;

    void Reset();

    void JustSummoned(Creature* summon);

    void MovementInform(uint32 MovementType, uint32 Data)
    {
        if(FlightCount == 7) //change hover point
        {
            if(me->getVictim())
            {
                me->SetInFront(me->getVictim());
                me->StopMoving();
            }
            EnterPhase(PHASE_FLIGHT);
        }
        else // handle flight sequence
            Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
    }

    void EnterCombat(Unit *who)
    {
        me->setActive(true);
        DoZoneInCombat();

        EnterPhase(PHASE_NORMAL);
    }

    void AttackStart(Unit *who)
    {
        if(!who || m_phase >= PHASE_TALK_SEQUENCE)
            return;

        if(m_phase == PHASE_FLIGHT || m_phase == PHASE_DEMON)
            AttackStartNoMove(who);
        else
            ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit *who) {}

    void JustDied(Unit *killer)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if(!pInstance)
            return;

        pInstance->SetData(EVENT_ILLIDANSTORMRAGE, DONE); // Completed

        for(uint8 i = DATA_GAMEOBJECT_ILLIDAN_DOOR_R; i < DATA_GAMEOBJECT_ILLIDAN_DOOR_L + 1; ++i)
        {
            GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(i));
            if(Door)
                Door->SetUInt32Value(GAMEOBJECT_STATE, 0); // Open Doors
        }
    }

    void KilledUnit(Unit *pVictim)
    {
        if (pVictim == me)
            return;

        DoScriptText(RAND(SAY_KILL1, SAY_KILL2), me);
     }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if(damage >= me->GetHealth() && done_by != me)
            damage = 0;

        if(done_by->GetGUID() == MaievGUID)
            done_by->AddThreat(me, -(3*(float)damage)/4); // do not let maiev tank him
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_GLAIVE_RETURNS) // Re-equip our warblades!
        {
            if(!me->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY))
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 45479);
            else
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 45481);

            me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE );
        }
    }

    void DeleteFromThreatList(uint64 TargetGUID)
    {
        for(std::list<HostilReference*>::iterator itr = me->getThreatManager().getThreatList().begin(); itr != me->getThreatManager().getThreatList().end(); ++itr)
        {
            if((*itr)->getUnitGuid() == TargetGUID)
            {
                (*itr)->removeReference();
                break;
            }
        }
    }

    void EnterPhase(IllidanPhase m_nextPhase);
    void CastEyeBlast();
    void SummonMaiev();
    void HandleTalkSequence();
    void HandleFlightSequence()
    {
        switch(FlightCount)
        {
        case 1://lift off
            me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            me->SetUnitMovementFlags(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
            me->StopMoving();
            DoScriptText(SAY_TAKEOFF, me);
            Timer[EVENT_FLIGHT_SEQUENCE] = 3000;
            break;
        case 2://move to center
            me->GetMotionMaster()->MovePoint(0, CENTER_X + 5, CENTER_Y, CENTER_Z); //+5, for SPELL_THROW_GLAIVE bug
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        case 3: // Throw Glaive no 1.
            {
                uint8 i=1;
                if (Creature *pGlaive = me->SummonCreature(BLADE_OF_AZZINOTH, GlaivePosition[i].x, GlaivePosition[i].y, GlaivePosition[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    GlaiveGUID[i] = pGlaive->GetGUID();
                    pGlaive->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pGlaive->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                    pGlaive->setFaction(me->getFaction());

                    AddSpellToCast(pGlaive, SPELL_THROW_GLAIVE2);
                }
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 700;
            break;
        case 4: // Throw Glaive no 2.
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
            {
                uint8 i=0;
                if (Creature* pGlaive = me->SummonCreature(BLADE_OF_AZZINOTH, GlaivePosition[i].x, GlaivePosition[i].y, GlaivePosition[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    GlaiveGUID[i] = pGlaive->GetGUID();

                    pGlaive->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pGlaive->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                    pGlaive->setFaction(me->getFaction());

                    AddSpellToCast(pGlaive, SPELL_THROW_GLAIVE);
                }
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
            break;
        case 5:
            DoScriptText(SAY_SUMMONFLAMES, me);
            Timer[EVENT_FLIGHT_SEQUENCE] = 3000;
            break;
        case 6: // Move to hover point
            me->GetMotionMaster()->MovePoint(0, HoverPosition[HoverPoint].x, HoverPosition[HoverPoint].y, HoverPosition[HoverPoint].z);
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        case 7://return to center
            me->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, CENTER_Z);
            Timer[EVENT_FLIGHT_SEQUENCE] = 0;
            break;
        case 8://glaive return
            for(uint8 i = 0; i < 2; i++)
            {
                if(GlaiveGUID[i])
                {
                    Unit* Glaive = Unit::GetUnit((*me), GlaiveGUID[i]);
                    if(Glaive)
                    {
                        Glaive->CastSpell(me, SPELL_GLAIVE_RETURNS, false); // Make it look like the Glaive flies back up to us
                        Glaive->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686); // disappear but not die for now
                    }
                }
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
            break;
        case 9://land
            me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
            me->StopMoving();
            me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            for(uint8 i = 0; i < 2; i++)
            {
                if(GlaiveGUID[i])
                {
                    if(GETUNIT(Glaive, GlaiveGUID[i]))
                    {
                        Glaive->SetVisibility(VISIBILITY_OFF);
                        Glaive->setDeathState(JUST_DIED); // Despawn the Glaive
                    }
                    GlaiveGUID[i] = 0;
                }
            }
            Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
            break;
        case 10://attack
            DoResetThreat();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE );
            EnterPhase(PHASE_NORMAL_2);
            break;
        default:
            break;
        }
        FlightCount++;
    }

    void HandleTransformSequence()
    {
        if(DemonTransformation[TransformCount].unaura)
            me->RemoveAurasDueToSpell(DemonTransformation[TransformCount].unaura);

        if(DemonTransformation[TransformCount].aura)
            DoCast(me, DemonTransformation[TransformCount].aura, true);

        if(DemonTransformation[TransformCount].displayid)
            me->SetUInt32Value(UNIT_FIELD_DISPLAYID, DemonTransformation[TransformCount].displayid); // It's morphin time!

        if(DemonTransformation[TransformCount].equip)
        {
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 45479);     // Requip warglaives if needed
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 45481);
            me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE );
        }
        else
        {
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);         // Unequip warglaives if needed
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
        }

        switch(TransformCount)
        {
        case 2:
            DoResetThreat();
            break;
        case 4:
            EnterPhase(PHASE_DEMON);
            break;
        case 7:
            DoResetThreat();
            break;
        case 9:
            if(MaievGUID)
                EnterPhase(PHASE_NORMAL_MAIEV); // Depending on whether we summoned Maiev, we switch to either phase 5 or 3
            else
                EnterPhase(PHASE_NORMAL_2);
            break;
        default:
            break;
        }

        if(m_phase == PHASE_TRANSFORM_SEQUENCE)
            Timer[EVENT_TRANSFORM_SEQUENCE] = DemonTransformation[TransformCount].timer;

        TransformCount++;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() && m_phase < PHASE_TALK_SEQUENCE)
            return;

        m_event = EVENT_NULL;
        for (uint32 i = 1; i <= MaxTimer[m_phase]; i++)
        {
            if (Timer[i]) // Event is enabled
                if (Timer[i] <= diff)
                {
                    if (m_event == EVENT_NULL) // No event with higher priority
                        m_event = IllidanEvent(i);
                }
                else
                    Timer[i] -= diff;
        }

        switch(m_phase)
        {
        case PHASE_NORMAL:
            if (HPPCT(me) < 65)
                EnterPhase(PHASE_FLIGHT_SEQUENCE);
            break;

        case PHASE_NORMAL_2:
            if (HPPCT(me) < 30)
                EnterPhase(PHASE_TALK_SEQUENCE);
            break;

        case PHASE_NORMAL_MAIEV:
            if (HPPCT(me) < 1)
                EnterPhase(PHASE_TALK_SEQUENCE);
            break;

        case PHASE_TALK_SEQUENCE:
            if (m_event == EVENT_TALK_SEQUENCE)
                HandleTalkSequence();
            break;

        case PHASE_FLIGHT_SEQUENCE:
            if (m_event == EVENT_FLIGHT_SEQUENCE)
                HandleFlightSequence();
            break;

        case PHASE_TRANSFORM_SEQUENCE:
            if (m_event == EVENT_TRANSFORM_SEQUENCE)
                HandleTransformSequence();
            break;
        }

        if (m_phase == PHASE_NORMAL || m_phase == PHASE_NORMAL_2 || m_phase == PHASE_NORMAL_MAIEV && !me->HasAura(SPELL_CAGED, 0))
        {
            switch(m_event)
            {
                //PHASE_NORMAL
            case EVENT_BERSERK:
                DoScriptText(SAY_ENRAGE, me);
                AddSpellToCast(me, SPELL_BERSERK);
                Timer[EVENT_BERSERK] = 5000; //The buff actually lasts forever.
                break;

            case EVENT_TAUNT:
            {
                DoScriptText(RAND(ILLIDAN_TAUNT_NO1, ILLIDAN_TAUNT_NO2, ILLIDAN_TAUNT_NO3, ILLIDAN_TAUNT_NO4), me);

                Timer[EVENT_TAUNT] = urand(25000, 35000);
                break;
            }
            case EVENT_SHEAR:
                AddSpellToCast(me->getVictim(), SPELL_SHEAR);
                Timer[EVENT_SHEAR] = 25000 + (rand()%16 * 1000);
                break;

            case EVENT_FLAME_CRASH:
                AddSpellToCast(me->getVictim(), SPELL_FLAME_CRASH);
                Timer[EVENT_FLAME_CRASH] = 30000 + rand()%10000;
                break;

            case EVENT_PARASITIC_SHADOWFIEND:
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true, me->getVictimGUID()))
                        AddSpellToCast(pTarget, SPELL_PARASITIC_SHADOWFIEND);

                    Timer[EVENT_PARASITIC_SHADOWFIEND] = 35000 + rand()%10000;
                break;

            case EVENT_PARASITE_CHECK:
                Timer[EVENT_PARASITE_CHECK] = 0;
                break;

            case EVENT_DRAW_SOUL:
                AddSpellToCast(me->getVictim(), SPELL_DRAW_SOUL);
                Timer[EVENT_DRAW_SOUL] = 50000 + rand()%10000;
                break;

            //PHASE_NORMAL_2
            case EVENT_AGONIZING_FLAMES:
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0, 60, true))
                    AddSpellToCast(pTarget, SPELL_AGONIZING_FLAMES);

                Timer[EVENT_AGONIZING_FLAMES] = 0;
                break;

            case EVENT_TRANSFORM_NORMAL:
                EnterPhase(PHASE_TRANSFORM_SEQUENCE);
                break;

            //PHASE_NORMAL_MAIEV
            case EVENT_ENRAGE:
                AddSpellToCast(me, SPELL_ENRAGE);
                Timer[EVENT_ENRAGE] = 0;
                break;

            default:
                break;
            }

            CastNextSpellIfAnyAndReady();
            DoMeleeAttackIfReady();
            return;
        }

        if(m_phase == PHASE_FLIGHT)
        {
            switch(m_event)
            {
            case EVENT_FIREBALL:
                if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true))
                    AddSpellToCast(pTarget, SPELL_FIREBALL);

                Timer[EVENT_FIREBALL] = 3000;
                break;

            case EVENT_DARK_BARRAGE:
                if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true))
                    AddSpellToCast(pTarget, SPELL_DARK_BARRAGE);

                if (Timer[EVENT_EYE_BLAST] < 13000)
                    Timer[EVENT_EYE_BLAST] = 13000;

                Timer[EVENT_DARK_BARRAGE] = 0;
                break;

            case EVENT_EYE_BLAST:
                CastEyeBlast();
                Timer[EVENT_EYE_BLAST] = 0;
                break;

            case EVENT_MOVE_POINT:
                m_phase = PHASE_FLIGHT_SEQUENCE;

                Timer[EVENT_FLIGHT_SEQUENCE] = 0;//do not start Event when changing hover point

                HoverPoint += (rand()%3 + 1);
                if(HoverPoint > 3)
                    HoverPoint -= 4;

                me->GetMotionMaster()->MovePoint(0, HoverPosition[HoverPoint].x, HoverPosition[HoverPoint].y, HoverPosition[HoverPoint].z);
                break;

            default:
                break;
            }

            if (!FindCreature(FLAME_OF_AZZINOTH, 100.0f, me))
                EnterPhase(PHASE_FLIGHT_SEQUENCE);
        }

        if(m_phase == PHASE_DEMON)
        {
            switch(m_event)
            {
            case EVENT_SHADOW_BLAST:
                me->GetMotionMaster()->Clear(false);

                if(!me->IsWithinDistInMap(me->getVictim(), 50) || !me->IsWithinLOSInMap(me->getVictim()))
                    me->GetMotionMaster()->MoveChase(me->getVictim(), 30);
                else
                    me->GetMotionMaster()->MoveIdle();

                AddSpellToCast(me->getVictim(), SPELL_SHADOW_BLAST);

                Timer[EVENT_SHADOW_BLAST] = 4000;
                break;
            case EVENT_SHADOWDEMON:
                AddSpellToCast(me, SPELL_SUMMON_SHADOWDEMON);

                Timer[EVENT_SHADOWDEMON] = 0;
                Timer[EVENT_FLAME_BURST] += 10000;
                break;
            case EVENT_FLAME_BURST:
                AddSpellToCast(me, SPELL_FLAME_BURST);

                Timer[EVENT_FLAME_BURST] = 15000;
                break;
            case EVENT_TRANSFORM_DEMON:
                EnterPhase(PHASE_TRANSFORM_SEQUENCE);
                break;
            default:
                break;
            }
        }
        CastNextSpellIfAnyAndReady();
    }
};

/********************************** End of Illidan AI ******************************************/

/******* Functions and vars for Akama's AI ******/
enum AkamaPhase
{
    PHASE_AKAMA_NULL    =   0,
    PHASE_OPEN_DOOR     =   1,
    PHASE_WALK          =   2,
    PHASE_TALK          =   3,
    PHASE_FIGHT_ILLIDAN =   4,
    PHASE_FIGHT_MINIONS =   5,
    PHASE_RETURN        =   6
};

static Locations AkamaPath[]=
{
    { 770.01, 304.50, 312.29 }, // Bottom of the first stairs, at the doors
    { 780.66, 304.50, 319.74 }, // Top of the first stairs
    { 790.13, 319.68, 319.76 }, // Bottom of the second stairs (left from the entrance)
    { 787.17, 347.38, 341.42 }, // Top of the second stairs
    { 781.34, 350.31, 341.44 }, // Bottom of the third stairs
    { 762.60, 361.06, 353.60 }, // Top of the third stairs
    { 756.35, 360.52, 353.27 }, // Before the door-thingy
    { 743.82, 342.21, 353.00 }, // Somewhere further
    { 732.69, 305.13, 353.00 }, // In front of Illidan - (8)
    { 738.11, 365.44, 353.00 }, // in front of the door-thingy (the other one!)
    { 792.18, 366.62, 341.42 }, // Down the first flight of stairs
    { 796.84, 304.89, 319.76 }, // Down the second flight of stairs
    { 782.01, 304.55, 319.76 }  // Final location - back at the initial gates. This is where he will fight the minions! (12)
};
// 755.762, 304.0747, 312.1769 -- This is where Akama should be spawned

static Locations SpiritSpawns[]=
{
    { 755.5426, 309.9156, 312.2129 },
    { 755.5426, 298.7923, 312.0834 }
};

//Akama spells
enum akamaSpells
{
    SPELL_AKAMA_DOOR_CHANNEL      = 41268, // Akama's channel spell on the door before the Temple Summit
    SPELL_DEATHSWORN_DOOR_CHANNEL = 41269, // Olum and Udalo's channel spell on the door before the Temple Summit
    SPELL_AKAMA_DOOR_FAIL         = 41271, // Not sure where this is really used...
    SPELL_HEALING_POTION          = 40535, // Akama uses this to heal himself to full.
    SPELL_CHAIN_LIGHTNING         = 40536  // 6938 to 8062 for 5 targets
};

enum conversationText
{
    SAY_ILLIDAN_NO1 = -1999998,
    SAY_AKAMA_NO1   = -1529099,
    SAY_ILLIDAN_NO2 = -1529000,
    SAY_AKAMA_NO2   = -1529001,
    SAY_ILLIDAN_NO3 = -1529002,
};

struct TRINITY_DLL_DECL npc_akama_illidanAI : public ScriptedAI
{
    npc_akama_illidanAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance *pInstance;

    AkamaPhase m_phase;

    bool Event;

    uint8 m_step;
    uint32 Timer;
    uint32 m_walkStep;
    uint32 TalkCount;

    uint32 m_checkTimer;
    uint32 m_phaseTimer;

    uint64 m_spiritGUID[2];
    uint64 GateGUID;
    uint64 DoorGUID[2];
    uint64 ChannelGUID;

    bool m_movePath;

    void Reset()
    {
        pInstance->SetData(EVENT_ILLIDANSTORMRAGE, NOT_STARTED);

        if (pInstance->GetData(EVENT_ILLIDARIDOOR) != DONE)                   //close all doors at create
        {
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_DOOR_R), false);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_DOOR_L), false);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_GATE), false);

            m_walkStep = 0;
        }
        else
        {
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_DOOR_R), true);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_DOOR_L), true);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_GATE), true);

            me->SetSelection(pInstance->GetData64(DATA_ILLIDANSTORMRAGE));
            me->SetHomePosition(AkamaPath[8].x, AkamaPath[8].y, AkamaPath[8].z, me->GetOrientation());
            me->GetMap()->CreatureRelocation(me, AkamaPath[8].x, AkamaPath[8].y, AkamaPath[8].z, 0);

            m_walkStep = 9;
        }

        m_phase = PHASE_AKAMA_NULL;

        m_movePath = false;

        m_checkTimer = 2000;
        m_phaseTimer = 0;

        m_step = 0;

        TalkCount = 0;

        KillAllElites();

        me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        me->SetVisibility(VISIBILITY_OFF);
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        me->setActive(true);
    }

    void EnterEvadeMode()
    {
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
    }

    void MoveInLineOfSight(Unit *){}

    void MovementInform(uint32 MovementType, uint32 data)
    {
        if (MovementType != POINT_MOTION_TYPE)
            return;

        switch (data)
        {
            case 2:
                pInstance->SetData(EVENT_ILLIDARIDOOR, DONE);
                m_movePath = true;
                break;
            case 8:
                if (m_phase == PHASE_WALK)
                {
                    m_step = 0;
                    m_phase = PHASE_TALK;
                }
                else
                {
                }
                break;
            case 9:
                DoScriptText(SAY_AKAMA_LEAVE, me);
                m_movePath = true;
                break;
            case 12:
                m_phase = PHASE_FIGHT_MINIONS;
                break;
            default:
                m_movePath = true;
                break;
        }

        ++m_walkStep;
    }

    void KillAllElites()
    {
        std::list<HostilReference*>& threatList = me->getThreatManager().getThreatList();
        std::vector<Unit*> eliteList;
        for(std::list<HostilReference*>::iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());
            if(pUnit && pUnit->GetEntry() == ILLIDARI_ELITE)
                eliteList.push_back(pUnit);
        }

        for(std::vector<Unit*>::iterator itr = eliteList.begin(); itr != eliteList.end(); ++itr)
            (*itr)->setDeathState(JUST_DIED);

        EnterEvadeMode();
    }

    void StartEvent()
    {
        if (pInstance->GetData(EVENT_ILLIDARIDOOR) == DONE)
        {
            me->SetSelection(pInstance->GetData64(DATA_ILLIDANSTORMRAGE));

            pInstance->SetData(EVENT_ILLIDANSTORMRAGE, IN_PROGRESS);
            m_phase = PHASE_TALK;
        }
        else
        {
            float x, y, z;
            if (GameObject *pGo = me->GetMap()->GetGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_GATE)))
                pGo->GetPosition(x, y, z);

            me->SummonCreature(ILLIDAN_DOOR_TRIGGER, x, y, z +5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000);
            m_phase = PHASE_OPEN_DOOR;
        }

        me->RemoveUnitMovementFlag(SPLINEFLAG_WALKMODE_MODE);
        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        me->setActive(true);
    }

    void HandleDoorEvent()
    {
        switch (m_step)
        {
            case 0:
                if (Unit *pDoor = FindCreature(ILLIDAN_DOOR_TRIGGER, 30.0f, me))
                    ForceSpellCast(pDoor, SPELL_AKAMA_DOOR_CHANNEL);

                m_phaseTimer = 5000;
                break;
            case 1:
                me->InterruptNonMeleeSpells(true);
                m_phaseTimer = 2000;
                break;
            case 2:
                for(uint8 i = 0; i < 2; ++i)
                {
                    if(Creature *pSpirit = me->SummonCreature(SPIRIT_OF_UDALO +i, SpiritSpawns[i].x, SpiritSpawns[i].y, SpiritSpawns[i].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000))
                         m_spiritGUID[i] = pSpirit->GetGUID();
                }
                m_phaseTimer = 2000;
                break;
            case 3:
                if (Unit *pDoor = FindCreature(ILLIDAN_DOOR_TRIGGER, 30.0f, me))
                {
                     for(uint8 i = 0; i < 2; ++i)
                     {
                          if (Creature *pSpirit = pInstance->GetCreature(m_spiritGUID[i]))
                              pSpirit->CastSpell(pDoor, SPELL_DEATHSWORN_DOOR_CHANNEL, false);
                     }

                     ForceSpellCast(pDoor, SPELL_AKAMA_DOOR_CHANNEL);
                }
                m_phaseTimer = 5000;
                break;
            case 4:
                if (Unit *pDoor = FindCreature(ILLIDAN_DOOR_TRIGGER, 30.0f, me))
                {
                    pDoor->SetVisibility(VISIBILITY_OFF);
                    pDoor->DestroyForNearbyPlayers();
                    pDoor->Kill(pDoor, false);
                }

                if (GameObject *pDoor = me->GetMap()->GetGameObject(pInstance->GetData64(DATA_GAMEOBJECT_ILLIDAN_GATE)))
                    pDoor->SetGoState(GO_STATE_ACTIVE);

                m_phaseTimer = 2000;
                break;
            case 5:
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                m_phaseTimer = 2000;
                break;
            case 6:
                DoScriptText(SAY_AKAMA_BEWARE, me);

                for(uint8 i = 0; i < 2; ++i)
                {
                    if (Creature *pSpirit = pInstance->GetCreature(m_spiritGUID[i]))
                    {
                        pSpirit->SetVisibility(VISIBILITY_OFF);
                        pSpirit->DestroyForNearbyPlayers();
                    }
                }
                m_phaseTimer = 3000;
                break;
            case 7:
                me->GetMotionMaster()->MovePoint(m_walkStep, AkamaPath[m_walkStep].x, AkamaPath[m_walkStep].y, AkamaPath[m_walkStep].z);
                m_phase = PHASE_WALK;
                m_step = 0;
                return;
        }
        ++m_step;
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_checkTimer < diff)
        {
            if (me->GetVisibility() == VISIBILITY_OFF)
            {
                if (pInstance->GetData(EVENT_ILLIDARICOUNCIL) == DONE)
                    me->SetVisibility(VISIBILITY_ON);
            }

            m_checkTimer = 2000;
        }
        else
            m_checkTimer -= diff;

        switch (m_phase)
        {
            case PHASE_OPEN_DOOR:
            {
                if (m_phaseTimer < diff)
                    HandleDoorEvent();
                else
                    m_phaseTimer -= diff;

                CastNextSpellIfAnyAndReady();
                return;
            }
            case PHASE_TALK:
                if (m_phaseTimer < diff)
                {
                    switch (m_step)
                    {
                        case 0:
                            if(Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
                            {
                                pIllidan->SetSelection(me->GetGUID());

                                DoScriptText(SAY_ILLIDAN_NO1, pIllidan);
                                m_phaseTimer = 12000;
                            }
                            break;
                        case 1:
                            DoScriptText(SAY_AKAMA_NO1, me);
                            m_phaseTimer = 10000;
                            break;
                        case 2:
                            if(Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
                            {
                                pIllidan->RemoveAurasDueToSpell(SPELL_KNEEL);
                                DoScriptText(SAY_ILLIDAN_NO2, pIllidan);
                                m_phaseTimer = 7000;
                            }
                            break;
                        case 3:
                            if(Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
                            {
                                pIllidan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                                pIllidan->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 45479);
                                pIllidan->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 45481);
                                pIllidan->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE );
                                pIllidan->RemoveUnitMovementFlag(SPLINEFLAG_WALKMODE_MODE);
                            }

                            DoScriptText(SAY_AKAMA_NO2, me);
                            m_phaseTimer = 5000;
                            break;
                        case 4:
                            if(Creature *pIllidan = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDANSTORMRAGE)))
                            {
                                DoScriptText(SAY_ILLIDAN_NO3, pIllidan);
                                AttackStart(pIllidan);
                            }

                            m_phase = PHASE_FIGHT_ILLIDAN;
                            return;
                    }
                    ++m_step;
                }
                else
                    m_phaseTimer -= diff;
            break;
            case PHASE_WALK:
            case PHASE_RETURN:
                if (m_movePath)
                {
                    me->GetMotionMaster()->MovePoint(m_walkStep, AkamaPath[m_walkStep].x, AkamaPath[m_walkStep].y, AkamaPath[m_walkStep].z);
                    m_movePath = false;
                }
            break;
            case PHASE_FIGHT_ILLIDAN:
            {
                if (m_phaseTimer < diff)
                {
                    GETUNIT(Illidan, pInstance->GetData64(DATA_ILLIDANSTORMRAGE));
                    if(Illidan && HPPCT(Illidan) < 90)
                    {
                        DoScriptText(SAY_AKAMA_MINION, Illidan);

                        m_phase = PHASE_WALK;
                        m_walkStep = 9;
                        m_movePath = true;
                    }
                    m_phaseTimer = 2000;
                }
                else
                    m_phaseTimer -= diff;
            }
            break;
            case PHASE_FIGHT_MINIONS:
            {
                if (m_phaseTimer < diff)
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);

                    if (Creature* Elite = me->SummonCreature(ILLIDARI_ELITE, x+rand()%10, y+rand()%10, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000))
                    {
                        Elite->AI()->AttackStart(me);
                        Elite->AddThreat(me, 1000000.0f);

                        AttackStart(Elite);
                        me->AddThreat(Elite, 1000000.0f);
                    }
                    m_phaseTimer = urand(10000, 16000);

                    GETUNIT(Illidan, pInstance->GetData64(DATA_ILLIDANSTORMRAGE));

                    if(Illidan && HPPCT(Illidan) < 10)
                    {
                        KillAllElites();
                        m_phase = PHASE_RETURN;
                        m_walkStep = 1;
                        m_movePath = true;
                    }
                }
                else
                    m_phaseTimer -= diff;
            }
            break;
            default:
                break;
        }

        if(!UpdateVictim())
            return;

        if(me->GetHealth()*100 / me->GetMaxHealth() < 20)
            DoCast(me, SPELL_HEALING_POTION);

        DoMeleeAttackIfReady();
    }
};

enum MaievEvent
{
    EVENT_MAIEV_NULL            =   0,
    EVENT_MAIEV_STEALTH         =   1,
    EVENT_MAIEV_TAUNT           =   2,
    EVENT_MAIEV_SHADOW_STRIKE   =   3,
    EVENT_MAIEV_THROW_DAGGER    =   4,
    EVENT_MAIEV_TRAP            =   4,
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

struct TRINITY_DLL_DECL boss_maievAI : public ScriptedAI
{
    boss_maievAI(Creature *c) : ScriptedAI(c) {};

    uint64 IllidanGUID;

    IllidanPhase m_phase;
    MaievEvent m_event;
    uint32 Timer[5];
    uint32 MaxTimer;

    void Reset()
    {
        MaxTimer = 0;
        m_phase = PHASE_NORMAL_MAIEV;
        IllidanGUID = 0;
        Timer[EVENT_MAIEV_STEALTH] = 0;
        Timer[EVENT_MAIEV_TAUNT] = 22000 + rand()%21 * 1000;
        Timer[EVENT_MAIEV_SHADOW_STRIKE] = 30000;
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 44850);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 2, 45738);
    }

    void EnterCombat(Unit *who) {}
    void MoveInLineOfSight(Unit *who) {}
    void EnterEvadeMode() {}
    void GetIllidanGUID(uint64 guid) { IllidanGUID = guid; }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if(done_by->GetGUID() != IllidanGUID )
            damage = 0;
        else
        {
            GETUNIT(Illidan, IllidanGUID);
            if(Illidan && Illidan->getVictim() == me)
                damage = me->GetMaxHealth()/10;
            if(damage >= me->GetHealth())
                damage = 0;
        }
    }

    void AttackStart(Unit *who)
    {
        if(!who || Timer[EVENT_MAIEV_STEALTH])
            return;

        if(m_phase == PHASE_TALK_SEQUENCE)
            AttackStartNoMove(who);
        else if(m_phase == PHASE_DEMON || m_phase == PHASE_TRANSFORM_SEQUENCE )
        {
            GETUNIT(Illidan, IllidanGUID);
            if(Illidan && me->IsWithinDistInMap(Illidan, 25))
                BlinkToPlayer();    //Do not let dread aura hurt her.

            AttackStartNoMove(who);
        }
        else
            ScriptedAI::AttackStart(who);
    }

    void DoAction(const int32 param)
    {
        if(param > PHASE_ILLIDAN_NULL && param < PHASE_ILLIDAN_MAX)
            EnterPhase(IllidanPhase(param));
    }

    void EnterPhase(IllidanPhase m_nextPhase)//This is in fact Illidan's phase.
    {
        switch(m_nextPhase)
        {
        case PHASE_TALK_SEQUENCE:
            if(Timer[EVENT_MAIEV_STEALTH])
            {
                me->SetHealth(me->GetMaxHealth());
                me->SetVisibility(VISIBILITY_ON);
                Timer[EVENT_MAIEV_STEALTH] = 0;
            }
            me->InterruptNonMeleeSpells(false);
            me->GetMotionMaster()->Clear(false);
            me->AttackStop();
            me->SetSelection(IllidanGUID);
            MaxTimer = 0;
            break;
        case PHASE_TRANSFORM_SEQUENCE:
            MaxTimer = 4;
            Timer[EVENT_MAIEV_TAUNT] += 10000;
            Timer[EVENT_MAIEV_THROW_DAGGER] = 2000;
            break;
        case PHASE_DEMON:
            break;
        case PHASE_NORMAL_MAIEV:
            MaxTimer = 4;
            Timer[EVENT_MAIEV_TAUNT] += 10000;
            Timer[EVENT_MAIEV_TRAP] = 22000;
            break;
        default:
            break;
        }

        if(Timer[EVENT_MAIEV_STEALTH])
            MaxTimer = 1;

        m_phase = m_nextPhase;
    }

    void BlinkTo(float x, float y, float z)
    {
        me->AttackStop();
        me->InterruptNonMeleeSpells(false);
        me->GetMotionMaster()->Clear(false);
        DoTeleportTo(x, y, z);
        DoCast(me, SPELL_TELEPORT_VISUAL, true);
    }

    void BlinkToPlayer()
    {
        if(GETCRE(Illidan, IllidanGUID))
        {
            Unit* target = ((boss_illidan_stormrageAI*)Illidan->AI())->SelectUnit(SELECT_TARGET_RANDOM, 0, 80, true, 0, 20);

            if(!target || !me->IsWithinDistInMap(target, 80))
            {
                uint8 pos = rand()%4;
                BlinkTo(HoverPosition[pos].x, HoverPosition[pos].y, HoverPosition[pos].z);
            }
            else
            {
                float x, y, z;
                target->GetPosition(x, y, z);
                BlinkTo(x, y, z);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim() && !Timer[EVENT_MAIEV_STEALTH])
            return;

        m_event = EVENT_MAIEV_NULL;
        for(uint8 i = 1; i <= MaxTimer; i++)
        {
            if( Timer[i])
            {
                if (Timer[i] <= diff)
                    m_event = MaievEvent(i);
                else
                    Timer[i] -= diff;
            }

            switch(m_event)
            {
            case EVENT_MAIEV_STEALTH:
                {
                    me->SetHealth(me->GetMaxHealth());
                    me->SetVisibility(VISIBILITY_ON);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Timer[EVENT_MAIEV_STEALTH] = 0;
                    BlinkToPlayer();
                    EnterPhase(m_phase);
                }
                break;
            case EVENT_MAIEV_TAUNT:
            {
                DoScriptText(RAND(MAIEV_TAUNT_NO1, MAIEV_TAUNT_NO2, MAIEV_TAUNT_NO3, MAIEV_TAUNT_NO4), me);
                Timer[EVENT_MAIEV_TAUNT] = urand(22000, 43000);
                break;
            }
            case EVENT_MAIEV_SHADOW_STRIKE:
                DoCast(me->getVictim(), SPELL_SHADOW_STRIKE);
                Timer[EVENT_MAIEV_SHADOW_STRIKE] = 60000;
                break;
            case EVENT_MAIEV_TRAP:
                if(m_phase == PHASE_NORMAL_MAIEV)
                {
                    BlinkToPlayer();
                    DoCast(me, SPELL_CAGE_TRAP_SUMMON);
                    Timer[EVENT_MAIEV_TRAP] = 22000;
                }
                else
                {
                    if(!me->IsWithinDistInMap(me->getVictim(), 40))
                        me->GetMotionMaster()->MoveChase(me->getVictim(), 30);

                    DoCast(me->getVictim(), SPELL_THROW_DAGGER);
                    Timer[EVENT_MAIEV_THROW_DAGGER] = 2000;
                }
                break;
            default:
                break;
            }

            if(me->GetHealth()*100 / me->GetMaxHealth() < 50)
            {
                me->SetVisibility(VISIBILITY_OFF);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if(GETCRE(Illidan, IllidanGUID))
                    ((boss_illidan_stormrageAI*)Illidan->AI())->DeleteFromThreatList(me->GetGUID());
                me->AttackStop();
                Timer[EVENT_MAIEV_STEALTH] = 60000; //reappear after 1 minute
                MaxTimer = 1;
            }

            if(m_phase == PHASE_NORMAL_MAIEV)
                DoMeleeAttackIfReady();
        }
    }
};


bool GossipSelect_npc_akama_at_illidan(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF) // Time to begin the Event
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ((npc_akama_illidanAI*)pCreature->AI())->StartEvent();
    }
    return true;
}

bool GossipHello_npc_akama_at_illidan(Player *player, Creature *_Creature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    player->SEND_GOSSIP_MENU(10465, _Creature->GetGUID());

    return true;
}

struct TRINITY_DLL_DECL cage_trap_triggerAI : public ScriptedAI
{
    cage_trap_triggerAI(Creature *c) : ScriptedAI(c) {}

    uint64 IllidanGUID;
    uint32 DespawnTimer;

    bool Active;
    bool SummonedBeams;

    void Reset()
    {
        IllidanGUID = 0;

        Active = false;
        SummonedBeams = false;

        DespawnTimer = 0;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void EnterCombat(Unit *who){}

    void MoveInLineOfSight(Unit *who)
    {
        if(!Active)
            return;

        if(who && (who->GetTypeId() != TYPEID_PLAYER))
        {
            if(who->GetEntry() == ILLIDAN_STORMRAGE) // Check if who is Illidan
            {
                if(!IllidanGUID && me->IsWithinDistInMap(who, 3) && (!who->HasAura(SPELL_CAGED, 0)))
                {
                    IllidanGUID = who->GetGUID();
                    who->CastSpell(who, SPELL_CAGED, true);
                    DespawnTimer = 5000;
                    if(who->HasAura(SPELL_ENRAGE, 0))
                        who->RemoveAurasDueToSpell(SPELL_ENRAGE); // Dispel his enrage
                    //if(GameObject* CageTrap = GameObject::GetGameObject(*me, CageTrapGUID))
                    //    CageTrap->SetLootState(GO_JUST_DEACTIVATED);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(DespawnTimer)
            if(DespawnTimer < diff)
                me->DealDamage(me, me->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            else DespawnTimer -= diff;

            //if(IllidanGUID && !SummonedBeams)
            //{
            //    if(Unit* Illidan = Unit::GetUnit(*me, IllidanGUID)
            //    {
            //        //TODO: Find proper spells and properly apply 'caged' Illidan effect
            //    }
            //}
    }
};

bool GOHello_cage_trap(Player* plr, GameObject* go)
{
    float x, y, z;
    plr->GetPosition(x, y, z);

    Creature* trigger = NULL;

    // Grid search for nearest live creature of entry 23304 within 10 yards
    Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck check(*plr, 23304, true, 10);
    Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(trigger, check);

    Cell::VisitGridObjects(plr, searcher, 10.0);

    if(trigger)
        ((cage_trap_triggerAI*)trigger->AI())->Active = true;

    go->SetUInt32Value(GAMEOBJECT_STATE, 0);
    return true;
}

enum demonSpells
{
    SPELL_SHADOW_DEMON_PASSIVE      = 41079,  // Adds the "shadowform" aura to Shadow Demons.
    SPELL_SHADOW_DEMON_CONSUME_SOUL = 41080,  // Once the Shadow Demons reach their target, they use this to kill them
    SPELL_SHADOW_DEMON_PARALYZE     = 41083,  // Shadow Demons cast this on their target
    SPELL_SHADOW_DEMON_PURPLE_BEAM  = 39123   // Purple Beam connecting Shadow Demon to their target
};

struct TRINITY_DLL_DECL shadow_demonAI : public ScriptedAI
{
    shadow_demonAI(Creature *c) : ScriptedAI(c) {}

    uint64 m_targetGUID;

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
    }

    void Reset()
    {
        m_targetGUID = 0;
        ForceSpellCast(me, SPELL_SHADOW_DEMON_PASSIVE, INTERRUPT_AND_CAST_INSTANTLY);
    }

    void JustDied(Unit *killer)
    {
        if (Unit *pTarget = me->GetUnit(m_targetGUID))
            pTarget->RemoveAurasDueToSpell(SPELL_SHADOW_DEMON_PARALYZE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        // Only cast the below on players.
        if (me->getVictim() && me->getVictim()->GetTypeId() != TYPEID_PLAYER)
            return;

        if(!me->getVictim()->HasAura(SPELL_SHADOW_DEMON_PARALYZE, 0))
        {
            m_targetGUID = me->getVictimGUID();

            me->AddThreat(me->getVictim(), 10000000.0f);

            ForceSpellCast(me->getVictim(), SPELL_SHADOW_DEMON_PURPLE_BEAM);
            ForceSpellCast(me->getVictim(), SPELL_SHADOW_DEMON_PARALYZE);
        }
        // Kill our target if we're very close.
        if (me->IsWithinDistInMap(me->getVictim(), 3.0f))
            ForceSpellCast(me->getVictim(), SPELL_SHADOW_DEMON_CONSUME_SOUL);

        CastNextSpellIfAnyAndReady();
    }
};

// Shadowfiends interact with Illidan, setting more targets in Illidan's hashmap
struct TRINITY_DLL_DECL mob_parasitic_shadowfiendAI : public ScriptedAI
{
    mob_parasitic_shadowfiendAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance* pInstance;
    uint64 IllidanGUID;
    uint32 CheckTimer;

    void Reset()
    {
        if(pInstance)
            IllidanGUID = pInstance->GetData64(DATA_ILLIDANSTORMRAGE);
        else
            IllidanGUID = 0;

        CheckTimer = 5000;
        ForceSpellCast(me, SPELL_SHADOWFIEND_PASSIVE, INTERRUPT_AND_CAST_INSTANTLY);
    }

    void EnterCombat(Unit* who)
    {
        DoZoneInCombat();
    }

    void DoMeleeAttackIfReady()
    {
        if (me->isAttackReady() && me->IsWithinMeleeRange(me->getVictim()))
        {
            if(!me->getVictim()->HasAura(SPELL_PARASITIC_SHADOWFIEND, 0) && !me->getVictim()->HasAura(SPELL_PARASITIC_SHADOWFIEND2, 0))
                me->CastSpell(me->getVictim(), SPELL_PARASITIC_SHADOWFIEND2, true, 0, 0, IllidanGUID); //do not stack

            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->getVictim())
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 999, true))
                AttackStart(pTarget);
            else
            {
                me->SetVisibility(VISIBILITY_OFF);
                me->setDeathState(JUST_DIED);
                return;
            }
        }

        if (CheckTimer < diff)
        {
            GETUNIT(Illidan, IllidanGUID);
            if (!Illidan || ((Creature*)Illidan)->IsInEvadeMode())
            {
                me->SetVisibility(VISIBILITY_OFF);
                me->setDeathState(JUST_DIED);
                return;
            }
            else
                CheckTimer = 5000;
        }
        else
            CheckTimer -= diff;

        DoMeleeAttackIfReady();
    }
};


enum bladeSpells
{
    SPELL_SUMMON_FLAME  = 39855,
    SPELL_FLAME_CHANNEL = 39857
};

struct TRINITY_DLL_DECL blade_of_azzinothAI : public Scripted_NoMovementAI
{
    blade_of_azzinothAI(Creature* c) : Scripted_NoMovementAI(c)
    {
        pInstance = (c->GetInstanceData());
    }

    ScriptedInstance *pInstance;

    uint32 m_checkTimer;
    uint64 m_flameGUID;

    void Reset()
    {
        ClearCastQueue();

        m_checkTimer = 2000;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_THROW_GLAIVE2 || spell->Id == SPELL_THROW_GLAIVE)
        {
            me->SetUInt32Value(UNIT_FIELD_DISPLAYID, 21431);
            AddSpellToCast(me, SPELL_SUMMON_FLAME);
        }
    }

    void JustSummoned(Creature *pWho)
    {
        if (pWho->GetEntry() == FLAME_OF_AZZINOTH)
        {
            m_flameGUID = pWho->GetGUID();

            pWho->SetOwnerGUID(me->GetGUID());

            pWho->setFaction(me->getFaction()); // Just in case the database has it as a different faction
            pWho->SetMeleeDamageSchool(SPELL_SCHOOL_FIRE);
        }
    }

    void UpdateAI(uint32 const diff)
    {
        if (m_checkTimer < diff)
        {
            if (Creature *pFlame = pInstance->GetCreature(m_flameGUID))
            {
                if (pFlame->isAlive())
                {
                    if (!me->IsNonMeleeSpellCasted(false, false))
                    {
                        if (me->IsWithinDistInMap(pFlame, FLAME_ENRAGE_DISTANCE) && !pFlame->HasAura(SPELL_FLAME_ENRAGE, 0))
                            AddSpellToCast(pFlame, SPELL_FLAME_CHANNEL);
                    }
                }
                else
                    m_flameGUID = NULL;
            }
            m_checkTimer = 2000;
        }
        else
            m_checkTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};

struct TRINITY_DLL_DECL flame_of_azzinothAI : public ScriptedAI
{
    flame_of_azzinothAI(Creature *c) : ScriptedAI(c){}

    uint32 m_blastTimer;

    uint32 m_checkTimer;

    void Reset()
    {
        ClearCastQueue();

        m_blastTimer = 15000;
        m_checkTimer = 5000;
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
    }

    void JustDied(Unit *pKiller)
    {
        me->RemoveCorpse();
    }

    void ChargeCheck()
    {
        if (Unit *pTarget = SelectUnit(SELECT_TARGET_FARTHEST, 0, 200, false))
        {
            if (!me->IsWithinCombatRange(pTarget, FLAME_CHARGE_DISTANCE))
            {
                AttackStart(pTarget);

                AddSpellToCast(pTarget, SPELL_CHARGE);
                DoTextEmote("sets its gaze on $N!", pTarget);
            }
        }
    }

    void EnrageCheck()
    {
        if (Creature *pGlaive = me->GetCreature(me->GetOwnerGUID()))
        {
            if (!me->IsWithinDistInMap(pGlaive, FLAME_ENRAGE_DISTANCE))
            {
                pGlaive->InterruptNonMeleeSpells(false);

                AddSpellToCast(me, SPELL_FLAME_ENRAGE);

                DoResetThreat();

                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
                    AttackStart(pTarget);
            }
            else if(me->HasAura(SPELL_FLAME_ENRAGE, 0))
                me->RemoveAurasDueToSpell(SPELL_FLAME_ENRAGE);
        }
    }


    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_blastTimer < diff)
        {
            AddSpellToCast(me->getVictim(), SPELL_BLAZE_SUMMON);
            AddSpellToCast(me->getVictim(), SPELL_FLAME_BLAST);

            m_blastTimer = urand(10000, 15000);
        }
        else
            m_blastTimer -= diff;

        if (m_checkTimer < diff)
        {
            EnrageCheck();
            ChargeCheck();
            m_checkTimer = 2000;
        }
        else
            m_checkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

void boss_illidan_stormrageAI::Reset()
{
    ClearCastQueue();

    if(pInstance)
        pInstance->SetData(EVENT_ILLIDANSTORMRAGE, NOT_STARTED);

    AkamaGUID = pInstance->GetData64(DATA_AKAMA);

    if (AkamaGUID)
    {
        if (GETCRE(Akama, AkamaGUID))
        {
            if (!Akama->isAlive())
                Akama->Respawn();
            else
            {
                ((npc_akama_illidanAI*)Akama->AI())->EnterEvadeMode();
                Akama->GetMotionMaster()->MoveTargetedHome();
                ((npc_akama_illidanAI*)Akama->AI())->Reset();
            }
        }
        AkamaGUID = 0;
    }

    MaievGUID = 0;
    for (int i = 0; i < 2; ++i)
        GlaiveGUID[i] = 0;

    m_phase = PHASE_ILLIDAN_NULL;
    m_event = EVENT_NULL;
    Timer[EVENT_BERSERK] = 1500000;

    HoverPoint = 0;
    TalkCount = 0;
    FlightCount = 0;
    TransformCount = 0;

    me->SetUInt32Value(UNIT_FIELD_DISPLAYID, 21135);
    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
    me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
    me->RemoveUnitMovementFlag(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
    me->setActive(false);

    ForceSpellCast(me, SPELL_KNEEL, INTERRUPT_AND_CAST_INSTANTLY);
    Summons.DespawnAll();
}

void boss_illidan_stormrageAI::JustSummoned(Creature* summon)
{
    Summons.Summon(summon);
    switch(summon->GetEntry())
    {
    case PARASITIC_SHADOWFIEND:
        {
            if (m_phase == PHASE_TALK_SEQUENCE)
            {
                summon->SetVisibility(VISIBILITY_OFF);
                summon->setDeathState(JUST_DIED);
                return;
            }
            Unit *target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 999, true);
            if(!target || target->HasAura(SPELL_PARASITIC_SHADOWFIEND, 0)
                || target->HasAura(SPELL_PARASITIC_SHADOWFIEND2, 0))
                target = SelectUnit(SELECT_TARGET_RANDOM, 0, 999, true);
            if(target)
                summon->AI()->AttackStart(target);
        }
        break;
    case SHADOW_DEMON:
        if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 999, true)) // only on players.
        {
            summon->AddThreat(target, 5000000.0f);
            summon->AI()->AttackStart(target);
        }
        break;
    case MAIEV_SHADOWSONG:
        {
            summon->SetVisibility(VISIBILITY_OFF); // Leave her invisible until she has to talk
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            MaievGUID = summon->GetGUID();
            ((boss_maievAI*)summon->AI())->GetIllidanGUID(me->GetGUID());
            summon->AI()->DoAction(PHASE_TALK_SEQUENCE);
        }
        break;
    default:
        break;
    }
}

void boss_illidan_stormrageAI::HandleTalkSequence()
{
    switch(TalkCount)
    {
    case 0:
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        break;
    case 8:
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 45479);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 45481);
        me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE );
        me->RemoveUnitMovementFlag(SPLINEFLAG_WALKMODE_MODE);
        break;
    case 9:
        if(GETCRE(Akama, AkamaGUID))
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
            me->AddThreat(Akama, 100.0f);
            EnterPhase(PHASE_NORMAL);
        }
        break;
    case 10:
        SummonMaiev();
        break;
    case 11:
        if(GETUNIT(Maiev, MaievGUID))
        {
            Maiev->SetVisibility(VISIBILITY_ON); // Maiev is now visible
            Maiev->CastSpell(Maiev, SPELL_TELEPORT_VISUAL, true); // onoz she looks like she teleported!
            Maiev->SetInFront(me); // Have her face us
            me->SetInFront(Maiev); // Face her, so it's not rude =P
            Maiev->GetMotionMaster()->MoveIdle();
            me->GetMotionMaster()->MoveIdle();
        }break;
    case 14:
        if(GETCRE(Maiev, MaievGUID))
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
            Maiev->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
            Maiev->AddThreat(me, 10000000.0f); // Have Maiev add a lot of threat on us so that players don't pull her off if they damage her via AOE
            Maiev->AI()->AttackStart(me); // Force Maiev to attack us.
            EnterPhase(PHASE_NORMAL_MAIEV);
        }break;
    case 15:
        DoCast(me, SPELL_DEATH); // Animate his kneeling + stun him
        Summons.DespawnAll();
        break;
    case 17:
        if(GETUNIT(Akama, AkamaGUID))
        {
            if(!me->IsWithinDistInMap(Akama, 15))
            {
                float x, y, z;
                me->GetPosition(x, y, z);
                x += 10; y += 10;
                Akama->GetMotionMaster()->Clear(false);
                //Akama->GetMotionMaster()->MoveIdle();
                Akama->Relocate(x, y, z);
                Akama->SendMonsterMove(x, y, z, 0);//Illidan must not die until Akama arrives.
                Akama->GetMotionMaster()->MoveChase(me);
            }
        }
        break;
    case 19: // Make Maiev leave
        if(GETUNIT(Maiev, MaievGUID))
        {
            Maiev->CastSpell(Maiev, SPELL_TELEPORT_VISUAL, true);
            Maiev->setDeathState(JUST_DIED);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1,PLAYER_STATE_DEAD);
        }
        break;
    case 21: // Kill ourself.
        me->DealDamage(me, me->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        break;
    default:
        break;
    }

    TalkCount++;
}


void boss_illidan_stormrageAI::CastEyeBlast()
{
    me->InterruptNonMeleeSpells(false);

    DoScriptText(SAY_EYE_BLAST, me);

    float distx, disty, dist[2];
    for(uint8 i = 0; i < 2; ++i)
    {
        distx = EyeBlast[i].x - HoverPosition[HoverPoint].x;
        disty = EyeBlast[i].y - HoverPosition[HoverPoint].y;
        dist[i] = distx * distx + disty * disty;
    }

    Locations initial = EyeBlast[dist[0] < dist[1] ? 0 : 1];
    for(uint8 i = 0; i < 2; ++i)
    {
        distx = GlaivePosition[i].x - HoverPosition[HoverPoint].x;
        disty = GlaivePosition[i].y - HoverPosition[HoverPoint].y;
        dist[i] = distx * distx + disty * disty;
    }
    Locations final = GlaivePosition[dist[0] < dist[1] ? 0 : 1];

    final.x = 2 * final.x - initial.x;
    final.y = 2 * final.y - initial.y;

    Creature* Trigger = me->SummonTrigger(initial.x, initial.y, initial.z, 0, 13000);
    if(!Trigger)
        return;

    Trigger->SetSpeed(MOVE_WALK, 3);
    Trigger->SetUnitMovementFlags(SPLINEFLAG_WALKMODE_MODE);
    Trigger->GetMotionMaster()->MovePoint(0, final.x, final.y, final.z);

    me->SetSelection(Trigger->GetGUID());
    AddSpellToCast(Trigger, SPELL_EYE_BLAST);
}


void boss_illidan_stormrageAI::SummonMaiev()
{
    me->CastSpell(me, SPELL_SHADOW_PRISON, true);
    me->CastSpell(me, 40403, true);
    if(!MaievGUID) // If Maiev cannot be summoned, reset the encounter and post some errors to the console.
    {
        EnterEvadeMode();
        DoTextEmote("is unable to summon Maiev Shadowsong and enter Phase 4. Resetting Encounter.", NULL);
        error_log("SD2 ERROR: Unable to summon Maiev Shadowsong (entry: 23197). Check your database to see if you have the proper SQL for Maiev Shadowsong (entry: 23197)");
    }
}


void boss_illidan_stormrageAI::EnterPhase(IllidanPhase m_nextPhase)
{
    switch(m_nextPhase)
    {
    case PHASE_NORMAL:
    case PHASE_NORMAL_2:
    case PHASE_NORMAL_MAIEV:
        AttackStart(me->getVictim());
        Timer[EVENT_TAUNT] = 32000;
        Timer[EVENT_SHEAR] = 10000 + rand()%15 * 1000;
        Timer[EVENT_FLAME_CRASH] = 20000;
        Timer[EVENT_PARASITIC_SHADOWFIEND] = 25000;
        Timer[EVENT_PARASITE_CHECK] = 0;
        Timer[EVENT_DRAW_SOUL] = 30000;
        if(m_nextPhase == PHASE_NORMAL)
            break;
        Timer[EVENT_AGONIZING_FLAMES] = 35000;
        Timer[EVENT_TRANSFORM_NORMAL] = 60000;
        if(m_nextPhase == PHASE_NORMAL_2)
            break;
        Timer[EVENT_ENRAGE] = 30000 + rand()%10 * 1000;
        break;
    case PHASE_FLIGHT:
        Timer[EVENT_FIREBALL] = 1000;
        if(!(rand()%4))
            Timer[EVENT_DARK_BARRAGE] = 10000;
        Timer[EVENT_EYE_BLAST] = 10000 + rand()%15 * 1000;
        Timer[EVENT_MOVE_POINT] = 20000 + rand()%20 * 1000;
        break;
    case PHASE_DEMON:
        Timer[EVENT_SHADOW_BLAST] = 1000;
        Timer[EVENT_FLAME_BURST] = 10000;
        Timer[EVENT_SHADOWDEMON] = 30000;
        Timer[EVENT_TRANSFORM_DEMON] = 60000;
        AttackStart(me->getVictim());
        break;
    case PHASE_TALK_SEQUENCE:
        Timer[EVENT_TALK_SEQUENCE] = 100;
        me->RemoveAllAuras();
        me->InterruptNonMeleeSpells(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
        me->GetMotionMaster()->Clear(false);
        me->AttackStop();
        break;
    case PHASE_FLIGHT_SEQUENCE:
        if(m_phase == PHASE_NORMAL) //lift off
        {
            FlightCount = 1;
            Timer[EVENT_FLIGHT_SEQUENCE] = 1;
            me->RemoveAllAuras();
            me->InterruptNonMeleeSpells(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->GetMotionMaster()->Clear(false);
            me->AttackStop();
        }
        else //land
            Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
        break;
    case PHASE_TRANSFORM_SEQUENCE:
        if(m_phase == PHASE_DEMON)
            Timer[EVENT_TRANSFORM_SEQUENCE] = 500;
        else
        {
            TransformCount = 0;
            Timer[EVENT_TRANSFORM_SEQUENCE] = 500;
            DoScriptText(SAY_MORPH, me);
        }
        me->GetMotionMaster()->Clear();
        me->AttackStop();
        break;
    default:
        break;
    }
    if(MaievGUID)
    {
        GETCRE(Maiev, MaievGUID);
        if(Maiev && Maiev->isAlive())
            Maiev->AI()->DoAction(m_nextPhase);
    }
    m_phase = m_nextPhase;
    m_event = EVENT_NULL;
}

CreatureAI* GetAI_boss_illidan_stormrage(Creature *_Creature)
{
    return new boss_illidan_stormrageAI (_Creature);
}

CreatureAI* GetAI_npc_akama_at_illidan(Creature *_Creature)
{
    return new npc_akama_illidanAI(_Creature);
}

CreatureAI* GetAI_boss_maiev(Creature *_Creature)
{
    return new boss_maievAI (_Creature);
}

CreatureAI* GetAI_mob_flame_of_azzinoth(Creature *_Creature)
{
    return new flame_of_azzinothAI (_Creature);
}

CreatureAI* GetAI_cage_trap_trigger(Creature *_Creature)
{
    return new cage_trap_triggerAI (_Creature);
}

CreatureAI* GetAI_shadow_demon(Creature *_Creature)
{
    return new shadow_demonAI (_Creature);
}

CreatureAI* GetAI_blade_of_azzinoth(Creature *_Creature)
{
    return new blade_of_azzinothAI (_Creature);
}

CreatureAI* GetAI_parasitic_shadowfiend(Creature *_Creature)
{
    return new mob_parasitic_shadowfiendAI (_Creature);
}

void AddSC_boss_illidan()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_illidan_stormrage";
    newscript->GetAI = &GetAI_boss_illidan_stormrage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_akama_illidan";
    newscript->GetAI = &GetAI_npc_akama_at_illidan;
    newscript->pGossipHello = &GossipHello_npc_akama_at_illidan;
    newscript->pGossipSelect = &GossipSelect_npc_akama_at_illidan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_maiev_shadowsong";
    newscript->GetAI = &GetAI_boss_maiev;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flame_of_azzinoth";
    newscript->GetAI = &GetAI_mob_flame_of_azzinoth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_blade_of_azzinoth";
    newscript->GetAI = &GetAI_blade_of_azzinoth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "gameobject_cage_trap";
    newscript->pGOHello = &GOHello_cage_trap;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_cage_trap_trigger";
    newscript->GetAI = &GetAI_cage_trap_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadow_demon";
    newscript->GetAI = &GetAI_shadow_demon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_parasitic_shadowfiend";
    newscript->GetAI = &GetAI_parasitic_shadowfiend;
    newscript->RegisterSelf();
}

