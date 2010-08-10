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
SDName: Boss_Bloodboil
SD%Complete: 80
SDComment: Bloodboil not working correctly, missing enrage
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

//Speech'n'Sound
#define SAY_AGGRO               -1564029
#define SAY_SLAY1               -1564030
#define SAY_SLAY2               -1564031
#define SAY_SPECIAL1            -1564032
#define SAY_SPECIAL2            -1564033
#define SAY_ENRAGE1             -1564034
#define SAY_ENRAGE2             -1564035
#define SAY_DEATH               -1564036

//Spells diff in p1 and p2
#define SPELL_ARCING_SMASH          (Phase1 ? 40599 : 40457)
#define SPELL_FELBREATH             (Phase1 ? 40508 : 40595)
#define SPELL_EJECT                 (Phase1 ? 40486 : 40597)

#define SPELL_ACIDIC_WOUND          40484 //Trigger Aura

// Phase1
#define SPELL_BEWILDERING_STRIKE    40491
#define SPELL_BLOODBOIL             42005

//Phase2
#define SPELL_FEL_GEYSER            40593

//Fel Rage
#define SPELL_INSIGNIFIGANCE        40618
#define SPELL_TAUNT_GURTOGG         40603
#define SPELL_FEL_RAGE_SELF         40594
#define SPELL_FEL_RAGE_1            40604
#define SPELL_FEL_RAGE_2            40616
#define SPELL_FEL_RAGE_3            41625
#define SPELL_FEL_RAGE_SCALE        46787

#define SPELL_CHARGE                40602
#define SPELL_BERSERK               45078

//This is used to sort the players by distance in preparation for the Bloodboil cast.
struct ObjectDistanceOrderReversed : public std::binary_function<const WorldObject, const WorldObject, bool>
{
    const Unit* m_pSource;
    ObjectDistanceOrderReversed(const Unit* pSource) : m_pSource(pSource) {};

    bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
    {
        return !m_pSource->GetDistanceOrder(pLeft, pRight, false);
    }
};

struct TRINITY_DLL_DECL boss_gurtogg_bloodboilAI : public ScriptedAI
{
    boss_gurtogg_bloodboilAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_creature->GetPosition(wLoc);
    }

    ScriptedInstance* pInstance;
    WorldLocation wLoc;

    uint64 TargetGUID;

    float TargetThreat;

    uint32 BloodboilTimer;

    uint32 BewilderingStrikeTimer;

    uint32 ArcingSmashTimer;
    uint32 FelBreathTimer;
    uint32 EjectTimer;

    uint32 PhaseChangeTimer;

    uint32 EnrageTimer;

    uint32 ChargeTimer;
    uint32 CheckTimer;

    bool Phase1;

    void Reset()
    {
        if(pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, NOT_STARTED);

        TargetGUID = 0;

        TargetThreat = 0;

        BloodboilTimer = 10000;

        BewilderingStrikeTimer = 15000;

        ArcingSmashTimer = 19000;
        FelBreathTimer = 25000;
        EjectTimer = 10000;

        PhaseChangeTimer = 65000;
        CheckTimer = 3000;

        EnrageTimer = 600000;

        Phase1 = true;
        ChargeTimer = 30000;

        DoCast(m_creature, SPELL_ACIDIC_WOUND, true);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();
        DoScriptText(SAY_AGGRO, m_creature);
        if(pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SLAY1, m_creature); break;
        case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        if(pInstance)
            pInstance->SetData(DATA_GURTOGGBLOODBOILEVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    // Note: This seems like a very complicated fix. The fix needs to be handled by the core, as implementation of limited-target AoE spells are still not limited.
    void CastBloodboil()
    {
        std::list<Unit *> targets;
        Map *map = m_creature->GetMap();
        if(map->IsDungeon())
        {
            InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
            for (InstanceMap::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (Player* i_pl = i->getSource())
                {
                    if(i_pl && i_pl->isAlive() && !i_pl->isGameMaster())
                        targets.push_back(i_pl);
                }
            }
        }

        if(targets.empty())
            return; 

        targets.sort(ObjectDistanceOrderReversed(m_creature));
        targets.resize(5);

        //Aura each player in the targets list with Bloodboil.
        for(std::list<Unit *>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            Unit* target = *itr;
            if(target && target->isAlive())
                ForceSpellCast(target, SPELL_BLOODBOIL, INTERRUPT_AND_CAST_INSTANTLY, true);
        }
        targets.clear();
    }

    void RevertThreatOnTarget(uint64 guid)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*m_creature), guid);
        if(pUnit)
        {
            if(DoGetThreat(pUnit))
                DoModifyThreatPercent(pUnit, -100);
            if(TargetThreat)
                m_creature->AddThreat(pUnit, TargetThreat);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CheckTimer < diff)
        {
            DoZoneInCombat();
            CheckTimer = 3000;
        }
        else 
            CheckTimer -= diff;

        if (EnrageTimer)//if(!m_creature->HasAura(SPELL_BERSERK, 0))
        {
            if(EnrageTimer <= diff)
            {
                EnrageTimer = 0;
                int text = 0;
                switch (rand()%2)
                {
                    case 0: text = SAY_ENRAGE1; break;
                    case 1: text = SAY_ENRAGE2; break;
                }
                ForceSpellCastWithScriptText(m_creature, SPELL_BERSERK, text, INTERRUPT_AND_CAST_INSTANTLY);
            }
            else
                EnrageTimer -= diff;
        }

        if (ArcingSmashTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_ARCING_SMASH);
            ArcingSmashTimer = 10000;
        }
        else
            ArcingSmashTimer -= diff;

        if (FelBreathTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_FELBREATH);
            FelBreathTimer = 25000;
        }
        else
            FelBreathTimer -= diff;

        if (EjectTimer < diff)
        {
            AddSpellToCast(m_creature->getVictim(), SPELL_EJECT);
            EjectTimer = 15000;
        }
        else
            EjectTimer -= diff;

        if (Phase1)
        {
            if (BewilderingStrikeTimer < diff)
            {
                AddSpellToCast(m_creature->getVictim(), SPELL_BEWILDERING_STRIKE);
                BewilderingStrikeTimer = 20000;
            }
            else
                BewilderingStrikeTimer -= diff;

            if (BloodboilTimer < diff)
            {
                CastBloodboil();
                BloodboilTimer = 10000;
            }
            else
                BloodboilTimer -= diff;
        }

        if (PhaseChangeTimer < diff)
        {
            if (Phase1)
            {
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true);
                if (target && target->isAlive())
                {
                    Phase1 = false;

                    TargetThreat = DoGetThreat(target);
                    TargetGUID = target->GetGUID();
                    if(DoGetThreat(target))
                        DoModifyThreatPercent(target, -100);

                    m_creature->AddThreat(target, 50000000.0f);
                    target->CastSpell(m_creature, SPELL_TAUNT_GURTOGG, true);

                    ForceSpellCast(m_creature, SPELL_INSIGNIFIGANCE, INTERRUPT_AND_CAST, true);

                    ForceSpellCast(target, SPELL_FEL_RAGE_1, INTERRUPT_AND_CAST_INSTANTLY, true);
                    ForceSpellCast(target, SPELL_FEL_RAGE_2, INTERRUPT_AND_CAST_INSTANTLY, true);
                    ForceSpellCast(target, SPELL_FEL_RAGE_3, INTERRUPT_AND_CAST_INSTANTLY, true);
                    ForceSpellCast(target, SPELL_FEL_RAGE_SCALE, INTERRUPT_AND_CAST_INSTANTLY, true);

                    //Cast this without triggered so that it appears in combat logs and shows visual.
                    ForceSpellCast(m_creature, SPELL_FEL_RAGE_SELF, INTERRUPT_AND_CAST);

                    switch(rand()%2)
                    {
                        case 0: DoScriptText(SAY_SPECIAL1, m_creature); break;
                        case 1: DoScriptText(SAY_SPECIAL2, m_creature); break;
                    }

                    ForceSpellCast(target, SPELL_CHARGE, INTERRUPT_AND_CAST);
                    ForceSpellCast(target, SPELL_FEL_GEYSER, INTERRUPT_AND_CAST);
                    PhaseChangeTimer = 30000;
                    m_creature->SetSpeed(MOVE_RUN, 3.0);
                }
            }
            else                                           // Encounter is a loop pretty much. Phase 1 -> Phase 2 -> Phase 1 -> Phase 2 till death or enrage
            {
                if (TargetGUID)
                    RevertThreatOnTarget(TargetGUID);
                TargetGUID = 0;
                Phase1 = true;
                BloodboilTimer = 10000;
                ArcingSmashTimer += 2000;
                FelBreathTimer += 2000;
                EjectTimer += 2000;
                PhaseChangeTimer = 65000;
                m_creature->SetSpeed(MOVE_RUN, 1.0);
            }
        }
        else
            PhaseChangeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gurtogg_bloodboil(Creature *_Creature)
{
    return new boss_gurtogg_bloodboilAI (_Creature);
}

void AddSC_boss_gurtogg_bloodboil()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_gurtogg_bloodboil";
    newscript->GetAI = &GetAI_boss_gurtogg_bloodboil;
    newscript->RegisterSelf();
}

