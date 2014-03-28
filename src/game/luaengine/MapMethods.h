/*
* Copyright (C) 2010 - 2014 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef MAPMETHODS_H
#define MAPMETHODS_H

namespace LuaMap
{
    int GetName(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->GetMapName());
        return 1;
    }

    int GetHeight(lua_State* L, Map* map)
    {
        float x = sEluna->CHECKVAL<float>(L, 2);
        float y = sEluna->CHECKVAL<float>(L, 3);
        float z = map->GetTerrain()->GetHeight(x, y, MAX_HEIGHT);
        if (z == INVALID_HEIGHT)
            return 0;
        sEluna->Push(L, z);
        return 1;
    }

    int GetInstanceId(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->GetInstanceId());
        return 1;
    }

    int GetPlayerCount(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->GetPlayersCountExceptGMs());
        return 1;
    }

    int GetMapId(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->GetId());
        return 1;
    }

    int GetAreaId(lua_State* L, Map* map)
    {
        float x = sEluna->CHECKVAL<float>(L, 2);
        float y = sEluna->CHECKVAL<float>(L, 3);
        float z = sEluna->CHECKVAL<float>(L, 4);

        sEluna->Push(L, map->GetTerrain()->GetAreaId(x, y, z));
        return 1;
    }

    int IsArena(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->IsBattleArena());
        return 1;
    }

    int IsBattleground(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->IsBattleGround());
        return 1;
    }

    int IsDungeon(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->IsDungeon());
        return 1;
    }

    int IsEmpty(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->isEmpty());
        return 1;
    }

    int IsHeroic(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->IsHeroic());
        return 1;
    }

    int IsRaid(lua_State* L, Map* map)
    {
        sEluna->Push(L, map->IsRaid());
        return 1;
    }
};
#endif
