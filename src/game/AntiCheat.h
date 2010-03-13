#ifndef _ANTICHEAT_H
#define _ANTICHEAT_H

#include <ace/Method_Request.h>

#include "Common.h"
#include "Player.h"

class Player;

class ACRequest : public ACE_Method_Request
{
    public:
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
          
        virtual int call()
        {
            if (movementFlags & (MOVEMENTFLAG_JUMPING | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_ONTRANSPORT))
                return -1;

            if (speed < 7.0f)
                return -1;

            Player *player = objmgr.GetPlayer(guid);
            if (!player || player->hasUnitState(UNIT_STAT_CHARGING))
                return -1;

            if (player->GetMap() && player->GetMap()->IsBattleGroundOrArena())
                return -1;

            float server_distance = 2.0f + ((speed - 7.0f) * 3.5f + 0.2f/*zapas*/ );
            if (server_distance < 0)
                return -1;

            float distance = sqrt((x - x_pack)*(x - x_pack) + (y - y_pack)*(y - y_pack)) - player->GetObjectSize();
            if (distance > server_distance)
            {
                float clientspeed = (distance - 2.0f)/3.5f + 7.5f;
                sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) moved for distance %f with server speed %f (client speed %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. HomeBindPosition X:%f Y:%f Z:%f MAP: %u. MOVEMENTFLAGS: %u LATENCY: %u",
                              player->GetName(), player->GetGUIDLow(), account_id, distance, speed, clientspeed, mapId, x, y, z, x_pack, y_pack, z_pack, x_bind, y_bind, z_bind, map_bind, movementFlags, latency);
            }
            return 0;
        }

    private:
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
};

#endif