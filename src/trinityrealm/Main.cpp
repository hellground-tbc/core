/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/// \addtogroup realmd Realm Daemon
/// @{
/// \file

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "RealmList.h"

#include "Config/Config.h"
#include "Log.h"
#include "sockets/ListenSocket.h"
#include "AuthSocket.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Util.h"

#include <ace/Get_Opt.h>

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _REALMDCONFVERSION
# define _REALMDCONFVERSION 2011092901
#endif

#ifndef _TRINITY_REALM_CONFIG
# define _TRINITY_REALM_CONFIG  "trinityrealm.conf"
#endif //_TRINITY_REALM_CONFIG

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[] = "realmd";
char serviceLongName[] = "Trinity realm service";
char serviceDescription[] = "Massive Network Game Object Server";
#else
#include "PosixDaemon.h"
#endif

/*
 *  0 - not in daemon/service mode
 *  1 - windows service stopped
 *  2 - windows service running
 *  3 - windows service paused
 *  6 - linux daemon
 */

extern RunModes runMode = MODE_NORMAL;

// FG: for WorldTimer::getMSTime()
#include "Timer.h"

bool StartDB();
void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     ///< Setting it to true stops the server
RealmList m_realmList;                                      ///< Holds the list of realms for this server

DatabaseType LoginDatabase;                                 ///< Accessor to the realm server database

/// Print out the usage string for this program on the console.
void usage(const char *prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
        "    -v, --version            print version and exist\n\r"
        "    -c config_file           use config_file as configuration file\n\r"
        #ifdef WIN32
        "    Running as service functions:\n\r"
        "    -s run                   run as service\n\r"
        "    -s install               install service\n\r"
        "    -s uninstall             uninstall service\n\r"
        #else
        "    Running as daemon functions:\n\r"
        "    -s run                   run as daemon\n\r"
        "    -s stop                  stop daemon\n\r"
        #endif
        ,prog);
}

/// Launch the realm server
extern int main(int argc, char **argv)
{
    ///- Command line parsing
    char const* cfg_file = _TRINITY_REALM_CONFIG;

    char const *options = ":c:s:";

    ACE_Get_Opt cmd_opts(argc, argv, options);
    cmd_opts.long_option("version", 'v');

    char serviceDaemonMode = '\0';

    int option;
    while ((option = cmd_opts()) != EOF)
    {
        switch (option)
        {
            case 'c':
                cfg_file = cmd_opts.opt_arg();
                break;
            case 'v':
                printf("%s\n", _FULLVERSION);
                return 0;

            case 's':
            {
                const char *mode = cmd_opts.opt_arg();

                if (!strcmp(mode, "run"))
                    serviceDaemonMode = 'r';
#ifdef WIN32
                else if (!strcmp(mode, "install"))
                    serviceDaemonMode = 'i';
                else if (!strcmp(mode, "uninstall"))
                    serviceDaemonMode = 'u';
#else
                else if (!strcmp(mode, "stop"))
                    serviceDaemonMode = 's';
#endif
                else
                {
                    sLog.outError("Runtime-Error: -%c unsupported argument %s", cmd_opts.opt_opt(), mode);
                    usage(argv[0]);
                    return 1;
                }
                break;
            }
            case ':':
                sLog.outError("Runtime-Error: -%c option requires an input argument", cmd_opts.opt_opt());
                usage(argv[0]);
                return 1;
            default:
                sLog.outError("Runtime-Error: bad format of commandline arguments");
                usage(argv[0]);
                return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode)
    {
        case 'i':
            if (WinServiceInstall())
                sLog.outString("Installing service");
            return 1;
        case 'u':
            if (WinServiceUninstall())
                sLog.outString("Uninstalling service");
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif

    if (!sConfig.SetSource(cfg_file))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file);
        return 1;
    }

#ifndef WIN32                                               // posix daemon commands need apply after config read
    switch (serviceDaemonMode)
    {
        case 'r':
            startDaemon("Realm");
            break;
        case 's':
            stopDaemon();
            break;
    }
#endif

    sLog.Initialize();

    sLog.outString("Using configuration file %s.", cfg_file);

    ///- Check the version of the configuration file
    uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if (confVersion < _REALMDCONFVERSION)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: Your trinityrealm.conf version indicates your conf file is out of date!");
        sLog.outError("          Please check for updates, as your current default values may cause");
        sLog.outError("          strange behavior.");
        sLog.outError("*****************************************************************************");
        clock_t pause = 3000 + clock();

        while (pause > clock()) {}
    }

    sLog.outString( "%s (realm-daemon)", _FULLVERSION );
    sLog.outString( "<Ctrl-C> to stop.\n" );

    /// realmd PID file creation
    std::string pidfile = sConfig.GetStringDefault("PidFile", "");
    if(!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if( !pid )
        {
            sLog.outError( "Cannot create PID file %s.\n", pidfile.c_str() );
            return 1;
        }

        sLog.outString( "Daemon PID: %u\n", pid );
    }

    ///- Initialize the database connection
    if(!StartDB())
        return 1;

    ///- Get the list of realms for the server
    m_realmList.Initialize(sConfig.GetIntDefault("RealmsStateUpdateDelay", 20));
    if (m_realmList.size() == 0)
    {
        sLog.outError("No valid realms specified.");
        return 1;
    }

    // cleanup query
    // set expired bans to inactive
    LoginDatabase.BeginTransaction();
    LoginDatabase.Execute("UPDATE account_banned SET active = 0 WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    LoginDatabase.CommitTransaction();

    ///- Launch the listening network socket
    port_t rmport = sConfig.GetIntDefault( "RealmServerPort", DEFAULT_REALMSERVER_PORT );
    std::string bind_ip = sConfig.GetStringDefault("BindIP", "0.0.0.0");

    SocketHandler h;
    ListenSocket<AuthSocket> authListenSocket(h);
    if ( authListenSocket.Bind(bind_ip.c_str(),rmport))
    {
        sLog.outError( "Trinity realm can not bind to %s:%d",bind_ip.c_str(), rmport );
        return 1;
    }

    h.Add(&authListenSocket);

    ///- Catch termination signals
    HookSignals();

    ///- Handle affinity for multiple processors and process priority on Windows
    #ifdef WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = sConfig.GetIntDefault("UseProcessors", 0);
        if(Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if(GetProcessAffinityMask(hProcess,&appAff,&sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if(!curAff )
                {
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for realmd. Accessible processors bitmask (hex): %x",Aff,appAff);
                }
                else
                {
                    if(SetProcessAffinityMask(hProcess,curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x", curAff);
                }
            }
            sLog.outString();
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);

        if(Prio)
        {
            if(SetPriorityClass(hProcess,HIGH_PRIORITY_CLASS))
                sLog.outString("TrinityRealm process priority class set to HIGH");
            else
                sLog.outError("ERROR: Can't set realmd process priority class.");
            sLog.outString();
        }
    }
    #endif

    // maximum counter for next ping
    uint32 numLoops = (sConfig.GetIntDefault( "MaxPingTime", 30 ) * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;
    uint32 last_ping_time = 0;
    uint32 now = WorldTimer::getMSTime();
    uint32 last_ipprops_cleanup = 0;

#ifndef WIN32
    detachDaemon();
#endif

    ///- Wait for termination signal
    while (!stopEvent)
    {

        h.Select(0, 100000);
        now = WorldTimer::getMSTime();

        if( (++loopCounter) == numLoops )
        {
            // FG: protect against network system overloading
            // if that happens, force realmd close (autorestarter ftw!)

            if(WorldTimer::getMSTimeDiff(last_ping_time, now) < 10000)
            {
                sLog.outError("NETWORK SYSTEM OVERLOAD");
                raise(SIGSEGV); // force close
                abort();
            }

            last_ping_time = now;
            loopCounter = 0;
            sLog.outDetail("Ping MySQL to keep connection alive");
            LoginDatabase.Ping();
        }

        // FG: clear flood protect buffer periodically
        if(WorldTimer::getMSTimeDiff(last_ipprops_cleanup, now) > 30000) // flush stored IPs every 30 secs
        {
            last_ipprops_cleanup = now;
            uint32 flushed = 0, blocked = 0, stored = 0;
            CleanupIPPropmap(flushed, blocked, stored);
            sLog.outDetail("IPProp: Flushed %u total, %u of them blocked, now %u stored", flushed, blocked, stored);
        }

#ifdef WIN32
        if (runMode == MODE_SERVICE_STOPPED)
            stopEvent = true;

        while (runMode == MODE_SERVICE_PAUSED)
            Sleep(1000);
#endif
    }

    ///- Wait for the delay thread to exit
    LoginDatabase.HaltDelayThread();

    ///- Remove signal handling before leaving
    UnhookSignals();

    sLog.outString( "Halting process..." );
    return 0;
}

/// Handle termination signals
/** Put the global variable stopEvent to 'true' if a termination signal is caught **/
void OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
        //case SIGTERM:
            stopEvent = true;
            break;
        #ifdef _WIN32
        case SIGBREAK:
            stopEvent = true;
            break;
        #endif
    }

    signal(s, OnSignal);
}

/// Initialize connection to the database
bool StartDB()
{
    std::string dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Database not specified");
        return false;
    }

    sLog.outString("Database: %s", dbstring.c_str() );
    if(!LoginDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to database");
        return false;
    }

    return true;
}

/// Define hook 'OnSignal' for all termination signals
void HookSignals()
{
    signal(SIGINT, OnSignal);
    signal(SIGTERM, OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, OnSignal);
    #endif
}

/// Unhook the signals before leaving
void UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
    #ifdef _WIN32
    signal(SIGBREAK, 0);
    #endif
}

/// @}

