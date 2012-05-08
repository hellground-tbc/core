/* Copyright (C) 2009 Trinity <http://www.trinitycore.org/>
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
SDName: Boss_Eredar_Twins
SD%Complete: 80
SDComment: make gobj 187366 casting SPELL_BLAZE_BURN, make order with xxx_Touched spells
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum Quotes
{
    //Alytesh
    YELL_CANFLAGRATION          =   -1580044,
    YELL_SISTER_SACROLASH_DEAD  =   -1580045,
    YELL_ALY_KILL_1             =   -1580046,
    YELL_ALY_KILL_2             =   -1580047,
    YELL_ALY_DEAD               =   -1580048,
    YELL_BERSERK                =   -1580049,

    //Sacrolash
    YELL_SHADOW_NOVA            =   -1580050,
    YELL_SISTER_ALYTHESS_DEAD   =   -1580051,
    YELL_SAC_KILL_1             =   -1580052,
    YELL_SAC_KILL_2             =   -1580053,
    SAY_SAC_DEAD                =   -1580054,
    YELL_ENRAGE                 =   -1580055,

    //Intro
    YELL_INTRO_SAC_1            =   -1580056,
    YELL_INTRO_ALY_2            =   -1580057,
    YELL_INTRO_SAC_3            =   -1580058,
    YELL_INTRO_ALY_4            =   -1580059,
    YELL_INTRO_SAC_5            =   -1580060,
    YELL_INTRO_ALY_6            =   -1580061,
    YELL_INTRO_SAC_7            =   -1580062,
    YELL_INTRO_ALY_8            =   -1580063,

    //Emote
    EMOTE_SHADOW_NOVA           =   -1580064,
    EMOTE_CONFLAGRATION         =   -1580065
};

enum Spells
{
    //Lady Sacrolash spells
    SPELL_DARK_TOUCHED      =   45347,
    SPELL_SHADOW_BLADES     =   45248, //10 secs
    SPELL_DARK_STRIKE       =   45271,
    SPELL_SHADOW_NOVA       =   45329, //30-35 secs
    SPELL_CONFOUNDING_BLOW  =   45256, //25 secs

    //Shadow Image spells
    SPELL_SHADOW_FURY       =   45270,
    SPELL_IMAGE_VISUAL      =   45263,

    //Misc spells
    SPELL_ENRAGE            =   46587,
    SPELL_EMPOWER           =   45366,
    SPELL_DARK_FLAME        =   45345,

    //Grand Warlock Alythess spells
    SPELL_PYROGENICS        =   45230, //15secs
    SPELL_FLAME_TOUCHED     =   45348,
    SPELL_CONFLAGRATION     =   45342, //30-35 secs
    SPELL_BLAZE             =   45235, //on main target every 3 secs
    SPELL_FLAME_SEAR        =   46771
};

enum Creatures
{
    GRAND_WARLOCK_ALYTHESS  =   25166,
    MOB_SHADOW_IMAGE        =   25214,
    LADY_SACROLASH          =   25165
};

struct TRINITY_DLL_DECL boss_sacrolashAI : public ScriptedAI
{
    boss_sacrolashAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    InstanceData *pInstance;

    uint32 ShadowbladesTimer;
    uint32 ShadownovaTimer;
    uint32 ConfoundingblowTimer;
    uint32 ShadowimageTimer;
    uint32 ConflagrationTimer;
    uint32 EnrageTimer;

    void Reset()
    {
        ShadowbladesTimer = 10000;
        ShadownovaTimer = 30000;
        ConfoundingblowTimer = 25000;
        ShadowimageTimer = 20000;
        ConflagrationTimer = 30000;
        EnrageTimer = 360000;

        if (pInstance->GetData(DATA_EREDAR_TWINS_INTRO == DONE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        else
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        pInstance->SetData(DATA_SACROLASH, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (pInstance->GetData(DATA_EREDAR_TWINS_INTRO) == DONE)
            ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit *victim)
    {
        if (roll_chance_i(20))
            DoScriptText(RAND(YELL_SAC_KILL_1, YELL_SAC_KILL_2), me);
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance->GetData(DATA_ALYTHESS) == DONE)
        {
            DoScriptText(SAY_SAC_DEAD, me);
            pInstance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
        }
        else
            me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        pInstance->SetData(DATA_SACROLASH, DONE);
    }

    void DoAction(const int32 action)
    {
        if (action == SISTER_DEATH)
            AddSpellToCastWithScriptText(SPELL_EMPOWER, CAST_SELF, YELL_SISTER_ALYTHESS_DEAD);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (pInstance->GetData(DATA_ALYTHESS) == DONE)
        {
            if (ConflagrationTimer < diff)
            {
                AddSpellToCast(SPELL_CONFLAGRATION, CAST_RANDOM);
                ConflagrationTimer = urand(30000, 35000);
            }
            else 
                ConflagrationTimer -= diff;
        }
        
        if (ShadownovaTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_SHADOW_NOVA, CAST_RANDOM, EMOTE_SHADOW_NOVA);
            if (pInstance->GetData(DATA_ALYTHESS) != DONE)
                DoScriptText(YELL_SHADOW_NOVA, me);

            ShadownovaTimer = urand(30000,35000);
        }
        else
            ShadownovaTimer -= diff;

        if (ConfoundingblowTimer < diff)
        {
            AddSpellToCast(SPELL_CONFOUNDING_BLOW, CAST_RANDOM);
            ConfoundingblowTimer = urand(20000, 25000);
        }
        else
            ConfoundingblowTimer -= diff;

        if (ShadowimageTimer < diff)
        {
            for (int i = 0; i < 3; i++)
                DoSpawnCreature(MOB_SHADOW_IMAGE,0,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 15000);

            ShadowimageTimer = 20000;
        }
        else
            ShadowimageTimer -= diff;

        if (ShadowbladesTimer < diff)
        {
            AddSpellToCast(SPELL_SHADOW_BLADES, CAST_SELF);
            ShadowbladesTimer = 10000;
        }
        else
            ShadowbladesTimer -= diff;

        if (EnrageTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_ENRAGE, CAST_SELF, YELL_ENRAGE);
            EnrageTimer = 360000;
        }
        else
            EnrageTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sacrolash(Creature *_Creature)
{
    return new boss_sacrolashAI (_Creature);
};

struct TRINITY_DLL_DECL boss_alythessAI : public Scripted_NoMovementAI
{
    boss_alythessAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
        IntroStepCounter = 10;
    }

    InstanceData *pInstance;

    bool IntroDone;

    uint32 IntroStepCounter;
    uint32 IntroYellTimer;

    uint32 ConflagrationTimer;
    uint32 PyrogenicsTimer;
    uint32 ShadownovaTimer;
    uint32 FlamesearTimer;
    uint32 EnrageTimer;

    void Reset()
    {
        ConflagrationTimer = 45000;
        PyrogenicsTimer = 15000;
        ShadownovaTimer = 40000;
        EnrageTimer = 360000;
        FlamesearTimer = 15000;
        IntroYellTimer = 10000;
        IntroStepCounter = 10;

        IntroDone = false;

        if (pInstance->GetData(DATA_EREDAR_TWINS_INTRO == DONE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        else
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        pInstance->SetData(DATA_ALYTHESS, NOT_STARTED);

        SetAutocast(SPELL_BLAZE, 3000, false);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit *who)
    {
        // TODO: this should be moved to some trigger at doors
        if (pInstance->GetData(DATA_EREDAR_TWINS_INTRO) == NOT_STARTED && !me->IsFriendlyTo(who) && me->IsWithinDistInMap(who, 30))
        {
            IntroStepCounter = 0;
            pInstance->SetData(DATA_EREDAR_TWINS_INTRO, IN_PROGRESS);
        }

        if (pInstance->GetData(DATA_EREDAR_TWINS_INTRO) == DONE)
            Scripted_NoMovementAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit *victim)
    {
        if (roll_chance_i(20))
            DoScriptText(RAND(YELL_ALY_KILL_1, YELL_ALY_KILL_2), me);
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance->GetData(DATA_SACROLASH) == DONE)
        {
            DoScriptText(YELL_ALY_DEAD, me);
            pInstance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
        }
        else
            me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        pInstance->SetData(DATA_ALYTHESS, DONE);
    }

    uint32 IntroStep(uint32 step)
    {
        Creature* pSacrolash = me->GetCreature(pInstance->GetData64(DATA_SACROLASH));
        if (!pSacrolash)
            return 0;

        switch (step)
        {
            case 0:
                return 0;
            case 1:
                DoScriptText(YELL_INTRO_SAC_1, pSacrolash);
                return 1000;
            case 2:
                DoScriptText(YELL_INTRO_ALY_2, me);
                return 1000;
            case 3:
                DoScriptText(YELL_INTRO_SAC_3, pSacrolash);
                return 2000;
            case 4:
                DoScriptText(YELL_INTRO_ALY_4, me);
                return 1000;
            case 5:
                DoScriptText(YELL_INTRO_SAC_5, pSacrolash);
                return 2000;
            case 6:
                DoScriptText(YELL_INTRO_ALY_6, me);
                return 1000;
            case 7:
                DoScriptText(YELL_INTRO_SAC_7, pSacrolash);
                return 3000;
            case 8:
                DoScriptText(YELL_INTRO_ALY_8, me);
                return 3000;
            case 9:
                pInstance->SetData(DATA_EREDAR_TWINS_INTRO, DONE);
                return 900000;
            default:
                return 10000;
        }
    }

    void DoAction(const int32 action)
    {
        if (action == SISTER_DEATH)
            AddSpellToCastWithScriptText(SPELL_EMPOWER, CAST_SELF, YELL_SISTER_SACROLASH_DEAD);
    }

    void UpdateAI(const uint32 diff)
    {
        if (IntroStepCounter < 10)
        {
            if (IntroYellTimer < diff)
            {
                IntroYellTimer = IntroStep(++IntroStepCounter);
            }
            else 
                IntroYellTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (pInstance->GetData(DATA_SACROLASH) == DONE)
        {
            if (ShadownovaTimer < diff)
            {
                AddSpellToCast(SPELL_SHADOW_NOVA, CAST_RANDOM);
                ShadownovaTimer = urand(30000, 35000);
            }
            else 
                ShadownovaTimer -= diff;
        }
        
        if (ConflagrationTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_CONFLAGRATION, CAST_RANDOM, EMOTE_CONFLAGRATION);

            if (pInstance->GetData(DATA_SACROLASH) != DONE)
                DoScriptText(YELL_CANFLAGRATION, me);

            ConflagrationTimer = urand(30000, 35000);
        }
        else
            ConflagrationTimer -= diff;

        if (FlamesearTimer < diff)
        {
            AddSpellToCast(SPELL_FLAME_SEAR, CAST_SELF);
            FlamesearTimer = 15000;
        }
        else
            FlamesearTimer -=diff;

        if (PyrogenicsTimer < diff)
        {
            AddSpellToCast(SPELL_PYROGENICS, CAST_SELF);
            PyrogenicsTimer = 15000;
        }
        else
            PyrogenicsTimer -= diff;

        if (EnrageTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_ENRAGE, CAST_SELF, YELL_BERSERK);
            EnrageTimer = 360000;
        }
        else
            EnrageTimer -= diff;

        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_alythess(Creature *_Creature)
{
    return new boss_alythessAI (_Creature);
};

struct TRINITY_DLL_DECL mob_shadow_imageAI : public ScriptedAI
{
    mob_shadow_imageAI(Creature *c) : ScriptedAI(c) {}

    uint32 ShadowfuryTimer;
    uint32 DarkstrikeTimer;

    void Reset()
    {
        ShadowfuryTimer = urand(5000, 20000);
        DarkstrikeTimer = 3000;
    }

    void AttackStart(Unit * target)
    {
        if (me->getVictim())
            return;

        me->getThreatManager().addThreat(target, 100000.0f);
        ScriptedAI::AttackStart(target);
    }

    void IsSummonedBy(Unit *pSummoner)
    {
        ForceSpellCast(SPELL_IMAGE_VISUAL, CAST_SELF, INTERRUPT_AND_CAST_INSTANTLY);
        DoZoneInCombat();

        if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            AttackStart(pTarget);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ShadowfuryTimer < diff)
        {
            AddSpellToCast(SPELL_SHADOW_FURY, CAST_NULL);
            ShadowfuryTimer = 10000;
        }
        else
            ShadowfuryTimer -= diff;

        if (DarkstrikeTimer < diff)
        {
            if (!me->IsNonMeleeSpellCasted(false))
            {
                //If we are within range melee the target
                if (me->IsWithinMeleeRange(me->getVictim()))
                    AddSpellToCast(SPELL_DARK_STRIKE, CAST_TANK);
            }
            DarkstrikeTimer = 1000;
        }
        else
            DarkstrikeTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};

CreatureAI* GetAI_mob_shadow_image(Creature *_Creature)
{
    return new mob_shadow_imageAI (_Creature);
};

void AddSC_boss_eredar_twins()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_sacrolash";
    newscript->GetAI = &GetAI_boss_sacrolash;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_alythess";
    newscript->GetAI = &GetAI_boss_alythess;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_shadow_image";
    newscript->GetAI = &GetAI_mob_shadow_image;
    newscript->RegisterSelf();
}

/* SQLe do wrzucenia

-- formation for sacrolash and alythess
REPLACE INTO creature_formations VALUES
(53687, 53668, 0, 0, 2),
(53687, 53687, 0, 0, 2);


*/
