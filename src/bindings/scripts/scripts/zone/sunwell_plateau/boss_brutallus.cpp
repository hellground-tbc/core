/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Brutallus
SD%Complete: 99
SDComment: 
EndScriptData */

#include "precompiled.h"
#include "def_sunwell_plateau.h"

enum Quotes
{
 YELL_INTRO                 =   -1580017,
 YELL_INTRO_BREAK_ICE       =   -1580018,
 YELL_INTRO_CHARGE          =   -1580019,
 YELL_INTRO_KILL_MADRIGOSA  =   -1580020,
 YELL_INTRO_TAUNT           =   -1580021,

 YELL_MADR_ICE_BARRIER      =   -1580031,
 YELL_MADR_INTRO            =   -1580032,
 YELL_MADR_ICE_BLOCK        =   -1580033,
 YELL_MADR_TRAP             =   -1580034,
 YELL_MADR_DEATH            =   -1580035,

 YELL_AGGRO                 =   -1580022,
 YELL_KILL1                 =   -1580023,
 YELL_KILL2                 =   -1580024,
 YELL_KILL3                 =   -1580025,
 YELL_LOVE1                 =   -1580026,
 YELL_LOVE2                 =   -1580027,
 YELL_LOVE3                 =   -1580028,
 YELL_BERSERK               =   -1580029,
 YELL_DEATH                 =   -1580030
};

enum Spells
{
    SPELL_METEOR_SLASH                 =   45150,
    SPELL_BURN                         =   45141,
    SPELL_STOMP                        =   45185,
    SPELL_BERSERK                      =   26662,
    SPELL_DUAL_WIELD                   =   42459,
    SPELL_SUMMON_DEATH_CLOUD           =   45884,
    SPELL_DEATH_CLOUD                  =   45212,

    SPELL_INTRO_FROST_BLAST            =   45203,
    SPELL_INTRO_FROSTBOLT              =   44843,
    SPELL_INTRO_ENCAPSULATE            =   45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING =   45661
};

#define MOB_DEATH_CLOUD 25703
#define FELMYST 25038

struct TRINITY_DLL_DECL boss_brutallusAI : public ScriptedAI
{
    boss_brutallusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 SlashTimer;
    uint32 BurnTimer;
    uint32 StompTimer;
    uint32 BerserkTimer;
    uint32 CheckTimer;

    uint32 IntroPhase;
    uint32 IntroPhaseTimer;
    uint32 IntroFrostBoltTimer;

    bool IntroDone;
    bool IsIntro;
    bool Enraged;

    void Reset()
    {
        SlashTimer = 11000;
        StompTimer = 30000;
        BurnTimer = 60000;
        BerserkTimer = 360000;
        CheckTimer = 1000;

        IntroPhase = 0;
        IntroPhaseTimer = 0;
        IntroFrostBoltTimer = 0;

        IsIntro = false;
        IntroDone = false;
        Enraged = false;

        ForceSpellCast(me, SPELL_DUAL_WIELD, INTERRUPT_AND_CAST_INSTANTLY);
        pInstance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        if (!IsIntro)
        {
            DoScriptText(YELL_AGGRO, me);
            pInstance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (!IsIntro)
            DoScriptText(RAND(YELL_KILL1, YELL_KILL2, YELL_KILL3), me);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        ForceSpellCastWithScriptText(me, SPELL_SUMMON_DEATH_CLOUD, YELL_DEATH, INTERRUPT_AND_CAST_INSTANTLY);
        pInstance->SetData(DATA_BRUTALLUS_EVENT, DONE);
    }

    void DamageTaken(Unit *pAttacker, uint32 &damage)
    {
        if (IsIntro && pAttacker->isCharmedOwnedByPlayerOrPlayer())
            damage = 0;
    }

    void StartIntro()
    {
        if (Unit* pMadrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA)))
        {
            if (pMadrigosa->isAlive())
            {
                pMadrigosa->setActive(true);
                IsIntro = true;
                return;
            }
            else
            {
                ((Creature*)pMadrigosa)->SetCorpseDelay(51000000);
                pMadrigosa->setDeathState(JUST_DIED);  // to update corpse delay
            }
        }

        EndIntro();
    }

    void EndIntro()
    {
        IntroDone = true;
        IsIntro = false;
    }

    void DoIntro()
    {
        Creature *pMadrigosa = (Creature*)me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA));
        if (!pMadrigosa)
            return;

        switch (IntroPhase)
        {
            case 0:
                DoScriptText(YELL_MADR_ICE_BARRIER, pMadrigosa);
                IntroPhaseTimer = 5500;
                ++IntroPhase;
                break;
            case 1:
                me->SetInFront(pMadrigosa);
                pMadrigosa->SetInFront(me);
                DoScriptText(YELL_MADR_INTRO, pMadrigosa);
                IntroPhaseTimer = 4000;
                ++IntroPhase;
                break;
            case 2:
                DoScriptText(YELL_INTRO, me);
                IntroPhaseTimer = 5000;
                AttackStart(pMadrigosa);
                ++IntroPhase;
                break;
            case 3:
            {
                AddSpellToCast(me, SPELL_INTRO_FROST_BLAST);
                pMadrigosa->SetLevitate(true);
                float x, y, z;
                pMadrigosa->GetPosition(x, y, z);
                pMadrigosa->GetMotionMaster()->MovePoint(1, x, y, z+10);
                IntroFrostBoltTimer = 3000;
                IntroPhaseTimer = 28000;
                ++IntroPhase;
                break;
            }
            case 4:
                DoScriptText(YELL_INTRO_BREAK_ICE, me);
                IntroPhaseTimer = 6000;
                ++IntroPhase;
                break;
            case 5:
                pMadrigosa->CastSpell(me, SPELL_INTRO_ENCAPSULATE_CHANELLING, false);
                DoScriptText(YELL_MADR_TRAP, pMadrigosa);
                AddSpellToCast(me, SPELL_INTRO_ENCAPSULATE);
                IntroPhaseTimer = 11000;
                ++IntroPhase;
                break;
            case 6:
                me->SetSpeed(MOVE_RUN, 4.0f, true);
                DoScriptText(YELL_INTRO_CHARGE, me);
                IntroPhaseTimer = 3000;
                ++IntroPhase;
                break;
            case 7:
                me->DealDamage(pMadrigosa, pMadrigosa->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
                DoScriptText(YELL_MADR_DEATH, pMadrigosa);
                me->SetHealth(me->GetMaxHealth());
                me->AttackStop();
                me->SetSpeed(MOVE_RUN, 1.0f, true);
                IntroPhaseTimer = 3000;
                ++IntroPhase;
                break;
            case 8:
                DoScriptText(YELL_INTRO_KILL_MADRIGOSA, me);
                me->SetOrientation(0.14);
                ((Creature*)pMadrigosa)->SetCorpseDelay(51000000);
                pMadrigosa->setDeathState(JUST_DIED);
                IntroPhaseTimer = 6000;
                ++IntroPhase;
                break;
            case 9:
                DoScriptText(YELL_INTRO_TAUNT, me);
                IntroPhaseTimer = 5000;
                ++IntroPhase;
                break;
            case 10:
                EndIntro();
                break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!IsIntro && !IntroDone && who->isCharmedOwnedByPlayerOrPlayer() && !me->IsFriendlyTo(who) && me->IsWithinDist(who, 40))
            StartIntro();
        
        if (IntroDone)
            ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsIntro)
        {
            if (IntroPhaseTimer < diff)
                DoIntro();
            else
                IntroPhaseTimer -= diff;

            if (IntroPhase == 4)
            {
                if (IntroFrostBoltTimer < diff)
                {
                    if(Unit *pMadrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA)))
                    {
                        pMadrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, false);
                        IntroFrostBoltTimer = 2000;
                    }
                }
                else
                    IntroFrostBoltTimer -= diff;
            }
            if (IntroPhase >= 2)
                DoMeleeAttackIfReady();

            CastNextSpellIfAnyAndReady();
        }

        if (IsIntro || !UpdateVictim())
            return;

        if (CheckTimer < diff)
        {
            DoZoneInCombat();

            me->SetSpeed(MOVE_RUN, 2.0f);
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if (SlashTimer < diff)
        {
            AddSpellToCast(me, SPELL_METEOR_SLASH);
            SlashTimer = 11000;
        }
        else
            SlashTimer -= diff;

        if (StompTimer < diff)
        {
            AddSpellToCastWithScriptText(me->getVictim(), SPELL_STOMP, RAND(YELL_LOVE1, YELL_LOVE2, YELL_LOVE3));
            StompTimer = 30000;
        }
        else
            StompTimer -= diff;

        if (BurnTimer < diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                AddSpellToCast(pTarget, SPELL_BURN);

            BurnTimer = 60000;
        }
        else 
            BurnTimer -= diff;

        if (BerserkTimer < diff && !Enraged)
        {
            AddSpellToCastWithScriptText(me, SPELL_BERSERK, YELL_BERSERK);
            Enraged = true;
        }
        else
            BerserkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL npc_death_cloudAI : public ScriptedAI
{
    npc_death_cloudAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint32 SummonTimer;
    
    void Reset() 
    {
        SummonTimer = 1000;
    }

    void JustRespawned()
    {
        ForceSpellCast(me, SPELL_DEATH_CLOUD, INTERRUPT_AND_CAST_INSTANTLY);
        SummonTimer = 1000;
    }

    void UpdateAI(const uint32 diff) 
    {
        if (SummonTimer < diff)
        {
            float x, y, z;
            if (Unit* pMadrigosa= me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA)))
            {
                if (!me->IsWithinDist(pMadrigosa, 5.0f))
                {      
                    z = me->GetPositionZ();
                    me->GetNearPoint2D(x, y, 5, me->GetAngle(pMadrigosa));
                    me->UpdateAllowedPositionZ(x, y, z);
                    me->SummonCreature(MOB_DEATH_CLOUD, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
                } 
                else
                {
                    if (Creature* pFelmyst= me->GetCreature(pInstance->GetData64(DATA_FELMYST)))
                    {
                        pMadrigosa->GetPosition(x, y, z);
                        ((Creature*)pFelmyst)->Respawn();
                        pFelmyst->Relocate(x, y, z);
                        pFelmyst->SetLevitate(false);
                        pFelmyst->GetMotionMaster()->MoveRandom(1);
                    }
                    ((Creature*)pMadrigosa)->RemoveCorpse();
                }
            }
            SummonTimer = 120000000;
        }
        else
            SummonTimer -= diff;
    }
};

CreatureAI* GetAI_boss_brutallus(Creature *_Creature)
{
    return new boss_brutallusAI (_Creature);
}

CreatureAI* GetAI_npc_death_cloud(Creature *_Creature)
{
    return new npc_death_cloudAI (_Creature);
}

void AddSC_boss_brutallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_death_cloud";
    newscript->GetAI = &GetAI_npc_death_cloud;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_brutallus";
    newscript->GetAI = &GetAI_boss_brutallus;
    newscript->RegisterSelf();
}
