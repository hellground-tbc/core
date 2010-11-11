/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Illidari_Council
SD%Complete: 95
SDComment: Circle of Healing not working properly.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

//Speech'n'Sounds
#define SAY_GATH_SLAY           -1564085
#define SAY_GATH_SLAY_COMNT     -1564089
#define SAY_GATH_DEATH          -1564093
#define SAY_GATH_SPECIAL1       -1564077
#define SAY_GATH_SPECIAL2       -1564081

#define SAY_VERA_SLAY           -1564086
#define SAY_VERA_COMNT          -1564089
#define SAY_VERA_DEATH          -1564094
#define SAY_VERA_SPECIAL1       -1564078
#define SAY_VERA_SPECIAL2       -1564082

#define SAY_MALA_SLAY           -1564087
#define SAY_MALA_COMNT          -1564090
#define SAY_MALA_DEATH          -1564095
#define SAY_MALA_SPECIAL1       -1564079
#define SAY_MALA_SPECIAL2       -1564083

#define SAY_ZERE_SLAY           -1564088
#define SAY_ZERE_COMNT          -1564091
#define SAY_ZERE_DEATH          -1564096
#define SAY_ZERE_SPECIAL1       -1564080
#define SAY_ZERE_SPECIAL2       -1564084

#define ERROR_INST_DATA           "SD2 ERROR: Instance Data for Black Temple not set properly; Illidari Council event will not function properly."

#define AKAMAID                 23089

struct CouncilYells
{
    int32 entry;
    uint32 timer;
};

static CouncilYells CouncilAggro[]=
{
    {-1564069, 5000},                                       // Gathios
    {-1564070, 5500},                                       // Veras
    {-1564071, 5000},                                       // Malande
    {-1564072, 0},                                          // Zerevor
};

// Need to get proper timers for this later
static CouncilYells CouncilEnrage[]=
{
    {-1564073, 2000},                                       // Gathios
    {-1564074, 6000},                                       // Veras
    {-1564075, 5000},                                       // Malande
    {-1564076, 0},                                          // Zerevor
};

#define SPELL_BERSERK 45078

struct TRINITY_DLL_DECL mob_blood_elf_council_voice_triggerAI : public ScriptedAI
{
    mob_blood_elf_council_voice_triggerAI(Creature* c) : ScriptedAI(c)
    {
        for(uint8 i = 0; i < 4; ++i)
            Council[i] = 0;
    }

    uint64 Council[4];

    uint32 EnrageTimer;
    uint32 AggroYellTimer;

    uint8 YellCounter;                                      // Serves as the counter for both the aggro and enrage yells

    bool EventStarted;

    void Reset()
    {
        EnrageTimer = 900000;                               // 15 minutes
        AggroYellTimer = 500;

        YellCounter = 0;

        EventStarted = false;
    }

    // finds and stores the GUIDs for each Council member using instance data system.
    void LoadCouncilGUIDs()
    {
        if(ScriptedInstance* pInstance = ((ScriptedInstance*)m_creature->GetInstanceData()))
        {
            Council[0] = pInstance->GetData64(DATA_GATHIOSTHESHATTERER);
            Council[1] = pInstance->GetData64(DATA_VERASDARKSHADOW);
            Council[2] = pInstance->GetData64(DATA_LADYMALANDE);
            Council[3] = pInstance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
        }
        else
            error_log(ERROR_INST_DATA);
    }

    void AttackStart(Unit* who) {}

    void MoveInLineOfSight(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!EventStarted)
            return;

        if(YellCounter > 3)
            return;

        if(AggroYellTimer)
        {
            if(AggroYellTimer <= diff)
            {
                if(Unit* pMember = Unit::GetUnit(*m_creature, Council[YellCounter]))
                {
                    DoScriptText(CouncilAggro[YellCounter].entry, pMember);
                    AggroYellTimer = CouncilAggro[YellCounter].timer;
                }

                ++YellCounter;
                if(YellCounter > 3)
                    YellCounter = 0;                            // Reuse for Enrage Yells
            }
            else
                AggroYellTimer -= diff;
        }

        if(EnrageTimer)
        {
            if(EnrageTimer <= diff)
            {
                if(Unit* pMember = Unit::GetUnit(*m_creature, Council[YellCounter]))
                {
                    pMember->CastSpell(pMember, SPELL_BERSERK, true);
                    DoScriptText(CouncilEnrage[YellCounter].entry, pMember);
                    EnrageTimer = CouncilEnrage[YellCounter].timer;
                }
                ++YellCounter;
            }
            else
                EnrageTimer -= diff;
        }
    }
};

struct TRINITY_DLL_DECL mob_illidari_councilAI : public ScriptedAI
{
    mob_illidari_councilAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint64 m_council[4];

    uint32 m_checkTimer;
    uint32 EndEventTimer;

    uint8 DeathCount;

    bool EventBegun;

    void Reset()
    {
        m_checkTimer    = 2000;
        EndEventTimer = 0;

        DeathCount = 0;

        for(uint8 i = 0; i < 4; ++i)
        {
            if(Creature* pMember = pInstance->GetCreature(m_council[i]))
            {
                if(!pMember->isAlive())
                {
                    pMember->RemoveCorpse();
                    pMember->Respawn();
                }

                pMember->AI()->EnterEvadeMode();
            }
        }

        if(pInstance)
        {
            pInstance->SetData(DATA_ILLIDARICOUNCILEVENT, NOT_STARTED);
            if(Creature *pTrigger = pInstance->GetCreature(pInstance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE)))
                pTrigger->AI()->EnterEvadeMode();
        }

        EventBegun = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetDisplayId(11686);
    }

    void EnterCombat(Unit *who) {}
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}

    void StartEvent(Unit *target)
    {
        if(!pInstance)
            return;

        if(target && target->isAlive())
        {
            m_council[0] = pInstance->GetData64(DATA_GATHIOSTHESHATTERER);
            m_council[1] = pInstance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
            m_council[2] = pInstance->GetData64(DATA_LADYMALANDE);
            m_council[3] = pInstance->GetData64(DATA_VERASDARKSHADOW);

            // Start the event for the Voice Trigger
            if(Creature *pTrigger = pInstance->GetCreature(pInstance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE)))
            {
                ((mob_blood_elf_council_voice_triggerAI*)pTrigger->AI())->LoadCouncilGUIDs();
                ((mob_blood_elf_council_voice_triggerAI*)pTrigger->AI())->EventStarted = true;
            }

            for(uint8 i = 0; i < 4; ++i)
            {
                Unit* Member = NULL;
                if(m_council[i])
                {
                    Member = pInstance->GetCreature(m_council[i]);
                    if(Member && Member->isAlive())
                        ((Creature*)Member)->AI()->AttackStart(target);
                }
            }

            pInstance->SetData(DATA_ILLIDARICOUNCILEVENT, IN_PROGRESS);

            EventBegun = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!EventBegun)
            return;

        if(EndEventTimer)
        {
            if(EndEventTimer < diff)
            {
                if(DeathCount > 3)
                {
                    if (pInstance)
                    {
                        if(Creature *pTrigger = pInstance->GetCreature(pInstance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE)))
                            pTrigger->DealDamage(pTrigger,pTrigger->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                        pInstance->SetData(DATA_ILLIDARICOUNCILEVENT, DONE);
                    }
                    
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }

                Creature *pMember = pInstance->GetCreature(m_council[DeathCount]);
                if(pMember && pMember->isAlive())
                    pMember->DealDamage(pMember, pMember->GetHealth(), DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                ++DeathCount;
                EndEventTimer = 1500;
            }
            else
                EndEventTimer -= diff;
        }

        if(m_checkTimer)
        {
            if(m_checkTimer < diff)
            {
                uint8 EvadeCheck = 0;
                for(uint8 i = 0; i < 4; ++i)
                {
                    if(m_council[i])
                    {
                        if(Creature *pMember = pInstance->GetCreature(m_council[i]))
                        {
                            // This is the evade/death check.
                            if(pMember->isAlive() && !pMember->getVictim())
                                ++EvadeCheck;                   //If all members evade, we reset so that players can properly reset the event
                            else
                            {
                                if(!pMember->isAlive())         // If even one member dies, kill the rest, set instance data, and kill self.
                                {
                                    EndEventTimer = 1000;
                                    m_checkTimer = 0;
                                    return;
                                }
                            }
                        }
                    }
                }

                if(EvadeCheck > 3)
                    Reset();

                m_checkTimer = 2000;
            }
            else
                m_checkTimer -= diff;
        }
    }
};

struct TRINITY_DLL_DECL boss_illidari_councilAI : public ScriptedAI
{
    boss_illidari_councilAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        loadedGUIDs = false;
    }

    uint64 m_council[4];

    ScriptedInstance* pInstance;

    bool loadedGUIDs;

    void EnterCombat(Unit *pWho)
    {
        if(pInstance)
        {
            if (Creature *pController = pInstance->GetCreature(pInstance->GetData64(DATA_ILLIDARICOUNCIL)))
                ((mob_illidari_councilAI*)pController->AI())->StartEvent(pWho);
        }

        DoZoneInCombat();

        if(!loadedGUIDs)
        {
            m_council[0] = pInstance->GetData64(DATA_LADYMALANDE);
            m_council[1] = pInstance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
            m_council[2] = pInstance->GetData64(DATA_GATHIOSTHESHATTERER);
            m_council[3] = pInstance->GetData64(DATA_VERASDARKSHADOW);
        }
    }

    void EnterEvadeMode()
    {
        for(uint8 i = 0; i < 4; ++i)
        {
            if(Creature *pTemp = pInstance->GetCreature(m_council[i]))
            {
                if(pTemp != me && pTemp->getVictim())
                {
                    AttackStart(pTemp->getVictim());
                    return;
                }
            }
        }
        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if(done_by == m_creature)
            return;

        damage /= 4;
        for(uint8 i = 0; i < 4; ++i)
        {
            if(Creature *pUnit = pInstance->GetCreature(m_council[i]))
            {
                if(pUnit != m_creature && pUnit->isAlive())
                {
                    pUnit->LowerPlayerDamageReq(damage);

                    if(damage <= pUnit->GetHealth())
                        pUnit->SetHealth(pUnit->GetHealth() - damage);
                    else
                        pUnit->Kill(pUnit, false);
                }
            }
        }
    }

};

// Gathios the Shatterer's spells
enum gathiosSpells
{
    SPELL_BLESS_PROTECTION     = 41450,
    SPELL_BLESS_SPELLWARD      = 41451,
    SPELL_CONSECRATION         = 41541,
    SPELL_HAMMER_OF_JUSTICE    = 41468,
    SPELL_SEAL_OF_COMMAND      = 41469,
    SPELL_JUDGEMENT_OF_COMMAND = 41470,
    SPELL_SEAL_OF_BLOOD        = 41459,
    SPELL_JUDGEMENT_OF_BLOOD   = 41461,
    SPELL_CHROMATIC_AURA       = 41453,
    SPELL_DEVOTION_AURA        = 41452
};

struct TRINITY_DLL_DECL boss_gathios_the_shattererAI : public boss_illidari_councilAI
{
    boss_gathios_the_shattererAI(Creature *c) : boss_illidari_councilAI(c){}

    uint32 m_sealTimer;
    uint32 m_auraTimer;
    uint32 m_hammerTimer;
    uint32 m_blessingTimer;
    uint32 m_judgementTimer;
    uint32 m_consecrationTimer;

    uint32 m_checkTimer;

    void Reset()
    {
        m_consecrationTimer = 40000;
        m_hammerTimer = 10000;
        m_sealTimer = 40000;
        m_auraTimer = 90000;
        m_blessingTimer = 60000;
        m_judgementTimer = 45000;

        m_checkTimer = 1000;
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_GATH_SLAY, m_creature);
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_GATH_DEATH, m_creature);
    }

    Unit* SelectCouncil()
    {
        if(urand(0, 8))
        {
            if (Unit *pMelande = pInstance->GetCreature(m_council[0]))
            {
                if (pMelande->isAlive())
                    return pMelande;
            }
        }

        if (Unit *pCouncil = pInstance->GetCreature(m_council[urand(1,3)]))
        {
            if (pCouncil->isAlive())
                return pCouncil;
        }

        return me;
    }

    void ApplyAura(uint32 m_spellId)
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            if (Unit *pCouncil = pInstance->GetCreature(m_council[i]))
                pCouncil->CastSpell(pCouncil, m_spellId, true, 0, 0, me->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_checkTimer < diff)
        {
            DoZoneInCombat();
            m_checkTimer = 1000;
        }
        else
            m_checkTimer -= diff;

        if (m_blessingTimer < diff)
        {
            if(Unit *pUnit = SelectCouncil())
            {
                AddSpellToCast(pUnit, RAND(SPELL_BLESS_SPELLWARD, SPELL_BLESS_PROTECTION));
                m_blessingTimer = 15000;
            }
        }
        else
            m_blessingTimer -= diff;

        if (m_consecrationTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_CONSECRATION);
            m_consecrationTimer = 30000;
        }
        else
            m_consecrationTimer -= diff;

        if (m_hammerTimer < diff)
        {
            if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 40, true, 0, 10.0f))
            {
                AddSpellToCast(pTarget, SPELL_HAMMER_OF_JUSTICE);
                m_hammerTimer = 20000;
            }
        }
        else
            m_hammerTimer -= diff;

        if (m_sealTimer < diff)
        {
            AddSpellToCast(m_creature, RAND(SPELL_SEAL_OF_COMMAND, SPELL_SEAL_OF_BLOOD));
            m_sealTimer = 40000;
        }
        else
            m_sealTimer -= diff;

        if (m_judgementTimer < diff)
        {
            bool hasSeal = false;

            if (me->HasAura(SPELL_SEAL_OF_COMMAND,0))
            {
                hasSeal = true;
                AddSpellToCast(me->getVictim(), SPELL_JUDGEMENT_OF_COMMAND);
            }

            if (me->HasAura(SPELL_SEAL_OF_BLOOD,0))
            {
                hasSeal = true;
                AddSpellToCast(me->getVictim(), SPELL_JUDGEMENT_OF_BLOOD);
            }

            m_judgementTimer = hasSeal ? 45000 : 15000;
        }
        else
            m_judgementTimer -= diff;

        if (m_auraTimer < diff)
        {
            ApplyAura(RAND(SPELL_DEVOTION_AURA, SPELL_CHROMATIC_AURA));
            m_auraTimer = 90000;
        }
        else
            m_auraTimer -= diff;

        DoMeleeAttackIfReady();
        CastNextSpellIfAnyAndReady();
    }
};

// High Nethermancer Zerevor's spells
enum zerevorSpells
{
    SPELL_FLAMESTRIKE       = 41481,
    SPELL_BLIZZARD          = 41482,
    SPELL_ARCANE_BOLT       = 41483,
    SPELL_ARCANE_EXPLOSION  = 41524,
    SPELL_DAMPEN_MAGIC      = 41478
};

struct TRINITY_DLL_DECL boss_high_nethermancer_zerevorAI : public boss_illidari_councilAI
{
    boss_high_nethermancer_zerevorAI(Creature *c) : boss_illidari_councilAI(c)
    {
        m_creature->GetPosition(wLoc);
    }

    uint32 BlizzardTimer;
    uint32 FlamestrikeTimer;
    uint32 ArcaneBoltTimer;
    uint32 DampenMagicTimer;
    uint32 ArcaneExplosionTimer;
    uint32 CheckTimer;
    WorldLocation wLoc;

    void Reset()
    {
        BlizzardTimer = 30000 + rand()%61 * 1000;
        FlamestrikeTimer = 30000 + rand()%61 * 1000;
        ArcaneBoltTimer = 2000;
        DampenMagicTimer = 2000;
        ArcaneExplosionTimer = 14000;
        CheckTimer = 1000;
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_ZERE_SLAY, m_creature);
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_ZERE_DEATH, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(&wLoc, 100.0))
                EnterEvadeMode();
            else
                DoZoneInCombat();
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if(DampenMagicTimer < diff)
        {
            ForceSpellCast(m_creature, SPELL_DAMPEN_MAGIC);
            DampenMagicTimer = 67200;                      // almost 1,12 minutes
            ArcaneBoltTimer += 1000;                        // Give the Mage some time to spellsteal Dampen.
        }
        else
            DampenMagicTimer -= diff;

        if(BlizzardTimer < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                AddSpellToCast(target, SPELL_BLIZZARD);
                BlizzardTimer = 45000 + rand()%46 * 1000;
                FlamestrikeTimer += 10000;
            }
        }
        else
            BlizzardTimer -= diff;

        if(FlamestrikeTimer < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 200, true))
            {
                AddSpellToCast(target, SPELL_FLAMESTRIKE);
                FlamestrikeTimer = 55000 + rand()%46 * 1000;
                BlizzardTimer += 10000;
            }
        }
        else
            FlamestrikeTimer -= diff;

        if(ArcaneBoltTimer < diff)
        {
            if(!m_creature->IsNonMeleeSpellCasted(false))
            {
                AddSpellToCast(m_creature->getVictim(), SPELL_ARCANE_BOLT);
                ArcaneBoltTimer = 3000;
            }
        }
        else
            ArcaneBoltTimer -= diff;

        if(ArcaneExplosionTimer < diff)
        {
            bool InMeleeRange = false;
            std::list<HostilReference*>& m_threatlist = m_creature->getThreatManager().getThreatList();
            for (std::list<HostilReference*>::iterator i = m_threatlist.begin(); i!= m_threatlist.end();++i)
            {
                Unit* pUnit = Unit::GetUnit((*m_creature), (*i)->getUnitGuid());
                                                            //if in melee range
                if(pUnit && pUnit->IsWithinDistInMap(m_creature, 5))
                {
                    InMeleeRange = true;
                    break;
                }
            }

            if(InMeleeRange)
                ForceAOESpellCast(SPELL_ARCANE_EXPLOSION);

            ArcaneExplosionTimer = 2000+rand()%2000;
        }
        else
            ArcaneExplosionTimer -= diff;

        CastNextSpellIfAnyAndReady();
    }
};


// Lady Malande's spells
enum malandeSpells
{
    SPELL_EMPOWERED_SMITE   = 41471,
    SPELL_CIRCLE_OF_HEALING = 41455,
    SPELL_REFLECTIVE_SHIELD = 41475,
    SPELL_DIVINE_WRATH      = 41472,
    SPELL_HEAL_VISUAL       = 24171
};

struct TRINITY_DLL_DECL boss_lady_malandeAI : public boss_illidari_councilAI
{
    boss_lady_malandeAI(Creature *c) : boss_illidari_councilAI(c)
    {
        m_creature->GetPosition(wLoc);
    }

    uint32 EmpoweredSmiteTimer;
    uint32 CircleOfHealingTimer;
    uint32 DivineWrathTimer;
    uint32 ReflectiveShieldTimer;
    uint32 CheckTimer;
    WorldLocation wLoc;

    void Reset()
    {
        EmpoweredSmiteTimer = 18000;
        CircleOfHealingTimer = 20000;
        DivineWrathTimer = 40000;
        ReflectiveShieldTimer = 15000;
        CheckTimer = 1000;
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_MALA_SLAY, m_creature);
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_MALA_DEATH, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(CheckTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(&wLoc, 100.0))
                EnterEvadeMode();
            else
                DoZoneInCombat();
            CheckTimer = 1000;
        }
        else
            CheckTimer -= diff;

        if(EmpoweredSmiteTimer < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                AddSpellToCast(target, SPELL_EMPOWERED_SMITE);
                EmpoweredSmiteTimer = 30000;
            }
        }
        else
            EmpoweredSmiteTimer -= diff;

        if(CircleOfHealingTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_CIRCLE_OF_HEALING);
            CircleOfHealingTimer = 30000;
        }
        else
            CircleOfHealingTimer -= diff;

        if(DivineWrathTimer < diff)
        {
            if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                AddSpellToCast(target, SPELL_DIVINE_WRATH);
                DivineWrathTimer = 20000 + rand()%20000;
            }
        }
        else
            DivineWrathTimer -= diff;

        if(ReflectiveShieldTimer < diff)
        {
            AddSpellToCast(m_creature, SPELL_REFLECTIVE_SHIELD);
            ReflectiveShieldTimer = 45000;
        }
        else
            ReflectiveShieldTimer -= diff;

        CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

// Veras Darkshadow's spells
enum verasSpells
{
    SPELL_DEADLY_POISON   = 41485,
    SPELL_DEADLY_POISON_T = 41480,
    SPELL_ENVENOM         = 41487,
    SPELL_VANISH          = 41479
};

struct TRINITY_DLL_DECL boss_veras_darkshadowAI : public boss_illidari_councilAI
{
    boss_veras_darkshadowAI(Creature *c) : boss_illidari_councilAI(c){}

    uint64 m_envenomGUID;

    uint32 m_poisonTimer;
    uint32 m_vanishTimer;
    uint32 m_envenomTimer;

    uint32 m_checkTimer;

    void Reset()
    {
        m_envenomGUID = 0;

        m_poisonTimer = 20000;
        m_vanishTimer = 10000;
        m_envenomTimer = 3000;
        
        m_checkTimer = 1000;

        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_VERA_SLAY, m_creature);
    }

    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_DEADLY_POISON)
        {
            m_envenomGUID = pTarget->GetGUID();
            m_envenomTimer = 3000;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_VERA_DEATH, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (m_checkTimer < diff)
        {
            DoZoneInCombat();
            m_checkTimer = 1000;
        }
        else
            m_checkTimer -= diff;

        if (me->GetVisibility() == VISIBILITY_ON)
        {
            if (m_vanishTimer < diff)
            {
                DoResetThreat();

                AddSpellToCast(me, SPELL_VANISH);
                AddSpellToCast(me, SPELL_DEADLY_POISON_T);
                m_vanishTimer = 30000;

                if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoStartMovement(pTarget);

                me->SetVisibility(VISIBILITY_OFF);
            }
            else
                m_vanishTimer -= diff;

            CastNextSpellIfAnyAndReady();
            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_vanishTimer < diff)
            {
                me->SetVisibility(VISIBILITY_ON);
                m_vanishTimer = 55000;
                return;
            }
            else
                m_vanishTimer -= diff;

            if (m_envenomTimer < diff)
            {
                if (m_envenomGUID)
                {
                    if(Unit *pTarget = me->GetUnit(*me, m_envenomGUID))
                    {
                        AddSpellToCast(pTarget, SPELL_ENVENOM);
                        m_envenomGUID = 0;
                    }
                }
                m_envenomTimer = 3000;
            }
            else
                m_envenomTimer -= diff;

            CastNextSpellIfAnyAndReady();
        }
    }
};

CreatureAI* GetAI_mob_blood_elf_council_voice_trigger(Creature* c)
{
    return new mob_blood_elf_council_voice_triggerAI(c);
}

CreatureAI* GetAI_mob_illidari_council(Creature *_Creature)
{
    return new mob_illidari_councilAI (_Creature);
}

CreatureAI* GetAI_boss_gathios_the_shatterer(Creature *_Creature)
{
    return new boss_gathios_the_shattererAI (_Creature);
}

CreatureAI* GetAI_boss_lady_malande(Creature *_Creature)
{
    return new boss_lady_malandeAI (_Creature);
}

CreatureAI* GetAI_boss_veras_darkshadow(Creature *_Creature)
{
    return new boss_veras_darkshadowAI (_Creature);
}

CreatureAI* GetAI_boss_high_nethermancer_zerevor(Creature *_Creature)
{
    return new boss_high_nethermancer_zerevorAI (_Creature);
}

void AddSC_boss_illidari_council()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="mob_illidari_council";
    newscript->GetAI = &GetAI_mob_illidari_council;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_blood_elf_council_voice_trigger";
    newscript->GetAI = &GetAI_mob_blood_elf_council_voice_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_gathios_the_shatterer";
    newscript->GetAI = &GetAI_boss_gathios_the_shatterer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_lady_malande";
    newscript->GetAI = &GetAI_boss_lady_malande;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_veras_darkshadow";
    newscript->GetAI = &GetAI_boss_veras_darkshadow;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="boss_high_nethermancer_zerevor";
    newscript->GetAI = &GetAI_boss_high_nethermancer_zerevor;
    newscript->RegisterSelf();
}
