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
SDName: Boss_Thaddius
SD%Complete: 15
SDComment: ball lightning NYI, tesla coils not spawned, F&S not scripted correctly, some issues with reset, had to comment some part of the script
Don't read it, it's way too ugly... Script is not registered in ScriptLoader, don't do it, until 100% done
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025

#define STAL_TESLA_PASSIVE      28097
#define SPELL_POWERSURGE        28134

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028

#define FEUG_TESLA_PASSIVE      28109
#define SPELL_MANABURN          28135

//both
#define SPELL_WARSTOMP          28125
#define SPELL_MAGNETIC_PULL     28337    //way too strong, any ideas? maybe 28338

//Thaddus
#define SAY_GREET               -1533029
#define SAY_AGGRO1              -1533030
#define SAY_AGGRO2              -1533031
#define SAY_AGGRO3              -1533032
#define SAY_SLAY                -1533033
#define SAY_ELECT               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039
//8873-8876 sounds

#define SPELL_SELF_STUN                     28160            //Thaddius is stunned, while Feugen and Stalagg are alive
#define SPELL_BALL_LIGHTNING                28299

#define SPELL_CHARGE_POSITIVE_DMGBUFF       29659
#define SPELL_CHARGE_POSITIVE_NEARDMG       28059

#define SPELL_CHARGE_NEGATIVE_DMGBUFF       29660
#define SPELL_CHARGE_NEGATIVE_NEARDMG       28084

#define SPELL_CHAIN_LIGHTNING               28167

#define SPELL_BERSERK                       26662

 //generic
#define C_TESLA_COIL                        16218           //the coils (emotes "Tesla Coil overloads!")
#define GO_TESLA_COIL1                      181477
#define GO_TESLA_COIL2                      181478            //those 2 are not spawned

struct TRINITY_DLL_DECL boss_thaddiusAI : public ScriptedAI
{
    boss_thaddiusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance * pInstance;
    uint32 Enrage_Timer;
    uint32 Scream_Timer;	//random screams in Naxx while Thaddius is alive
    uint32 PolarityShift_Timer;
    uint32 Ball_Timer;
    uint32 Chain_Timer;
    uint32 Free_Timer;	//breaking free after killing F&S

    bool stunned;
    bool yelled;

    void Reset()
    {
        //what an ugly script! But it's working						
        //me->GetCreature(pInstance->GetData64(2))->Respawn();
        //me->GetCreature(pInstance->GetData64(2))->AI()->EnterEvadeMode();
        //me->GetCreature(pInstance->GetData64(3))->Respawn();
        //me->GetCreature(pInstance->GetData64(3))->AI()->EnterEvadeMode();
		//UP - Crash mode off
        Scream_Timer = 10000;
        PolarityShift_Timer = 30000;                        //30 secs
        Ball_Timer = 3000;                                  //only when there is noone in melee range
        Chain_Timer = 15000;                                //10-20 secs, could be wrong
        Enrage_Timer = 300000;                              //5 mins  
        Free_Timer = 20000;                                 //20 secs
        stunned = true;
        yelled = false;
        if (pInstance)
            pInstance->SetData(DATA_THADDIUS, NOT_STARTED);
    }

    void KilledUnit(Unit* Victim)
    {
        if (rand()%2)
            return;  
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (pInstance)
            pInstance->SetData(DATA_THADDIUS, DONE);
    }

    void EnterCombat(Unit *who)
    {
        /*if (stunned)
		{
        me->CastSpell(me, 28160, true);//selfstun visual, placed here to avoid visual bugs
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoScriptText(SAY_GREET, m_creature);
		}*/
        if (pInstance)
            pInstance->SetData(DATA_THADDIUS, IN_PROGRESS);
        //temp
		//me->RemoveAurasDueToSpell(28160);
        //me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {	
            return;
        }

		/*if (!me->GetCreature(pInstance->GetData64(2))->isAlive() && !me->GetCreature(pInstance->GetData64(3))->isAlive() && Free_Timer>=0)
			Free_Timer-=diff;

		if (Free_Timer<=0)
		{
			stunned = false;
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->RemoveAurasDueToSpell(28160);
		}*/

		//if (!stunned)
		//{
			if  (!yelled)
			{
				DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), m_creature);
				yelled=true;
			}
			if (Enrage_Timer < diff)
			{
				DoCast(m_creature, SPELL_BERSERK);
				Enrage_Timer = 300000;
			}
			else Enrage_Timer -= diff;

			if (Chain_Timer < diff)
			{
				if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, SPELL_CHAIN_LIGHTNING);
					Chain_Timer = urand(10000, 20000);
				}
			}
			else Chain_Timer -= diff;

			if (PolarityShift_Timer < diff)
			{
				DoCast(m_creature, 28089);
				PolarityShift_Timer = 30000;
			}
			else PolarityShift_Timer -= diff;
		//}
		
        //Zonewide scream, while he is still alive
        if (Scream_Timer < diff)
            {
                Map *map = m_creature->GetMap();
                if(!map->IsDungeon()) return;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* i_pl = i->getSource())
                    {
                        //Play random sound to the zone
                        i_pl->SendPlaySound(8873 + irand(0,3), true);
                    }
                }

                //One random scream every 90 - 300 seconds
                Scream_Timer = 90000 + (rand()% 210000);
                //shame, it only works, while he is in combat, when it should work as long as he is alive
            }
		else Scream_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL boss_feugenAI : public ScriptedAI
{
    boss_feugenAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance * pInstance;
    uint32 Switch_Timer;
    uint32 Rez_Timer;
    uint32 StalaggGUID;
    uint32 ThaddiusGUID;

    void Reset()
    {
        Rez_Timer = 5000;
        Switch_Timer = 20000;
        StalaggGUID=pInstance->GetData64(2);
        ThaddiusGUID=pInstance->GetData64(1);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_FEUG_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_FEUG_DEATH, m_creature);
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(SAY_FEUG_AGGRO, m_creature);
        DoCast(m_creature, FEUG_TESLA_PASSIVE);
        //crash prevent
        if (m_creature->GetCreature(pInstance->GetData64(2)))
        m_creature->GetCreature(pInstance->GetData64(2))->AI()->AttackStart(m_creature->getVictim());
        //m_creature->GetCreature(pInstance->GetData64(1))->AI()->AttackStart(m_creature->getVictim());
        if (pInstance)
            pInstance->SetData(DATA_THADDIUS, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        if (Switch_Timer < diff)
        {			
            //m_creature->getVictim()->KnockBackFrom(m_creature->getVictim(), -25,25);
            Switch_Timer = 20000;
        }
        else Switch_Timer -= diff;
		
		
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL boss_stalaggAI : public ScriptedAI
{
    boss_stalaggAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }
    ScriptedInstance * pInstance;
    uint32 Switch_Timer;
    uint32 Rez_Timer;

    void Reset()
    {
		Rez_Timer = 5000;
		Switch_Timer = 20000;
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_STAL_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_STAL_DEATH, m_creature);
    }

    void EnterCombat(Unit *who)
    {
        DoCast(m_creature, STAL_TESLA_PASSIVE);
        DoScriptText(SAY_STAL_AGGRO, m_creature);
        //ugly part to prevent crash
        if (m_creature->GetCreature(pInstance->GetData64(3)))
        m_creature->GetCreature(pInstance->GetData64(3))->AI()->AttackStart(m_creature->getVictim());
        if (m_creature->GetCreature(pInstance->GetData64(1)))
        m_creature->GetCreature(pInstance->GetData64(1))->AI()->AttackStart(m_creature->getVictim());
        if (pInstance)
            pInstance->SetData(DATA_THADDIUS, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        if (Switch_Timer < diff)
        {
            //m_creature->getVictim()->CastCustomSpell(m_creature->getVictim(), 28337, 50,0,0,true);
            //m_creature->getVictim()->KnockBackFrom(m_caster,-dist,30);
            Switch_Timer = 20000;
        }
        else Switch_Timer -= diff;
		
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thaddius(Creature *_Creature)
{
    return new boss_thaddiusAI (_Creature);
}
CreatureAI* GetAI_boss_feugen(Creature *_Creature)
{
    return new boss_feugenAI (_Creature);
}
CreatureAI* GetAI_boss_stalagg(Creature *_Creature)
{
    return new boss_stalaggAI (_Creature);
}
void AddSC_boss_thaddius()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_thaddius";
    newscript->GetAI = &GetAI_boss_thaddius;
    newscript->RegisterSelf();
}
void AddSC_boss_feugen()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_fugen";
    newscript->GetAI = &GetAI_boss_feugen;
    newscript->RegisterSelf();
}
void AddSC_boss_stalagg()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_stalagg";
    newscript->GetAI = &GetAI_boss_stalagg;
    newscript->RegisterSelf();
}