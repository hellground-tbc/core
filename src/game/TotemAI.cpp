/*
 * Copyright (C) 2005-2008 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2008 TrinityCore <http://www.trinitycore.org/>
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

#include "TotemAI.h"
#include "Totem.h"
#include "Creature.h"
#include "Player.h"
#include "DBCStores.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

int
TotemAI::Permissible(const Creature *creature)
{
    if (creature->isTotem())
        return PERMIT_BASE_PROACTIVE;

    return PERMIT_BASE_NO;
}

TotemAI::TotemAI(Creature *c) : CreatureAI(c), i_totem(static_cast<Totem&>(*c)), i_victimGuid(0)
{
}

void TotemAI::MoveInLineOfSight(Unit *)
{
}

void TotemAI::EnterEvadeMode()
{
    i_totem.addUnitState(UNIT_STAT_STUNNED);
    i_totem.CombatStop();
}

void TotemAI::UpdateAI(const uint32 /*diff*/)
{
    if (i_totem.GetTotemType() != TOTEM_ACTIVE)
        return;

    i_totem.SetSelection(0);

    if (!i_totem.isAlive() || i_totem.IsNonMeleeSpellCast(false))
        return;

    // Search spell
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(i_totem.GetSpell());
    if (!spellInfo)
        return;

    // Get spell rangy
    SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(spellInfo->rangeIndex);
    float max_range = SpellMgr::GetSpellMaxRange(srange);

    // SPELLMOD_RANGE not applied in this place just because not existence range mods for attacking totems

    // pointer to appropriate target if found any
    Unit* victim = i_victimGuid ? i_totem.GetMap()->GetUnit(i_victimGuid) : NULL;

    if (!max_range)
        victim = &i_totem;

    // Search victim if no, not attackable, or out of range, or friendly (possible in case duel end)
    if (!victim || (!SpellMgr::SpellIgnoreLOS(spellInfo, 0) && !i_totem.IsWithinLOSInMap(victim)) ||
       !victim->isTargetableForAttack() || !i_totem.IsWithinDistInMap(victim, max_range) ||  
       (i_totem.IsFriendlyTo(victim) && victim != &i_totem) || !victim->isVisibleForOrDetect(&i_totem, &i_totem, false))
    {
        victim = NULL;

        Hellground::NearestAttackableUnitInObjectRangeCheck u_check(&i_totem, &i_totem, max_range);
        Hellground::UnitLastSearcher<Hellground::NearestAttackableUnitInObjectRangeCheck> checker(victim, u_check);

        Cell::VisitAllObjects(m_creature, checker, max_range);
    }

    // If have target
    if (victim)
    {
    //this should prevent target-type totems from attacking from unattackable zones and attacking while being unattackable
        if ((i_totem.isInSanctuary() || victim->isInSanctuary()) && victim->GetCharmerOrOwnerPlayerOrPlayerItself())
            return;
        // remember
        i_victimGuid = victim->GetGUID();

        // attack
        i_totem.CastSpell(victim, i_totem.GetSpell(), false, NULL, NULL, i_totem.GetOwner()->GetGUID());
    }
    else
        i_victimGuid = 0;

    //i_totem.SetFacingToObject(&i_totem);
}

bool TotemAI::IsVisible(Unit *) const
{
    return false;
}

void TotemAI::AttackStart(Unit *)
{
    // Sentry totem sends ping on attack
    if (i_totem.GetEntry() == SENTRY_TOTEM_ENTRY && i_totem.GetOwner()->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(MSG_MINIMAP_PING, (8+4+4));
        data << i_totem.GetGUID();
        data << i_totem.GetPositionX();
        data << i_totem.GetPositionY();
        ((Player*)i_totem.GetOwner())->SendPacketToSelf(&data);
    }
}

