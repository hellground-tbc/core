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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef TRINITY_PLAYERAI_H
#define TRINITY_PLAYERAI_H

#include "CreatureAI.h"

struct SpellEntry;

struct PlayerAI : public UnitAI
{
    PlayerAI(Player *pPlayer) : UnitAI((Unit *)pPlayer), me(pPlayer) {}

    SpellEntry const *selectHighestRank(uint32 spell_id);
    bool PlayerHasSpell(uint32 spell_id);

    bool UpdateVictim(float = 20.0f);

    protected:
        Player *me;
};

struct WarriorAI: public PlayerAI
{
    WarriorAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct HunterAI: public PlayerAI
{
    HunterAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct PaladinAI: public PlayerAI
{
    PaladinAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct WarlockAI: public PlayerAI
{
    WarlockAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct DruidAI: public PlayerAI
{
    DruidAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct RogueAI: public PlayerAI
{
    RogueAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct ShamanAI: public PlayerAI
{
    ShamanAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct PriestAI: public PlayerAI
{
    PriestAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

struct MageAI: public PlayerAI
{
    MageAI(Player *plr): PlayerAI(plr) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

#endif
