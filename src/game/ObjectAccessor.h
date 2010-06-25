/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TRINITY_OBJECTACCESSOR_H
#define TRINITY_OBJECTACCESSOR_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "ace/Thread_Mutex.h"
#include "Utilities/UnorderedMap.h"
#include "Policies/ThreadingModel.h"

#include "ByteBuffer.h"
#include "UpdateData.h"

#include "GridDefines.h"
#include "Object.h"
#include "Player.h"
#include "Corpse.h"

#include <set>
#include <list>

class Creature;
class Corpse;
class Unit;
class GameObject;
class DynamicObject;
class WorldObject;
class Map;

template <class T>
class HashMapHolder
{
    public:

        typedef UNORDERED_MAP<uint64, T*> MapType;
        typedef ACE_Thread_Mutex LockType;
        typedef Trinity::GeneralLock<LockType> Guard;

        static void Insert(T* o)
        {
            Guard guard(i_lock);
            m_objectMap[o->GetGUID()] = o;
        }

        static void Remove(T* o)
        {
            Guard guard(i_lock);
            m_objectMap.erase(o->GetGUID());
        }

        static T* Find(uint64 guid)
        {
            Guard guard(i_lock);
            typename MapType::iterator itr = m_objectMap.find(guid);
            return (itr != m_objectMap.end()) ? itr->second : NULL;
        }

        static MapType& GetContainer() { return m_objectMap; }

        static LockType* GetLock() { return &i_lock; }
    private:

        //Non instanceable only static
        HashMapHolder() {}

        static LockType i_lock;
        static MapType  m_objectMap;
};

class ObjectAccessor : public Trinity::Singleton<ObjectAccessor, Trinity::ClassLevelLockable<ObjectAccessor, ACE_Thread_Mutex> >
{

    friend class Trinity::OperatorNew<ObjectAccessor>;
    ObjectAccessor();
    ~ObjectAccessor();
    ObjectAccessor(const ObjectAccessor &);
    ObjectAccessor& operator=(const ObjectAccessor &);

    public:
        typedef UNORDERED_MAP<uint64, Corpse*> Player2CorpsesMapType;

        static Unit*   GetUnitInWorld(WorldObject const& obj, uint64 guid);        
        static Creature*   GetCreatureInWorld(uint64 guid)   { return FindHelper<Creature>(guid); }
        static GameObject* GetGameObjectInWorld(uint64 guid) { return FindHelper<GameObject>(guid); }
        
        static Object* GetObjectByTypeMask(WorldObject const &p, uint64 guid, uint32 typemask);
        static Unit*   GetUnit(WorldObject const &, uint64);
        static Player* GetPlayer(Unit const &, uint64 guid) { return FindPlayer(guid); }
        static Player* FindPlayer(uint64);

        Player* FindPlayerByName(const char *name) ;

        HashMapHolder<Player>::MapType& GetPlayers()
        {
            return HashMapHolder<Player>::GetContainer();
        }

        void SaveAllPlayers();

        // For call from Player/Corpse AddToWorld/RemoveFromWorld only
        void AddObject(Corpse *object) { HashMapHolder<Corpse>::Insert(object); }
        void AddObject(Player *object) { HashMapHolder<Player>::Insert(object); }
        void RemoveObject(Corpse *object) { HashMapHolder<Corpse>::Remove(object); }
        void RemoveObject(Player *object) { HashMapHolder<Player>::Remove(object); }

        static void LinkMap(Map* map)   { i_mapList.push_back(map); }
        static void DelinkMap(Map* map) { i_mapList.remove(map); }

        Corpse* GetCorpseForPlayerGUID(uint64 guid);
        static Corpse* GetCorpseInMap(uint64 guid, uint32 mapid);
        void RemoveCorpse(Corpse *corpse);
        void AddCorpse(Corpse* corpse);
        void AddCorpsesToGrid(GridPair const& gridpair,GridType& grid,Map* map);
        Corpse* ConvertCorpseForPlayer(uint64 player_guid, bool insignia = false);

        typedef ACE_Thread_Mutex LockType;
        typedef Trinity::GeneralLock<LockType> Guard;
    private:
        template <class OBJECT> static OBJECT* FindHelper(uint64 guid)
        {
            for (std::list<Map*>::const_iterator i = i_mapList.begin() ; i != i_mapList.end(); ++i)
            {
                if (OBJECT* ret = (*i)->GetObjectsStore().find(guid, (OBJECT*)NULL))
                     return ret;
            }

            return NULL;
        }

        static std::list<Map*> i_mapList;
        Player2CorpsesMapType  i_player2corpse;

        void _update(void);
        LockType i_playerGuard;
        LockType i_corpseGuard;
};

inline Unit* ObjectAccessor::GetUnitInWorld(WorldObject const& obj, uint64 guid)
{
    if(!guid)
        return NULL;

    if (IS_PLAYER_GUID(guid))
        return FindPlayer(guid);

    if (IS_PET_GUID(guid))
        return obj.IsInWorld() ? obj.GetMap()->GetPet(guid) : NULL;

    return GetCreatureInWorld(guid);
}
#endif

