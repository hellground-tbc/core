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
#include "Player.h"

struct SpellEntry;

struct PlayerAI : public UnitAI
{
    PlayerAI(Player *pPlayer) : UnitAI((Unit *)pPlayer), me(pPlayer) {}

    SpellEntry const *selectHighestRank(uint32 spell_id);

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

#define RAINOFFIRE_R1   5740
#define SHADOWFURY_R1   30283
#define SHADOWBOLT_R1   686
#define INCINERATE_R1   29722
#define CORRUPTION_R1   172
#define UNSTABLEAFF_R1  30108
#define IMMOLATE_R1     348
#define HOWLOFTERROR_R1 5484


struct WarlockAI: public PlayerAI
{
    WarlockAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset()
    {
        if(!(AOESpell = selectHighestRank(SHADOWFURY_R1)))
            AOESpell = selectHighestRank(RAINOFFIRE_R1);

        bool fire = me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_FIRE) > me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_SHADOW);

        DOTSpell = NULL;
        if(!(DOTSpell = selectHighestRank(UNSTABLEAFF_R1)))
        {
            if(fire)
                DOTSpell = selectHighestRank(IMMOLATE_R1);
            if(!DOTSpell)
                DOTSpell = selectHighestRank(CORRUPTION_R1);
        }

        FearSpell = selectHighestRank(HOWLOFTERROR_R1);

        NormalSpell = NULL;
        if(fire)
            NormalSpell = selectHighestRank(INCINERATE_R1);
        if(!NormalSpell)
            NormalSpell = selectHighestRank(SHADOWBOLT_R1);

        AOE_Timer = 5000;
        Fear_Timer = 3000;
        DOT_Timer = 1500;
        NormalSpell_Timer = 3500;

    }

    void UpdateAI(const uint32 diff);

    uint32 AOE_Timer;
    SpellEntry const *AOESpell;

    uint32 DOT_Timer;
    SpellEntry const *DOTSpell;

    uint32 Fear_Timer;
    SpellEntry const *FearSpell;

    uint32 NormalSpell_Timer;
    SpellEntry const *NormalSpell;
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

#define BL                  2825
#define HERO                32182
#define LIGHTNING_SHIELD_R1 324
#define WATER_SHIELD_R1     24398
#define EARTH_SHIELD_R1     974
#define CHAIN_HEAL_R1       1064
#define CHAIN_LIGHTNING_R1  421

struct ShamanAI: public PlayerAI
{
    ShamanAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset()
    {
           if(Totem = selectHighestRank(30706))
                ShieldSpell = selectHighestRank(WATER_SHIELD_R1);
            else if (!(Totem = selectHighestRank(30706)) && !(ShieldSpell = selectHighestRank(EARTH_SHIELD_R1)))
                ShieldSpell = selectHighestRank(LIGHTNING_SHIELD_R1);
			else 
			{
				heal=true;
				ShieldSpell = selectHighestRank(EARTH_SHIELD_R1);
			}

        HealSpell = selectHighestRank(CHAIN_HEAL_R1);
        LightningSpell = selectHighestRank(CHAIN_LIGHTNING_R1);

        BLSpell = NULL;
        if(!(BLSpell=selectHighestRank(BL)))
            BLSpell=selectHighestRank(HERO);

        Shield_Timer = 10000;
        Heal_Timer = 15000;
        BL_Timer = 500;
        Lightning_Timer = 17000;
    }

	bool heal;

	uint32 Totem_Timer;
    SpellEntry const *Totem;

    uint32 Shield_Timer;
    SpellEntry const *ShieldSpell;

    uint32 Heal_Timer;
    SpellEntry const *HealSpell;

    uint32 BL_Timer;
    SpellEntry const *BLSpell;

    uint32 Lightning_Timer;
    SpellEntry const *LightningSpell;
	

	void UpdateAI(const uint32 diff);
};

struct PriestAI: public PlayerAI
{
    PriestAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset() {}

    void UpdateAI(const uint32 diff);
};

#define BLIZZARD_R1     10
#define CONEOFCOLD_R1   120
#define DRAGONBREATH_R1 31661
#define BLASTWAVE_R1    11113
#define ARCANEEXPLO_R1  1449
#define FIREBALL_R1     133
#define FROSTBOLT_R1    116
#define EVOCATION       12051
#define FLAMESTRIKE_R1  2120

struct MageAI: public PlayerAI
{
    MageAI(Player *plr): PlayerAI(plr) {}

    void Reset()
    {
        bool FireMage = me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_FIRE) > me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_FROST);
        bool Special = false;

        if(FireMage)
            MassiveAOESpell = selectHighestRank(FLAMESTRIKE_R1);
        else
            MassiveAOESpell = selectHighestRank(BLIZZARD_R1);

        if(!(ConeSpell = selectHighestRank(DRAGONBREATH_R1)))
            ConeSpell = selectHighestRank(CONEOFCOLD_R1);

        if(!(AOESpell = selectHighestRank(BLASTWAVE_R1)))
            AOESpell = selectHighestRank(ARCANEEXPLO_R1);

        ConeSpell_Timer = 5000;
        MassiveAOE_Timer = 10000;
        AOESpell_Timer = 2000;

        if(FireMage)
            NormalSpell = selectHighestRank(FIREBALL_R1);
        else
            NormalSpell = selectHighestRank(FROSTBOLT_R1);

        SpecialSpell = selectHighestRank(EVOCATION);

        NormalSpell_Timer = 3200;
    }

    uint32 MassiveAOE_Timer;
    SpellEntry const *MassiveAOESpell;

    uint32 ConeSpell_Timer;
    SpellEntry const *ConeSpell;

    uint32 AOESpell_Timer;
    SpellEntry const *AOESpell;

    uint32 NormalSpell_Timer;
    SpellEntry const *NormalSpell;
    
    bool Special;
    SpellEntry const *SpecialSpell;

    void UpdateAI(const uint32 diff);
};

#endif
