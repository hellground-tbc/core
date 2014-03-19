/*
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef HELLGROUND_MAPUPDATER_H
#define HELLGROUND_MAPUPDATER_H

#include <ace/Thread_Mutex.h>
#include <ace/Condition_Thread_Mutex.h>

#include "DelayExecutor.h"
#include "Common.h"
#include "Map.h"

struct MapUpdateInfo
{
    public:
        MapUpdateInfo();
        MapUpdateInfo(uint32 id, uint32 instanceId, uint32 startTime) : _id(id), _instanceId(instanceId), _startUpdateTime(startTime) {}

        uint32 GetId() const { return _id; }
        uint32 GetInstanceId() const { return _instanceId; }
        uint32 GetUpdateTime() const { return _startUpdateTime; }

    private:
        uint32 _id;
        uint32 _instanceId;

        uint32 _startUpdateTime;
};

typedef std::map<ACE_thread_t const, MapUpdateInfo> ThreadMapMap;

class MapUpdater
{
    public:
        MapUpdater();
        virtual ~MapUpdater();

        friend class MapUpdateRequest;

        /// schedule update on a map, the update will start
        /// as soon as possible,
        /// it may even start before the call returns
        int schedule_update(Map& map, ACE_UINT32 diff);

        /// Wait until all pending updates finish
        int wait();

        /// Start the worker threads
        int activate(size_t num_threads);

        /// Stop the worker threads
        int deactivate(void);

        bool activated();
        void update_finished();

        void register_thread(ACE_thread_t const threadId, uint32 mapId, uint32 instanceId);
        void unregister_thread(ACE_thread_t const threadId);

        void FreezeDetect();

        MapUpdateInfo const* GetMapUpdateInfo(ACE_thread_t const threadId);

    private:
        ThreadMapMap m_threads;

        uint32 freezeDetectTime;

        DelayExecutor m_executor;
        ACE_Condition_Thread_Mutex m_condition;
        ACE_Thread_Mutex m_mutex;
        size_t pending_requests;
};

#endif //_MAP_UPDATER_H_INCLUDED
