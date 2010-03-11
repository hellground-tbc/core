#ifndef _ANTICHEAT_H
#define _ANTICHEAT_H

#include <ace/Task.h>
#include "Common.h"
#include "Player.h"

class Player;

struct ACRequest
{
    ACRequest(Player *player, uint32 acc, uint32 delay,
              float p_x, float p_y, float p_z, uint32 movement_flags) :
              guid(player->GetGUID()), movementFlags(movement_flags), latency(delay), mapId(player->GetMapId()),
              x(player->GetPositionX()), y(player->GetPositionY()), z(player->GetPositionZ()), x_pack(p_x), y_pack(p_y), z_pack(p_z),
              map_bind(player->GetInnPosMapId()), x_bind(player->GetInnPosX()), y_bind(player->GetInnPosY()), z_bind(player->GetInnPosZ()), account_id(acc)
              {
                  if (movement_flags & MOVEMENTFLAG_FLYING)
                      speed = player->GetSpeed(MOVE_FLIGHT);
                  else
                      speed = player->GetSpeed(MOVE_RUN);
               }

    uint64 guid;
    uint32 account_id;

    uint64 movementFlags;

    uint32 latency;

    uint32 mapId;
    float x;
    float y;
    float z;

    float x_pack;
    float y_pack;
    float z_pack;

    uint32 map_bind;
    float x_bind;
    float y_bind;
    float z_bind;

    float speed;

    void checkCheating();
};

class AntiCheat : protected ACE_Task_Base
{

    public:
        AntiCheat() : active(false) {};
        virtual ~AntiCheat()
        {
            ACRequest *req;
            while (_queue.next(req))
                delete req;
        };

        void addRequest(ACRequest *req)
        {
            _queue.add(req);
        }

        void _wait()   { active = false; }
        void _resume() { active = true; }

        bool activated() { return active; }

        int activate(int num_threads = 1);

        virtual int svc(void);

    private:
        ACE_Based::LockedQueue<ACRequest*, ACE_Thread_Mutex> _queue;
        bool active;
};

#endif