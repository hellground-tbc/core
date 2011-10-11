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

INSTANTIATE_SINGLETON_1(VMAP::LoSProxy);

namespace VMAP
{
    void VMapClusterManager::RunTest()
    {
        VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(true);
        printf("Running test1\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 10, 10, 10, 20, 20, 20);
        ACE_OS::sleep(2);
        printf("Running test2\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 30, 30, 30, 20, 20, 20);
        ACE_OS::sleep(2);
        printf("Running test3\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 40, 40, 40, 20, 20, 20);
        ACE_OS::sleep(2);
        printf("Running test4\n");
        VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(0, 10, 10, 10, 50, 50, 50);
        ACE_OS::sleep(2);
        printf("Test finished\n");
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
        process.spawn(options);

        return 0;
    }

    VMapClusterManager::VMapClusterManager(uint32 processNumber) : m_processNumber(processNumber)
    {
        for(uint32 i = 0; i < processNumber; i++)
        {
            LoSProcess* process = new LoSProcess();
            process->GetPipe()->Connect(VMAP_CLUSTER_PROCESS, i);
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

     /*   while(!m_threads.empty())
        {
            delete m_threads.front();
            m_threads.pop_front();
        }
*/
    }

    LoSProcess* VMapClusterManager::FindProcess()
    {
        Guard g(m_processLock);
        if(!g.locked())
            return NULL; // handle error
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
        int n = ACE_Thread::spawn_n(tids, m_processNumber, &VMapClusterManager::RunThread, this, THR_NEW_LWP|THR_JOINABLE);
        if(n != m_processNumber)
            printf("Started %d out of %d processes because of error %d\n", n, m_processNumber, ACE_OS::last_error());

        ACE_OS::sleep(2);
        for(int i = 0; i < n; i++)
            if(ACE_Thread::join(tids[i], 0, 0) == -1)
                printf("Failed to join thread id=%d tid=%d because of error %d\n", i, tids[i], ACE_OS::last_error());
        
        ACE_OS::sleep(15);
        
        return 0;
    }

    ACE_THR_FUNC_RETURN VMapClusterManager::RunThread(void *param)
    {
        ((VMapClusterManager*)param)->Run();

        return (ACE_THR_FUNC_RETURN)0;
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
            printf("1\n");
            packet = m_coreStream.RecvPacket();
            printf("2\n");
            if(m_coreStream.Eof())
            {
                printf(VMAP_CLUSTER_MANAGER_PROCESS" pipe. EOF on pipe encountered. Returning...\n", packet.size());
                return;
            }

            if(packet.size() != 1+4+4+sizeof(float)*6)
            {
                printf(VMAP_CLUSTER_MANAGER_PROCESS" pipe. Received packet with invalid size %d\n", packet.size());
                return;
            }
            printf("3\n");
            packet.read_skip<uint8>();
            tid = packet.read<uint32>();

            printf("4\n");
            packet.rpos(0);
            process = FindProcess();

            printf("5\n");
            if(!process) 
            {// TODO: send failure code and log this
                printf("Failed to find free vmap process, exiting thread...\n");
                return;
            }
            printf("6\n");
            process->GetPipe()->SendPacket(packet);
            printf("7\n");
            ACE_Thread::yield();
            printf("8\n");
            packet = process->GetPipe()->RecvPacket();
            printf("9\n");
            process->SetInUse(false);

            if(packet.size() != 2)
            {
                printf(VMAP_CLUSTER_PROCESS" pipe. Received packet with invalid size %d\n", packet.size());
                return;
            }
            printf("10\n");
            pipe = GetCallbackPipe(tid);
            printf("11\n");
            pipe->SendPacket(packet);
            printf("12\n");
        }

        printf("Exiting cluster process manager\n");
        return;
    }


    VMapClusterProcess::VMapClusterProcess(uint32 processId) : m_processId(processId) 
    {
        m_pipe.Accept(VMAP_CLUSTER_PROCESS, processId);
        printf(VMAP_CLUSTER_PROCESS"_%d accepted\n", processId);
    }

    int VMapClusterProcess::Run()
    {
        printf("VMapClusterProcess::Run()\n");
        ByteBuffer packet;
        uint32 mapId;
        float x1, y1, z1, x2, y2, z2;

        VMAP::IVMapManager *vMapManager = VMAP::VMapFactory::createOrGetVMapManager();

        while(true)
        {
            packet = m_pipe.RecvPacket();
            if(m_pipe.Eof())
            {
                printf(VMAP_CLUSTER_PROCESS"_%d exiting process because of EOF encountered on pipe\n", m_processId);
                return 0;
            }
            packet.read_skip(1+4);
            packet >> mapId >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
            printf(VMAP_CLUSTER_PROCESS"_%d received packet %d %f %f %f %f %f %f\n", m_processId, mapId, x1, y1, z1, x2, y2, z2);

            bool res = vMapManager->isInLineOfSight(mapId, x1, y1, z1, x2, y2, z2);

            printf(VMAP_CLUSTER_PROCESS"_%d finished vmap computing\n", m_processId);
            packet.clear();
            packet << (uint8)2;
            packet << (uint8)res;
            m_pipe.SendPacket(packet);
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
                printf("[DEBUG]Error in ACE_SPIPE_Connector.connect() %d\n", ACE_OS::last_error());
                if(ACE_OS::last_error() != 2) // TODO: how it works on nix?
                {
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
            printf("acceptor.accept error %d", ACE_OS::last_error());
        else
            m_connected = true;
    }

    ByteBuffer SynchronizedPipeWrapper::RecvPacket()
    {
        Guard g(m_readLock);
        if(!g.locked())
        {
             // TODO: handle error
        }
        return PipeWrapper::RecvPacket();
    }

    ByteBuffer PipeWrapper::RecvPacket()
    {
        ByteBuffer packet;
        uint8 size;

        int32 n;
        while(true)
        {
            n = m_stream.recv_n(m_buffer, 1);
            if (n < 0) 
            {
                printf("[DEBUG]Recv_n error %d\n", ACE_OS::last_error());
                if(ACE_OS::last_error() != 234) // TODO:handle eof error,  how it works on *nix?
                {
                    m_eof = true;
                    return packet;
                }
            } else if(n == 0)
            {
                ACE_Thread::yield();
                printf("[DEBUG]Recv_n n == 0\n");
                continue;
            }
            break;
        }

        size = m_buffer[0];
        packet << size;

        if (size > 1)
            while (true)
            {
                n = m_stream.recv_n(m_buffer, size - 1);
                if (n < 0)
                {
                    printf("[DEBUG]Recv_n2 error %d\n", ACE_OS::last_error());
                    if(ACE_OS::last_error() != 234) // TODO:handle eof error,  how it works on *nix?
                    {
                        m_eof = true;
                        return packet;
                    }
                }   
                else if (n == 0)
                {
                    ACE_Thread::yield();
                    printf("[DEBUG]Recv_n2 n == 0\n");
                    continue;
                }
                for(int i = 0; i < size - 1; i++)
                    packet << m_buffer[i];
                break;
            }
        return packet;
    }

    void SynchronizedPipeWrapper::SendPacket(ByteBuffer &packet)
    {
        Guard g(m_sendLock);
        if(!g.locked())
            return; // TODO: handle error
            
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
        if (!m_requester.IsConnected())
            m_requester.Connect(VMAP_CLUSTER_MANAGER_PROCESS);

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
        ThreadCallback::iterator it = m_callbacks.find(tid);
        if (it == m_callbacks.end())
        {
            pipe = new PipeWrapper();
            pipe->Accept(VMAP_CLUSTER_MANAGER_CALLBACK, tid);
            printf("[LoSProxy]Callback accepted\n");
            m_callbacks.insert(ThreadCallback::value_type(tid, pipe));
        } else
            pipe = (*it).second;

        packet = pipe->RecvPacket();
        printf("[LoSProxy]Packet recv\n");
        packet.read_skip(1);
        uint8 response;
        packet >> response;

        return response;
    }
}