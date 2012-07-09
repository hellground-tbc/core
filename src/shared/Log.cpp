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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "Common.h"
#include "Log.h"
#include "Config/Config.h"
#include "Util.h"

#include <stdarg.h>

enum LogType
{
    LogNormal = 0,
    LogDetails,
    LogDebug,
    LogError
};

const int LogType_count = int(LogError) +1;

Log::Log() : m_colored(false), m_includeTime(false), m_gmlog_per_account(false)
{
    for (uint8 i = LOG_DEFAULT; i < LOG_MAX_FILES; i++)
        logFile[i] = NULL;

    Initialize();
}

void Log::InitColors(const std::string& str)
{
    if(str.empty())
    {
        m_colored = false;
        return;
    }

    int color[4];

    std::istringstream ss(str);

    for(int i = 0; i < LogType_count; ++i)
    {
        ss >> color[i];

        if(!ss)
            return;

        if(color[i] < 0 || color[i] >= Color_count)
            return;
    }

    for(int i = 0; i < LogType_count; ++i)
        m_colors[i] = Color(color[i]);

    m_colored = true;
}

void Log::SetColor(bool stdout_stream, Color color)
{
    #if PLATFORM == PLATFORM_WINDOWS

    static WORD WinColorFG[Color_count] =
    {
        0,                                                  // BLACK
        FOREGROUND_RED,                                     // RED
        FOREGROUND_GREEN,                                   // GREEN
        FOREGROUND_RED | FOREGROUND_GREEN,                  // BROWN
        FOREGROUND_BLUE,                                    // BLUE
        FOREGROUND_RED |                    FOREGROUND_BLUE,// MAGENTA
        FOREGROUND_GREEN | FOREGROUND_BLUE,                 // CYAN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,// WHITE
                                                            // YELLOW
        FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
                                                            // RED_BOLD
        FOREGROUND_RED |                                      FOREGROUND_INTENSITY,
                                                            // GREEN_BOLD
        FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,             // BLUE_BOLD
                                                            // MAGENTA_BOLD
        FOREGROUND_RED |                    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // CYAN_BOLD
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // WHITE_BOLD
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE );
    SetConsoleTextAttribute(hConsole, WinColorFG[color]);
    #else

    enum ANSITextAttr
    {
        TA_NORMAL=0,
        TA_BOLD=1,
        TA_BLINK=5,
        TA_REVERSE=7
    };

    enum ANSIFgTextAttr
    {
        FG_BLACK=30, FG_RED,  FG_GREEN, FG_BROWN, FG_BLUE,
        FG_MAGENTA,  FG_CYAN, FG_WHITE, FG_YELLOW
    };

    enum ANSIBgTextAttr
    {
        BG_BLACK=40, BG_RED,  BG_GREEN, BG_BROWN, BG_BLUE,
        BG_MAGENTA,  BG_CYAN, BG_WHITE
    };

    static uint8 UnixColorFG[Color_count] =
    {
        FG_BLACK,                                           // BLACK
        FG_RED,                                             // RED
        FG_GREEN,                                           // GREEN
        FG_BROWN,                                           // BROWN
        FG_BLUE,                                            // BLUE
        FG_MAGENTA,                                         // MAGENTA
        FG_CYAN,                                            // CYAN
        FG_WHITE,                                           // WHITE
        FG_YELLOW,                                          // YELLOW
        FG_RED,                                             // LRED
        FG_GREEN,                                           // LGREEN
        FG_BLUE,                                            // LBLUE
        FG_MAGENTA,                                         // LMAGENTA
        FG_CYAN,                                            // LCYAN
        FG_WHITE                                            // LWHITE
    };

    fprintf((stdout_stream? stdout : stderr), "\x1b[%d%sm",UnixColorFG[color],(color>=YELLOW&&color<Color_count ?";1":""));
    #endif
}

void Log::ResetColor(bool stdout_stream)
{
    #if PLATFORM == PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE );
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED );
    #else
    fprintf(( stdout_stream ? stdout : stderr ), "\x1b[0m");
    #endif
}

void Log::SetLogLevel(char *Level)
{
    int32 NewLevel =atoi((char*)Level);
    if ( NewLevel <0 )
        NewLevel = 0;
    m_logLevel = NewLevel;

    printf( "LogLevel is %u\n",m_logLevel );
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

    const char* logToStr[LOG_MAX_FILES] =
    {
        "LogFile",          //LOG_DEFAULT
        "unused",           //LOG_STATUS
        "GMLogFile",        //LOG_GM
        "CharLogFile",      //LOG_CHAR
        "DBErrorLogFile",   //LOG_DB_ERR
        "ArenaLogFile",     //LOG_ARENA
        "CheatLogFile",     //LOG_CHEAT
        "ACLogFile",        //LOG_AC
        "SpecialLogFile",   //LOG_SPECIAL
        "MailLogFile",      //LOG_MAIL
        "GannLogFile",      //LOG_GUILD_ANN
        "BossLogFile",      //LOG_BOSS
        "WardenLogFile",    //LOG_WARDEN
        "AuctionLogFile",   //LOG_AUCTION
        "DiffLogFile",      //LOG_DIFF
        "CrashLogFile",     //LOG_CRASH
    };

    /// Open specific log files
    logFile[LOG_DEFAULT] = openLogFile(logToStr[LOG_DEFAULT],"LogTimestamp","w");

    m_gmlog_per_account = sConfig.GetBoolDefault("GmLogPerAccount",false);
    if(!m_gmlog_per_account)
        logFile[LOG_GM] = openLogFile(logToStr[LOG_GM],"GmLogTimestamp","a");
    else
    {
        // GM log settings for per account case
        m_gmlog_filename_format = sConfig.GetStringDefault(logToStr[LOG_GM], "");
        if(!m_gmlog_filename_format.empty())
        {
            bool m_gmlog_timestamp = sConfig.GetBoolDefault("GmLogTimestamp",false);

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

            m_gmlog_filename_format = m_logsDir + m_gmlog_filename_format;
        }
    }

    m_whisplog_filename_format = (m_logsDir.empty() ? "." : m_logsDir) + sConfig.GetStringDefault("WhispLogDir", "whisps/") + "whisp_%u_.log";

    logFile[LOG_CHAR] = openLogFile(logToStr[LOG_CHAR],"CharLogTimestamp","a");

    for (uint8 i = LOG_DB_ERR; i < LOG_MAX_FILES; i++)
        logFile[i] = openLogFile(logToStr[i], NULL, "a");

    // Main log file settings
    m_includeTime  = sConfig.GetBoolDefault("LogTime", false);
    m_logLevel     = sConfig.GetIntDefault("LogLevel", 0);
    m_logFileLevel = sConfig.GetIntDefault("LogFileLevel", 0);
    InitColors(sConfig.GetStringDefault("LogColors", ""));

    m_logFilter = 0;

    if(sConfig.GetBoolDefault("LogFilter_TransportMoves", true))
        m_logFilter |= LOG_FILTER_TRANSPORT_MOVES;
    if(sConfig.GetBoolDefault("LogFilter_CreatureMoves", true))
        m_logFilter |= LOG_FILTER_CREATURE_MOVES;
    if(sConfig.GetBoolDefault("LogFilter_VisibilityChanges", true))
        m_logFilter |= LOG_FILTER_VISIBILITY_CHANGES;

    // Char log settings
    m_charLog_Dump = sConfig.GetBoolDefault("CharLogDump", false);

}

FILE* Log::openLogFile(char const* configFileName,char const* configTimeStampFlag, char const* mode)
{
    std::string logfn=sConfig.GetStringDefault(configFileName, "");
    if(logfn.empty())
        return NULL;

    if(configTimeStampFlag && sConfig.GetBoolDefault(configTimeStampFlag,false))
    {
        size_t dot_pos = logfn.find_last_of(".");
        if(dot_pos!=logfn.npos)
            logfn.insert(dot_pos,m_logsTimestamp);
        else
            logfn += m_logsTimestamp;
    }

    return fopen((m_logsDir+logfn).c_str(), mode);
}

FILE* Log::openGmlogPerAccount(uint32 account)
{
    if(m_gmlog_filename_format.empty())
        return NULL;

    char namebuf[HELLGROUND_PATH_MAX];
    snprintf(namebuf,HELLGROUND_PATH_MAX,m_gmlog_filename_format.c_str(),account);
    return fopen(namebuf, "a");
}

FILE* Log::openWhisplogPerAccount(uint32 account)
{
    if(m_whisplog_filename_format.empty())
        return NULL;

    char namebuf[HELLGROUND_PATH_MAX];
    snprintf(namebuf,HELLGROUND_PATH_MAX,m_whisplog_filename_format.c_str(),account);
    return fopen(namebuf, "a");
}

void Log::outTimestamp(FILE* file)
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    fprintf(file,"%-4d-%02d-%02d %02d:%02d:%02d ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
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

    if(m_colored)
        SetColor(true,WHITE);

    // not expected utf8 and then send as-is
    //printf( str );

    if(m_colored)
        ResetColor(true);

    //printf( "\n" );
    if(logFile[LOG_DEFAULT])
    {
        fprintf(logFile[LOG_DEFAULT], str);
        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }

    //flush(stdout);
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

    if(m_colored)
        SetColor(true,m_colors[LogNormal]);

    if(m_includeTime)
        outTime();

    UTF8PRINTF(stdout,str,);

    if(m_colored)
        ResetColor(true);

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

void Log::outError(const char * err, ...)
{
    if( !err )
        return;

    if(m_colored)
        SetColor(false,m_colors[LogError]);

    if(m_includeTime)
        outTime();

    //UTF8PRINTF(stderr,err,);

    if(m_colored)
        ResetColor(false);

    //fprintf( stderr, "\n" );
    if(logFile[LOG_DEFAULT])
    {
        outTimestamp(logFile[LOG_DEFAULT]);
        fprintf(logFile[LOG_DEFAULT], "ERROR:" );

        va_list ap;
        va_start(ap, err);
        vfprintf(logFile[LOG_DEFAULT], err, ap);
        va_end(ap);

        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }
    //fflush(stderr);
}

void Log::outBoss(const char *str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_BOSS])
    {
        va_list ap;
        outTimestamp(logFile[LOG_BOSS]);
        va_start(ap, str);
        vfprintf(logFile[LOG_BOSS], str, ap);
        fprintf(logFile[LOG_BOSS], "\n" );
        va_end(ap);
        fflush(logFile[LOG_BOSS]);
    }
}

void Log::outArena(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_ARENA])
    {
        va_list ap;
        outTimestamp(logFile[LOG_ARENA]);
        va_start(ap, str);
        vfprintf(logFile[LOG_ARENA], str, ap);
        fprintf(logFile[LOG_ARENA], "\n" );
        va_end(ap);
        fflush(logFile[LOG_ARENA]);
    }
}

void Log::outCheat(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_CHEAT])
    {
        va_list ap;
        outTimestamp(logFile[LOG_CHEAT]);
        va_start(ap, str);
        vfprintf(logFile[LOG_CHEAT], str, ap);
        fprintf(logFile[LOG_CHEAT], "\n" );
        va_end(ap);
        fflush(logFile[LOG_CHEAT]);
    }
}

void Log::outAC(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_AC])
    {
        va_list ap;
        outTimestamp(logFile[LOG_AC]);
        va_start(ap, str);
        vfprintf(logFile[LOG_AC], str, ap);
        fprintf(logFile[LOG_AC], "\n" );
        va_end(ap);
        fflush(logFile[LOG_AC]);
    }
}

void Log::outErrorDb(const char * err, ...)
{
    if (!err)
        return;

    if (m_colored)
        SetColor(false,m_colors[LogError]);

    if (m_includeTime)
        outTime();

    if (m_colored)
        ResetColor(false);

    if (logFile[LOG_DEFAULT])
    {
        outTimestamp(logFile[LOG_DEFAULT]);
        fprintf(logFile[LOG_DEFAULT], "ERROR:" );

        va_list ap;
        va_start(ap, err);
        vfprintf(logFile[LOG_DEFAULT], err, ap);
        va_end(ap);

        fprintf(logFile[LOG_DEFAULT], "\n" );
        fflush(logFile[LOG_DEFAULT]);
    }

    if (logFile[LOG_DB_ERR])
    {
        outTimestamp(logFile[LOG_DB_ERR]);

        va_list ap;
        va_start(ap, err);
        vfprintf(logFile[LOG_DB_ERR], err, ap);
        va_end(ap);

        fprintf(logFile[LOG_DB_ERR], "\n" );
        fflush(logFile[LOG_DB_ERR]);
    }
}

void Log::outBasic(const char * str, ...)
{
    if (!str)
        return;

    if (m_logLevel > 0)
    {
        if (m_colored)
            SetColor(true,m_colors[LogDetails]);

        if (m_includeTime)
            outTime();

        if (m_colored)
            ResetColor(true);
    }

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
void Log::outIrc(const char * str, ... )
{
    if (!str)
        return;

    if (logFile[LOG_STATUS] = openLogFile("StatusParserFile",NULL,"w"))
    {
        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_STATUS], str, ap);
        fprintf(logFile[LOG_STATUS], "\n" );
        va_end(ap);
        fclose(logFile[LOG_STATUS]);
        logFile[LOG_STATUS] = NULL;
    }
}

void Log::outDetail(const char * str, ...)
{
    if (!str)
        return;

    if (m_logLevel > 1)
    {

        if (m_colored)
            SetColor(true,m_colors[LogDetails]);

        if (m_includeTime)
            outTime();

        if (m_colored)
            ResetColor(true);
    }

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

    if (m_logLevel > 2)
    {
        if (m_colored)
            SetColor(true,m_colors[LogDebug]);

        if (m_colored)
            ResetColor(true);
    }

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

    if (m_logLevel > 2)
    {
        if (m_colored)
            SetColor(true,m_colors[LogDebug]);

        if (m_includeTime)
            outTime();

        if (m_colored)
            ResetColor(true);
    }

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

    if (m_logLevel > 1)
    {
        if (m_colored)
            SetColor(true,m_colors[LogDetails]);

        if (m_includeTime)
            outTime();

        if (m_colored)
            ResetColor(true);
    }

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

void Log::outChar(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_CHAR])
    {
        va_list ap;
        outTimestamp(logFile[LOG_CHAR]);
        va_start(ap, str);
        vfprintf(logFile[LOG_CHAR], str, ap);
        fprintf(logFile[LOG_CHAR], "\n" );
        va_end(ap);
        fflush(logFile[LOG_CHAR]);
    }
}

void Log::outSpecial(const char * str, ...)
{
    if (!str)
        return;

    if(logFile[LOG_SPECIAL])
    {
        va_list ap;
        outTimestamp(logFile[LOG_SPECIAL]);
        va_start(ap, str);
        vfprintf(logFile[LOG_SPECIAL], str, ap);
        fprintf(logFile[LOG_SPECIAL], "\n" );
        va_end(ap);
        fflush(logFile[LOG_SPECIAL]);
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

void Log::outMail(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_MAIL])
    {
        va_list ap;
        outTimestamp(logFile[LOG_MAIL]);
        va_start(ap, str);
        vfprintf(logFile[LOG_MAIL], str, ap);
        fprintf(logFile[LOG_MAIL], "\n" );
        va_end(ap);
        fflush(logFile[LOG_MAIL]);
    }
}

void Log::outWarden(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_WARDEN])
    {
        va_list ap;
        outTimestamp(logFile[LOG_WARDEN]);
        va_start(ap, str);
        vfprintf(logFile[LOG_WARDEN], str, ap);
        fprintf(logFile[LOG_WARDEN], "\n");
        va_end(ap);
        fflush(logFile[LOG_WARDEN]);
    }
}

void Log::outAuction(const char * str, ...)
{
    if (!str)
         return;


    if (logFile[LOG_AUCTION])
    {
        va_list ap;
        outTimestamp(logFile[LOG_AUCTION]);
        va_start(ap, str);
        vfprintf(logFile[LOG_AUCTION], str, ap);
        fprintf(logFile[LOG_AUCTION], "\n" );
        va_end(ap);
        fflush(logFile[LOG_AUCTION]);
    }
}

void Log::outGann(const char * str, ...)
{
    if (!str)
        return;

    if (logFile[LOG_GUILD_ANN])
    {
        va_list ap;
        outTimestamp(logFile[LOG_GUILD_ANN]);
        va_start(ap, str);
        vfprintf(logFile[LOG_GUILD_ANN], str, ap);
        fprintf(logFile[LOG_GUILD_ANN], "\n" );
        va_end(ap);
        fflush(logFile[LOG_GUILD_ANN]);
    }
}

void Log::outDiff(const char * str, ...)
{
    if (!str)
        return;

    if(logFile[LOG_DIFF])
    {
        outTimestamp(logFile[LOG_DIFF]);

        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_DIFF], str, ap);
        va_end(ap);

        fprintf(logFile[LOG_DIFF], "\n" );
        fflush(logFile[LOG_DIFF]);
    }
}

void Log::outCrash(const char * str, ...)
{
    if (!str)
        return;

    if(logFile[LOG_CRASH])
    {
        outTimestamp(logFile[LOG_CRASH]);

        va_list ap;
        va_start(ap, str);
        vfprintf(logFile[LOG_CRASH], str, ap);
        va_end(ap);

        fprintf(logFile[LOG_CRASH], "\n" );
        fflush(logFile[LOG_CRASH]);
    }
}

void Log::outMenu( const char * str, ... )
{
    if( !str )
        return;

    SetColor(true,m_colors[LogNormal]);

    if(m_includeTime)
        outTime();

    ResetColor(true);

    if (logFile[LOG_DEFAULT])
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

void Log::OutLogToFile(logFiles log, const char * str, ...)
{
    if (!str)
        return;

    if(logFile[log])
    {
        outTimestamp(logFile[log]);

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

    sLog.outError(buf);
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

    sLog.outErrorDb(buf);
}
