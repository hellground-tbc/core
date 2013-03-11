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
    };

    enum Flags
    {
        FLAG_RESPAWN_AT_END   = 0x01,
        FLAG_CAN_ATTACK       = 0x02,
        FLAG_IS_AGGRESSIVE    = 0x04,
        FLAG_ASSIST_IN_COMBAT = 0x08,
        FLAG_DESPAWN_AT_END   = 0x10
    };

    public:
        explicit EscortAI(Creature* owner) : CreatureAI(owner) {}

        void AttackStart(Unit* who) override;
        void MoveInLineOfSight(Unit* who) override;

        void JustDied(Unit* killer) override;
        void Reset() override;

        void EnterCombat(Unit* who) override;
        void EnterEvadeMode() override;

        void MovementInform(uint32 type, uint32 data) override;

        virtual void UpdateAI(const uint32 diff) override;

        virtual void WaypointReached(uint32 pointId) = 0;
        virtual void WaypointStart(uint32 pointId) {}

        void SetState(State st) { state = st; }
        bool HasState(State st) { return state == st; }

        State GetState() { return state; }

    private:
        uint32 pointId;
        std::vector<Waypoint> path;

        uint32 questId;

        State state;
        Flags flags;

        TimeTrackerSmall delayTimer;

        ObjectGuid escort;
        Position origin;
};

#endif
