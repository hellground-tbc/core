#ifndef _VMAPCLUSTER_H
#define _VMAPCLUSTER_H

#include "Common.h"

#define VMAP_CLUSTER_MANAGER_PROCESS    "VMAP_MANAGER"
#define VMAP_CLUSTER_PROCESS            "VMAP_PROCESS"

namespace VMAP
{

    class TRINITY_DLL_DECL VMapClusterManager
    {
    public:
        explicit VMapClusterManager() {}

        int Run();

        void SpawnClusterProcess(int count);

        static int SpawnVMapProcesses(const char* runnable, const char* cfg_file);

    private:
        std::string runnable;
        std::string cfg_file;

        static int SpawnVMapProcess(const char* runnable, const char* cfg_file, const char* name, int32 id = -1);
    };

    class TRINITY_DLL_DECL VMapClusterProcess
    {
    public:
        explicit VMapClusterProcess(uint32 processId) : m_processId(processId) {}

        int Run();

    private:
        uint32 m_processId;
    };

}

#endif
