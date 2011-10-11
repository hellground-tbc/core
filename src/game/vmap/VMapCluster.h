#ifndef _VMAPCLUSTER_H
#define _VMAPCLUSTER_H

#include "Common.h"

#include <ace/SPIPE_Stream.h>

#define VMAP_CLUSTER_MANAGER_PROCESS    "VMAP_MANAGER"
#define VMAP_CLUSTER_PROCESS            "VMAP_PROCESS"
#define VMAP_CLUSTER_MANAGER_CALLBACK   "VMAP_CALLBACK"

class ByteBuffer;

namespace VMAP
{
    class PipeWrapper 
    {
    public:
        explicit PipeWrapper() : m_counter(0), m_bufferSize(0), m_eof(false), m_connected(false) {}
        ByteBuffer RecvPacket();
        void SendPacket(ByteBuffer &packet);
        bool Eof() { return m_eof; }

        void Connect(const char* name, int32 id = -1);
        void Accept(const char* name, int32 id = -1);

        bool IsConnected() { return m_connected; }

    private:
        int32 m_counter;
        int32 m_bufferSize;
        uint8 m_buffer[100];
        bool m_eof;
        bool m_connected;

        ACE_SPIPE_Stream m_stream;
    };

    class LoSProcess
    {
    public:
        explicit LoSProcess() : m_inUse(false) {}

        PipeWrapper* GetPipe() { return &m_pipe; }

        void SetInUse(bool inUse) { m_inUse = inUse; }
        bool InUse() { return m_inUse; }

    private:
        PipeWrapper m_pipe;
        bool m_inUse;

    };

    typedef UNORDERED_MAP<ACE_thread_t, PipeWrapper*> ThreadCallback;

    class LoSProxy
    {
    public:
        explicit LoSProxy() {}
        ~LoSProxy();

        bool isInLineOfSight(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2);

    private:
        ThreadCallback m_callbacks;
        PipeWrapper m_requester;
    };

    class TRINITY_DLL_DECL VMapClusterManager
    {
    public:
        explicit VMapClusterManager(uint32 processNumber);
        ~VMapClusterManager();

        int Run();

        LoSProcess* FindProcess();
        PipeWrapper* GetCallbackPipe(ACE_thread_t tid);

        static int SpawnVMapProcesses(const char* runnable, const char* cfg_file, int count);
        static void RunTest();

    private:
        uint32 m_processNumber;

        PipeWrapper m_coreStream;
        std::list<LoSProcess*> m_losProcess;
        ThreadCallback m_callbackStreams;


        static int SpawnVMapProcess(const char* runnable, const char* cfg_file, const char* name, int32 id = -1);
    };

    class TRINITY_DLL_DECL VMapClusterProcess
    {
    public:
        explicit VMapClusterProcess(uint32 processId);

        int Run();

        
    private:
        uint32 m_processId;
        PipeWrapper m_pipe;
    };
}

#define sLoSProxy Trinity::Singleton<LoSProxy>::Instance()


#endif
