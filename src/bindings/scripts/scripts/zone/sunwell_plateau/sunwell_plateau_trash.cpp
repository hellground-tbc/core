/* Copyright (C) 2008 - 2011 HellgroundDev <http://gamefreedom.pl/>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/* ScriptData
SDName: Sunwell_Plateau_Trash
SD%Complete: 27% (9/33)
SDComment: Trash NPCs divided by to boss links
SDCategory: Sunwell Plateau
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

#define AGGRO_RANGE             45.0

/* ============================
*
*      KALECGOS & SATHROVARR & BRUTALLUS & FELMYST
*
* ============================*/

/* Content Data
    * Sunblade Arch Mage
    * Sunblade Cabalist
    * Sunblade Dawn Priest
    * Sunblade Dragonhawk - EventAI
    * Sunblade Dusk Priest
    * Sunblade Protector
    * Sunblade Scout
    * Sunblade Slayer
    * Sunblade Vindicator
*/

/****************
* Sunblade Arch Mage - id 25367

  Immunities: stun

*****************/

enum SunbladeArchMage
{
    SPELL_ARCANE_EXPLOSION          = 46553,
    SPELL_FROST_NOVA                = 46555,
    SPELL_BLINK                     = 46573
};

struct TRINITY_DLL_DECL mob_sunblade_arch_mageAI : public ScriptedAI
{
    mob_sunblade_arch_mageAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 ArcaneExplosion;
    uint32 FrostNova;
    uint32 Blink;

    void Reset()
    {
        ClearCastQueue();
        ArcaneExplosion = urand(5000, 12000);
        FrostNova = urand(5000, 10000);
        Blink = urand(10000, 18000);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Blink > 1000 && (me->isFrozen() || me->HasAuraType(SPELL_AURA_MOD_ROOT)))
            Blink = 1000;

        if(ArcaneExplosion < diff)
        {
            AddSpellToCast(SPELL_ARCANE_EXPLOSION, CAST_SELF);
            ArcaneExplosion = urand(4000, 8000);
        }
        else
            ArcaneExplosion -= diff;

        if(FrostNova < diff)
        {
            AddSpellToCast(SPELL_FROST_NOVA, CAST_SELF);
            FrostNova = urand(6000, 16000);
        }
        else
            FrostNova -= diff;

        if(Blink < diff)
        {
            AddSpellToCast(SPELL_BLINK, CAST_SELF);
            Blink = urand(15000, 30000);
        }
        else
            Blink -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_arch_mage(Creature *_Creature)
{
    return new mob_sunblade_arch_mageAI(_Creature);
}

/****************
* Sunblade Cabalist - id 25363
  Fire Fiend - id 26101 - EventAI

  Immunities: stun, cyclone

*****************/

enum SunbladeCabalist
{
    SPELL_IGNITE_MANA               = 46543,
    SPELL_SHADOW_BOLT               = 47248,
    SPELL_SUMMON_IMP                = 46544
};

struct TRINITY_DLL_DECL mob_sunblade_cabalistAI : public ScriptedAI
{
    mob_sunblade_cabalistAI(Creature *c) : ScriptedAI(c), summons(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 IgniteMana;
    uint32 ShadowBolt;
    uint32 SummonImp;
    SummonList summons;

    void Reset()
    {
        ClearCastQueue();
        summons.DespawnAll();
        IgniteMana = urand(3000, 10000);;
        ShadowBolt = urand(500, 1000);
        SummonImp = urand(1000, 8000);
    }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who, CHECK_TYPE_CASTER);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void JustSummoned(Creature* summon)
    {
        summons.Summon(summon);
        if(me->getVictim())
            summon->AI()->AttackStart(me->getVictim());
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(IgniteMana < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 50, true, POWER_MANA))
                AddSpellToCast(target, SPELL_IGNITE_MANA, false, true);
            IgniteMana = urand(8000, 16000);
        }
        else
            IgniteMana -= diff;

        if(ShadowBolt < diff)
        {
            AddSpellToCast(SPELL_SHADOW_BOLT, CAST_TANK);
            ShadowBolt = urand(2100, 2500);
        }
        else
            ShadowBolt -= diff;

        if(SummonImp < diff)
        {
            AddSpellToCast(SPELL_SUMMON_IMP, CAST_NULL);
            SummonImp = urand(15000, 20000);
        }
        else
            SummonImp -= diff;

        CheckCasterNoMovementInRange(diff, 48.0);
        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_cabalist(Creature *_Creature)
{
    return new mob_sunblade_cabalistAI(_Creature);
}

/****************
* Sunblade Dawn Priest - id 25371

  Immunities: stun

*****************/

enum SunbladeDawnPriest
{
    SPELL_HOLYFORM                  = 46565,
    SPELL_HOLY_NOVA                 = 46564,
    SPELL_RENEW                     = 46563
};

struct TRINITY_DLL_DECL mob_sunblade_dawn_priestAI : public ScriptedAI
{
    mob_sunblade_dawn_priestAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 HolyNova;
    uint32 Renew;
    uint32 SelfRenew;
    bool canRenew;
    bool canSelfRenew;

    void Reset()
    {
        ClearCastQueue();
        DoCast(me, SPELL_HOLYFORM);

        HolyNova = urand(5000, 10000);
        Renew = 1000;
        SelfRenew = 500;
        canRenew = true;
        canSelfRenew = true;
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(HealthBelowPct(70.0f) && canRenew)
        {
            AddSpellToCast(me, SPELL_RENEW);
            canSelfRenew = false;
            SelfRenew = 15000;
        }

        if(Unit* healTarget = SelectLowestHpFriendly(100, 2000))
        {
            if(canRenew)
            {
                AddSpellToCast(healTarget, SPELL_RENEW);
                canRenew = false;
                Renew = urand(10000, 15000);
            }
        }

        if(!canRenew)
        {
            if(Renew < diff)
                canRenew = true;
            else
                Renew -= diff;
        }
        if(!canSelfRenew)
        {
            if(SelfRenew < diff)
                canSelfRenew = true;
            else
                SelfRenew -= diff;
        }

        if(HolyNova < diff)
        {
            AddSpellToCast(SPELL_HOLY_NOVA, CAST_NULL);
            HolyNova = urand(5000, 10000);
        }
        else
            HolyNova -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_dawn_priest(Creature *_Creature)
{
    return new mob_sunblade_dawn_priestAI(_Creature);
}

/****************
* Sunblade Dusk Priest - id 25370

  Immunities: stun, snare

*****************/

enum SunbladeDuskPriest
{
    SPELL_FEAR                      = 46561,
    SPELL_SHADOW_WORD_PAIN          = 46560,
    SPELL_MIND_FLAY                 = 46562
};

struct TRINITY_DLL_DECL mob_sunblade_dusk_priestAI : public ScriptedAI
{
    mob_sunblade_dusk_priestAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 Fear;
    uint32 WordPain;
    uint32 MindFlay;

    void Reset()
    {
        ClearCastQueue();
        Fear = urand(5000, 15000);
        WordPain = urand(6000, 12000);
        MindFlay = urand(2000, 10000);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(Fear < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 25, true, me->getVictim()->GetGUID()))
                AddSpellToCast(target, SPELL_FEAR);
            Fear = urand(6000, 18000);
        }
        else
            Fear -= diff;

        if(WordPain < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true))
                AddSpellToCast(target, SPELL_SHADOW_WORD_PAIN);
            WordPain = urand(12000, 30000);
        }
        else
            WordPain -= diff;

        if(MindFlay < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 50, true))
                AddSpellToCast(target, SPELL_MIND_FLAY, false, true);
            MindFlay = urand(9000, 12000);
        }
        else
            MindFlay -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_dusk_priest(Creature *_Creature)
{
    return new mob_sunblade_dusk_priestAI(_Creature);
}

/****************
* Sunblade Protector - id 25507

  Immunities: bleed, snare, stun

*****************/

enum SunbladeProtector
{
    SPELL_FEL_LIGHTNING             = 46480
};

#define PROTECTOR_YELL "Unit entering energy conservation mode."

struct TRINITY_DLL_DECL mob_sunblade_protectorAI : public ScriptedAI
{
    mob_sunblade_protectorAI(Creature *c) : ScriptedAI(c)
    {
        if(me->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
        {
            isInactive = true;
            me->SetAggroRange(7.0f);
        }
        else
        {
            isInactive = false;
            me->SetAggroRange(AGGRO_RANGE);
        }
    }

    uint32 FelLightning;
    bool isInactive;

    void Reset()
    {
        ClearCastQueue();
        FelLightning = urand(3000, 6000);
    }

    void EnterEvadeMode()
    {
        if(isInactive)
            DoYell(PROTECTOR_YELL, 0, me);
        CreatureAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(FelLightning < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 60, true))
                AddSpellToCast(target, SPELL_FEL_LIGHTNING);
            FelLightning = 6000;
        }
        else
            FelLightning -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_protector(Creature *_Creature)
{
    return new mob_sunblade_protectorAI(_Creature);
}

/****************
* Sunblade Scout - id 25372

  Immunities: root, snares, stunes, poly, detect stealth

*****************/

enum SunbladeScout
{
    SPELL_STEALTH_DETECT                = 18950,
    SPELL_SINISTER_STRIKE               = 46558,
    SPELL_ACTIVATE_SUNBLADE_PROTECTOR   = 46475
};

#define SCOUT_YELL "Enemies Spotted! Attack while I try to activate a Protector!"

struct TRINITY_DLL_DECL mob_sunblade_scoutAI : public ScriptedAI
{
    mob_sunblade_scoutAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 SinisterStrike;
    uint32 Unstack_Timer;

    void Reset()
    {
        ClearCastQueue();
        Unstack_Timer = 7000;
        DoCast(me, SPELL_STEALTH_DETECT, true);

        SinisterStrike = urand(3000, 10000);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    bool ActivateProtector(Unit* who)
    {
        if(Unit* Protector = GetClosestCreatureWithEntry(me, 25507, 65))
        {
            if(Protector->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
            {
                DoYell(SCOUT_YELL, 0, who);
                float x, y, z;
                me->GetMotionMaster()->Clear();
                Protector->GetNearPoint(Protector, x, y, z, 0, urand(10, 20), Protector->GetAngle(me));
                Protector->UpdateAllowedPositionZ(x, y, z);
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(0, x, y, z);
                me->SetSelection(Protector->GetGUID());
                me->SetInFront(Protector);
                return true;
            }
        }
        return false;
    }

    void MovementInform(uint32 MovementType, uint32 Id)
    {
        if(MovementType != POINT_MOTION_TYPE)
            return;
        if(Id == 0)
        {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            DoCast((Unit*)NULL, SPELL_ACTIVATE_SUNBLADE_PROTECTOR);
        }
    }

    void EnterCombat(Unit* who) { DoZoneInCombat(80.0f); }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            if(!ActivateProtector(pWho))
                DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(me->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
        {
            if(Unstack_Timer < diff)
            {
                DoStartMovement(me->getVictim());
                Unstack_Timer = 7000;   // just in any case
            }
            else
                Unstack_Timer -= diff;
        }

        if(SinisterStrike < diff)
        {
            AddSpellToCast(me->getVictim(), SPELL_SINISTER_STRIKE);
            SinisterStrike = urand(4000, 8000);
        }
        else
            SinisterStrike -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_scout(Creature *_Creature)
{
    return new mob_sunblade_scoutAI(_Creature);
}

/****************
* Sunblade Slayer - id 25368

  Immunities: stun

*****************/

enum SunbladeSlayer
{
    SPELL_SCATTER_SHOT                  = 46681,
    SPELL_SHOOT                         = 47001,
    SPELL_SLAYING_SHOT                  = 46557
};

struct TRINITY_DLL_DECL mob_sunblade_slayerAI : public ScriptedAI
{
    mob_sunblade_slayerAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 ScatterShot;
    uint32 Shoot;
    uint32 SlayingShot;

    void Reset()
    {
        ClearCastQueue();
        ScatterShot = urand(6000, 12000);
        Shoot = urand(100, 1000);
        SlayingShot = urand(8000, 15000);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void AttackStart(Unit* who)
    {
        ScriptedAI::AttackStartNoMove(who, CHECK_TYPE_SHOOTER);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(ScatterShot < diff)
        {
            AddSpellToCast(SPELL_SCATTER_SHOT, CAST_TANK);
            ScatterShot = urand(6000, 12000);
        }
        else
            ScatterShot -= diff;

        if(Shoot < diff)
        {
            AddSpellToCast(SPELL_SHOOT, CAST_TANK);
            Shoot = urand(2500, 4000);
        }
        else
            Shoot -= diff;

        if(SlayingShot < diff)
        {
            AddSpellToCast(SPELL_SLAYING_SHOT, CAST_TANK);
            SlayingShot = urand(8000, 15000);
        }
        else
            SlayingShot -= diff;

        CheckShooterNoMovementInRange(diff, 40.0);
        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_slayer(Creature *_Creature)
{
    return new mob_sunblade_slayerAI(_Creature);
}

/****************
* Sunblade Vindicator - id 25369

  Immunities: stun

*****************/

enum SunbladeVindicator
{
    SPELL_BRUTAL_STRIKE                 = 58460,
    SPELL_CLEAVE                        = 46559,
    SPELL_MORTAL_STRIKE                 = 44268
};

struct TRINITY_DLL_DECL mob_sunblade_vindicatorAI : public ScriptedAI
{
    mob_sunblade_vindicatorAI(Creature *c) : ScriptedAI(c) { me->SetAggroRange(AGGRO_RANGE); }

    uint32 BrutalStrike;
    uint32 Cleave;
    uint32 MortalStrike;

    void Reset()
    {
        ClearCastQueue();
        BrutalStrike = urand(1000, 5000);
        Cleave = urand(4000, 9000);
        MortalStrike = urand(5000, 15000);
    }

    void EnterEvadeMode()
    {
        if (CreatureGroup *formation = me->GetFormation())
            formation->RespawnFormation(me);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*) { DoZoneInCombat(80.0f); }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(BrutalStrike < diff)
        {
            AddSpellToCast(SPELL_BRUTAL_STRIKE, CAST_TANK);
            BrutalStrike = urand(5000,11000);
        }
        else
            BrutalStrike -= diff;

        if(Cleave < diff)
        {
            AddSpellToCast(SPELL_CLEAVE, CAST_TANK);
            Cleave = urand(4000, 9000);
        }
        else
            Cleave -= diff;

        if(MortalStrike < diff)
        {
            AddSpellToCast(SPELL_MORTAL_STRIKE, CAST_TANK);
            MortalStrike = urand(8000, 15000);
        }
        else
            MortalStrike -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sunblade_vindicator(Creature *_Creature)
{
    return new mob_sunblade_vindicatorAI(_Creature);
}

/* ============================
*
*      EREDAR TWINS
*
* ============================*/

/* Content Data
    * Blazing Infernal
    * Felguard Slayer
    * Shadowsword Assassin
    * Shadowsword Lifeshaper
    * Shadowsword Manafiend
    * Shadowsword Soulbinder
    * Shadowsword Vanquisher
    * Unyielding Dead
*/

/****************
* Blazing Infernal - id
*****************/

/* ============================
*
*      M'URU & ENTROPIUS
*
* ============================*/

/* Content Data
    * Apocalypse Guard
    * Cataclysm Hound
    * Chaos Gazer
    * Doomfire Destroyer
    * Doomfire Shard
    * Oblivion Mage
    * Painbringer
    * Priestess of Torment
    * Volatile Fiend
    * Volatile Felfire Fiend
    * Shadowsword Berserker
    * Shadowsword Fury Mage
    * Void Sentinel
    * Void Spawn
*/

/****************
* Apocalypse Guard - id
*****************/

/* ============================
*
*      KIL'JAEDEN
*
* ============================*/

/* Content Data
    * Shadowsword Guardian
    * Hand of the Deceiver
*/

/****************
* Shadowsword Guardian - id
*****************/

void AddSC_sunwell_plateau_trash()
{
    Script *newscript;

    // Kalecgos & Sathovarr & Brutallus & Felmyst
    newscript = new Script;
    newscript->Name = "mob_sunblade_arch_mage";
    newscript->GetAI = &GetAI_mob_sunblade_arch_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_cabalist";
    newscript->GetAI = &GetAI_mob_sunblade_cabalist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_dawn_priest";
    newscript->GetAI = &GetAI_mob_sunblade_dawn_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_dusk_priest";
    newscript->GetAI = &GetAI_mob_sunblade_dusk_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_protector";
    newscript->GetAI = &GetAI_mob_sunblade_protector;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_scout";
    newscript->GetAI = &GetAI_mob_sunblade_scout;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_slayer";
    newscript->GetAI = &GetAI_mob_sunblade_slayer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sunblade_vindicator";
    newscript->GetAI = &GetAI_mob_sunblade_vindicator;
    newscript->RegisterSelf();
}
