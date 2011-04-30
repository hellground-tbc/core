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
    boss_brutallusAI(Creature *c) : ScriptedAI(c){
        pInstance = (c->GetInstanceData());
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

        m_creature->CastSpell(m_creature, SPELL_DUAL_WIELD, true);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);    

        if(pInstance)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        if(!IsIntro)
        {
            DoScriptText(YELL_AGGRO, m_creature);

            if(pInstance)
                pInstance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* victim)
    {
        if(!IsIntro)
            DoScriptText(RAND(YELL_KILL1, YELL_KILL2, YELL_KILL3), m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(YELL_DEATH, m_creature);

        if(pInstance)
        {
            pInstance->SetData(DATA_BRUTALLUS_EVENT, DONE);
            DoCast(me, SPELL_SUMMON_DEATH_CLOUD, true);
            /*
            
            Unit *Madrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA));
            if(Madrigosa && Madrigosa->GetTypeId() == TYPEID_UNIT)
            {
                ((Creature*)Madrigosa)->RemoveCorpse();
                Madrigosa->GetPosition(x,y,z);
            } else
                m_creature->GetPosition(x,y,z);

            me->SummonCreature(MOB_DEATH_CLOUD, x, y, z, 0, TE

            

            */
        }
    }

    void JustSummoned(Creature *c)
    {

    }

    void DamageTaken(Unit *attacker, uint32 &damage)
    {
        if(IsIntro && attacker->GetTypeId() == TYPEID_PLAYER)
            damage = 0;
    }

    void StartIntro()
    {
        if(Unit *Madrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA)))
        {
            if(Madrigosa->isAlive())
            {
                Madrigosa->setActive(true);
                IsIntro = true;
                return;
            } else
            {
                ((Creature*)Madrigosa)->SetCorpseDelay(51000000);
                Madrigosa->setDeathState(JUST_DIED);  // to update corpse delay
            }
        }
        EndIntro();
    }

    void EndIntro()
    {
        //m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        IntroDone = true;
        IsIntro = false;
    }

    void DoIntro()
    {
        Unit *Madrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA));
        if(!Madrigosa)
            return;

        switch(IntroPhase)
        {
            case 0:
                DoScriptText(YELL_MADR_ICE_BARRIER, Madrigosa);
                IntroPhaseTimer = 5500;
                ++IntroPhase;
                break;
            case 1:
                m_creature->SetInFront(Madrigosa);
                Madrigosa->SetInFront(m_creature);
                DoScriptText(YELL_MADR_INTRO, Madrigosa);
                IntroPhaseTimer = 4000;
                ++IntroPhase;
                break;
            case 2:
                DoScriptText(YELL_INTRO, m_creature);
                IntroPhaseTimer = 5000;
                m_creature->Attack(Madrigosa, true);
                ++IntroPhase;
                break;
            case 3:
            {
                DoCast(m_creature, SPELL_INTRO_FROST_BLAST);
                Madrigosa->AddUnitMovementFlag(SPLINEFLAG_FLYINGING2 | MOVEFLAG_CAN_FLY);
                float x, y, z;
                Madrigosa->GetPosition(x, y, z);
                Madrigosa->GetMotionMaster()->MovePoint(1, x, y, z+10);
                IntroFrostBoltTimer = 3000;
                IntroPhaseTimer = 28000;
                ++IntroPhase;
                break;
            }
            case 4:
                DoScriptText(YELL_INTRO_BREAK_ICE, m_creature);
                IntroPhaseTimer = 6000;
                ++IntroPhase;
                break;
            case 5:
                Madrigosa->CastSpell(m_creature, SPELL_INTRO_ENCAPSULATE_CHANELLING, false);
                DoScriptText(YELL_MADR_TRAP, Madrigosa);
                DoCast(m_creature, SPELL_INTRO_ENCAPSULATE);
                IntroPhaseTimer = 11000;
                ++IntroPhase;
                break;
            case 6:
                m_creature->SetSpeed(MOVE_RUN, 4.0f, true);
                DoScriptText(YELL_INTRO_CHARGE, m_creature);
                IntroPhaseTimer = 3000;
                ++IntroPhase;
                break;
            case 7:
                m_creature->DealDamage(Madrigosa, Madrigosa->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
                DoScriptText(YELL_MADR_DEATH, Madrigosa);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->AttackStop();
                m_creature->SetSpeed(MOVE_RUN, 1.0f, true);
                IntroPhaseTimer = 3000;
                ++IntroPhase;
                break;
            case 8:
                DoScriptText(YELL_INTRO_KILL_MADRIGOSA, m_creature);
                m_creature->SetOrientation(0.14);
                ((Creature*)Madrigosa)->SetCorpseDelay(51000000);
                Madrigosa->setDeathState(JUST_DIED);
                IntroPhaseTimer = 6000;
                ++IntroPhase;
                break;
            case 9:
                DoScriptText(YELL_INTRO_TAUNT, m_creature);
                IntroPhaseTimer = 5000;
                ++IntroPhase;
                break;
            case 10:
                EndIntro();
                break;
        }
    }

    void MoveInLineOfSight(Unit *who){
        if(!IsIntro && !IntroDone && who->GetTypeId() == TYPEID_PLAYER && !me->IsFriendlyTo(who) && me->IsWithinDist(who, 40))
        {
            StartIntro();
        }
        
        if(IntroDone)
            CreatureAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if(IsIntro)
        {
            if(IntroPhaseTimer < diff)
                DoIntro();
            else
                IntroPhaseTimer -= diff;

            if(IntroPhase == 4)
            {
                if(IntroFrostBoltTimer < diff)
                {
                    Unit *Madrigosa = me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA));
                    if(Madrigosa)
                    {
                        Madrigosa->CastSpell(m_creature, SPELL_INTRO_FROSTBOLT, false);
                        IntroFrostBoltTimer = 2000;
                    }
                }else
                    IntroFrostBoltTimer -= diff;
            }
            if(IntroPhase >= 2)
                DoMeleeAttackIfReady();
        }

        if(IsIntro || !UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            DoZoneInCombat();
            m_creature->SetSpeed(MOVE_RUN,2);
            CheckTimer = 1000;
        } else
            CheckTimer -= diff;

        if(SlashTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_METEOR_SLASH);
            SlashTimer = 11000;
        }else
            SlashTimer -= diff;

        if(StompTimer < diff)
        {
            AddSpellToCastWithScriptText(m_creature->getVictim(), SPELL_STOMP, RAND(YELL_LOVE1, YELL_LOVE2, YELL_LOVE3));
            StompTimer = 30000;
        }else
            StompTimer -= diff;

        if(BurnTimer < diff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                AddSpellToCast(target, SPELL_BURN);
            BurnTimer = 60000;
        }else 
            BurnTimer -= diff;

        if(BerserkTimer < diff && !Enraged)
        {
            AddSpellToCastWithScriptText(m_creature, SPELL_BERSERK, YELL_BERSERK);
            Enraged = true;
        }else
            BerserkTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL npc_death_cloudAI : public ScriptedAI
{
    npc_death_cloudAI(Creature *c) : ScriptedAI(c){
        pInstance = (c->GetInstanceData());
    }
    ScriptedInstance* pInstance;
    uint32 SummonTimer;
    
    void Reset() 
    {
        SummonTimer = 1000;
    }

    void JustRespawned()
    {
        me->CastSpell(me, SPELL_DEATH_CLOUD, true);
        SummonTimer = 1000;
    }

    void UpdateAI(const uint32 diff) 
    {
        if(SummonTimer < diff)
        {
            float x, y, z;
            if(Unit *Madrigosa= me->GetUnit(pInstance->GetData64(DATA_MADRIGOSA)))
            {
                if(!me->IsWithinDist(Madrigosa, 5))
                {      
                    z = me->GetPositionZ();
                    me->GetNearPoint2D(x, y, 5, me->GetAngle(Madrigosa));
                    me->UpdateAllowedPositionZ(x, y, z);
                    me->SummonCreature(MOB_DEATH_CLOUD, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
                } 
                else
                {
                    Unit *Felmyst= me->GetUnit(pInstance->GetData64(DATA_FELMYST));
                    if(Felmyst && Felmyst->GetTypeId() == TYPEID_UNIT)
                    {
                        Madrigosa->GetPosition(x, y, z);
                        ((Creature*)Felmyst)->Respawn();
                        Felmyst->Relocate(x, y, z);
                        Felmyst->RemoveUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
                        Felmyst->GetMotionMaster()->MoveRandom(1);
                        //((Creature*)Felmyst)->AI()->EnterEvadeMode();
                    }
                    ((Creature*)Madrigosa)->RemoveCorpse();
                }
            }
            //me->Kill(me, false);
            //me->RemoveCorpse();
            SummonTimer = 120000000;
        } else
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
