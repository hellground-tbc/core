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
SDName: Thaddius encounter
SD%Complete: 0
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028

//Thaddus
#define SAY_GREET               -1533029
#define SAY_AGGRO1              -1533030
#define SAY_AGGRO2              -1533031
#define SAY_AGGRO3              -1533032
#define SAY_SLAY                -1533033
#define SAY_ELECT               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039

#define GO_TESLA_COIL1    181477
#define GO_TESLA_COIL2    181478    //those 2 are not spawned

enum eSpells
{
    // Fuegen
    FEUG_TESLA_PASSIVE            = 28109,
    SPELL_MANA_BURN               = 28135,

    // Stalagg
    STAL_TESLA_PASSIVE            = 28097,
    SPELL_POWER_SURGE             = 28134,

    // shared
    SPELL_WAR_STOMP               = 28125,
    SPELL_MAGNETIC_PULL_F         = 28338,
    SPELL_MAGNETIC_PULL_S         = 28339,

    // Thaddius
    SPELL_SELF_STUN               = 28160,
    SPELL_BALL_LIGHTNING          = 28299,
    SPELL_POLARITY_SHIFT          = 28089, // dummy to script

    SPELL_CHAIN_LIGHTNING         = 28167,
    SPELL_BERSERK                 = 26662
};

enum eEvents
{
    // 1st phase shared
    EVENT_CHECK_COIL     = 1,
    EVENT_WAR_STOMP      = 2,
    EVENT_PULL_TANK      = 3,

    // Fuegen
    EVENT_MANA_BURN      = 4,

    // Stalagg
    EVENT_POWER_SURGE    = 5,

    // Thaddius
    EVENT_POLARITY_SHIFT = 6,
    EVENT_BERSERK        = 7
};

struct boss_thaddiusAI : public BossAI
{
    boss_thaddiusAI(Creature *c) : BossAI(c, DATA_THADDIUS) {}

    void Reset()
    {
        ClearCastQueue();

        events.ScheduleEvent(EVENT_POLARITY_SHIFT, 30000);
        events.ScheduleEvent(EVENT_BERSERK, 300000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_POLARITY_SHIFT:
                {
                    AddSpellToCast(SPELL_POLARITY_SHIFT, CAST_SELF);
                    events.ScheduleEvent(EVENT_POLARITY_SHIFT, 30000);
                    break;
                }
                case EVENT_BERSERK:
                {
                    AddSpellToCast(SPELL_BERSERK, CAST_SELF);
                    break;
                }
                default: break;
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct boss_stalaggAI : public BossAI
{
    boss_stalaggAI(Creature *c) : BossAI(c, DATA_STALAGG) {}

    void Reset()
    {
        ClearCastQueue();

        // proper timers
        events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
        events.ScheduleEvent(EVENT_PULL_TANK, 20500);

        // guessed timers, to FIX
        events.ScheduleEvent(EVENT_POWER_SURGE, 10000);
        events.ScheduleEvent(EVENT_WAR_STOMP, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHECK_COIL:
                {
                    events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
                    break;
                }
                case EVENT_POWER_SURGE:
                {
                    AddSpellToCast(SPELL_POWER_SURGE, CAST_SELF);
                    events.ScheduleEvent(EVENT_POWER_SURGE, 20000); // guessed timer
                    break;
                }
                case EVENT_WAR_STOMP:
                {
                    AddSpellToCast(SPELL_WAR_STOMP, CAST_SELF);
                    events.ScheduleEvent(EVENT_WAR_STOMP, 15000); // guessed timer
                    break;
                }
                case EVENT_PULL_TANK:
                {
                    AddSpellToCast(SPELL_MAGNETIC_PULL_S, CAST_NULL);
                    events.ScheduleEvent(EVENT_PULL_TANK, 20500);
                    break;
                }
                default: break;
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct boss_feugenAI : public BossAI
{
    boss_feugenAI(Creature *c): BossAI(c, DATA_FEUGEN) {}

    void Reset()
    {
        ClearCastQueue();

        // proper timers
        events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
        events.ScheduleEvent(EVENT_PULL_TANK, 20500);

        // guessed timers, to FIX
        events.ScheduleEvent(EVENT_MANA_BURN, 10000);
        events.ScheduleEvent(EVENT_WAR_STOMP, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHECK_COIL:
                {
                    events.ScheduleEvent(EVENT_CHECK_COIL, 2000);
                    break;
                }
                case EVENT_MANA_BURN:
                {
                    AddSpellToCast(SPELL_MANA_BURN, CAST_SELF);
                    events.ScheduleEvent(EVENT_MANA_BURN, 10000); // guessed timer
                    break;
                }
                case EVENT_WAR_STOMP:
                {
                    AddSpellToCast(SPELL_WAR_STOMP, CAST_SELF);
                    events.ScheduleEvent(EVENT_WAR_STOMP, 15000); // guessed timer
                    break;
                }
                case EVENT_PULL_TANK:
                {
                    AddSpellToCast(SPELL_MAGNETIC_PULL_F, CAST_NULL);
                    events.ScheduleEvent(EVENT_PULL_TANK, 20500);
                    break;
                }
                default: break;
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

/*
28089 just sample, move it to scripteffec or effectdummy later ;]

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        // those spells have 1 min duration so they need be removed to prevent stack of both types
        unitTarget->RemoveAurasDueToSpell(28059);
        unitTarget->RemoveAurasDueToSpell(28084);

        // Apply positive or negative charge
        m_caster->CastSpell(unitTarget, RAND(28059, 28084), true);
    }
*/
