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
        ESCORT_PAUSED      = 1,
        ESCORT_FAILED      = 2,
    };

    enum Flags
    {
        FLAG_CAN_RESPAWN      = 0x1,
        FLAG_CAN_ATTACK       = 0x2,
        FLAG_IS_AGGRESSIVE    = 0x4,
        FLAG_ASSIST_IN_COMBAT = 0x8
    };

    public:
        explicit EscortAI(Creature* owner) : CreatureAI(owner) {}

        void AttackStart(Unit* who) override;
        void MoveInLineOfSight(Unit* who) override;

        void JustDied(Unit* killer) override;
        void JustRespawned() override;

        virtual void UpdateAI(const uint32 diff) override;

        virtual void WaypointReached(uint32 pointId) = 0;
        virtual void WaypointStart(uint32 pointId) {}

    private:
        std::vector<Waypoint> path;

        uint32 questId;

        State state;
        Flags flags;

        ObjectGuid escort;
        Position origin;
};

#endif
