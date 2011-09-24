/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
#include "../../../creature/simple_ai.h"
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
    SPELL_WHIRL        = 37363,
    SPELL_WATERBOLT    = 37138,
    SPELL_SUBMERGE     = 37550,
};

#define SPOUT_WIDTH 1.2f

struct TRINITY_DLL_DECL boss_the_lurker_belowAI : public Scripted_NoMovementAI
{
    boss_the_lurker_belowAI(Creature *c) : Scripted_NoMovementAI(c), m_summons(m_creature)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    SummonList m_summons;

    bool m_rotating;
    bool m_submerged;
    bool m_emoteDone;

    uint32 m_spoutTimer;
    uint32 m_phaseTimer;
    uint32 m_whirlTimer;
    uint32 m_geyserTimer;
    uint32 m_checkTimer;

    std::map<uint64, uint8> m_immunemap;

    void Reset()
    {
        ClearCastQueue();

        if(pInstance)
        {
            pInstance->SetData(DATA_THELURKERBELOWEVENT, NOT_STARTED);
            pInstance->SetData(DATA_STRANGE_POOL, NOT_STARTED);
        }

        // Do not fall to the ground ;]
        me->AddUnitMovementFlag(MOVEFLAG_SWIMMING);
        me->SetLevitate(true);

        // Set reactstate to: Aggresive
        me->SetReactState(REACT_AGGRESSIVE);
        me->SetVisibility(VISIBILITY_OFF);

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

        // Timers
        m_spoutTimer = 42000;
        m_phaseTimer = 90000;
        m_whirlTimer = 18000;
        m_geyserTimer = 0;
        m_checkTimer = 3000;

        // Bools
        m_rotating = false;
        m_submerged = false;
        m_emoteDone = false;

        m_summons.DespawnAll();
        ForceSpellCast(me, SPELL_SUBMERGE, INTERRUPT_AND_CAST_INSTANTLY);
    }

    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(DATA_THELURKERBELOWEVENT, IN_PROGRESS);

        AttackStart(who);
    }

    void AttackStart(Unit *pWho)
    {
        if (me->HasReactState(REACT_PASSIVE))
            return;

        Scripted_NoMovementAI::AttackStart(pWho);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (me->GetVisibility() == VISIBILITY_OFF || me->isInCombat())
            return;

        AttackStart(pWho);
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance)
            pInstance->SetData(DATA_THELURKERBELOWEVENT, DONE);

        m_summons.DespawnAll();
    }

    void SummonAdds()
    {
        for (uint8 i = 0; i < 9; i++)
        {
            Creature *pSummon = me->SummonCreature(addPos[i][0], addPos[i][1], addPos[i][2], addPos[i][3], 0, TEMPSUMMON_DEAD_DESPAWN, 2000);
            m_summons.Summon(pSummon);
        }
    }

    void MovementInform(uint32 type, uint32 data)
    {
        // data: 0 = FINALIZE
        // data: 1 = UPDATE
        if (type == ROTATE_MOTION_TYPE)
        {
            if (data == 1) //Rotate movegen update
            {
                me->SetSelection(0);

                WorldLocation wLoc;
                me->GetClosePoint(wLoc.coord_x, wLoc.coord_y, wLoc.coord_z, 0, 95.0f, 0);

                Map *pMap = me->GetMap();
                Map::PlayerList const& players = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                {
                    Player *pPlayer = i->getSource();

                    if (uint8 count = m_immunemap[pPlayer->GetGUID()])
                    {
                        if (count >= 10)
                            m_immunemap[pPlayer->GetGUID()] = 0;
                        else
                        {
                            m_immunemap[pPlayer->GetGUID()]++;
                            continue;
                        }
                    }

                    if (/*pPlayer->IsInWater() ||*/ pPlayer->GetPositionZ() < -19.9645)
                        continue;

                    if (me->GetDistance2d(pPlayer) > 100.0f)
                        continue;

                    if (!me->HasInArc(M_PI, pPlayer))
                        continue;

                    Unit *pTemp = me->SummonCreature(WORLD_TRIGGER, wLoc.coord_x, wLoc.coord_y, wLoc.coord_z, 0, TEMPSUMMON_TIMED_DESPAWN, 1000);
                    if (!pTemp)
                        continue;

                    if (pPlayer->isBetween(me, pTemp, SPOUT_WIDTH))
                    {
                        ForceSpellCast(pPlayer, SPELL_SPOUT_EFFECT, INTERRUPT_AND_CAST_INSTANTLY, true);
                        m_immunemap[pPlayer->GetGUID()] = 1;
                    }
                }
            }
            else
            {
                me->RemoveAurasDueToSpell(SPELL_SPOUT_VISUAL);
                me->SetReactState(REACT_AGGRESSIVE);
                m_rotating = false;
            }
        }
    }

    void DoMeleeAttackIfReady()
    {
        if (me->hasUnitState(UNIT_STAT_CASTING))
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
        if (pInstance->GetData(DATA_STRANGE_POOL) != IN_PROGRESS)
            return;

        if (me->GetVisibility() == VISIBILITY_OFF)
        {
            me->SetVisibility(VISIBILITY_ON);
            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            DoZoneInCombat(75.0f);
            return;
        }

        if (m_phaseTimer < diff)
        {
            if (m_submerged)
            {
                me->RemoveAurasDueToSpell(SPELL_SUBMERGE);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                m_submerged = false;
                m_spoutTimer = 0;
                m_whirlTimer = 2000;
                m_phaseTimer = 90000;
            }
            else
            {
                ForceSpellCast(me, SPELL_SUBMERGE, INTERRUPT_AND_CAST_INSTANTLY);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);

                SummonAdds();
                m_submerged = true;
                m_phaseTimer = 60000;
            }
        }
        else
            m_phaseTimer -= diff;

        if (m_checkTimer < diff)
        {
             if (!me->isInCombat())
                 return;

             DoZoneInCombat(80.0f);
             m_checkTimer = 2500;
        }
        else
            m_checkTimer -= diff;

        if (m_submerged || m_rotating || !UpdateVictim())
            return;

        if (m_spoutTimer < diff)
        {
            if (m_emoteDone)
            {
                me->SetReactState(REACT_PASSIVE);

                Map *pMap = me->GetMap();
                Map::PlayerList const& players = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                {
                    Player *pPlayer = i->getSource();
                    m_immunemap[pPlayer->GetGUID()] = 0;
                }

                me->SetSelection(0);
                me->GetMotionMaster()->MoveRotate(20000, RAND(ROTATE_DIRECTION_LEFT, ROTATE_DIRECTION_RIGHT));

                ForceSpellCast(me, SPELL_SPOUT_VISUAL, INTERRUPT_AND_CAST_INSTANTLY);

                m_rotating = true;
                m_emoteDone = false;
                m_spoutTimer = 25500;
                m_whirlTimer = 1000;
            }
            else
            {
                me->MonsterTextEmote(EMOTE_SPOUT, 0, true);
                ForceSpellCast(me, SPELL_SPOUT_BREATH);

                m_spoutTimer = 3000;
                m_emoteDone = true;
            }
        }
        else
            m_spoutTimer -= diff;

        if (m_whirlTimer < diff)
        {
            AddSpellToCast(me, SPELL_WHIRL);
            m_whirlTimer = 17500;
        }
        else
            m_whirlTimer -= diff;

        if (m_geyserTimer < diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100.0f, true, me->getVictimGUID()))
            {
                AddSpellToCast(pTarget, SPELL_GEYSER);
                m_geyserTimer = urand(10000, 30000);
            }
        }
        else
            m_geyserTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

enum guardianSpells
{
    SPELL_HARMSTRING   = 9080,
    SPELL_ARCING_SMASH = 28168
};

struct TRINITY_DLL_DECL mob_coilfang_guardianAI : public ScriptedAI
{
    mob_coilfang_guardianAI(Creature *c) : ScriptedAI(c)
    {
    }

    uint32 m_harmstringTimer;
    uint32 m_arcingTimer;

    void Reset()
    {
        ClearCastQueue();

        m_harmstringTimer = urand(5000, 15000);
        m_arcingTimer = urand(15000, 20000);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat(80.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_harmstringTimer < diff)
        {
            AddSpellToCast(me->getVictim(), SPELL_HARMSTRING);
            m_harmstringTimer = 10500;
        }
        else
            m_harmstringTimer -= diff;

        if (m_arcingTimer < diff)
        {
            AddSpellToCast(me->getVictim(), SPELL_ARCING_SMASH);
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

struct TRINITY_DLL_DECL mob_coilfang_ambusherAI : public Scripted_NoMovementAI
{
    mob_coilfang_ambusherAI(Creature *c) : Scripted_NoMovementAI(c)
    {
    }

    uint32 m_spreadTimer;
    uint32 m_shootTimer;

    void Reset()
    {
        ClearCastQueue();

        m_spreadTimer = urand(10000, 20000);
        m_shootTimer = 2000;
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat(80.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_spreadTimer < diff)
        {
            if (Unit *pTemp = SelectUnit(SELECT_TARGET_RANDOM, 100.0f))
            {
                AddSpellToCast(pTemp, SPELL_SPREAD_SHOT);
                m_spreadTimer = urand(10000, 20000);
            }
        }
        else
            m_spreadTimer -= diff;

        if (m_shootTimer < diff)
        {
            if (Unit *pTemp = SelectUnit(SELECT_TARGET_RANDOM, 100.0f))
            {
                AddSpellToCast(pTemp, SPELL_NORMAL_SHOT);
                m_shootTimer = 2000;
            }
        }
        else
            m_shootTimer -= diff;

        CastNextSpellIfAnyAndReady();
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
    newscript->Name="boss_the_lurker_below";
    newscript->GetAI = &GetAI_boss_the_lurker_below;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_coilfang_guardian";
    newscript->GetAI = &GetAI_mob_coilfang_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_coilfang_ambusher";
    newscript->GetAI = &GetAI_mob_coilfang_ambusher;
    newscript->RegisterSelf();
}
