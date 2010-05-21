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
SDName: Boss_Archimonde
SD%Complete: 85
SDComment: Doomfires not completely offlike due to core limitations for random moving. Tyrande and second phase not fully implemented.
SDCategory: Caverns of Time, Mount Hyjal
EndScriptData */

#include "precompiled.h"
#include "def_hyjal.h"
#include "SpellAuras.h"
#include "hyjal_trash.h"

//text id -1534018 are the text used when previous events complete. Not part of this script.
#define SAY_AGGRO                   -1534019
#define SAY_DOOMFIRE1               -1534020
#define SAY_DOOMFIRE2               -1534021
#define SAY_AIR_BURST1              -1534022
#define SAY_AIR_BURST2              -1534023
#define SAY_SLAY1                   -1534024
#define SAY_SLAY2                   -1534025
#define SAY_SLAY3                   -1534026
#define SAY_ENRAGE                  -1534027
#define SAY_DEATH                   -1534028
#define SAY_SOUL_CHARGE1            -1534029
#define SAY_SOUL_CHARGE2            -1534030

#define SPELL_DENOUEMENT_WISP      32124
#define SPELL_ANCIENT_SPARK        39349
#define SPELL_PROTECTION_OF_ELUNE  38528

#define SPELL_DRAIN_WORLD_TREE      39140
#define SPELL_DRAIN_WORLD_TREE_2    39141

#define SPELL_FINGER_OF_DEATH       31984
#define SPELL_HAND_OF_DEATH         35354
#define SPELL_AIR_BURST             32014
#define SPELL_GRIP_OF_THE_LEGION    31972
#define SPELL_DOOMFIRE_STRIKE       31903                   //summons two creatures - not working properly, not used
#define SPELL_DOOMFIRE_SPAWN        32074                   //visual
#define SPELL_DOOMFIRE              31945
#define SPELL_DOOMFIRE_DAMAGE       31944                   //triggered by doomfire persistent aura
#define SPELL_UNLEASH_SOUL_YELLOW   32054
#define SPELL_UNLEASH_SOUL_GREEN    32057
#define SPELL_UNLEASH_SOUL_RED      32053
#define SPELL_FEAR                  31970

#define CREATURE_ARCHIMONDE             17968
#define CREATURE_DOOMFIRE               18095
#define CREATURE_DOOMFIRE_TARGETING     18104
#define CREATURE_ANCIENT_WISP           17946
#define CREATURE_CHANNEL_TARGET         22418

#define NORDRASSIL_X        5503.713
#define NORDRASSIL_Y       -3523.436
#define NORDRASSIL_Z        1608.781

struct mob_ancient_wispAI : public ScriptedAI
{
    mob_ancient_wispAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;
    uint32 CheckTimer;

    void Reset()
    {
        CheckTimer = 1000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* who) {}

    void DamageTaken(Unit* done_by, uint32 &damage) { damage = 0; }

    void UpdateAI(const uint32 diff)
    {
        if(CheckTimer < diff)
        {
            if(pInstance)
            {
                Unit* Archimonde = Unit::GetUnit((*m_creature), pInstance->GetData64(DATA_ARCHIMONDE));
                if(Archimonde)
                {
                    if((((Archimonde->GetHealth()*100) / Archimonde->GetMaxHealth()) < 2) || !Archimonde->isAlive())
                        //AddSpellToCast(m_creature, SPELL_DENOUEMENT_WISP);
                        DoCast(m_creature, SPELL_DENOUEMENT_WISP);
                    else
                        //AddSpellToCast(Archimonde, SPELL_ANCIENT_SPARK);
                        DoCast(Archimonde, SPELL_ANCIENT_SPARK);
                }
            }
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;
    }
};

/* This is the script for the Doomfire Targetting Mob. This mob simply follows players and/or travels in random directions and spawns Doomfire Persistent Area Aura which deals dmg to players in range.  */
struct TRINITY_DLL_DECL mob_doomfire_targettingAI : public ScriptedAI
{
    mob_doomfire_targettingAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance *pInstance;

    uint32 ChangeTargetTimer;
    uint32 SummonTimer;                                     // This timer will serve to check on Archionde


    void Reset()
    {
        ChangeTargetTimer = 5000;
        SummonTimer = 1000;
        DoCast(m_creature, SPELL_DOOMFIRE);
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(SummonTimer < diff)
        {
            if(pInstance)
            {
                Unit* Archimonde = Unit::GetUnit((*m_creature), pInstance->GetData64(DATA_ARCHIMONDE));
                if(Archimonde && Archimonde->isAlive())
                    SummonTimer = 1000;
                else
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        else
            SummonTimer -= diff;

        if(ChangeTargetTimer < diff)
        {
            DoZoneInCombat();
            m_creature->SetSpeed(MOVE_RUN, 0.5);
            m_creature->SetSpeed(MOVE_WALK, 0.5);
            Unit* target = NULL;
            switch(rand()%2)    //!!TODO: force Doomfire to ONLY "walk" on the ground; the problem is when chasing players that have air burst, also looking for random Z position
            {
                case 0:                                     // stalk player
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 1, 200, true, m_creature->getVictim()))
                    {
                        m_creature->AddThreat(target, DoGetThreat(m_creature->getVictim()));
                        m_creature->GetMotionMaster()->MoveChase(target);
                    }
                    break;

                case 1:                                     // random location
                    float x = 0;
                    float y = 0;
                    float z = 0;
                    m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 40, x, y, z);
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    break;
            }
            ChangeTargetTimer = 5000;
        }
        else
            ChangeTargetTimer -= diff;
    }

};

/* Finally, Archimonde's script. His script isn't extremely complex, most are simply spells on timers.
   The only complicated aspect of the battle is Finger of Death and Doomfire, with Doomfire being the
   hardest bit to code. Finger of Death is simply a distance check - if no one is in melee range, then
   select a random target and cast the spell on them. However, if someone IS in melee range, and this
   is NOT the main tank (creature's victim), then we aggro that player and they become the new victim.
   For Doomfire, we summon a mob (Doomfire Targetting) that summons another mob (Doomfire every second)
   Doomfire Targetting 'stalks' players whilst Doomfire damages player that are within range. */

// This is used to sort by distance in order to see who is the closest target, when checking for Finger of Death
struct TargetDistanceOrder : public std::binary_function<const Unit, const Unit, bool>
{
    const Unit* MainTarget;
    TargetDistanceOrder(const Unit* Target) : MainTarget(Target) {};
    // functor for operator "<"
    bool operator()(const Unit* _Left, const Unit* _Right) const
    {
        return (MainTarget->GetDistance(_Left) < MainTarget->GetDistance(_Right));
    }
};

struct TRINITY_DLL_DECL boss_archimondeAI : public hyjal_trashAI
{
    boss_archimondeAI(Creature *c) : hyjal_trashAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint32 DrainNordrassilTimer;
    uint32 FearTimer;
    uint32 AirBurstTimer;
    uint32 GripOfTheLegionTimer;
    uint32 DoomfireTimer;
    uint32 SoulChargeTimer;
    uint32 SoulChargeCount;
    uint32 SoulChargeUnleashTimer;
    uint32 MeleeRangeCheckTimer;
    uint32 HandOfDeathTimer;
    uint32 SummonWispTimer;
    uint32 WispCount;
    uint32 EnrageTimer;
    uint32 CheckDistanceTimer;
    uint32 CheckTimer;

    uint32 chargeSpell;
    uint32 unleashSpell;

    bool Enraged;
    bool BelowTenPercent;
    bool HasProtected;
    bool IsChanneling;
    bool SoulChargeUnleash;

    void Reset()
    {
        if(pInstance)
            pInstance->SetData(DATA_ARCHIMONDEEVENT, NOT_STARTED);

        damageTaken = 0;
        DrainNordrassilTimer = 0;
        FearTimer = 42000;
        AirBurstTimer = 30000;
        GripOfTheLegionTimer = 5000 + rand()%20000;
        DoomfireTimer = 20000;
        SoulChargeTimer = 2000 + rand()%27000;
        SoulChargeCount = 0;
        SoulChargeUnleashTimer = 0;
        MeleeRangeCheckTimer = 15000;
        HandOfDeathTimer = 2000;
        WispCount = 0;                                      // When ~30 wisps are summoned, Archimonde dies
        EnrageTimer = 600000;                               // 10 minutes
        CheckTimer = 3000;
        CheckDistanceTimer = 30000;                         // This checks if he's too close to the World Tree (75 yards from a point on the tree), if true then he will enrage
        SummonWispTimer = 0;
        chargeSpell = 0;
        unleashSpell = 0;

        Enraged = false;
        BelowTenPercent = false;
        HasProtected = false;
        IsChanneling = false;
        SoulChargeUnleash = false;

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
    }

    void Aggro(Unit *who)
    {
        m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);
        DoScriptText(SAY_AGGRO, m_creature);
        DoZoneInCombat();

        if(pInstance)
            pInstance->SetData(DATA_ARCHIMONDEEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        hyjal_trashAI::JustDied(victim);
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
            pInstance->SetData(DATA_ARCHIMONDEEVENT, DONE);
    }

    bool CanUseFingerOfDeath()
    {
        if(!m_creature->getVictim())
            return true;

        if(m_creature->IsWithinDistInMap(m_creature->getVictim(), 5.0))
            return false;
            
        if(Unit *target = m_creature->SelectNearestTarget(m_creature->GetAttackDistance(m_creature->getVictim())))
        {
            m_creature->AddThreat(target, DoGetThreat(m_creature->getVictim()));
            return false;
        }
        
        return true;
    }

    void SummonDoomfire(Unit* target)
    {
        if(Creature* Doomfire = DoSpawnCreature(CREATURE_DOOMFIRE_TARGETING, rand()%30, rand()%30, 1, 0, TEMPSUMMON_TIMED_DESPAWN, 60000))
        {
            Doomfire->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            Doomfire->setFaction(m_creature->getFaction());

            ForceSpellCast(Doomfire, SPELL_DOOMFIRE_SPAWN);

            if(target)
                Doomfire->AI()->AttackStart(target);

            if(rand()%2 == 0)
                DoScriptText(SAY_DOOMFIRE1, m_creature);
            else
                DoScriptText(SAY_DOOMFIRE2, m_creature);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!InCombat)
        {
            if(pInstance)
            {
                // Do not let the raid skip straight to Archimonde. Visible and hostile ONLY if Azagalor is finished.
                if((pInstance->GetData(DATA_AZGALOREVENT) < DONE) && ((m_creature->GetVisibility() != VISIBILITY_OFF) || (m_creature->getFaction() != 35)))
                {
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    m_creature->setFaction(35);
                }
                else if((pInstance->GetData(DATA_AZGALOREVENT) >= DONE) && ((m_creature->GetVisibility() != VISIBILITY_ON) || (m_creature->getFaction() == 35)))
                {
                    m_creature->setFaction(1720);
                    m_creature->SetVisibility(VISIBILITY_ON);
                }
            }

            if(DrainNordrassilTimer < diff)
            {
                if(!IsChanneling)
                {
                    Creature* Nordrassil = m_creature->SummonCreature(CREATURE_CHANNEL_TARGET, NORDRASSIL_X, NORDRASSIL_Y, NORDRASSIL_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 1200000);
                    if(Nordrassil)
                    {
                        Nordrassil->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Nordrassil->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                        //AddSpellToCast(Nordrassil, SPELL_DRAIN_WORLD_TREE);
                        DoCast(Nordrassil, SPELL_DRAIN_WORLD_TREE);
                        IsChanneling = true;
                    }
                }
                Creature* Nordrassil = m_creature->SummonCreature(CREATURE_CHANNEL_TARGET, NORDRASSIL_X, NORDRASSIL_Y, NORDRASSIL_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);
                if(Nordrassil)
                {
                    Nordrassil->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Nordrassil->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                    Nordrassil->CastSpell(m_creature, SPELL_DRAIN_WORLD_TREE_2, true);
                    DrainNordrassilTimer = 1000;
                }
            }
            else
                DrainNordrassilTimer -= diff;
        }

        if(!UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            DoZoneInCombat();

            if(m_creature->GetUInt64Value(UNIT_FIELD_TARGET) != m_creature->getVictim()->GetGUID())
                m_creature->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->getVictim()->GetGUID());

            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if(((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 10) && !BelowTenPercent && !Enraged)
            BelowTenPercent = true;

        if(!Enraged)
        {
            if(EnrageTimer < diff)
            {
                if((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) > 10)
                {
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    Enraged = true;
                    DoScriptText(SAY_ENRAGE, m_creature);
                }
            }
            else
                EnrageTimer -= diff;

            if(CheckDistanceTimer < diff)
            {
                // To simplify the check, we simply summon a creature in the location and then check how far we are from the creature
                Creature* Check = m_creature->SummonCreature(CREATURE_CHANNEL_TARGET, NORDRASSIL_X, NORDRASSIL_Y, NORDRASSIL_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 2000);
                if(Check)
                {
                    Check->SetVisibility(VISIBILITY_OFF);
                    if(m_creature->IsWithinDistInMap(Check, 100.0))
                    {
                        m_creature->GetMotionMaster()->Clear(false);
                        m_creature->GetMotionMaster()->MoveIdle();
                        Enraged = true;
                        DoScriptText(SAY_ENRAGE, m_creature);
                    }
                }
                CheckDistanceTimer = 5000;
            }
            else
                CheckDistanceTimer -= diff;
        }

        if(BelowTenPercent)
        {
            if(!HasProtected)
            {
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                //all members of raid must get this buff
                ForceSpellCast(m_creature->getVictim(), SPELL_PROTECTION_OF_ELUNE, INTERRUPT_AND_CAST_INSTANTLY);
                HasProtected = true;
                Enraged = true;
            }

            if(SummonWispTimer < diff)
            {
                Creature* Wisp = DoSpawnCreature(CREATURE_ANCIENT_WISP, rand()%40, rand()%40, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if(Wisp)
                    Wisp->AI()->AttackStart(m_creature);

                SummonWispTimer = 1500;
                ++WispCount;
            }
            else
                SummonWispTimer -= diff;

            if(WispCount >= 30)
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        if(Enraged)
        {
            if(HandOfDeathTimer < diff)
            {
                ForceSpellCast(m_creature, SPELL_HAND_OF_DEATH, INTERRUPT_AND_CAST_INSTANTLY);
                HandOfDeathTimer = 2000;
            }
            else
                HandOfDeathTimer -= diff;

            return;                                         // Don't do anything after this point.
        }

        if(SoulChargeTimer < diff)
        {
            if(SoulChargeUnleash)
            {
                if(SoulChargeUnleashTimer < diff)
                {
                    while(m_creature->HasAura(chargeSpell, 0))
                    {
                        SoulChargeCount++;
                        m_creature->RemoveSingleAuraFromStack(chargeSpell, 0);
                    }

                    if(SoulChargeCount)
                    {
                        SoulChargeCount--;
                        DoCast(m_creature->getVictim(), unleashSpell);
                        //AddSpellToCast(m_creature->getVictim(), unleashSpell);
                        SoulChargeTimer = 1000;
                    }
                    else
                    {
                        SoulChargeUnleash = false;
                        SoulChargeTimer = 4000;
                    }                    
                }
                else
                    SoulChargeUnleashTimer -= diff;
            }
            
            if(!SoulChargeUnleash)
            {
                if(m_creature->HasAura(SPELL_SOUL_CHARGE_YELLOW, 0))
                {
                    SoulChargeUnleash = true;
                    chargeSpell = SPELL_SOUL_CHARGE_YELLOW;
                    unleashSpell = SPELL_UNLEASH_SOUL_YELLOW;
                }
                else if(m_creature->HasAura(SPELL_SOUL_CHARGE_RED, 0))
                {
                    SoulChargeUnleash = true;
                    chargeSpell = SPELL_SOUL_CHARGE_RED;
                    unleashSpell = SPELL_UNLEASH_SOUL_RED;
                }
                else if(m_creature->HasAura(SPELL_SOUL_CHARGE_GREEN, 0))
                {
                    SoulChargeUnleash = true;
                    chargeSpell = SPELL_SOUL_CHARGE_GREEN;
                    unleashSpell = SPELL_UNLEASH_SOUL_GREEN;
                }
            }

            if(!SoulChargeUnleash)
                SoulChargeTimer = 2000;
            else
                SoulChargeUnleashTimer = rand()%5000+5000;
        }
        else
            SoulChargeTimer -= diff;

        if(GripOfTheLegionTimer < diff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
                //AddSpellToCast(target, SPELL_GRIP_OF_THE_LEGION);
                DoCast(target, SPELL_GRIP_OF_THE_LEGION);
            
            if(AirBurstTimer < 3000)
                AirBurstTimer = 3000;

            GripOfTheLegionTimer = 5000 + rand()%20000;
        }
        else
            GripOfTheLegionTimer -= diff;

        if(AirBurstTimer < diff)
        {
            if(rand()%2 == 0)
                DoScriptText(SAY_AIR_BURST1, m_creature);
            else
                DoScriptText(SAY_AIR_BURST2, m_creature);

            Unit *target = NULL;
            // aby miec pewnosc, ze naszym targetem nie jest tank
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 1, 100, true, m_creature->getVictim()))
            {
                // ustawia target jako aktualny
                m_creature->SetUInt64Value(UNIT_FIELD_TARGET, target->GetGUID());
                DoCast(target, SPELL_AIR_BURST);
            }

            if(FearTimer < 10000)
                FearTimer += 10000;

            AirBurstTimer = 25000 + rand()%15000;
        }
        else
            AirBurstTimer -= diff;

        if(FearTimer < diff)
        {
            //AddSpellToCast(m_creature->getVictim(), SPELL_FEAR);
            DoCast(m_creature->getVictim(), SPELL_FEAR);
            FearTimer = 42000;
        }
        else
            FearTimer -= diff;

        if(DoomfireTimer < diff)
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 1, 150, true, m_creature->getVictim()))
                SummonDoomfire(target);
            else if (Unit* target = m_creature->getVictim()) // only for testing purposes
                SummonDoomfire(target);

            DoomfireTimer = 9000+rand()%3000;
        }
        else
            DoomfireTimer -= diff;

        if(MeleeRangeCheckTimer < diff)
        {
            if(CanUseFingerOfDeath())
            {
                if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0, 150, true))
                    //AddSpellToCast(target, SPELL_FINGER_OF_DEATH);
                    DoCast(target, SPELL_FINGER_OF_DEATH);

                MeleeRangeCheckTimer = 1000;
            }
            else
                MeleeRangeCheckTimer = 5000;
        }
        else
            MeleeRangeCheckTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
    void WaypointReached(uint32 i){}
};

CreatureAI* GetAI_boss_archimonde(Creature *_Creature)
{
    return new boss_archimondeAI (_Creature);
}

CreatureAI* GetAI_mob_doomfire_targetting(Creature* _Creature)
{
    return new mob_doomfire_targettingAI(_Creature);
}

CreatureAI* GetAI_mob_ancient_wisp(Creature* _Creature)
{
    return new mob_ancient_wispAI(_Creature);
}

void AddSC_boss_archimonde()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_archimonde";
    newscript->GetAI = &GetAI_boss_archimonde;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_doomfire_targetting";
    newscript->GetAI = &GetAI_mob_doomfire_targetting;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ancient_wisp";
    newscript->GetAI = &GetAI_mob_ancient_wisp;
    newscript->RegisterSelf();
}

