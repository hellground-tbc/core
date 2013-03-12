#ifndef H_ESCORTAI
#define H_ESCORTAI

#include <vector>

#include "CreatureAI.h"
#include "Object.h"
#include "ObjectGuid.h"

struct Waypoint
{
    Waypoint(uint32 id, uint32 delay, float x, float y, float z);
    Waypoint(const Waypoint&);

    uint32 Id;
    uint32 Delay;

    Position Pos;
};

struct EscortAI : public CreatureAI
{
    enum State
    {
        ESCORT_IN_PROGRESS = 0,
        ESCORT_NEXT_POINT  = 1,
        ESCORT_PAUSED      = 2,
        ESCORT_NOT_STARTED = 3,
        ESCORT_DONE        = 4
    };

    enum Flags
    {
        FLAG_RESPAWN_AT_END   = 0x01, // it will de-spawn creature and re-spawn it in place where escort was started
        FLAG_IS_DEFENSIVE     = 0x02, // it allows creature to defend self and chase enemies when attacked
        FLAG_IS_AGGRESSIVE    = 0x04, // it allows creature to attack every enemy that moves in line of sight
        FLAG_DESPAWN_AT_END   = 0x08  // it forces creature to disappear on last way point without re-spawning (use it for summoned creatures)
    };

    public:
        explicit EscortAI(Creature* owner) : CreatureAI(owner), pathIndex(0) {}

        void AttackStart(Unit* who) override;
        void MoveInLineOfSight(Unit* who) override;

        void JustDied(Unit* killer) override final;
        void JustRespawned();

        void Reset() override;

        void EnterCombat(Unit* who) override final;
        void EnterEvadeMode() override final;

        void MovementInform(uint32 type, uint32 data) override;

        bool EscortInRange() const;
        void DespawnOrRespawn();

        void AddWaypoint(uint32 id, float x, float y, float z, uint32 delay);

        void UpdateAI(const uint32 diff) override final;

        virtual void WaypointReached(uint32 pointId) = 0;
        virtual void WaypointStart(uint32 pointId);
        
        virtual bool EscortEnterCombat(Unit* who) { return true; }
        virtual void EscortUpdateAI(const uint32 diff);
        virtual void EscortEnterEvadeMode() {}
        virtual void EscortReset() {}
        virtual void EscortJustDied(Unit* killer) {}

        State GetState() { return state; }
        bool HasState(State st) { return GetState() == st; }

    private:
        void setState(State st) { state = st; }

        uint32 pathIndex;
        std::vector<Waypoint> path;

        uint32 questId;

        State state;
        Flags flags;

        bool startDone;
        TimeTrackerSmall delayTimer;

        ObjectGuid escort;
        Position origin;
};

#endif
