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

/** \file
    \ingroup Trinityd
*/

#include <ace/OS_NS_signal.h>

#include "WorldSocketMgr.h"
#include "Common.h"
#include "Master.h"
#include "WorldSocket.h"
#include "WorldRunnable.h"
#include "World.h"
#include "Log.h"
#include "MapManager.h"
#include "Timer.h"
#include "Policies/SingletonImp.h"
#include "SystemConfig.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "CliRunnable.h"
#include "ScriptMgr.h"
#include "Util.h"
#include "InstanceSaveMgr.h"
#include "DBCStores.h"

#ifdef WIN32
#include "ServiceWin32.h"
#else
#include "PosixDaemon.h"
#endif

#include <ace/Stack_Trace.h>

extern RunModes runMode;

/// \todo Warning disabling not useful under VC++2005. Can somebody say on which compiler it is useful?
#pragma warning(disable:4305)

INSTANTIATE_SINGLETON_1( Master );

volatile uint32 Master::m_masterLoopCounter = 0;

class FreezeDetectorRunnable : public ACE_Based::Runnable
{
public:
    FreezeDetectorRunnable() { _delaytime = 0; }
    uint32 m_loops, m_lastchange;
    uint32 w_loops, w_lastchange;
    uint32 _delaytime;
    uint32 freezeCheckPeriod;
    void SetDelayTime(uint32 t) { _delaytime = t; }
    void run(void)
    {
        if(!_delaytime)
            return;
        sLog.outString("Starting up anti-freeze thread (%u seconds max stuck time)...",_delaytime/1000);
        m_loops = 0;
        w_loops = 0;
        m_lastchange = 0;
        w_lastchange = 0;
        freezeCheckPeriod = sWorld.getConfig(CONFIG_VMSS_FREEZECHECKPERIOD);

        while(!World::IsStopped())
        {
            ACE_Based::Thread::Sleep(freezeCheckPeriod);

            sMapMgr.GetMapUpdater()->FreezeDetect();

            uint32 curtime = WorldTimer::getMSTime();
            //DEBUG_LOG("anti-freeze: time=%u, counters=[%u; %u]",curtime,Master::m_masterLoopCounter,World::m_worldLoopCounter);

            // normal work
            if(w_loops != World::m_worldLoopCounter)
            {
                w_lastchange = curtime;
                w_loops = World::m_worldLoopCounter;
            }
            // possible freeze
            else
            {
                if(WorldTimer::getMSTimeDiff(w_lastchange,curtime) > _delaytime)
                {
                    sLog.outError("World Thread hangs, kicking out server!");
                    *((uint32 volatile*)NULL) = 0;                       // bang crash
                }
            }
        }
        sLog.outString("Anti-freeze thread exiting without problems.");
    }
};

Master::Master()
{
}

Master::~Master()
{
}

/// Main function
int Master::Run()
{
    sLog.outString( "%s (core-daemon)", _FULLVERSION );
    sLog.outString( "<Ctrl-C> to stop.\n" );

    sLog.outTitle( " ______                       __");
    sLog.outTitle( "/\\__  _\\       __          __/\\ \\__");
    sLog.outTitle( "\\/_/\\ \\/ _ __ /\\_\\    ___ /\\_\\ \\ ,_\\  __  __");
    sLog.outTitle( "   \\ \\ \\/\\`'__\\/\\ \\ /' _ `\\/\\ \\ \\ \\/ /\\ \\/\\ \\");
    sLog.outTitle( "    \\ \\ \\ \\ \\/ \\ \\ \\/\\ \\/\\ \\ \\ \\ \\ \\_\\ \\ \\_\\ \\");
    sLog.outTitle( "     \\ \\_\\ \\_\\  \\ \\_\\ \\_\\ \\_\\ \\_\\ \\__\\\\/`____ \\");
    sLog.outTitle( "      \\/_/\\/_/   \\/_/\\/_/\\/_/\\/_/\\/__/ `/___/> \\");
    sLog.outTitle( "                                 C O R E  /\\___/");
    sLog.outTitle( "http://TrinityCore.org                    \\/__/\n");

    /// worldd PID file creation
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

#ifndef WIN32
    detachDaemon();
#endif

    ///- Start the databases
    if (!_StartDB())
        return 1;

    ///- Initialize the World
    sWorld.SetInitialWorldSettings();
    //server loaded successfully => enable async DB requests
    //this is done to forbid any async transactions during server startup!
    CharacterDatabase.AllowAsyncTransactions();
    WorldDatabase.AllowAsyncTransactions();
    LoginDatabase.AllowAsyncTransactions();

    ///- Catch termination signals
    _HookSignals();

    ///- Launch WorldRunnable thread
    ACE_Based::Thread t(new WorldRunnable);
    t.setPriority(ACE_Based::Highest);

    // set realmbuilds depend on mangosd expected builds, and set server online
    {
        std::string builds = AcceptableClientBuildsListStr();
        LoginDatabase.escape_string(builds);
        LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags & ~(%u), population = 0, realmbuilds = '%s'  WHERE id = '%u'", REALM_FLAG_OFFLINE, builds.c_str(), realmID);
    }

    // console should be disabled in service/daemon mode
    if (sConfig.GetBoolDefault("Console.Enable", true) && (runMode == MODE_NORMAL))
    {
        ///- Launch CliRunnable thread
        ACE_Based::Thread td1(new CliRunnable);
    }

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
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for Trinityd. Accessible processors bitmask (hex): %x",Aff,appAff);
                }
                else
                {
                    if(SetProcessAffinityMask(hProcess,curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x",curAff);
                }
            }
            sLog.outString();
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);

//        if(Prio && (m_ServiceStatus == -1)/* need set to default process priority class in service mode*/)
        if(Prio)
        {
            if(SetPriorityClass(hProcess,HIGH_PRIORITY_CLASS))
                sLog.outString("TrinityCore process priority class set to HIGH");
            else
                sLog.outError("ERROR: Can't set Trinityd process priority class.");
            sLog.outString();
        }
    }
    #endif

    uint32 realCurrTime, realPrevTime;
    realCurrTime = realPrevTime = WorldTimer::getMSTime();

    uint32 socketSelecttime = sWorld.getConfig(CONFIG_SOCKET_SELECTTIME);

    //server has started up successfully => enable async DB requests
    LoginDatabase.AllowAsyncTransactions();

    // maximum counter for next ping
    uint32 numLoops = (sConfig.GetIntDefault( "MaxPingTime", 30 ) * (MINUTE * 1000000 / socketSelecttime));
    uint32 loopCounter = 0;

    ///- Start up freeze catcher thread
    uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0);
    if(freeze_delay)
    {
        FreezeDetectorRunnable *fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay*1000);
        ACE_Based::Thread t(fdr);
        t.setPriority(ACE_Based::Highest);
    }

#ifdef ANTICHEAT_SOCK
    AntiCheatRunnable *acr = new AntiCheatRunnable();
    if (acr)
        ACE_Based::Thread t2(acr);
#endif

    ///- Launch the world listener socket
    uint16 wsport = sWorld.getConfig(CONFIG_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault ("BindIP", "0.0.0.0");

    if (sWorldSocketMgr->StartNetwork(wsport, bind_ip) == -1)
    {
        sLog.outError ("Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    sWorldSocketMgr->Wait ();

    ///- Set server offline in realmlist
    LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags | %u WHERE id = '%u'", REALM_FLAG_OFFLINE, realmID);

    ///- Remove signal handling before leaving
    _UnhookSignals();

    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    t.wait();

    ///- Clean database before leaving
    clearOnlineAccounts();

    sLog.outString( "Halting process..." );

    #ifdef WIN32
    if (sConfig.GetBoolDefault("Console.Enable", true))
    {
        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        BOOL ret = WriteConsoleInput(hStdIn, b, 4, &numb);
    }
    #endif

    sInstanceSaveManager.UnbindBeforeDelete();

    ///- Wait for delay threads to end
    CharacterDatabase.HaltDelayThread();
    WorldDatabase.HaltDelayThread();
    LoginDatabase.HaltDelayThread();

    // Exit the process with specified return value
    return World::GetExitCode();
}

/// Initialize connection to the databases
bool Master::_StartDB()
{
    ///- Get world database info from configuration file
    std::string dbstring;
    dbstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Database not specified in configuration file");
        return false;
    }
    int nConnections = sConfig.GetIntDefault("WorldDatabaseConnections", 1);
    sLog.outString("World Database: %s, total connections: %i", dbstring.c_str(), nConnections + 1);

    ///- Initialise the world database
    if(!WorldDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to world database %s",dbstring.c_str());
        return false;
    }

    dbstring = sConfig.GetStringDefault("CharacterDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Character Database not specified in configuration file");
        return false;
    }
    nConnections = sConfig.GetIntDefault("CharacterDatabaseConnections", 1);
    sLog.outDetail("Character Database: %s", dbstring.c_str());

    ///- Initialise the Character database
    if(!CharacterDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outString("Character Database: %s, total connections: %i", dbstring.c_str(), nConnections + 1);
        return false;
    }

    ///- Get login database info from configuration file
    dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Login database not specified in configuration file");
        return false;
    }
    nConnections = sConfig.GetIntDefault("LoginDatabaseConnections", 1);
    ///- Initialise the login database
    sLog.outString("Login Database: %s, total connections: %i", dbstring.c_str(), nConnections + 1);
    if(!LoginDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to login database %s",dbstring.c_str());
        return false;
    }

    ///- Get the realm Id from the configuration file
    realmID = sConfig.GetIntDefault("RealmID", 0);
    if(!realmID)
    {
        sLog.outError("Realm ID not defined in configuration file");
        return false;
    }
    sLog.outString("Realm running as realm ID %d", realmID);

    ///- Clean the database before starting
    clearOnlineAccounts();

    ///- Insert version info into DB
    WorldDatabase.PExecute("UPDATE `version` SET `core_version` = '%s', `core_revision` = '%s'", _FULLVERSION, _REVISION);

    sWorld.LoadDBVersion();

    sLog.outString("Using %s", sWorld.GetDBVersion());
    return true;
}

/// Clear 'online' status for all accounts with characters in this realm
void Master::clearOnlineAccounts()
{
    // Cleanup online status for characters hosted at current realm
    /// \todo Only accounts with characters logged on *this* realm should have online status reset. Move the online column from 'account' to 'realmcharacters'?
    LoginDatabase.PExecute(
        "UPDATE account SET online = 0 WHERE online > 0 "
        "AND id IN (SELECT acctid FROM realmcharacters WHERE realmid = '%d')",realmID);


    CharacterDatabase.Execute("UPDATE characters SET online = 0 WHERE online<>0");
}

/// Handle termination signals
void Master::_OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
            World::StopNow(RESTART_EXIT_CODE);
            signal(s, _OnSignal);
            break;
        #ifdef _WIN32
        case SIGBREAK:
            World::StopNow(SHUTDOWN_EXIT_CODE);
            signal(s, _OnSignal);
            break;
        #endif
        case SIGSEGV:
        case SIGABRT:
        case SIGFPE:
            if (sWorld.getConfig(CONFIG_VMSS_ENABLE))
            {
                ACE_thread_t const threadId = ACE_OS::thr_self();
                sLog.outCrash("Signal Handler: Signal %.2u received from thread "I64FMT".\r\n",s,threadId);
                ACE_Stack_Trace StackTrace;
                sLog.outCrash("\r\n************ BackTrace *************\r\n%s\r\n***********************************\r\n",StackTrace.c_str());

                if (MapIDs const* mapPair = sMapMgr.GetMapUpdater()->GetMapPairByThreadId(threadId))
                {
                    sLog.outError("Signal Handler: crushed thread is update map %u instance %u",mapPair->first, mapPair->second);
                    sLog.outCrash("Signal Handler: crushed thread is update map %u instance %u",mapPair->first, mapPair->second);
                    if (Map* map = sMapMgr.FindMap(mapPair->first, mapPair->second))
                        map->SetBroken(true);
                    sMapMgr.GetMapUpdater()->MapBrokenEvent(mapPair);
                    if (sMapMgr.GetMapUpdater()->GetMapBrokenData(mapPair)->count > sWorld.getConfig(CONFIG_VMSS_MAXTHREADBREAKS))
                    {
                        sLog.outError("Signal Handler: Limit of map restarting (map %u instance %u) exceeded. Stopping world!",mapPair->first, mapPair->second);
                        sLog.outCrash("Signal Handler: Limit of map restarting (map %u instance %u) exceeded. Stopping world!",mapPair->first, mapPair->second);
                        signal(s, SIG_DFL);
                        ACE_OS::kill(getpid(), s);
                    }
                    else
                    {
                        sLog.outError("Signal Handler: Restarting virtual map server (map %u instance %u). Count of restarts: %u",mapPair->first, mapPair->second, sMapMgr.GetMapUpdater()->GetMapBrokenData(mapPair)->count);
                        sLog.outCrash("Signal Handler: Restarting virtual map server (map %u instance %u). Count of restarts: %u",mapPair->first, mapPair->second, sMapMgr.GetMapUpdater()->GetMapBrokenData(mapPair)->count);
                        sMapMgr.GetMapUpdater()->unregister_thread(threadId);
                        sMapMgr.GetMapUpdater()->update_finished();
                        sMapMgr.GetMapUpdater()->SetBroken(true);
                        ACE_OS::thr_exit();
                    }
                }
                else
                {
                    sLog.outError("Signal Handler: Thread "I64FMT" is not virtual map server. Stopping world.",threadId);
                    sLog.outCrash("Signal Handler: Thread "I64FMT" is not virtual map server. Stopping world.",threadId);
                    signal(s, SIG_DFL);
                    ACE_OS::kill(getpid(), s);
                }
            }
            else
            {
                signal(s, SIG_DFL);
                ACE_OS::kill(getpid(), s);
            }
            break;
        default:
            signal(s, SIG_DFL);
            break;
    }
}

/// Define hook '_OnSignal' for all termination signals
void Master::_HookSignals()
{
    signal(SIGINT,   _OnSignal);
    signal(SIGTERM,  _OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, _OnSignal);
    #endif
    if (sWorld.getConfig(CONFIG_VMSS_ENABLE))
    {
        signal(SIGSEGV,  _OnSignal);
        signal(SIGABRT,  _OnSignal);
        signal(SIGFPE ,  _OnSignal);
    }
}

/// Unhook the signals before leaving
void Master::_UnhookSignals()
{
    signal(SIGINT,   SIG_DFL);
    signal(SIGTERM,  SIG_DFL);
    #ifdef _WIN32
    signal(SIGBREAK, SIG_DFL);
    #endif
    signal(SIGSEGV,  SIG_DFL);
    signal(SIGABRT,  SIG_DFL);
    signal(SIGFPE ,  SIG_DFL);
}
