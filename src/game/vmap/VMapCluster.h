#ifndef _VMAPCLUSTER_H
#define _VMAPCLUSTER_H

#include "PipeWrapper.h"
#include "Common.h"

#define VMAP_CLUSTER_MANAGER_PROCESS        "VMAP_MANAGER"
#define VMAP_CLUSTER_PROCESS                "VMAP_PROCESS"
#define VMAP_CLUSTER_PROCESS_REPLY          "VMAP_PROCESS_R"
#define VMAP_CLUSTER_MANAGER_CALLBACK       "VMAP_CALLBACK"


namespace VMAP
{
    class LoSProcess
    {
    public:
        explicit LoSProcess() : m_inUse(false) {}

        RecvPipeWrapper* GetInPipe() { return &m_inPipe; }
        SendPipeWrapper* GetOutPipe() { return &m_outPipe; }

        void SetInUse(bool inUse) { m_inUse = inUse; }
        bool InUse() { return m_inUse; }

    private:
        RecvPipeWrapper m_inPipe;
        SendPipeWrapper m_outPipe;
        bool m_inUse;

    };

    typedef UNORDERED_MAP<ACE_thread_t, SendPipeWrapper*> ThreadSendCallback;
    typedef UNORDERED_MAP<ACE_thread_t, RecvPipeWrapper*> ThreadRecvCallback;

    class LoSProxy
    {
    public:
        explicit LoSProxy() {}
        ~LoSProxy();

        bool isInLineOfSight(unsigned int pMapId, float x1, float y1, float z1, float x2, float y2, float z2);

    private:
        ThreadRecvCallback m_callbacks;
        SynchronizedSendPipeWrapper m_requester;
        LockType m_lock;
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

        SynchronizedRecvPipeWrapper m_coreStream;
        std::list<LoSProcess*> m_losProcess;
        ThreadSendCallback m_callbackStreams;

        LockType m_processLock;

        LoSProcess* FindProcess();
        SendPipeWrapper* GetCallbackPipe(ACE_thread_t tid);

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
        RecvPipeWrapper m_inPipe;
        SendPipeWrapper m_outPipe;
        GridLoadedMap m_gridLoaded;
        std::string m_dataPath;
    };
}

#define sLoSProxy Trinity::Singleton<LoSProxy>::Instance()


#endif
