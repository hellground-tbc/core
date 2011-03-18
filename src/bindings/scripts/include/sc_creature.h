/* Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Thanks to the original authors: ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_CREATURE_H
#define SC_CREATURE_H

#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "InstanceData.h"

#define SCRIPT_CAST_TYPE dynamic_cast

#define CAST_PLR(a)     (SCRIPT_CAST_TYPE<Player*>(a))
#define CAST_CRE(a)     (SCRIPT_CAST_TYPE<Creature*>(a))
#define CAST_SUM(a)     (SCRIPT_CAST_TYPE<TempSummon*>(a))
#define CAST_PET(a)     (SCRIPT_CAST_TYPE<Pet*>(a))
#define CAST_AI(a,b)    (SCRIPT_CAST_TYPE<a*>(b))

#define GET_SPELL(a)    (const_cast<SpellEntry*>(GetSpellStore()->LookupEntry(a)))

class ScriptedInstance;

class SummonList : std::list<uint64>
{
public:
    SummonList(Creature* creature) : m_creature(creature) {}
    void Summon(Creature *summon) {push_back(summon->GetGUID());}
    void Despawn(Creature *summon);
    void DespawnEntry(uint32 entry);
    void DespawnAll();
    void AuraOnEntry(uint32 entry, uint32 spellId, bool apply);
    void DoAction(uint32 entry, uint32 info);
private:
    Creature *m_creature;
};

//Get a single creature of given entry
Unit* FindCreature(uint32 entry, float range, Unit* Finder);

//Get a single gameobject of given entry
GameObject* FindGameObject(uint32 entry, float range, Unit* Finder);

struct PointMovement
{
    uint32 m_uiCreatureEntry;
    uint32 m_uiPointId;
    float m_fX;
    float m_fY;
    float m_fZ;
    uint32 m_uiWaitTime;
};

enum interruptSpell
{
    DONT_INTERRUPT                = 0,
    INTERRUPT_AND_CAST            = 1,   //cast when CastNextSpellIfAnyAndReady() is called
    INTERRUPT_AND_CAST_INSTANTLY  = 2    //cast instantly (CastSpell())
};

enum autocastTargetMode
{
    AUTOCAST_TANK                   = 0,    //cast on GetVictim() target
    AUTOCAST_NULL                   = 1,    //cast on (Unit*)NULL target
    AUTOCAST_RANDOM                 = 2,    //cast on SelectUnit(SELECT_TARGET_RANDOM) target (needs additionals: range, only player)
    AUTOCAST_RANDOM_WITHOUT_TANK    = 3,    //same as AUTOCAST_RANDOM but without tank
    AUTOCAST_SELF                   = 4     //target is m_creature
};

class SpellToCast
{
public:
    float castDest[3];
    uint64 targetGUID;
    uint32 spellId;
    bool triggered;
    bool isAOECast;
    bool isDestCast;
    bool setAsTarget;
    int32 scriptTextEntry;

    SpellToCast(Unit* target, uint32 spellId, bool triggered, int32 scriptTextEntry, bool isAOECast, bool visualTarget)
    {

        if (target)
            this->targetGUID = target->GetGUID();
        else
            this->targetGUID = 0;

        this->isDestCast = false;
        this->spellId = spellId;
        this->triggered = triggered;
        this->isAOECast = isAOECast;
        this->scriptTextEntry = scriptTextEntry;
        this->setAsTarget = visualTarget;
    }

    SpellToCast(uint64 target, uint32 spellId, bool triggered, int32 scriptTextEntry, bool isAOECast, bool visualTarget)
    {
        this->isDestCast = false;
        this->targetGUID = target;
        this->spellId = spellId;
        this->triggered = triggered;
        this->isAOECast = isAOECast;
        this->scriptTextEntry = scriptTextEntry;
        this->setAsTarget = visualTarget;
    }

    SpellToCast(float x, float y, float z, uint32 spellId, bool triggered, int32 scriptTextEntry, bool isAOECast, bool visualTarget)
    {
        isDestCast = true;
        this->castDest[0] = x;
        this->castDest[1] = y;
        this->castDest[2] = z;
        this->spellId = spellId;
        this->triggered = triggered;
        this->isAOECast = isAOECast;
        this->scriptTextEntry = scriptTextEntry;
        this->setAsTarget = visualTarget;
    }

    SpellToCast()
    {
        this->targetGUID = 0;
        this->spellId = 0;
        this->triggered = false;
        this->isAOECast = false;
        this->scriptTextEntry = 0;
        this->setAsTarget = false;
        this->isDestCast = false;
        for(uint8 i=0;i<3;++i)
            this->castDest[i] = 0;
    }

    ~SpellToCast()
    {
        this->targetGUID = 0;
        this->spellId = 0;
        this->triggered = false;
        this->isAOECast = false;
        this->scriptTextEntry = 0;
        this->setAsTarget = false;
        this->isDestCast = false;
        for(uint8 i=0;i<3;++i)
            this->castDest[i] = 0;
    }
};

struct TRINITY_DLL_DECL ScriptedAI : public CreatureAI
{
    explicit ScriptedAI(Creature* pCreature);
    ~ScriptedAI() {}

    //*************
    //CreatureAI Functions
    //*************

    //Called at each attack of m_creature by any victim
    void AttackStartNoMove(Unit *target);
    void AttackStart(Unit *);
    void AttackStart(Unit *, bool melee);

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* pDone_by, uint32& uiDamage) {}

    //Called at World update tick
    void UpdateAI(const uint32);

    //Called at creature death
    void JustDied(Unit*){}

    //Called at creature killing another unit
    void KilledUnit(Unit*){}

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature* ) {}

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature* /*unit*/) {}

    // Called when hit by a spell
    void SpellHit(Unit* caster, const SpellEntry*) {}

    // Called when aura is applied
    void OnAuraApply(Aura* aur, Unit* caster, bool stackApply) {}

    // Called when aura is removed
    void OnAuraRemove(Aura* aur, bool stackRemove) {}

    //Called when creature deals damage to player
    void DamageMade(Unit* target, uint32 & damage, bool direct_damage) {}

    // Called when spell hits a target
    void SpellHitTarget(Unit* target, const SpellEntry*) {}

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32, uint32){}

    // Called when AI is temporarily replaced or put back when possess is applied or removed
    void OnPossess(bool apply) {}

    //*************
    // Variables
    //*************

    //Pointer to creature we are manipulating
    Creature* m_creature;

    //For fleeing
    bool IsFleeing;

    //Spell list to cast
    std::list<SpellToCast> spellList;

    //Spell id which should be autocast
    uint32 autocastId;

    //timer for autocast spell
    uint32 autocastTimer;
    uint32 autocastTimerDef;

    autocastTargetMode autocastMode;
    uint32 autocastTargetRange;
    bool autocastTargetPlayer;

    //True if autocast is enabled
    bool autocast;

    //*************
    //Pure virtual functions
    //*************

    //Called at creature reset either by death or evade
    virtual void Reset(){}

    //Called at creature aggro either by MoveInLOS or Attack Start
    virtual void EnterCombat(Unit* who) {}

    //*************
    //AI Helper Functions
    //*************

    //Start movement toward victim
    void DoStartMovement(Unit* pVictim, float fDistance = 0, float fAngle = 0);

    //Start no movement on victim
    void DoStartNoMovement(Unit* victim);

    //Stop attack of current victim
    void DoStopAttack();

    //Cast next spell from list
    void CastNextSpellIfAnyAndReady(uint32 diff = 0);

    //Cast spell by Id
    void DoCast(Unit* victim, uint32 spellId, bool triggered = false);
    void DoCastAOE(uint32 spellId, bool triggered = false);

    //Casts queue
    void AddSpellToCast(Unit* victim, uint32 spellId, bool triggered = false, bool visualTarget = false);
    void AddSpellToCast(float x, float y, float z, uint32 spellId, bool triggered = false, bool visualTarget = false);
    void AddSpellToCastWithScriptText(Unit* victim, uint32 spellId, int32 scriptTextEntry, bool triggered = false, bool visualTarget = false);
    void AddAOESpellToCast(uint32 spellId, bool triggered = false);
    void AddAOESpellToCastWithScriptText(uint32 spellId, int32 scriptTextEntry, bool triggered = false);

    //Forces spell cast by Id
    void ForceSpellCast(Unit* victim, uint32 spellId, interruptSpell interruptCurrent = DONT_INTERRUPT, bool triggered = false, bool visualTarget = false);
    void ForceSpellCastWithScriptText(Unit* victim, uint32 spellId, int32 scriptTextEntry, interruptSpell interruptCurrent = DONT_INTERRUPT, bool triggered = false, bool visualTarget = false);
    void ForceAOESpellCast(uint32 spellId, interruptSpell interruptCurrent = DONT_INTERRUPT, bool triggered = false);
    void ForceAOESpellCastWithScriptText(uint32 spellId, int32 scriptTextEntry, interruptSpell interruptCurrent = DONT_INTERRUPT, bool triggered = false);

    //Autocast
    void SetAutocast (uint32 spellId, uint32 timer, bool startImmediately = false, autocastTargetMode mode = AUTOCAST_TANK, uint32 range = 0, bool player = false);
    void StartAutocast() { autocast = true; }
    void StopAutocast() { autocast = false; }

    //Additional
    void ClearCastQueue() { spellList.clear(); }
    void RemoveFromCastQueue(uint32 spellId);
    void RemoveFromCastQueue(uint64 targetGUID);

    //Cast spell by spell info
    void DoCastSpell(Unit* who, SpellEntry const *spellInfo, bool triggered = false);

    //Creature say
    void DoSay(const char* text, uint32 language, Unit* target, bool SayEmote = false);

    //Creature Yell
    void DoYell(const char* text, uint32 language, Unit* target);

    //Creature Text emote, optional bool for boss emote text
    void DoTextEmote(const char* text, Unit* target, bool IsBossEmote = false);

    //Creature whisper, optional bool for boss whisper
    void DoWhisper(const char* text, Unit* reciever, bool IsBossWhisper = false);

    //Plays a sound to all nearby players
    void DoPlaySoundToSet(WorldObject* pSource, uint32 sound);

    //Drops all threat to 0%. Does not remove players from the threat list
    void DoResetThreat();

    float DoGetThreat(Unit *u);
    void DoModifyThreatPercent(Unit *pUnit, int32 pct);

    void DoTeleportTo(float x, float y, float z, uint32 time = 0);

    void DoAction(const int32 param) {}

    //Teleports a player without dropping threat (only teleports to same map)
    void DoTeleportPlayer(Unit* pUnit, float x, float y, float z, float o);
    void DoTeleportAll(float x, float y, float z, float o);

    bool HealthBelowPct(uint32 pct) const { return me->GetHealth() * 100 < m_creature->GetMaxHealth() * pct; }

    void SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand = EQUIP_NO_CHANGE, int32 uiOffHand = EQUIP_NO_CHANGE, int32 uiRanged = EQUIP_NO_CHANGE);

    void SetCombatMovement(bool CombatMove);

    bool IsCombatMovement() { return m_bCombatMovement; }

    //Returns friendly unit with the most amount of hp missing from max hp
    Unit* DoSelectLowestHpFriendly(float range, uint32 MinHPDiff = 1);

    //Returns a list of creatures with specified entry in range
    std::list<Creature*> DoFindAllCreaturesWithEntry(uint32 entry, float range);

    //Returns a list of all friendly units in grid within range
    std::list<Creature*> DoFindAllFriendlyInGrid(float range);

    //Returns a list of friendly CC'd units within range
    std::list<Creature*> DoFindFriendlyCC(float range);

    //Returns a list of all friendly units missing a specific buff within range
    std::list<Creature*> DoFindFriendlyMissingBuff(float range, uint32 spellid);

    //Returns a list of all units that are flagged as DEAD or CORPSE
    std::list<Unit*> DoFindAllDeadInRange(float range);

    //Return a list of all players in range
    std::list<Player*> DoFindAllPlayersInRange(float range, Unit* finder = NULL);

    //Spawns a creature relative to m_creature
    Creature* DoSpawnCreature(uint32 id, float x, float y, float z, float angle, uint32 type, uint32 despawntime);

    //Selects a unit from the creature's current aggro list
    Unit* SelectUnit(SelectAggroTarget target, uint32 position, float dist, bool playerOnly, uint64 = 0, float mindist = 0.0f);
    Unit* SelectUnit(SelectAggroTarget targetType, uint32 position, float maxdist, bool playerOnly, Powers powerOnly);
    Unit* SelectUnit(SelectAggroTarget target, uint32 position);

    void SelectUnitList(std::list<Unit*> &targetList, uint32 num, SelectAggroTarget target, float dist, bool playerOnly);

    //Returns spells that meet the specified criteria from the creatures spell list
    SpellEntry const* SelectSpell(Unit* Target, int32 School, int32 Mechanic, SelectTargetType Targets,  uint32 PowerCostMin, uint32 PowerCostMax, float RangeMin, float RangeMax, SelectEffect Effect);
    float GetSpellMaxRange(uint32 id);

    bool m_bCombatMovement;
    bool HeroicMode;
    uint32 m_uiEvadeCheckCooldown;
};

struct TRINITY_DLL_DECL Scripted_NoMovementAI : public ScriptedAI
{
    Scripted_NoMovementAI(Creature* creature) : ScriptedAI(creature) {}

    //Called at each attack of m_creature by any victim
    void AttackStart(Unit *);
};

struct TRINITY_DLL_DECL BossAI : public ScriptedAI
{
    BossAI(Creature *c, uint32 id);

    uint32 bossId;
    EventMap events;
    SummonList summons;
    InstanceData *instance;

    void JustSummoned(Creature *summon);
    void SummonedCreatureDespawn(Creature *summon);

    void UpdateAI(const uint32 diff) = 0;

    void Reset() { _Reset(); }
    void EnterCombat(Unit *who) { _EnterCombat(); }
    void JustDied(Unit *killer) { _JustDied(); }

    protected:
        void _Reset();
        void _EnterCombat();
        void _JustDied();
};

// SD2 grid searchers
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 Entry, float MaxSearchRange);

#endif

