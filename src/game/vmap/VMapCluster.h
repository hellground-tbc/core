#ifndef _VMAPCLUSTER_H
#define _VMAPCLUSTER_H

#include "Common.h"

#include <ace/SPIPE_Stream.h>

#define VMAP_CLUSTER_MANAGER_PROCESS    "VMAP_MANAGER"
#define VMAP_CLUSTER_PROCESS            "VMAP_PROCESS"

class ByteBuffer;

namespace VMAP
{
    class PipeWrapper 
    {
    public:
        explicit PipeWrapper() : m_counter(0), m_bufferSize(0), m_eof(false), m_connected(false) {}
        ByteBuffer RecvPacket();
        void SendPacket(ByteBuffer packet);
        bool Eof() { return m_eof; }

        void Connect(const char* name, int32 id = -1);
        void Accept(const char* name, int32 id = -1);

    private:
        int32 m_counter;
        int32 m_bufferSize;
        uint8 m_buffer[100];
        bool m_eof;
        bool m_connected;

        ACE_SPIPE_Stream m_stream;
    };


    class TRINITY_DLL_DECL VMapClusterManager
    {
    public:
        explicit VMapClusterManager() {}
        ~VMapClusterManager();

        int Run();

        void SpawnClusterProcess(int count);

        static int SpawnVMapProcesses(const char* runnable, const char* cfg_file);

    private:

        ACE_SPIPE_Stream m_coreStream;
        std::list<PipeWrapper*> m_vmapPipe;
        std::list<ACE_SPIPE_Stream> m_callbackStreams;


        static int SpawnVMapProcess(const char* runnable, const char* cfg_file, const char* name, int32 id = -1);
    };

    class TRINITY_DLL_DECL VMapClusterProcess
    {
    public:
        explicit VMapClusterProcess(uint32 processId) : m_processId(processId) {}

        int Run();

        

    private:
        uint32 m_processId;
        PipeWrapper m_pipe;
    };

}

#endif
