#include "MapUpdater.h"

#include "DelayExecutor.h"
#include "Map.h"
#include "MapManager.h"
#include "World.h"
#include "Database/DatabaseEnv.h"

#include <ace/Guard_T.h>
#include <ace/Method_Request.h>

//the reason this things are here is that i want to make
//the netcode patch and the multithreaded maps independant
//once they are merged 1 class should be used
class  WDBThreadStartReq1 : public ACE_Method_Request
{
public:
    WDBThreadStartReq1 () { }
    virtual int call (void)
    {
        WorldDatabase.ThreadStart ();
        return 0;
    }
};

class  WDBThreadEndReq1 : public ACE_Method_Request
{
public:
    WDBThreadEndReq1 () { }
    virtual int call (void)
    {
        WorldDatabase.ThreadEnd ();
        return 0;
    }
};

class MapUpdateRequest : public ACE_Method_Request
{
public:
    Map& m_map;
    MapUpdater& m_updater;
    ACE_UINT32 m_diff;
    MapUpdateRequest (Map& m,MapUpdater& u,ACE_UINT32 d) : m_map(m),m_updater(u),m_diff(d) { }
    virtual int call (void)
    {
        ACE_thread_t const threadId = ACE_OS::thr_self();
        m_updater.register_thread(threadId, m_map.GetId(),m_map.GetInstanceId());

        if (m_map.IsBroken())
            m_map.ForcedUnload();
        else
            m_map.Update(m_diff);

        m_updater.unregister_thread(threadId);
        m_updater.update_finished ();
        return 0;
    }
};

MapUpdater::MapUpdater () :
    m_mutex (),
    m_condition (m_mutex),
    m_executor (),
    pending_requests (0),
    m_broken(false)
{
    freezeDetectTime = sWorld.getConfig(CONFIG_VMSS_FREEZEDETECTTIME);
    brokenEventTime = sWorld.getConfig(CONFIG_VMSS_TBREMTIME);
}

MapUpdater::~MapUpdater ()
{
    this->deactivate ();
}

int MapUpdater::activate (size_t num_threads)
{
    return this->m_executor.activate (static_cast<int> (num_threads), new WDBThreadStartReq1, new WDBThreadEndReq1);
}

int MapUpdater::deactivate (void)
{
    this->wait ();

    return this->m_executor.deactivate ();
}

void MapUpdater::ReActivate(uint32 threads)
{
    deactivate();
    activate(threads);
    SetBroken(false);
}

int MapUpdater::wait ()
{
    ACE_GUARD_RETURN(ACE_Thread_Mutex,guard,this->m_mutex,-1);

    while (this->pending_requests > 0)
        this->m_condition.wait ();

    return 0;
}

int MapUpdater::schedule_update(Map& map, ACE_UINT32 diff)
{
    ACE_GUARD_RETURN(ACE_Thread_Mutex,guard,this->m_mutex,-1);

    ++this->pending_requests;

    if (this->m_executor.execute (new MapUpdateRequest(map,*this,diff)) == -1)
    {
        ACE_DEBUG ((LM_ERROR, ACE_TEXT ("(%t) \n"), ACE_TEXT ("Failed to schedule Map Update")));

        --this->pending_requests;
        return -1;
    }

    return 0;
}

bool MapUpdater::activated ()
{
    return m_executor.activated();
}

void MapUpdater::update_finished ()
{
    ACE_GUARD (ACE_Thread_Mutex, guard, this->m_mutex);

    if (this->pending_requests == 0)
    {
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("(%t)\n"),
                    ACE_TEXT ("MapUpdater::update_finished BUG, report to devs")));

        return;
    }

    --this->pending_requests;

    //TODO can more than one thread call wait (), it shouldnt happen
    //however I ensure if in future more than 1 thread call it by
    //using broadcast instead of signal ()
    this->m_condition.broadcast();
}

void MapUpdater::register_thread(ACE_thread_t const threadId, uint32 mapId, uint32 instanceId)
{
    ACE_GUARD(ACE_Thread_Mutex, guard, m_mutex);
    MapIDs pair = MapIDs(mapId, instanceId);
    m_threads.insert(std::make_pair(threadId, pair));
    m_starttime.insert(std::make_pair(threadId, WorldTimer::getMSTime()));
}

void MapUpdater::unregister_thread(ACE_thread_t const threadId)
{
    ACE_GUARD(ACE_Thread_Mutex, guard, m_mutex);
    m_threads.erase(threadId);
    m_starttime.erase(threadId);
}

MapIDs const* MapUpdater::GetMapPairByThreadId(ACE_thread_t const threadId)
{
    if (!m_threads.empty())
    {
        ThreadMapMap::const_iterator itr = m_threads.find(threadId);
        if (itr != m_threads.end())
            return &itr->second;
    }
    return NULL;
}

void MapUpdater::FreezeDetect()
{
    ACE_GUARD(ACE_Thread_Mutex, guard, m_mutex);
    if (!m_starttime.empty())
    {
        for (ThreadStartTimeMap::const_iterator itr = m_starttime.begin(); itr != m_starttime.end(); ++itr)
        if (WorldTimer::getMSTime() - itr->second > freezeDetectTime)
        {
            if (MapIDs const* mapPair = GetMapPairByThreadId(itr->first))
            {
                sLog.outCrash("MapUpdater::FreezeDetect thread "I64FMT" possible freezed (is update map %u instance %u). Killing.",itr->first,mapPair->first, mapPair->second);
                DEBUG_LOG("MapUpdater::FreezeDetect thread "I64FMT" possible freezed (is update map %u instance %u). Killing.",itr->first,mapPair->first, mapPair->second);
                ACE_OS::thr_kill(itr->first, SIGABRT);
            }
        }
    }
}

void MapUpdater::MapBrokenEvent(MapIDs const* mapPair)
{
    if (!m_brokendata.empty())
    {
        MapBrokenDataMap::iterator itr =  m_brokendata.find(*mapPair);
        if (itr != m_brokendata.end())
        {
            if ((time(NULL) - itr->second.lastErrorTime) > brokenEventTime)
                itr->second.Reset();
            else
                itr->second.IncreaseCount();

            return;
        }
    }
    m_brokendata.insert(std::make_pair(*mapPair, MapBrokenData()));
}

MapBrokenData const* MapUpdater::GetMapBrokenData(MapIDs const* mapPair)
{
    if (!m_brokendata.empty())
    {
        MapBrokenDataMap::const_iterator itr =  m_brokendata.find(*mapPair);
        if (itr != m_brokendata.end())
            return &itr->second;
    }
    return NULL;
}
