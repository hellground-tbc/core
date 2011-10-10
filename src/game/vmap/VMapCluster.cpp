#include "VMapCluster.h"
#include "Config/Config.h"
#include "ByteBuffer.h"

#include <stdio.h>
#include <ace/Process.h>
#include <ace/SPIPE_Acceptor.h>
#include <ace/SPIPE_Connector.h>
#include <ace/SPIPE_Addr.h>


namespace VMAP
{
    int VMapClusterManager::SpawnVMapProcesses(const char* runnable, const char* cfg_file)
    {
        SpawnVMapProcess(runnable, cfg_file, VMAP_CLUSTER_MANAGER_PROCESS);
        int count = sConfig.GetIntDefault("VMAPProcessNumber", 4); // TODO: add to config file
        int i;
        for(i = 0; i < count; i++)
            SpawnVMapProcess(runnable, cfg_file, VMAP_CLUSTER_PROCESS, i);

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
/*
    VMapClusterManager::VMapClusterManager(const char* runnable, const char* cfg_file)
    {
        
    }
*/

    VMapClusterManager::~VMapClusterManager()
    {
        while(!m_vmapPipe.empty())
        {
            delete m_vmapPipe.front();
            m_vmapPipe.pop_front();
        }
    }

    int VMapClusterManager::Run()
    {
        int count = sConfig.GetIntDefault("VMAPProcessNumber", 4);
        for(int i = 0; i < count; i++)
        {
            PipeWrapper* pipe = new PipeWrapper();
            pipe->Connect(VMAP_CLUSTER_PROCESS, i);
            m_vmapPipe.push_back(pipe);

            ByteBuffer packet;
            packet << (uint8)1;
            pipe->SendPacket(packet);
        }

        printf("Exiting cluster process manager\n");
        return 0;
    }




/*
    VMapClusterProcess::VMapClusterProcess()
    {


    }
*/
    int VMapClusterProcess::Run()
    {
        m_pipe.Accept(VMAP_CLUSTER_PROCESS, m_processId);
        ByteBuffer packet;
        while(true)
        {
            packet = m_pipe.RecvPacket();
            if(m_pipe.Eof())
            {
                printf("**EOF**\n");
                return 0;
            }
            printf("RecvPacket size = %u\n", packet[0]);
        }


        printf("Exiting cluster process\n");
        return 0;
    }

    void PipeWrapper::Connect(const char* name, int32 id)
    {
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
                if(ACE_OS::last_error() != 2) // TODO: how it works on nix?
                    return;
            }
            else
                return;
        }

    }

    void PipeWrapper::Accept(const char* name, int32 id)
    {
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
    }

    ByteBuffer PipeWrapper::RecvPacket()
    {
        ByteBuffer packet;
        uint8 size;

        int32 n;
        n = m_stream.recv_n(m_buffer, 1);
        if (n <= 0) 
        {
            m_eof = true;
            return packet;
        }

        size = m_buffer[0];
        packet << size;
        if (size > 1)
        {
            n = m_stream.recv_n(m_buffer, size - 1);
            if (n <= 0)
            {
                m_eof = true;
                return packet;
            }
            for(int i = 0; i < size - 1; i++)
                packet << m_buffer[i];
        }
        return packet;
    }

    void PipeWrapper::SendPacket(ByteBuffer packet)
    {
        uint32 len = packet.size();
        uint8 *buf = new uint8[len];
        packet.read(buf, len);

        uint32 offset = 0;
        while(offset < len)
            offset += m_stream.send(buf + offset, len - offset);

        delete [] buf;
    }

}