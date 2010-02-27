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
SDName: Boss_Void_Reaver
SD%Complete: 90
SDComment: Should reset if raid are out of room.
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "def_the_eye.h"

#define SAY_AGGRO                   -1550000
#define SAY_SLAY1                   -1550001
#define SAY_SLAY2                   -1550002
#define SAY_SLAY3                   -1550003
#define SAY_DEATH                   -1550004
#define SAY_POUNDING1               -1550005
#define SAY_POUNDING2               -1550006

#define SPELL_POUNDING              34162
#define SPELL_ARCANE_ORB            34172
#define SPELL_KNOCK_AWAY            25778
#define SPELL_BERSERK               27680
#define TRIGGER                     29530 

struct TRINITY_DLL_DECL boss_void_reaverAI : public ScriptedAI
{
    boss_void_reaverAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_creature->GetPosition(wLoc);
    }

    ScriptedInstance* pInstance;

    uint32 Pounding_Timer;
    uint32 ArcaneOrb_Timer;
    uint32 KnockAway_Timer;
    uint32 Berserk_Timer;
    uint32 Check_Timer;

    WorldLocation wLoc;

    bool Enraged;

    void Reset()
    {
        Pounding_Timer = 15000;
        ArcaneOrb_Timer = 3000;
        KnockAway_Timer = 30000;
        Berserk_Timer = 600000;
        Check_Timer = 3000;
        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
        m_creature->ApplySpellImmune(2, IMMUNITY_EFFECT, SPELL_EFFECT_HEALTH_LEECH, true);
        m_creature->ApplySpellImmune(3, IMMUNITY_STATE, SPELL_AURA_PERIODIC_LEECH, true);
        m_creature->ApplySpellImmune(4, IMMUNITY_STATE, SPELL_AURA_PERIODIC_MANA_LEECH, true);

        if (pInstance && m_creature->isAlive())
            pInstance->SetData(DATA_VOIDREAVEREVENT, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
            pInstance->SetData(DATA_VOIDREAVEREVENT, DONE);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoZoneInCombat();
        
        if(pInstance)
            pInstance->SetData(DATA_VOIDREAVEREVENT, IN_PROGRESS);
    }
    
    void SpellHit(Unit* caster,const SpellEntry* spell)
    {
        for(uint8 i = 0; i<3; i++)
            if(spell->Effect[i] == SPELL_EFFECT_INTERRUPT_CAST)
                return;
    }
    
    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() )
            return;

        //Check_Timer
        if(Check_Timer < diff)
        {
            if(m_creature->GetDistance(wLoc.x,wLoc.y,wLoc.z) > 135.0f)
                EnterEvadeMode();
            else
                DoZoneInCombat();
            
            Check_Timer = 3000;
        }else Check_Timer -= diff;

        // Pounding
        if(Pounding_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_POUNDING);

            switch(rand()%2)
            {
                case 0: DoScriptText(SAY_POUNDING1, m_creature); break;
                case 1: DoScriptText(SAY_POUNDING2, m_creature); break;
            }

            if(KnockAway_Timer < 3100)
                KnockAway_Timer = 3100;

            Pounding_Timer = 15000;                         //cast time(3000) + cooldown time(12000)
        }else Pounding_Timer -= diff;

        // Arcane Orb
        if(ArcaneOrb_Timer < diff)
        {
            Unit *target = NULL;
            std::list<HostilReference *> t_list = m_creature->getThreatManager().getThreatList();
            std::vector<Unit *> target_list;
            for(std::list<HostilReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                //18 yard radius minimum
                if(target && target->GetTypeId() == TYPEID_PLAYER && target->isAlive() && target->GetDistance2d(m_creature) >= 18)
                    target_list.push_back(target);
                target = NULL;
            }
            if(target_list.size())
                target = *(target_list.begin()+rand()%target_list.size());
            else
                target = m_creature->getVictim();

            if(target)
              if(Creature* t = DoSpawnCreature(TRIGGER, 0, 0, 10, 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
                 t->CastSpell(target, SPELL_ARCANE_ORB, false, 0, 0, m_creature->GetGUID());


            ArcaneOrb_Timer = 3000 + rand()%1001;
        }else ArcaneOrb_Timer -= diff;

        // Single Target knock back, reduces aggro
        if(KnockAway_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_KNOCK_AWAY);

            Unit *target = NULL;
            target = SelectUnit(SELECT_TARGET_TOPAGGRO,0);
            if(DoGetThreat(target))
                DoModifyThreatPercent(target,-25);
            
            target = SelectUnit(SELECT_TARGET_TOPAGGRO,0);
            if(target)
                m_creature->Attack(target,true);

            KnockAway_Timer = 30000;
        }else KnockAway_Timer -= diff;

        //Berserk
        if(Berserk_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature,SPELL_BERSERK);
            Berserk_Timer = 600000;
        }else Berserk_Timer -= diff;

        m_creature->RemoveAurasWithDispelType(DISPEL_POISON);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_void_reaver(Creature *_Creature)
{
    return new boss_void_reaverAI (_Creature);
}

void AddSC_boss_void_reaver()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_void_reaver";
    newscript->GetAI = &GetAI_boss_void_reaver;
    newscript->RegisterSelf();
}
