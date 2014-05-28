/* 
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 * 
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

#include "precompiled.h"
#include "def_serpent_shrine.h"
#include "../../../special/simple_ai.h"
#include "Spell.h"

#define EMOTE_SPOUT "takes a deep breath."

enum lurkerAdds
{
    MOB_COILFANG_GUARDIAN = 21873,
    MOB_COILFANG_AMBUSHER = 21865
};

float addPos[9][4] =
{
    { MOB_COILFANG_AMBUSHER, 2.8553810, -459.823914, -19.182686},   //MOVE_AMBUSHER_1 X, Y, Z
    { MOB_COILFANG_AMBUSHER, 12.400000, -466.042267, -19.182686},   //MOVE_AMBUSHER_2 X, Y, Z
    { MOB_COILFANG_AMBUSHER, 51.366653, -460.836060, -19.182686},   //MOVE_AMBUSHER_3 X, Y, Z
    { MOB_COILFANG_AMBUSHER, 62.597980, -457.433044, -19.182686},   //MOVE_AMBUSHER_4 X, Y, Z
    { MOB_COILFANG_AMBUSHER, 77.607452, -384.302765, -19.182686},   //MOVE_AMBUSHER_5 X, Y, Z
    { MOB_COILFANG_AMBUSHER, 63.897900, -378.984924, -19.182686},   //MOVE_AMBUSHER_6 X, Y, Z
    { MOB_COILFANG_GUARDIAN, 34.447250, -387.333618, -19.182686},   //MOVE_GUARDIAN_1 X, Y, Z
    { MOB_COILFANG_GUARDIAN, 14.388216, -423.468018, -19.625271},   //MOVE_GUARDIAN_2 X, Y, Z
    { MOB_COILFANG_GUARDIAN, 42.471519, -445.115295, -19.769423}    //MOVE_GUARDIAN_3 X, Y, Z
};

enum lurkerSpells
{
    SPELL_SPOUT_VISUAL = 37429,
    SPELL_SPOUT_BREATH = 37431,
    SPELL_SPOUT_EFFECT = 37433,
    SPELL_GEYSER       = 37478,
    SPELL_WHIRL        = 37660,
    SPELL_WATERBOLT    = 37138,
    SPELL_SUBMERGE     = 37550,
};

#define SPOUT_WIDTH 1.2f

enum LurkerEvents
{
    LURKER_EVENT_SPOUT_EMOTE    = 1,
    LURKER_EVENT_SPOUT          = 2,
    LURKER_EVENT_WHIRL          = 3,
    LURKER_EVENT_GEYSER         = 4,
    LURKER_EVENT_SUBMERGE       = 5,
    LURKER_EVENT_REEMERGE       = 6
};

struct boss_the_lurker_belowAI : public BossAI
{
    boss_the_lurker_belowAI(Creature *c) : BossAI(c, DATA_THELURKERBELOW) { }

    bool m_rotating;
    bool m_submerged;
    bool m_emoteDone;

    uint32 m_checkTimer;

    void Reset()
    {
        ClearCastQueue();
        events.Reset();

        instance->SetData(DATA_THELURKERBELOWEVENT, NOT_STARTED);
        instance->SetData(DATA_STRANGE_POOL, NOT_STARTED);

        // Do not fall to the ground ;]
        me->AddUnitMovementFlag(MOVEFLAG_SWIMMING);
        me->SetLevitate(true);

        // Set reactstate to: Defensive
        me->SetReactState(REACT_DEFENSIVE);
        me->SetVisibility(VISIBILITY_OFF);

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

        events.ScheduleEvent(LURKER_EVENT_SPOUT_EMOTE, 42000);
        events.ScheduleEvent(LURKER_EVENT_WHIRL, 18000);
        events.ScheduleEvent(LURKER_EVENT_GEYSER, 0);
        events.ScheduleEvent(LURKER_EVENT_SUBMERGE, 90000);

        // Timers
        m_checkTimer = 3000;

        // Bools
        m_rotating = false;
        m_submerged = false;

        summons.DespawnAll();
        me->CastSpell(me, SPELL_SUBMERGE, false);
    }

    void EnterCombat(Unit *who)
    {
        instance->SetData(DATA_THELURKERBELOWEVENT, IN_PROGRESS);
        me->SetReactState(REACT_AGGRESSIVE);

        AttackStart(who);
    }

    void AttackStart(Unit *pWho)
    {
        if (!pWho || !me->HasReactState(REACT_AGGRESSIVE))
            return;

        if (m_creature->Attack(pWho, true))
            DoStartNoMovement(pWho);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (me->GetVisibility() == VISIBILITY_OFF || me->isInCombat())
            return;

        AttackStart(pWho);
    }

    void JustDied(Unit* Killer)
    {
        instance->SetData(DATA_THELURKERBELOWEVENT, DONE);
    }

    void SummonAdds()
    {
        for (uint8 i = 0; i < 9; i++)
            Creature *pSummon = me->SummonCreature(addPos[i][0], addPos[i][1], addPos[i][2], addPos[i][3], 0, TEMPSUMMON_DEAD_DESPAWN, 2000);
    }

    void MovementInform(uint32 type, uint32 data)
    {
        // data: 0 = FINALIZE
        // data: 1 = UPDATE
        if (type == ROTATE_MOTION_TYPE)
        {
            if (data == 0) //Rotate movegen finalize
            {
                me->RemoveAurasDueToSpell(SPELL_SPOUT_VISUAL);
                me->SetReactState(REACT_AGGRESSIVE);
                m_rotating = false;
            }
        }
    }

    void DoMeleeAttackIfReady()
    {
        if (me->hasUnitState(UNIT_STAT_CASTING) || m_submerged || m_rotating)
            return;

        //Make sure our attack is ready and we aren't currently casting before checking distance
        if (me->isAttackReady())
        {
            //If we are within range melee the target
            if (me->IsWithinMeleeRange(me->getVictim()))
            {
                me->AttackerStateUpdate(me->getVictim());
                me->resetAttackTimer();
            }
            else
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 0, 5.0f, true))
                {
                    me->AttackerStateUpdate(pTarget);
                    me->resetAttackTimer();
                }
                else
                {
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true, 0, 5.0f))
                        AddSpellToCast(pTarget, SPELL_WATERBOLT);
                    else
                        EnterEvadeMode();
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (instance->GetData(DATA_STRANGE_POOL) != IN_PROGRESS)
            return;

        if (me->GetVisibility() == VISIBILITY_OFF)
        {
            me->SetVisibility(VISIBILITY_ON);
            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            DoZoneInCombat();
        }

        if (!UpdateVictim())
            return;

        DoSpecialThings(diff, DO_PULSE_COMBAT);

        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case LURKER_EVENT_SPOUT_EMOTE:
                {
                    me->MonsterTextEmote(EMOTE_SPOUT, 0, true);
                    ForceSpellCast(me, SPELL_SPOUT_BREATH);
                    events.ScheduleEvent(LURKER_EVENT_SPOUT, 3000);
                    break;
                }
                case LURKER_EVENT_SPOUT:
                {
                    me->SetReactState(REACT_PASSIVE);

                    me->SetSelection(0);
                   // me->GetMotionMaster()->MoveRotate(20000, RAND(ROTATE_DIRECTION_LEFT, ROTATE_DIRECTION_RIGHT));

                    ForceSpellCast(me, SPELL_SPOUT_VISUAL, INTERRUPT_AND_CAST_INSTANTLY);

                    m_rotating = true;

                    events.DelayEvents(20000, 0);
                    events.ScheduleEvent(LURKER_EVENT_SPOUT_EMOTE, 45000);
                    events.RescheduleEvent(LURKER_EVENT_WHIRL, 21000);
                    break;
                }
                case LURKER_EVENT_WHIRL:
                {
                    AddSpellToCast(me, SPELL_WHIRL);
                    events.ScheduleEvent(LURKER_EVENT_WHIRL, 17500);
                    break;
                }
                case LURKER_EVENT_GEYSER:
                {
                    AddSpellToCast(SPELL_GEYSER, CAST_RANDOM);
                    events.ScheduleEvent(LURKER_EVENT_GEYSER, urand(10000, 30000));
                    break;
                }
                case LURKER_EVENT_SUBMERGE:
                {
                    ForceSpellCast(me, SPELL_SUBMERGE, INTERRUPT_AND_CAST_INSTANTLY);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

                    SummonAdds();
                    m_submerged = true;

                    events.CancelEvent(LURKER_EVENT_SPOUT_EMOTE);
                    events.CancelEvent(LURKER_EVENT_SPOUT);
                    events.CancelEvent(LURKER_EVENT_WHIRL);
                    events.CancelEvent(LURKER_EVENT_GEYSER);
                    events.ScheduleEvent(LURKER_EVENT_REEMERGE, 60000);
                    break;
                }
                case LURKER_EVENT_REEMERGE:
                {
                    me->RemoveAurasDueToSpell(SPELL_SUBMERGE);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    m_submerged = false;

                    events.ScheduleEvent(LURKER_EVENT_SPOUT_EMOTE, 0);
                    events.ScheduleEvent(LURKER_EVENT_WHIRL, 2000);
                    events.ScheduleEvent(LURKER_EVENT_GEYSER, urand(5000, 15000));
                    events.ScheduleEvent(LURKER_EVENT_SUBMERGE, 90000);
                    break;
                }
            }
        }

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum guardianSpells
{
    SPELL_HARMSTRING   = 9080,
    SPELL_ARCING_SMASH = 28168
};

struct mob_coilfang_guardianAI : public ScriptedAI
{
    mob_coilfang_guardianAI(Creature *c) : ScriptedAI(c) { }

    uint32 m_harmstringTimer;
    uint32 m_arcingTimer;

    void Reset()
    {
        ClearCastQueue();

        m_harmstringTimer = urand(5000, 15000);
        m_arcingTimer = urand(15000, 20000);
    }

    void JustRespawned()
    {
        DoZoneInCombat(200.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_harmstringTimer < diff)
        {
            AddSpellToCast(SPELL_HARMSTRING);
            m_harmstringTimer = 10500;
        }
        else
            m_harmstringTimer -= diff;

        if (m_arcingTimer < diff)
        {
            AddSpellToCast(SPELL_ARCING_SMASH);
            m_arcingTimer = urand(10000, 20000);
        }
        else
            m_arcingTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum ambusherSpells
{
    SPELL_SPREAD_SHOT = 37790,
    SPELL_NORMAL_SHOT = 37770
};

struct mob_coilfang_ambusherAI : public Scripted_NoMovementAI
{
    mob_coilfang_ambusherAI(Creature *c) : Scripted_NoMovementAI(c) { }

    uint32 m_spreadTimer;
    uint32 m_shootTimer;

    void Reset()
    {
        ClearCastQueue();

        m_spreadTimer = urand(10000, 20000);
        m_shootTimer = 2000;
    }

    void JustRespawned()
    {
        DoZoneInCombat(200.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_spreadTimer < diff)
        {
            AddSpellToCast(SPELL_SPREAD_SHOT, CAST_RANDOM);
            m_spreadTimer = urand(10000, 20000);
        }
        else
            m_spreadTimer -= diff;

        if (m_shootTimer < diff)
        {
            AddSpellToCast(SPELL_NORMAL_SHOT, CAST_RANDOM);
            m_shootTimer = 2000;
        }
        else
            m_shootTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_coilfang_guardian(Creature* pCreature)
{
    return new mob_coilfang_guardianAI (pCreature);
}

CreatureAI* GetAI_mob_coilfang_ambusher(Creature* pCreature)
{
    return new mob_coilfang_ambusherAI (pCreature);
}

CreatureAI* GetAI_boss_the_lurker_below(Creature* pCreature)
{
    return new boss_the_lurker_belowAI (pCreature);
}

void AddSC_boss_the_lurker_below()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_the_lurker_below";
    newscript->GetAI = &GetAI_boss_the_lurker_below;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilfang_guardian";
    newscript->GetAI = &GetAI_mob_coilfang_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilfang_ambusher";
    newscript->GetAI = &GetAI_mob_coilfang_ambusher;
    newscript->RegisterSelf();
}
