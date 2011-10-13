#include "VMapCluster.h"
#include "Config/Config.h"
#include "ByteBuffer.h"
#include "IVMapManager.h"
#include "VMapFactory.h"

#include <stdio.h>
#include <ace/Process.h>
#include <ace/SPIPE_Acceptor.h>
#include <ace/SPIPE_Connector.h>
#include <ace/SPIPE_Addr.h>
#include <ace/Thread.h>

#include "Policies/SingletonImp.h"
#include "Log.h"
#include "Util.h"
#include "GridDefines.h"

INSTANTIATE_SINGLETON_1(VMAP::LoSProxy);

#define ERROR_CONNECT_NO_PIPE   2

#define ERROR_EOF_ON_PIPE       109
#define ERROR_MORE_DATA_IN_PIPE 234

#define sLog Logger::Instance()

class Logger
{
public:
    static Logger& Instance() { return logger; }
    static Logger logger;
    void outError(const char *fmt, ...) 
    {
        UTF8PRINTF(stdout, fmt,);
        printf("\n");
    }
};

Logger Logger::logger = Logger();

ACE_THR_FUNC_RETURN VMapClusterTest(void *arg)
{
        VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(true);

//        ACE_OS::sleep(urand(1, 2));
        printf("Running test1\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 10, 10, 10, 20, 20, 20);
        
        printf("Running test2\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 30, 30, 30, 20, 20, 20);
        //ACE_OS::sleep(2);
        printf("Running test3\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 40, 40, 40, 20, 20, 20);
        //ACE_OS::sleep(2);
        printf("Running test4\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 10, 10, 10, 50, 50, 50);
        //ACE_OS::sleep(2);
        printf("Test finished\n");

        return 0;
}

namespace VMAP
{
    void VMapClusterManager::RunTest()
    {
        ACE_thread_t tids[100];
        ACE_hthread_t htids[100];
        int k = 6;

        int n = ACE_Thread::spawn_n(tids, k, &VMapClusterTest, 0, THR_NEW_LWP|THR_JOINABLE, 0, 0, 0, htids);
        if(n != k)
            sLog.outError("VMapClusterManager::RunTest(): started only %d out of %d threads because of error %d", n, k, ACE_OS::last_error());


        for(int i = 0; i < n; i++)
            if(ACE_Thread::join(htids[i]) == -1)
                sLog.outError("VMapClusterManager::RunTest(): failed to join thread id=%d tid=%d because of error %d", i, tids[i], ACE_OS::last_error());
        
        //ACE_OS::sleep(15);   
    }


    int VMapClusterManager::SpawnVMapProcesses(const char* runnable, const char* cfg_file, int count)
    {
        SpawnVMapProcess(runnable, cfg_file, VMAP_CLUSTER_MANAGER_PROCESS);
        for(int i = 0; i < count; i++)
            SpawnVMapProcess(runnable, cfg_file, VMAP_CLUSTER_PROCESS, i);

        VMAP::VMapFactory::createOrGetVMapManager()->setEnableClusterComputing(true);

        return 0;
    }

    int VMapClusterManager::SpawnVMapProcess(const char* runnable, const char* cfg_file, const char* name, int32 id)
    {
        ACE_Process process;
        ACE_Process_Options options;
        if (id >= 0)
            options.command_line("%s -c \"%s\" -p %s -i %d", runnable, cfg_file, name, id);
        else
            options.command_line("%s -c \"%s\" -p %s", runnable, cfg_file, name);

        if(process.spawn(options) == -1)
            sLog.outError("SpawnVMapProcess: failed to create process %s with id %d because of error %d", name, id, ACE_OS::last_error());

        return 0;
    }

    VMapClusterManager::VMapClusterManager(uint32 processNumber) : m_processNumber(processNumber)
    {
        for(uint32 i = 0; i < processNumber; i++)
        {
            LoSProcess* process = new LoSProcess();
            process->GetOutPipe()->Connect(VMAP_CLUSTER_PROCESS, i);
            process->GetInPipe()->Accept(VMAP_CLUSTER_PROCESS_REPLY, i);
            m_losProcess.push_back(process);
        }
        printf("VMapClusterManager connected to %u vmap processes\n", processNumber);

        m_coreStream.Accept(VMAP_CLUSTER_MANAGER_PROCESS);
        printf("VMapClusterManager connected to main core process\n");
    }


    VMapClusterManager::~VMapClusterManager()
    {
        while(!m_losProcess.empty())
        {
            delete m_losProcess.front();
            m_losProcess.pop_front();
        }

        while(!m_callbackStreams.empty())
        {
            ThreadCallback::iterator it = m_callbackStreams.begin();
            delete (*it).second;
            m_callbackStreams.erase(it);
        }
    }

    LoSProcess* VMapClusterManager::FindProcess()
    {
        Guard g(m_processLock);
        if(!g.locked())
        {
            sLog.outError("FindProcess: failed to aquire lock");
            return NULL;
        }
        std::list<LoSProcess*>::iterator it;
        for(it = m_losProcess.begin(); it != m_losProcess.end(); ++it)
        {
            if(!(*it)->InUse())
            {
                (*it)->SetInUse(true);
                return *it;
            }
        }
        return NULL;
    }

    PipeWrapper* VMapClusterManager::GetCallbackPipe(ACE_thread_t tid)
    {
        ThreadCallback::iterator it = m_callbackStreams.find(tid);
        if (it == m_callbackStreams.end())
        {
            PipeWrapper *pipe = new PipeWrapper();
            pipe->Connect(VMAP_CLUSTER_MANAGER_CALLBACK, tid);
            m_callbackStreams.insert(ThreadCallback::value_type(tid, pipe));
            return pipe;
        } else
            return (*it).second;

    }

    int VMapClusterManager::Start()
    {
        ACE_thread_t tids[100];
        ACE_hthread_t htids[100];

        int n = ACE_Thread::spawn_n(tids, m_processNumber, &VMapClusterManager::RunThread, this, THR_NEW_LWP|THR_JOINABLE, 0, 0, 0, htids);
        if(n != m_processNumber)
            sLog.outError("VMapClusterManager::Start(): started only %d out of %d threads because of error %d", n, m_processNumber, ACE_OS::last_error());

        //ACE_OS::sleep(2); // FIXME: remove
        for(int i = 0; i < n; i++)
            if(ACE_Thread::join(htids[i]) == -1)
                sLog.outError("VMapClusterManager::Start(): failed to join thread id=%d tid=%d because of error %d", i, tids[i], ACE_OS::last_error());
            else
                printf("Joined with thread\n");
        
        //ACE_OS::sleep(15);
        
        return 0;
    }

    ACE_THR_FUNC_RETURN VMapClusterManager::RunThread(void *param)
    {
        ((VMapClusterManager*)param)->Run();

        return (ACE_THR_FUNC_RETURN)0;
    }

    void VMapClusterManager::SendFailCode(ACE_thread_t tid)
    {
        ByteBuffer packet;
        packet << (uint8)2;
        packet << (uint8)2;
        PipeWrapper *pipe = GetCallbackPipe(tid);
        pipe->SendPacket(packet);
    }

    void VMapClusterManager::Run()
    {
        printf("VMapClusterManager::Run()\n");
        ACE_thread_t tid;
        PipeWrapper *pipe;
        LoSProcess *process;
        ByteBuffer packet;
        while(true)
        {
            printf("VMapClusterManager::about to recv\n");
            packet = m_coreStream.RecvPacket();
            if(m_coreStream.Eof())
            {
                printf("VMapClusterManager:eof\n");
                return;
            }
            printf("VMapClusterManager::recved\n");

            if(packet.size() != 1+4+4+sizeof(float)*6)
            {
                sLog.outError("VMapClusterManager::Run(): received packet with invalid size %d (%d)", packet.size(),  1+4+4+sizeof(float)*6);
                return;
            }
            packet.read_skip<uint8>();
            tid = packet.read<uint32>();

            packet.rpos(0);
            printf("VMapClusterManager::about to find\n");
            process = FindProcess();

            if(!process) 
            {
                SendFailCode(tid);
                sLog.outError("VMapClusterManager::Run(): failed to find free vmap process", packet.size());
                return;
            }
            printf("VMapClusterManager::about to found\n");
            process->GetOutPipe()->SendPacket(packet);
            ACE_Thread::yield();

            packet = process->GetInPipe()->RecvPacket();
            process->SetInUse(false);

            if(process->GetInPipe()->Eof())
            {
                printf("VMapClusterManager:eof\n");
                SendFailCode(tid);
                return;
            }
            
            if(packet.size() != 2)
            {
                SendFailCode(tid);
                sLog.outError("VMapClusterManager::Run(): received packet with invalid size %d (2)", packet.size());
                return;
            }

            pipe = GetCallbackPipe(tid);
            pipe->SendPacket(packet);
        }

        printf("Exiting cluster process manager\n");
        return;
    }

    VMapClusterProcess::VMapClusterProcess(uint32 processId) : m_processId(processId) 
    {
        m_dataPath = sConfig.GetStringDefault("DataDir","./");
        if (m_dataPath.at(m_dataPath.length()-1)!='/' && m_dataPath.at(m_dataPath.length()-1)!='\\')
            m_dataPath.append("/");


        m_inPipe.Accept(VMAP_CLUSTER_PROCESS, processId);
        m_outPipe.Connect(VMAP_CLUSTER_PROCESS_REPLY, processId);
        printf(VMAP_CLUSTER_PROCESS"_%d connected\n", processId);
    }

    VMapClusterProcess::~VMapClusterProcess()
    {
        while(!m_gridLoaded.empty())
        {
            GridLoadedMap::iterator it = m_gridLoaded.begin();
            delete [] (*it).second;
            m_gridLoaded.erase(it);
        }
    }

    void VMapClusterProcess::EnsureVMapLoaded(uint32 mapId, float x, float y)
    {
        VMAP::IVMapManager *vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
        GridLoadedMap::iterator it = m_gridLoaded.find(mapId);
        bool *grid;
        if (it == m_gridLoaded.end())
        {
            grid = new bool[MAX_NUMBER_OF_GRIDS*MAX_NUMBER_OF_GRIDS];
            for(int i = 0; i < MAX_NUMBER_OF_GRIDS*MAX_NUMBER_OF_GRIDS; i++)
                grid[i] = false;
            m_gridLoaded.insert(GridLoadedMap::value_type(mapId, grid));
            char buff[20];
            sprintf(buff, "%d", mapId);
            vMapManager->setLOSonmaps(buff);
        } 
        else
            grid = it->second;

        GridPair p = Trinity::ComputeGridPair(x, y);
        if (!grid[p.y_coord*MAX_NUMBER_OF_GRIDS+p.x_coord])
        {
            int gx = (MAX_NUMBER_OF_GRIDS - 1) - p.x_coord;
            int gy = (MAX_NUMBER_OF_GRIDS - 1) - p.y_coord;
            VMAPLoadResult res = vMapManager->loadMap((m_dataPath + "vmaps").c_str(),  mapId, gx, gy);
            printf("VMAP LOAD: %d\n", res); // TODO: log errors
            grid[p.y_coord*MAX_NUMBER_OF_GRIDS+p.x_coord] = true;
        }

    }

    int VMapClusterProcess::Run()
    {
        printf("VMapClusterProcess::Run()\n");
        ByteBuffer packet;
        uint32 mapId;
        float x1, y1, z1, x2, y2, z2;

        VMAP::IVMapManager *vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
        vMapManager->setEnableLineOfSightCalc(true);

        while(true)
        {
            packet = m_inPipe.RecvPacket();
            if(m_inPipe.Eof())
            {
                printf(VMAP_CLUSTER_PROCESS"_%d exiting process because of EOF encountered on pipe\n", m_processId);
                return 0;
            }
            if(packet.size() != 1+4+4+sizeof(float)*6)
            {
                sLog.outError("VMapClusterProcess::Run(): received packet with invalid size %d (%d)", packet.size(), 1+4+4+sizeof(float)*6);
                return 0;
            }
            packet.read_skip(1+4);
            packet >> mapId >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            printf(VMAP_CLUSTER_PROCESS"_%d received packet %d %f %f %f %f %f %f\n", m_processId, mapId, x1, y1, z1, x2, y2, z2);

            EnsureVMapLoaded(mapId, x1, y1);
            EnsureVMapLoaded(mapId, x2, y2);
            EnsureVMapLoaded(mapId, x2, y1);
            EnsureVMapLoaded(mapId, x1, y2);
            bool res = vMapManager->isInLineOfSight2(mapId, x1, y1, z1, x2, y2, z2);

            printf("RES: %d\n", (int32)res);

            printf(VMAP_CLUSTER_PROCESS"_%d finished vmap computing\n", m_processId);
            packet.clear();
            packet << (uint8)2;
            packet << (uint8)res;
            m_outPipe.SendPacket(packet);
            printf(VMAP_CLUSTER_PROCESS"_%d send packet\n", m_processId);
        }
        return 0;
    }

    void PipeWrapper::Connect(const char* name, int32 id)
    {
        if(m_connected)
            return;

        ACE_SPIPE_Addr addr;
        if (id >= 0)
        {
            char addr_buf[50];
            sprintf(addr_buf, "%s_%u", name, id);
            addr.set(addr_buf);
        } 
        else
            addr.set(name);

        ACE_SPIPE_Connector connetor = ACE_SPIPE_Connector();

        // block until connected
        while(true)
        {
            if (connetor.connect(m_stream, addr) == -1)
            {
                if(ACE_OS::last_error() != ERROR_CONNECT_NO_PIPE)
                {
                    sLog.outError("Connect: failed to connect to stream %s because of error %d", addr.get_path_name(), ACE_OS::last_error());
                    return;
                }
            }
            else
            {
                m_connected = true;
                return;
            }
            ACE_Thread::yield();
        }

    }

    void PipeWrapper::Accept(const char* name, int32 id)
    {
        if(m_connected)
            return;

        ACE_SPIPE_Addr addr;
        if (id >= 0)
        {
            char addr_buf[50];
            sprintf(addr_buf, "%s_%u", name, id);
            addr.set(addr_buf);
        } 
        else
            addr.set(name);

        ACE_SPIPE_Acceptor acceptor = ACE_SPIPE_Acceptor(addr);
        if(acceptor.accept(m_stream) == -1)
            sLog.outError("Accept: failed to accept on stream %s becaus of error %d", addr.get_path_name(), ACE_OS::last_error());
        else
            m_connected = true;
    }

    ByteBuffer SynchronizedPipeWrapper::RecvPacket()
    {
        Guard g(m_readLock);
        if(!g.locked())
        {
            ByteBuffer packet;
            sLog.outError("RecvPacket: failed to aquire lock");
            m_eof = true;
            return packet;
        }
        printf("SynchronizedPipeWrapper::RecvPacket() into recv\n");
        return PipeWrapper::RecvPacket();
    }

    bool PipeWrapper::recv(ByteBuffer &packet, uint32 size)
    {
        int n;
        while(true)
        {
            n = m_stream.recv_n(m_buffer, size);
            if (n < 0) 
            {
                int code = ACE_OS::last_error();
                if(code == ERROR_EOF_ON_PIPE)
                {
                    m_eof = true;
                    return false;
                } 
                else if(code == ERROR_MORE_DATA_IN_PIPE) 
                {
                    // ignore error
                }
                else 
                {
                    sLog.outError("recv: failed to recv data from stream because of error %d", code);
                    m_eof = true;
                    return false;
                }
            } 
            else if(n == 0)
            {
                ACE_Thread::yield();
                printf("[DEBUG]Recv_n n == 0\n");
                continue;
            }
            break;
        }

        for(uint32 i = 0; i < size; i++)
            packet << m_buffer[i];

        return true;
    }

    ByteBuffer PipeWrapper::RecvPacket()
    {
        ByteBuffer packet;
        uint8 size;

        if(!recv(packet, 1))
            return packet;

        size = m_buffer[0];

        if (size > 1)
            if(!recv(packet, size - 1))
                return packet;
        return packet;
    }

    void SynchronizedPipeWrapper::SendPacket(ByteBuffer &packet)
    {
        Guard g(m_sendLock);
        if(!g.locked())
            sLog.outError("SendPacket: failed to aquire lock, unintended bahaviour possible");

        return PipeWrapper::SendPacket(packet);
    }

    void PipeWrapper::SendPacket(ByteBuffer &packet)
    {
        uint32 len = packet.size();
        uint8 *buf = new uint8[len];
        packet.read(buf, len);
        packet.rpos(0);

        uint32 offset = 0;
        while(offset < len)
            offset += m_stream.send(buf + offset, len - offset);

        delete [] buf;
    }


    LoSProxy::~LoSProxy()
    {
        for(ThreadCallback::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
            delete (*it).second;
    }

    bool LoSProxy::isInLineOfSight(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2)
    { 
        printf("[LoSProxy]isInLineOfSight\n");
        {
            Guard g(m_pipeLock);
            if(!g.locked())
                sLog.outError("LoSProxy::isInLineOfSight: failed to aquire pipe lock, unintended bahaviour possible\n");
            if (!m_requester.IsConnected())
                m_requester.Connect(VMAP_CLUSTER_MANAGER_PROCESS);
        }

        ACE_thread_t tid = ACE_Thread::self();
        ByteBuffer packet;
        packet << (uint8)(1+4+4+sizeof(float)*6);
        packet << (uint32)tid;
        packet << (uint32)pMapId;
        packet << x1 << y1 << z1 << x2 << y2 << z2;

        printf("[LoSProxy]About to send packet\n");
        m_requester.SendPacket(packet);
        printf("[LoSProxy]Packet send\n");

        PipeWrapper *pipe;
        {
            Guard g(m_callbackLock);
            if(!g.locked())
                 sLog.outError("LoSProxy::isInLineOfSight: failed to aquire callback lock, unintended bahaviour possible\n");
            ThreadCallback::iterator it = m_callbacks.find(tid);
            if (it == m_callbacks.end())
            {
                pipe = new PipeWrapper();
                pipe->Accept(VMAP_CLUSTER_MANAGER_CALLBACK, tid);
                printf("[LoSProxy]Callback accepted\n");
                m_callbacks.insert(ThreadCallback::value_type(tid, pipe));
            } else
                pipe = (*it).second;
        }

        packet = pipe->RecvPacket();
        printf("[LoSProxy]Packet recv\n");
        packet.read_skip(1);
        uint8 response;
        packet >> response;
        if (response == 2)
        {
            sLog.outError("LoSProxy::isInLineOfSight: cluster failed to check line of sight, checking locally");
            return VMapFactory::createOrGetVMapManager()->isInLineOfSight2(pMapId, x1, y1, z1, x2, y2, z2);
        }
        bool a = VMapFactory::createOrGetVMapManager()->isInLineOfSight2(pMapId, x1, y1, z1, x2, y2, z2);
        if(a != (bool)response)
            printf("!!!!!!!!!!!!!!!!!!error %d %d\n", (int32)a, (int32)response);
        return response;
    }
}