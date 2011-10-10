#include "VMapCluster.h"
#include "Config/Config.h"

#include <stdio.h>
#include <ace/Process.h>

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
    int VMapClusterManager::Run()
    {
        printf("\n\n*****************************************************\nVMapClusterManager::Run()\n********************************************\n\n");
        return 0;
    }


/*
    VMapClusterProcess::VMapClusterProcess()
    {


    }
*/
    int VMapClusterProcess::Run()
    {
        printf("\n\n***************************************************************\nVMapClusterProcess::Run() %u\n************************************************\n\n", m_processId);
        return 0;
    }

}