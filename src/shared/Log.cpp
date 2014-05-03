/*
 * Copyright (C) 2005-2008 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2008 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "Log.h"

#include <cstdarg>

#include "Common.h"
#include "Config/Config.h"
#include "Util.h"

const char* logToStr[LOG_MAX_FILES][3] =
{     // file name conf    mode  timestamp conf name
    { "GMLogFile",          "a", "GmLogTimestamp" },    // LOG_GM
    { "LogFile",            "w", "LogTimestamp" },      // LOG_DEFAULT
    { "StatusParserFile",   "w", NULL },                // LOG_STATUS
    { "CharLogFile",        "a", "CharLogTimestamp" },  // LOG_CHAR
    { "DBErrorLogFile",     "a", NULL },                // LOG_DB_ERR
    { "ArenaLogFile",       "a", NULL },                // LOG_ARENA
    { "CheatLogFile",       "a", NULL },                // LOG_CHEAT
    { "SpecialLogFile",     "a", NULL },                // LOG_SPECIAL
    { "MailLogFile",        "a", NULL },                // LOG_MAIL
    { "GannLogFile",        "a", NULL },                // LOG_GUILD_ANN
    { "BossLogFile",        "a", NULL },                // LOG_BOSS
    { "WardenLogFile",      "a", NULL },                // LOG_WARDEN
    { "AuctionLogFile",     "a", NULL },                // LOG_AUCTION
    { "DiffLogFile",        "a", NULL },                // LOG_DIFF
    { "SessionDiffLogFile", "a", NULL },                // LOG_SESSION_DIFF
    { "CrashLogFile",       "a", NULL },                // LOG_CRASH
    { "DBDiffFile",         "a", NULL },                // LOG_DB_DIFF
    { "ExpLogFile",         "a", NULL },                // LOG_EXP
    { "TradeLogFile",       "a", NULL },                // LOG_TRADE
    { "RaceChangeLogFile",  "a", NULL }                 // LOG_RACE_CHANGE
};

Log::Log() : m_includeTime(false), m_gmlog_per_account(false)
{
    for (uint8 i = LOG_DEFAULT; i < LOG_MAX_FILES; i++)
        logFile[i] = NULL;

    Initialize();
}

void Log::SetLogFileLevel(char *Level)
{
    int32 NewLevel =atoi((char*)Level);
    if ( NewLevel <0 )
        NewLevel = 0;
    m_logFileLevel = NewLevel;

    printf( "LogFileLevel is %u\n",m_logFileLevel );
}

void Log::Initialize()
{
    /// Common log files data
    m_logsDir = sConfig.GetStringDefault("LogsDir","");
    if(!m_logsDir.empty())
    {
        if((m_logsDir.at(m_logsDir.length()-1)!='/') && (m_logsDir.at(m_logsDir.length()-1)!='\\'))
            m_logsDir.append("/");
    }

    m_logsTimestamp = "_" + GetTimestampStr();

    /// Open specific log files
    m_gmlog_per_account = sConfig.GetBoolDefault("GmLogPerAccount",false);
    if(!m_gmlog_per_account)
        logFile[LOG_GM] = openLogFile(LOG_GM);
    else
    {
        // GM log settings for per account case
        m_gmlog_filename_format =  sConfig.GetStringDefault(logToStr[LOG_GM][0],"gm_commands.log");
        if(!m_gmlog_filename_format.empty())
        {
            bool m_gmlog_timestamp = sConfig.GetBoolDefault(logToStr[LOG_GM][2],false);

            size_t dot_pos = m_gmlog_filename_format.find_last_of(".");
            if(dot_pos!=m_gmlog_filename_format.npos)
            {
                if(m_gmlog_timestamp)
                    m_gmlog_filename_format.insert(dot_pos,m_logsTimestamp);

                m_gmlog_filename_format.insert(dot_pos,"_%u");
            }
            else
            {
                m_gmlog_filename_format += "__%u";

                if(m_gmlog_timestamp)
                    m_gmlog_filename_format += m_logsTimestamp;
            }

            m_gmlog_filename_format = (m_logsDir.empty() ? "" : m_logsDir + m_gmlog_filename_format);
        }
    }

    m_whisplog_filename_format = (m_logsDir.empty() ? "." : m_logsDir) + sConfig.GetStringDefault("WhispLogDir", "whisps/") + "whisp_%u_.log";

    for (uint8 i = LOG_DEFAULT; i < LOG_MAX_FILES; ++i)
        logFile[i] = openLogFile(LogNames(i));

    // Main log file settings
    m_includeTime  = sConfig.GetBoolDefault("LogTime", false);
    m_logFileLevel = sConfig.GetIntDefault("LogFileLevel", 0);

    m_logFilter = 0;

    if(sConfig.GetBoolDefault("LogFilter_TransportMoves", true))
        m_logFilter |= LOG_FILTER_TRANSPORT_MOVES;
    if(sConfig.GetBoolDefault("LogFilter_CreatureMoves", true))
        m_logFilter |= LOG_FILTER_CREATURE_MOVES;
    if(sConfig.GetBoolDefault("LogFilter_VisibilityChanges", true))
        m_logFilter |= LOG_FILTER_VISIBILITY_CHANGES;

}

FILE* Log::openLogFile(LogNames log)
{
    std::string logfn = sConfig.GetStringDefault(logToStr[log][0], "");
    if (logfn.empty())
        return NULL;

    if (logToStr[log][2] && sConfig.GetBoolDefault(logToStr[log][2], false))
    {
        size_t dot_pos = logfn.find_last_of(".");
        if (dot_pos != logfn.npos)
            logfn.insert(dot_pos, m_logsTimestamp);
        else
            logfn += m_logsTimestamp;
    }

    logFileNames[log] = m_logsDir + logfn;
    return fopen(logFileNames[log].c_str(), logToStr[log][1]);
}

FILE* Log::openGmlogPerAccount(uint32 account)
{
    if(m_gmlog_filename_format.empty())
        return NULL;

    char namebuf[HELLGROUND_PATH_MAX];
    snprintf(namebuf,HELLGROUND_PATH_MAX,m_gmlog_filename_format.c_str(),account);
    return fopen(namebuf, "a");
}

void Log::outPacket(uint32 glow, const char * str, ...)
{
    char namebuf[HELLGROUND_PATH_MAX];
    snprintf(namebuf, HELLGROUND_PATH_MAX, "packets//%u.txt", glow);

    FILE* per_file = fopen(namebuf, "a");
    va_list ap;
    va_start(ap, str);
    vfprintf(per_file, str, ap);
    fprintf(per_file, "\n" );
    va_end(ap);
    fclose(per_file);
}

FILE* Log::openWhisplogPerAccount(uint32 account)
{
    if(m_whisplog_filename_format.empty())
        return NULL;

    char namebuf[HELLGROUND_PATH_MAX];
    snprintf(namebuf,HELLGROUND_PATH_MAX,m_whisplog_filename_format.c_str(),account);
    return fopen(namebuf, "a");
}

bool Log::outTimestamp(FILE* file)
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    if (fprintf(file, "%-4d-%02d-%02d %02d:%02d:%02d ", aTm->tm_year+1900, aTm->tm_mon+1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec) < 0)
        return false;

    return true;
}

void Log::outTime()
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    printf("%02d:%02d:%02d ",aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
}

std::string Log::GetTimestampStr()
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[20];
    snprintf(buf,20,"%04d-%02d-%02d_%02d-%02d-%02d",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
    return std::string(buf);
}

void Log::outTitle( const char * str)
{
    if( !str )
        return;

    if(logFile[LOG_DEFAULT])
    {
        fprintf(logFile[LOG_DEFAULT], str);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }
}

void Log::outString()
{
    if(m_includeTime)
        outTime();
    printf( "\n" );
    if(logFile[LOG_DEFAULT])
    {
        outTimestamp(logFile[LOG_DEFAULT]);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }
    fflush(stdout);
}

void Log::outString( const char * str, ... )
{
    if( !str )
        return;

    if(m_includeTime)
        outTime();

    UTF8PRINTF(stdout,str,);

    printf( "\n" );
    if(logFile[LOG_DEFAULT])
    {
        outTimestamp(logFile[LOG_DEFAULT]);

        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        va_end(ap);

        fflush(logFile[LOG_DEFAULT]);
    }
    fflush(stdout);
}

void Log::outBasic(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_DEFAULT] && m_logFileLevel > 0)
    {
        va_list ap;
        outTimestamp(logFile[LOG_DEFAULT]);
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        va_end(ap);
        fflush(logFile[LOG_DEFAULT]);
    }
}

void Log::outDetail(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_DEFAULT] && m_logFileLevel > 1)
    {
        va_list ap;
        outTimestamp(logFile[LOG_DEFAULT]);
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        va_end(ap);
        fflush(logFile[LOG_DEFAULT]);
    }
}

void Log::outDebugInLine(const char * str, ...)
{
    if (!str)
        return;

    if(logFile[LOG_DEFAULT] && m_logFileLevel > 2)
    {
        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        va_end(ap);
    }
}

void Log::outDebug(const char * str, ...)
{
    if(!str)
        return;

    if (logFile[LOG_DEFAULT] && m_logFileLevel > 2)
    {
        outTimestamp(logFile[LOG_DEFAULT]);

        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        va_end(ap);

        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }
}

void Log::outCommand(uint32 account, const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_DEFAULT] && m_logFileLevel > 1)
    {
        va_list ap;
        outTimestamp(logFile[LOG_DEFAULT]);
        va_start(ap, str);
        vfprintf(logFile[LOG_DEFAULT], str, ap);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        va_end(ap);
        fflush(logFile[LOG_DEFAULT]);
    }

    if (m_gmlog_per_account)
    {
        if (FILE* per_file = openGmlogPerAccount(account))
        {
            va_list ap;
            outTimestamp(per_file);
            va_start(ap, str);
            vfprintf(per_file, str, ap);
            fprintf(per_file, "\n" );
            va_end(ap);
            fclose(per_file);
        }
    }
    else if (logFile[LOG_GM])
    {
        va_list ap;
        outTimestamp(logFile[LOG_GM]);
        va_start(ap, str);
        vfprintf(logFile[LOG_GM], str, ap);
        fprintf(logFile[LOG_GM], "\n" );
        va_end(ap);
        fflush(logFile[LOG_GM]);
    }
}

void Log::outWhisp(uint32 account, const char * str, ...)
{
    if (FILE* per_file = openWhisplogPerAccount(account))
    {
        va_list ap;
        outTimestamp(per_file);
        va_start(ap, str);
        vfprintf(per_file, str, ap);
        fprintf(per_file, "\n" );
        va_end(ap);
        fclose(per_file);
    }
}

void Log::outLog(LogNames log, const char * str, ...)
{
    if (!str)
        return;

    if (logFile[log])
    {
        // check for errors
        if (log == LOG_STATUS)
        {
            // we need to reopen file
            logFile[log] = freopen(logFileNames[log].c_str(), logToStr[log][1], logFile[log]);
        }
        else if (!outTimestamp(logFile[log]))
        {
            // if error reopen file
            logFile[log] = freopen(logFileNames[log].c_str(), logToStr[log][1], logFile[log]);
            outTimestamp(logFile[log]);
        }

        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[log], str, ap);
        va_end(ap);

        fprintf(logFile[log], "\n" );
        fflush(logFile[log]);
    }
}

void outstring_log(const char * str, ...)
{
    if (!str)
        return;

    char buf[256];
    va_list ap;
    va_start(ap, str);
    vsnprintf(buf,256, str, ap);
    va_end(ap);

    sLog.outString(buf);
}

void detail_log(const char * str, ...)
{
    if (!str)
        return;

    char buf[256];
    va_list ap;
    va_start(ap, str);
    vsnprintf(buf,256, str, ap);
    va_end(ap);

    sLog.outDetail(buf);
}

void debug_log(const char * str, ...)
{
    if (!str)
        return;

    char buf[256];
    va_list ap;
    va_start(ap, str);
    vsnprintf(buf,256, str, ap);
    va_end(ap);

    sLog.outDebug(buf);
}

void error_log(const char * str, ...)
{
    if (!str)
        return;

    char buf[256];
    va_list ap;
    va_start(ap, str);
    vsnprintf(buf,256, str, ap);
    va_end(ap);

    sLog.outLog(LOG_DEFAULT, buf);
}

void error_db_log(const char * str, ...)
{
    if (!str)
        return;

    char buf[256];
    va_list ap;
    va_start(ap, str);
    vsnprintf(buf,256, str, ap);
    va_end(ap);

    sLog.outLog(LOG_DB_ERR, buf);
}
