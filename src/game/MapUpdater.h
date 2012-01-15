#ifndef _MAP_UPDATER_H_INCLUDED
#define _MAP_UPDATER_H_INCLUDED


#include <ace/Thread_Mutex.h>
#include <ace/Condition_Thread_Mutex.h>

#include "DelayExecutor.h"
#include "Common.h"
#include "Map.h"
//class Map;

typedef std::pair<uint32, uint32> MapIDs;

struct MapBrokenData
{
    explicit MapBrokenData()
    {
        Reset();
    }

    void Reset()
    {
        count = 1;
        lastErrorTime = time(NULL);
    };

    void IncreaseCount()
    {
        ++count;
        lastErrorTime = time(NULL);
    };
    uint32 count;
    time_t lastErrorTime;
};

typedef std::map<ACE_thread_t const, MapIDs> ThreadMapMap;
typedef std::map<ACE_thread_t const, uint32/*MSTime*/>  ThreadStartTimeMap;
typedef std::map<MapIDs, MapBrokenData> MapBrokenDataMap;

class MapUpdater
{
public:
    MapUpdater ();
    virtual ~MapUpdater ();

    friend class MapUpdateRequest;

    /// schedule update on a map, the update will start
    /// as soon as possible ,
    /// it may even start before the call returns
    int schedule_update(Map& map, ACE_UINT32 diff);

    /// Wait untill all pending updates finish
    int wait ();

    /// Start the worker threads
    int activate (size_t num_threads);

    /// Stop the worker threads
    int deactivate (void);

    bool activated ();
    void update_finished();

    void register_thread(ACE_thread_t const threadId, uint32 mapId, uint32 instanceId);
    void unregister_thread(ACE_thread_t const threadId);

    MapIDs const* GetMapPairByThreadId(ACE_thread_t const threadId);
    void FreezeDetect();

    void SetBroken(bool value = false)
    {
        m_broken = value;
    };

    bool IsBroken()
    {
        return m_broken;
    };

    void ReActivate(uint32 threads);

    void MapBrokenEvent(MapIDs const* mapPair);
    MapBrokenData const* GetMapBrokenData(MapIDs const* mapPair);

private:
    ThreadMapMap m_threads;
    ThreadStartTimeMap m_starttime;
    MapBrokenDataMap   m_brokendata;
    bool m_broken;

    uint32 freezeDetectTime;
    uint32 brokenEventTime;

    DelayExecutor m_executor;
    ACE_Condition_Thread_Mutex m_condition;
    ACE_Thread_Mutex m_mutex;
    size_t pending_requests;
};

#endif //_MAP_UPDATER_H_INCLUDED
