#ifndef _VMAPCLUSTER_H
#define _VMAPCLUSTER_H

#include "Common.h"
#include <ace/SPIPE_Stream.h>

#define VMAP_CLUSTER_MANAGER_PROCESS        "VMAP_MANAGER"
#define VMAP_CLUSTER_PROCESS                "VMAP_PROCESS"
#define VMAP_CLUSTER_PROCESS_REPLY          "VMAP_PROCESS_R"
#define VMAP_CLUSTER_MANAGER_CALLBACK       "VMAP_CALLBACK"

class ByteBuffer;

namespace VMAP
{
    typedef ACE_Thread_Mutex LockType;
    typedef ACE_Guard<LockType> Guard;

    class PipeWrapper 
    {
    public:
        explicit PipeWrapper() : m_counter(0), m_bufferSize(0), m_eof(false), m_connected(false) {}
        virtual ByteBuffer RecvPacket();
        virtual void SendPacket(ByteBuffer &packet);
        bool Eof() { return m_eof; }

        void Connect(const char* name, int32 id = -1);
        void Accept(const char* name, int32 id = -1);

        bool IsConnected() { return m_connected; }

    protected:
        bool m_eof;
        bool m_connected;

    private:
        int32 m_counter;
        int32 m_bufferSize;
        uint8 m_buffer[100];

        bool recv(ByteBuffer &packet, uint32 size);

        ACE_SPIPE_Stream m_stream;
    };


    class SynchronizedPipeWrapper : public PipeWrapper
    {
    public:
        explicit SynchronizedPipeWrapper() {}

        ByteBuffer RecvPacket();
        void SendPacket(ByteBuffer &packet);

    private:
        LockType m_readLock;
        LockType m_sendLock;
    };


    class LoSProcess
    {
    public:
        explicit LoSProcess() : m_inUse(false) {}

        PipeWrapper* GetInPipe() { return &m_inPipe; }
        PipeWrapper* GetOutPipe() { return &m_outPipe; }

        void SetInUse(bool inUse) { m_inUse = inUse; }
        bool InUse() { return m_inUse; }

    private:
        PipeWrapper m_inPipe;
        PipeWrapper m_outPipe;
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
        SynchronizedPipeWrapper m_requester;
        LockType m_pipeLock;
        LockType m_callbackLock;
    };

    class TRINITY_DLL_DECL VMapClusterManager
    {
    public:
        explicit VMapClusterManager(uint32 processNumber);
        ~VMapClusterManager();

        int Start();

        static int SpawnVMapProcesses(const char* runnable, const char* cfg_file, int count);
        static void RunTest();

    private:
        uint32 m_processNumber;

        SynchronizedPipeWrapper m_coreStream;
        std::list<LoSProcess*> m_losProcess;
        ThreadCallback m_callbackStreams;

        LockType m_processLock;

        LoSProcess* FindProcess();
        PipeWrapper* GetCallbackPipe(ACE_thread_t tid);

        void SendFailCode(ACE_thread_t tid);

        void Run();
        static ACE_THR_FUNC_RETURN RunThread(void *arg);
        static int SpawnVMapProcess(const char* runnable, const char* cfg_file, const char* name, int32 id = -1);
    };

    typedef UNORDERED_MAP<uint32, bool*> GridLoadedMap;

    class TRINITY_DLL_DECL VMapClusterProcess
    {
    public:
        explicit VMapClusterProcess(uint32 processId);
        ~VMapClusterProcess();

        int Run();
        void EnsureVMapLoaded(uint32 mapId, float x, float y);

    private:
        uint32 m_processId;
        PipeWrapper m_inPipe;
        PipeWrapper m_outPipe;
        GridLoadedMap m_gridLoaded;
        std::string m_dataPath;
    };
}

#define sLoSProxy Trinity::Singleton<LoSProxy>::Instance()


#endif
