#ifndef H_ESCORTAI
#define H_ESCORTAI

#include <vector>

#include "CreatureAI.h"
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
        FLAG_CAN_RESPAWN    = 0x1,
        FLAG_CAN_ATTACK     = 0x2,
        FLAG_IS_AGGRESSIVE  = 0x4,
    };

    public:
        explicit EscortAI(Creature* owner) : CreatureAI(owner) {}

        void AttackStart(Unit* who) override;
        void MoveInLineOfSight(Unit* who) override;

        void JustDied(Unit* killer) override;

    private:
        std::vector<Waypoint> path;

        uint32 questId;

        State state;
        Flags flags;

        ObjectGuid escort;
};

#endif
