
#include "AntiCheat.h"
#include "Player.h"
#include "ObjectMgr.h"

void ACRequest::checkCheating()
{
    if (movementFlags & (MOVEMENTFLAG_JUMPING | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_ONTRANSPORT))
        return;

    if (speed < 7.0f)
        return;

    Player *player = objmgr.GetPlayer(guid);
    if (!player || player->hasUnitState(UNIT_STAT_CHARGING))
        return;

    if (player->GetMap() && player->GetMap()->IsBattleGroundOrArena())
        return;

    float server_distance = 2.0f + ((speed - 7.0f) * 3.5f + 0.2f/*zapas*/ );
    if (server_distance < 0)
        return;

    float distance = sqrt((x - x_pack)*(x - x_pack) + (y - y_pack)*(y - y_pack)) - player->GetObjectSize();
    if (distance > server_distance)
    {
        float clientspeed = (distance - 2.0f)/3.5f + 7.5f;
        sLog.outCheat("Player %s (GUID: %u / ACCOUNT_ID: %u) moved for distance %f with server speed %f (client speed %f). MapID: %u, player's coord before X:%f Y:%f Z:%f. Player's coord now X:%f Y:%f Z:%f. HomeBindPosition X:%f Y:%f Z:%f MAP: %u. MOVEMENTFLAGS: %u LATENCY: %u",
                      player->GetName(), player->GetGUIDLow(), account_id, distance, speed, clientspeed, mapId, x, y, z, x_pack, y_pack, z_pack, x_bind, y_bind, z_bind, map_bind, movementFlags, latency);
    }
}

int AntiCheat::svc()
{
    while (true)
    {
        if (!active)
            ACE_OS::sleep(ACE_Time_Value(0, 500));
        else
        {
            ACRequest *req;
            if (_queue.next(req))
            {
                req->checkCheating();
                delete req;
            }
        }
    }
    return 0;
}

int AntiCheat::activate(int num_threads)
{
    if (active)
        return -1;

    if (num_threads < 1)
        return -1;

    if( ACE_Task_Base::activate (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, num_threads) == -1)
        return -1;
    active = true;
    return 0;
}
