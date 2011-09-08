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
    SPELL_FLAME_SEAR        =   46771,
    SPELL_BLAZE_SUMMON      =   45236, //187366 GO
    SPELL_BLAZE_BURN        =   45246
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

    ScriptedInstance *pInstance;

    bool SisterDeath;

    uint32 ShadowbladesTimer;
    uint32 ShadownovaTimer;
    uint32 ConfoundingblowTimer;
    uint32 ShadowimageTimer;
    uint32 ConflagrationTimer;
    uint32 EnrageTimer;

    uint32 checkTimer;

    void Reset()
    {
        Unit* Alythess = m_creature->GetUnit(pInstance->GetData64(DATA_ALYTHESS));
        if (Alythess)
        {
            if (Alythess->isDead())
                ((Creature*)Alythess)->Respawn();
            else if(Alythess->getVictim())
                m_creature->getThreatManager().addThreat(Alythess->getVictim(),0.0f);

            return;
        }

        ShadowbladesTimer = 10000;
        ShadownovaTimer = urand(25000, 35000);
        ConfoundingblowTimer = urand(20000, 30000);
        ShadowimageTimer = 20000;
        ConflagrationTimer = urand(25000, 35000);
        EnrageTimer = 360000;

        checkTimer = 3000;

        SisterDeath = false;

        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void OnAuraApply(Aura * aur, Unit * caster, bool addStack)
    {
        if (aur->GetId() == SPELL_EMPOWER)
            SisterDeath = true;
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();

        Unit* Alythess =  m_creature->GetUnit(pInstance->GetData64(DATA_ALYTHESS));
        if (Alythess && Alythess->isAlive() && !(Alythess->getVictim()))
            ((Creature*)Alythess)->AI()->AttackStart(who);

        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        if (rand()%4 == 0)
            DoScriptText(RAND(YELL_SAC_KILL_1, YELL_SAC_KILL_2), m_creature);
    }

    void JustDied(Unit* Killer)
    {
        // only if ALY death
        if (SisterDeath)
        {
            DoScriptText(SAY_SAC_DEAD, m_creature);

            pInstance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
        }
        else
        {
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            Creature * alythess = me->GetCreature(pInstance->GetData64(DATA_ALYTHESS));

            if (alythess)
                alythess->CastSpell(alythess, SPELL_EMPOWER, true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (checkTimer < diff)
        {
            DoZoneInCombat();
            checkTimer = 3000;
        }
        else
            checkTimer -= diff;

        if (SisterDeath)
        {
            if (ConflagrationTimer < diff)
            {
                Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1);
                if (target)
                    AddSpellToCast(target, SPELL_CONFLAGRATION);

                ConflagrationTimer = urand(25000, 35000);
            }
            else
                ConflagrationTimer -= diff;
        }

        if (ShadownovaTimer < diff)
        {
            Creature * alythess = me->GetCreature(pInstance->GetData64(DATA_ALYTHESS));
            if (alythess)
            {
                Unit * target = alythess->AI()->SelectUnit(SELECT_TARGET_TOPAGGRO, 1);
                if (target)
                {
                    AddSpellToCastWithScriptText(target, SPELL_SHADOW_NOVA, EMOTE_SHADOW_NOVA);
                    if (!SisterDeath)
                        DoScriptText(YELL_SHADOW_NOVA, m_creature);
                }
            }

            ShadownovaTimer = urand(25000, 35000);
        }
        else
            ShadownovaTimer -= diff;


        if (ConfoundingblowTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_CONFOUNDING_BLOW);

            ConfoundingblowTimer = urand(20000, 30000);
        }
        else
            ConfoundingblowTimer -= diff;

        if (ShadowimageTimer < diff)
        {
            Unit* target = NULL;
            Creature* shadowImage = NULL;
            for (uint8 i = 0; i < 3; i++)
            {
                // TODO: wybieranie tylko z osob bedacych w LOSie + preferowanie ranged nad melee
                target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                shadowImage = DoSpawnCreature(MOB_SHADOW_IMAGE, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                if (shadowImage && target)
                    shadowImage->AI()->AttackStart(target);
            }

            ShadowimageTimer = urand(10000, 25000);
        }
        else
            ShadowimageTimer -= diff;

        if (ShadowbladesTimer < diff)
        {
            ForceSpellCast(SPELL_SHADOW_BLADES, false);
            ShadowbladesTimer = 10000;
        }
        else
            ShadowbladesTimer -= diff;

        if (EnrageTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_ENRAGE, true, YELL_ENRAGE);
            EnrageTimer = 60000;
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
    }

    enum Intro
    {
        INTRO_NOT_STARTED = 0,
        INTRO_IN_PROGRESS,
        INTRO_DONE
    };

    ScriptedInstance *pInstance;

    bool SisterDeath;

    Intro intro;
    uint32 IntroStepCounter;
    uint32 IntroYellTimer;

    uint32 ConflagrationTimer;
    uint32 BlazeTimer;
    uint32 PyrogenicsTimer;
    uint32 ShadownovaTimer;
    uint32 FlamesearTimer;
    uint32 EnrageTimer;

    uint32 checkTimer;

    void Reset()
    {
        SetAutocast(SPELL_BLAZE, 2750, true);

        Unit* Sacrolash = m_creature->GetUnit(pInstance->GetData64(DATA_SACROLASH));
        if (Sacrolash)
        {
            if (Sacrolash->isDead())
                ((Creature*)Sacrolash)->Respawn();
            else if(Sacrolash->getVictim())
                m_creature->getThreatManager().addThreat(Sacrolash->getVictim(),0.0f);

            return;
        }

        ConflagrationTimer = urand(25000, 35000);
        BlazeTimer = 100;
        PyrogenicsTimer = 15000;
        ShadownovaTimer = urand(25000, 35000);
        EnrageTimer = 360000;
        FlamesearTimer = 15000;
        IntroYellTimer = 10000;
        IntroStepCounter = 0;

        SisterDeath = false;

        checkTimer = 3000;

        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, NOT_STARTED);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();

        Unit* Sacrolash =  m_creature->GetUnit(pInstance->GetData64(DATA_SACROLASH));
        if (Sacrolash && Sacrolash->isAlive() && !(Sacrolash->getVictim()))
            ((Creature*)Sacrolash)->AI()->AttackStart(who);

        pInstance->SetData(DATA_EREDAR_TWINS_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit *who)
    {
        if (!m_creature->isInCombat())
            Scripted_NoMovementAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(intro == INTRO_NOT_STARTED && !m_creature->IsFriendlyTo(who) && m_creature->IsWithinDistInMap(who, 30))
            intro = INTRO_IN_PROGRESS;

        Scripted_NoMovementAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit *victim)
    {
        if(rand()%4 == 0)
            DoScriptText(RAND(YELL_ALY_KILL_1, YELL_ALY_KILL_2), m_creature);
    }

    void JustDied(Unit* Killer)
    {
        if (SisterDeath)
        {
            DoScriptText(YELL_ALY_DEAD, m_creature);

            pInstance->SetData(DATA_EREDAR_TWINS_EVENT, DONE);
        }
        else
        {
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            Creature* Sacrolash = m_creature->GetCreature(pInstance->GetData64(DATA_SACROLASH));

            if (Sacrolash)
                Sacrolash->CastSpell(Sacrolash, SPELL_EMPOWER, true);
        }
    }

    uint32 IntroStep(uint32 step)
    {
        Creature* Sacrolash = (Creature*)(m_creature->GetUnit(pInstance->GetData64(DATA_SACROLASH)));

        switch (step)
        {
            case 0:
                return 1000;
            case 1:
                if(Sacrolash)
                    DoScriptText(YELL_INTRO_SAC_1, Sacrolash);
                return 1000;
            case 2:
                DoScriptText(YELL_INTRO_ALY_2, m_creature);
                return 1000;
            case 3:
                if(Sacrolash)
                    DoScriptText(YELL_INTRO_SAC_3, Sacrolash);
                return 2000;
            case 4:
                DoScriptText(YELL_INTRO_ALY_4, m_creature);
                return 1000;
            case 5:
                if(Sacrolash)
                    DoScriptText(YELL_INTRO_SAC_5, Sacrolash);
                return 2000;
            case 6:
                DoScriptText(YELL_INTRO_ALY_6, m_creature);
                return 1000;
            case 7:
                if(Sacrolash)
                    DoScriptText(YELL_INTRO_SAC_7, Sacrolash);
                return 3000;
            case 8:
                DoScriptText(YELL_INTRO_ALY_8, m_creature);
                if(Sacrolash)
                    Sacrolash->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                intro = INTRO_DONE;
                return 10000;
            default:
                break;
        }

        return 10000;
    }

    void OnAuraApply(Aura * aur, Unit * caster, bool addStack)
    {
        if (aur->GetId() == SPELL_EMPOWER)
            SisterDeath = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (intro == INTRO_IN_PROGRESS)
        {
            if (IntroYellTimer < diff)
                IntroYellTimer = IntroStep(++IntroStepCounter);
            else
                IntroYellTimer -= diff;

            return;
        }

        if (!UpdateVictim())
            return;

        if (checkTimer < diff)
        {
            DoZoneInCombat();
            checkTimer = 3000;
        }
        else
            checkTimer -= diff;

        if (SisterDeath)
        {
            if (ShadownovaTimer < diff)
            {
                Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1);
                if(target)
                    AddSpellToCast(target, SPELL_SHADOW_NOVA);

                ShadownovaTimer = urand(25000, 35000);
            }
            else
                ShadownovaTimer -= diff;
        }

        if (ConflagrationTimer < diff)
        {
            Creature * sacrolash = me->GetCreature(pInstance->GetData64(DATA_SACROLASH));
            if (sacrolash)
            {
                Unit * target = sacrolash->AI()->SelectUnit(SELECT_TARGET_TOPAGGRO, 1);

                if (target)
                {
                    AddSpellToCastWithScriptText(target, SPELL_CONFLAGRATION, EMOTE_CONFLAGRATION);

                    if (!SisterDeath)
                        DoScriptText(YELL_CANFLAGRATION, m_creature);
                }
            }

            ConflagrationTimer = urand(25000, 35000);
        }
        else
            ConflagrationTimer -= diff;

        if (FlamesearTimer < diff)
        {
            AddSpellToCast(SPELL_FLAME_SEAR, false);
            FlamesearTimer = 15000;
        }
        else
            FlamesearTimer -= diff;

        if (PyrogenicsTimer < diff)
        {
            AddSpellToCast(SPELL_PYROGENICS, true);
            PyrogenicsTimer = urand(10000, 20000);
        }
        else
            PyrogenicsTimer -= diff;

        if (EnrageTimer < diff)
        {
            AddSpellToCastWithScriptText(SPELL_ENRAGE, true, YELL_BERSERK);
            EnrageTimer = 60000;
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
    uint32 KillTimer;
    uint32 DarkstrikeTimer;

    void Reset()
    {
        ShadowfuryTimer = urand(5000, 20000);
        DarkstrikeTimer = 3000;
        KillTimer = 15000;
    }

    void AttackStart(Unit * target)
    {
        if (target)
            m_creature->getThreatManager().addThreat(target, 10000.0f);

        ScriptedAI::AttackStart(target);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->HasAura(SPELL_IMAGE_VISUAL, 0))
            DoCast(m_creature, SPELL_IMAGE_VISUAL);

        if (KillTimer < diff)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->DestroyForNearbyPlayers();
            m_creature->Kill(m_creature, false);
            KillTimer = 9999999;
        }
        else
            KillTimer -=diff;

        if (!UpdateVictim())
            return;

        if (ShadowfuryTimer < diff)
        {
            m_creature->CastSpell((Unit*)NULL, SPELL_SHADOW_FURY, false);
            ShadowfuryTimer = 10000;
        }
        else
            ShadowfuryTimer -= diff;

        if (DarkstrikeTimer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                //If we are within range melee the target
                if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_DARK_STRIKE, false);
            }

            DarkstrikeTimer = 1000;
        }
        else
            DarkstrikeTimer -= diff;
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
