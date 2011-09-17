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

#include "UnitAI.h"
#include "Player.h"

struct SpellEntry;

struct PlayerAI : public UnitAI
{
    PlayerAI(Player *pPlayer) : UnitAI((Unit *)pPlayer), me(pPlayer) {}

    SpellEntry const *selectHighestRank(uint32 spell_id);

    bool UpdateVictim(float = 20.0f);
    void OnCharmed(bool){}

    protected:
        Player *me;
};

#define THUNDERCLAP_R1        6343
#define BLOODRAGE            2687
#define DEMORALIZING_R1        1160

struct WarriorAI: public PlayerAI
{
    WarriorAI(Player *pPlayer): PlayerAI(pPlayer) {}

void Reset()
    {
        TCSpell = selectHighestRank(THUNDERCLAP_R1);
        BloodrageSpell = selectHighestRank(BLOODRAGE);
        DemoSpell = selectHighestRank(DEMORALIZING_R1);

        TC_Timer = 3000+urand(0, 10000);
        Bloodrage_Timer = 3000+urand(0, 10000);
        Demo_Timer = 3000+urand(0, 10000);
    }

    void UpdateAI(const uint32 diff);

    uint32 TC_Timer;
    SpellEntry const *TCSpell;

    uint32 Bloodrage_Timer;
    SpellEntry const *BloodrageSpell;

    uint32 Demo_Timer;
    SpellEntry const *DemoSpell;
};

#define STEADY_R1        34120
#define ARCANE_R1        3044
#define MULTI_R1        2643
#define VOLLEY_R1        1510
#define RAPIDFIRE        3045
#define BESTIAL            19574
#define AUTO            75

struct HunterAI: public PlayerAI
{
    HunterAI(Player *pPlayer): PlayerAI(pPlayer) {}

void Reset()
    {
        SteadySpell = selectHighestRank(STEADY_R1);
        ArcaneSpell = selectHighestRank(ARCANE_R1);
        MultiSpell = selectHighestRank(MULTI_R1);
        VolleySpell = selectHighestRank(VOLLEY_R1);
        RapidSpell = selectHighestRank(RAPIDFIRE);
        AutoSpell = selectHighestRank(AUTO);
        if (!(BestialSpell = selectHighestRank(BESTIAL)))
            bestial = false;

        Steady_Timer = 1000+urand(0, 4000);
        Arcane_Timer = 1000+urand(0, 6000);
        Multi_Timer = 1000+urand(0, 7000);
        Volley_Timer = 1000+urand(0, 10000);
        Rapid_Timer = urand(0, 20000);
        Bestial_Timer = urand(0, 20000);
        Auto_Timer=500;
    }

    void UpdateAI(const uint32 diff);
    bool bestial;

    uint32 Steady_Timer;
    SpellEntry const *SteadySpell;

    uint32 Arcane_Timer;
    SpellEntry const *ArcaneSpell;

    uint32 Multi_Timer;
    SpellEntry const *MultiSpell;

    uint32 Volley_Timer;
    SpellEntry const *VolleySpell;

    uint32 Rapid_Timer;
    SpellEntry const *RapidSpell;

    uint32 Bestial_Timer;
    SpellEntry const *BestialSpell;

    uint32 Auto_Timer;
    SpellEntry const *AutoSpell;
};

#define AVENGING            31884
#define CRUSADER_R1         35395
#define CONSECRATION_R1     26573
#define JUDGEMENT           20271
#define HOLY_SHOCK_R1       20473
#define FLASH_LIGHT_R1      19750

struct PaladinAI: public PlayerAI
{
    PaladinAI(Player *pPlayer): PlayerAI(pPlayer) {}

void Reset()
    {
        AvengingSpell = selectHighestRank(AVENGING);
        if (!(CrusaderSpell = selectHighestRank(CRUSADER_R1)))
            crusader=false;
        ConsecrationSpell = selectHighestRank(CONSECRATION_R1);
        JudgementSpell = selectHighestRank(JUDGEMENT);
        if (!(ShockSpell = selectHighestRank(HOLY_SHOCK_R1)))
            shock=false;
        FlashSpell = selectHighestRank(FLASH_LIGHT_R1);

        Avenging_Timer = urand (0, 35000);
        Crusader_Timer = 1000+urand(0, 6000);
        Consecration_Timer = 1000+urand(0, 8000);
        Judgement_Timer = 1000+urand(0, 4000);
        Shock_Timer = 1000+urand(0, 12000);
        Flash_Timer = 1000+urand(0, 6000);
    }

    void UpdateAI(const uint32 diff);
    bool crusader;
    bool shock;

    uint32 Avenging_Timer;
    SpellEntry const *AvengingSpell;

    uint32 Crusader_Timer;
    SpellEntry const *CrusaderSpell;

    uint32 Consecration_Timer;
    SpellEntry const *ConsecrationSpell;

    uint32 Judgement_Timer;
    SpellEntry const *JudgementSpell;

    uint32 Shock_Timer;
    SpellEntry const *ShockSpell;

    uint32 Flash_Timer;
    SpellEntry const *FlashSpell;
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
        if (!(AOESpell = selectHighestRank(SHADOWFURY_R1)))
            AOESpell = selectHighestRank(RAINOFFIRE_R1);

        bool fire = me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_FIRE) > me->SpellBaseDamageBonus(SPELL_SCHOOL_MASK_SHADOW);

        DOTSpell = NULL;
        if (!(DOTSpell = selectHighestRank(UNSTABLEAFF_R1)))
        {
            if (fire)
                DOTSpell = selectHighestRank(IMMOLATE_R1);
            if (!DOTSpell)
                DOTSpell = selectHighestRank(CORRUPTION_R1);
        }

        FearSpell = selectHighestRank(HOWLOFTERROR_R1);

        NormalSpell = NULL;
        if (fire)
            NormalSpell = selectHighestRank(INCINERATE_R1);
        if (!NormalSpell)
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

#define DIREBEAR            9635
#define CAT                    3025
#define MOONKIN                24905
#define TREE                33891
#define MANGLE_C_R1            33876
#define MANGLE_B_R1            33878
#define DEMOROAR_R1            99
#define REJUVENATION_R1        774
#define LIFEBLOOM            33763
#define REGROWTH_R1            8936
#define MOONFIRE_R1            8921
#define WRATH_R1            5176
#define STARFIRE_R1            2912
#define HURRICANE_R1        16914


struct DruidAI: public PlayerAI
{
    DruidAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset()
    {
        if (!(MangleBSpell = selectHighestRank(MANGLE_B_R1)))
                    feral=false;

        DemoSpell = selectHighestRank(DEMOROAR_R1);
        MangleBSpell = selectHighestRank(MANGLE_B_R1);
        MangleCSpell = selectHighestRank(MANGLE_C_R1);
        Heal1Spell = selectHighestRank(REJUVENATION_R1);
        Heal2Spell = selectHighestRank(LIFEBLOOM);
        Heal3Spell = selectHighestRank(REGROWTH_R1);
        Dmg1Spell = selectHighestRank(WRATH_R1);
        Dmg2Spell = selectHighestRank(STARFIRE_R1);
        Dmg3Spell = selectHighestRank(MOONFIRE_R1);
        HurricaneSpell = selectHighestRank(TREE);

        Demo_Timer = 500;
        MangleB_Timer = 1000;
        MangleC_Timer = 1000;
        Heal_Timer = 3000;
        Dmg_Timer = 2000;
        Hurricane_Timer = 15000;

    }

    void UpdateAI(const uint32 diff);

    bool feral;

    uint32 Demo_Timer;
    SpellEntry const *DemoSpell;

    uint32 MangleB_Timer;
    SpellEntry const *MangleBSpell;

    uint32 MangleC_Timer;
    SpellEntry const *MangleCSpell;

    uint32 Heal_Timer;
    SpellEntry const *Heal1Spell;
    SpellEntry const *Heal2Spell;
    SpellEntry const *Heal3Spell;

    uint32 Dmg_Timer;
    SpellEntry const *Dmg1Spell;
    SpellEntry const *Dmg2Spell;
    SpellEntry const *Dmg3Spell;

    uint32 Hurricane_Timer;
    SpellEntry const *HurricaneSpell;

};

#define BLADE_FLURRY    13877
#define GOUGE_R1        1776
#define SINISTER_R1        1752

struct RogueAI: public PlayerAI
{
    RogueAI(Player *pPlayer): PlayerAI(pPlayer) {}

void Reset()
    {
        if (!(FlurrySpell = selectHighestRank(BLADE_FLURRY)))
            blade=false;
        GougeSpell = selectHighestRank(GOUGE_R1);
        SinisterSpell = selectHighestRank(SINISTER_R1);

        Flurry_Timer = 3000+urand(0, 30000);
        Gouge_Timer = 3000+urand(0, 10000);
        Sinister_Timer = 2000+urand(0, 3000);
    }

    void UpdateAI(const uint32 diff);

    bool blade;
    uint32 Flurry_Timer;
    SpellEntry const *FlurrySpell;

    uint32 Gouge_Timer;
    SpellEntry const *GougeSpell;

    uint32 Sinister_Timer;
    SpellEntry const *SinisterSpell;

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
        if (Totem = selectHighestRank(30706))
            ShieldSpell = selectHighestRank(WATER_SHIELD_R1);
        else if (ShieldSpell = selectHighestRank(EARTH_SHIELD_R1))
            heal = true;
        else
            ShieldSpell = selectHighestRank(LIGHTNING_SHIELD_R1);

        HealSpell = selectHighestRank(CHAIN_HEAL_R1);
        LightningSpell = selectHighestRank(CHAIN_LIGHTNING_R1);

        if (!(BLSpell = selectHighestRank(BL)))
            BLSpell = selectHighestRank(HERO);

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

#define MINDBLAST_R1    8092
#define VAMPIRIC        15286
#define MINDFLY_R1        15407
#define SMITE_R1        585
#define FLASH_R1        2061
#define NOVA_R1            15237
#define HOLY_FIRE_R1    14914
#define PW_SHIELD_R1    17

struct PriestAI: public PlayerAI
{
    PriestAI(Player *pPlayer): PlayerAI(pPlayer) {}

    void Reset()
    {
        if (VampiricSpell = selectHighestRank(VAMPIRIC))
            vampiric = true;

        if (!(DOTSpell = selectHighestRank(MINDFLY_R1)))
            DOTSpell = selectHighestRank(HOLY_FIRE_R1);

        DmgSpell = selectHighestRank(SMITE_R1);
        if (vampiric == true)
            DmgSpell = selectHighestRank(MINDBLAST_R1);

        FlashSpell = selectHighestRank(FLASH_R1);

        if (NovaSpell = selectHighestRank(NOVA_R1))
            holynova = true;

        PWShieldSpell = selectHighestRank(PW_SHIELD_R1);

        Vampiric_Timer = 500;
        DmgSpell_Timer = 1500;
        Flash_Timer = 0;
        Nova_Timer = 500;
        DOTSpell_Timer = 4000;
        PWShield_Timer = 2000;
    }

    bool vampiric;
    bool holynova;

    uint32 Vampiric_Timer;
    SpellEntry const *VampiricSpell;

    uint32 DmgSpell_Timer;
    SpellEntry const *DmgSpell;

    uint32 Flash_Timer;
    SpellEntry const *FlashSpell;

    uint32 Nova_Timer;
    SpellEntry const *NovaSpell;

    uint32 DOTSpell_Timer;
    SpellEntry const *DOTSpell;

    uint32 PWShield_Timer;
    SpellEntry const *PWShieldSpell;

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

        if (FireMage)
            MassiveAOESpell = selectHighestRank(FLAMESTRIKE_R1);
        else
            MassiveAOESpell = selectHighestRank(BLIZZARD_R1);

        if (!(ConeSpell = selectHighestRank(DRAGONBREATH_R1)))
            ConeSpell = selectHighestRank(CONEOFCOLD_R1);

        if (!(AOESpell = selectHighestRank(BLASTWAVE_R1)))
            AOESpell = selectHighestRank(ARCANEEXPLO_R1);

        ConeSpell_Timer = 5000;
        MassiveAOE_Timer = 10000;
        AOESpell_Timer = 2000;

        if (FireMage)
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
