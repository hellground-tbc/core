/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "Creature.h"
#include "World.h"
#include "SpellMgr.h"

void CreatureAI::OnCharmed(bool apply)
{
    if (!(m_creature->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_CHARM_AI))
    {
        me->NeedChangeAI = true;
        me->IsAIEnabled = false;
    }
}

AISpellInfoType * UnitAI::AISpellInfo;
TRINITY_DLL_SPEC AISpellInfoType * GetAISpellInfo(uint32 i) { return &CreatureAI::AISpellInfo[i]; }

void CreatureAI::DoZoneInCombat(float max_dist)
{
     Unit *creature = me;

    if (!me->CanHaveThreatList() || me->IsInEvadeMode())
        return;

    Map *pMap = me->GetMap();
    if (!pMap->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
    {
        sLog.outError("DoZoneInCombat call for map that isn't an instance (creature entry = %d)", creature->GetTypeId() == TYPEID_UNIT ? ((Creature*)creature)->GetEntry() : 0);
        return;
    }

    Map::PlayerList const &plList = pMap->GetPlayers();
    if (plList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator i = plList.begin(); i != plList.end(); ++i)
    {
        if (Player* pPlayer = i->getSource())
        {
            if (pPlayer->isGameMaster() || pPlayer->IsFriendlyTo(me))
                continue;

            if (pPlayer->isAlive() && me->IsWithinDistInMap(pPlayer, max_dist))
            {
                me->SetInCombatWith(pPlayer);
                pPlayer->SetInCombatWith(me);
                me->AddThreat(pPlayer, 0.0f);
            }
        }
    }
}

// scripts does not take care about MoveInLineOfSight loops
// MoveInLineOfSight can be called inside another MoveInLineOfSight and cause stack overflow
void CreatureAI::MoveInLineOfSight_Safe(Unit *who)
{
    if (m_MoveInLineOfSight_locked == true)
        return;
    m_MoveInLineOfSight_locked = true;
    MoveInLineOfSight(who);
    m_MoveInLineOfSight_locked = false;
}

void CreatureAI::MoveInLineOfSight(Unit *who)
{
    if (me->getVictim())
        return;

    if (me->canStartAttack(who))
    {
        AttackStart(who);
        who->CombatStart(me);
    }
    //else if (who->getVictim() && me->IsFriendlyTo(who)
    //    && me->IsWithinDistInMap(who, sWorld.getConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS))
    //    && me->canAttack(who->getVictim()))
    //    AttackStart(who->getVictim());
}

void CreatureAI::SelectNearestTarget(Unit *who)
{
    if (me->getVictim() && me->GetDistanceOrder(who, me->getVictim()) && me->canAttack(who))
    {
        me->getThreatManager().modifyThreatPercent(me->getVictim(), -100);
        me->AddThreat(who, 1000000.0f);
    }
}

void CreatureAI::EnterEvadeMode()
{
    if (!_EnterEvadeMode())
        return;

    sLog.outDebug("Creature %u enters evade mode.", me->GetEntry());

    if (Unit *owner = me->GetCharmerOrOwner())
    {
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, m_creature->GetFollowAngle());
    }
    else
         me->GetMotionMaster()->MoveTargetedHome();
    Reset();
}

/*void CreatureAI::AttackedBy(Unit* attacker)
 {
    if (!m_creature->getVictim())
        AttackStart(attacker);
}*/
