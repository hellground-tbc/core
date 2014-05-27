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

/// \addtogroup world The World
/// @{
/// \file

#ifndef HELLGROUND_WORLD_H
#define HELLGROUND_WORLD_H

#include "ace/Singleton.h"

#include "Common.h"
#include "Timer.h"
#include "SharedDefines.h"
#include "ace/Atomic_Op.h"
#include "DelayExecutor.h"
#include "QueryResult.h"
#include "WorldSession.h"

#include <map>
#include <set>
#include <list>
#include <tbb/concurrent_hash_map.h>
#include <tbb/blocked_range.h>

class Object;
class WorldPacket;
class WorldSession;
class Player;
class Weather;
struct ScriptAction;
struct ScriptInfo;
class SqlResultQueue;
class QueryResult;
class WorldSocket;
class AntiCheat;

// ServerMessages.dbc
enum ServerMessageType
{
    SERVER_MSG_SHUTDOWN_TIME      = 1,
    SERVER_MSG_RESTART_TIME       = 2,
    SERVER_MSG_STRING             = 3,
    SERVER_MSG_SHUTDOWN_CANCELLED = 4,
    SERVER_MSG_RESTART_CANCELLED  = 5
};

enum ShutdownMask
{
    SHUTDOWN_MASK_RESTART = 1,
    SHUTDOWN_MASK_IDLE    = 2,
};

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

/// Timers for different object refresh rates
enum WorldTimers
{
    WUPDATE_OBJECTS         = 0,
    WUPDATE_SESSIONS        = 1,
    WUPDATE_AUCTIONS        = 2,
    WUPDATE_WEATHERS        = 3,
    WUPDATE_UPTIME          = 4,
    WUPDATE_CORPSES         = 5,
    WUPDATE_EVENTS          = 6,
    WUPDATE_AUTOBROADCAST   = 7,
    WUPDATE_GUILD_ANNOUNCES = 8,
    WUPDATE_DELETECHARS     = 9,
    WUPDATE_OLDMAILS        = 10,
    WUPDATE_ACTIVE_BANS     = 11,

    WUPDATE_COUNT
};

/// Configuration elements
enum WorldConfigs
{
    CONFIG_PORT_WORLD = 0,

    // Performance settings
    CONFIG_COMPRESSION,
    CONFIG_MAX_OVERSPEED_PINGS,
    CONFIG_ADDON_CHANNEL,
    CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY,
    CONFIG_GRID_UNLOAD,

    CONFIG_SOCKET_SELECTTIME,
    CONFIG_INTERVAL_GRIDCLEAN,
    CONFIG_INTERVAL_MAPUPDATE,
    CONFIG_INTERVAL_CHANGEWEATHER,
    CONFIG_INTERVAL_SAVE,
    CONFIG_INTERVAL_DISCONNECT_TOLERANCE,
    CONFIG_UPTIME_UPDATE,

    CONFIG_NUMTHREADS,
    CONFIG_MAPUPDATE_MAXVISITORS,
    CONFIG_CUMULATIVE_LOG_METHOD,

    CONFIG_SESSION_UPDATE_MAX_TIME,
    CONFIG_SESSION_UPDATE_OVERTIME_METHOD,
    CONFIG_SESSION_UPDATE_VERBOSE_LOG,
    CONFIG_SESSION_UPDATE_IDLE_KICK,
    CONFIG_SESSION_UPDATE_MIN_LOG_DIFF,
    CONFIG_INTERVAL_LOG_UPDATE,
    CONFIG_MIN_LOG_UPDATE,

    // Server settings
    CONFIG_GAME_TYPE,
    CONFIG_REALM_ZONE,
    CONFIG_EXPANSION,
    CONFIG_DECLINED_NAMES_USED,
    CONFIG_STRICT_PLAYER_NAMES,
    CONFIG_STRICT_CHARTER_NAMES,
    CONFIG_STRICT_PET_NAMES,
    CONFIG_CHARACTERS_PER_REALM,
    CONFIG_CHARACTERS_PER_ACCOUNT,
    CONFIG_ACTIVE_BANS_UPDATE_TIME,
    CONFIG_ELUNA_ENABLED,

    // Server customization basic
    CONFIG_CHARACTERS_CREATING_DISABLED,
    CONFIG_MAX_PLAYER_LEVEL,
    CONFIG_START_PLAYER_LEVEL,
    CONFIG_START_PLAYER_MONEY,
    CONFIG_MAX_HONOR_POINTS,
    CONFIG_START_HONOR_POINTS,
    CONFIG_MAX_ARENA_POINTS,
    CONFIG_START_ARENA_POINTS,
    CONFIG_START_ALL_TAXI_PATHS,
    CONFIG_START_ALL_REP,
    CONFIG_START_ALL_SPELLS,
    CONFIG_START_ALL_EXPLORED,

    CONFIG_ALWAYS_MAXSKILL,
    CONFIG_CAST_UNSTUCK,
    CONFIG_DAILY_BLIZZLIKE,
    CONFIG_DAILY_MAX_PER_DAY,
    CONFIG_DISABLE_DUEL,
    CONFIG_DISABLE_PVP,
    CONFIG_EVENT_ANNOUNCE,
    CONFIG_FFA_DISALLOWGROUP,
    CONFIG_HONOR_AFTER_DUEL,
    CONFIG_INSTANCE_IGNORE_LEVEL,
    CONFIG_INSTANCE_IGNORE_RAID,
    CONFIG_MAX_PRIMARY_TRADE_SKILL,
    CONFIG_PVP_TOKEN_ENABLE,
    CONFIG_PVP_TOKEN_ID,
    CONFIG_PVP_TOKEN_COUNT,
    CONFIG_PVP_TOKEN_MAP_TYPE,
    CONFIG_ENABLE_SINFO_LOGIN,
    CONFIG_SHOW_KICK_IN_WORLD,

    CONFIG_DONT_DELETE_CHARS,
    CONFIG_DONT_DELETE_CHARS_LVL,
    CONFIG_KEEP_DELETED_CHARS_TIME,

    // Server customization advanced
    CONFIG_WEATHER,
    CONFIG_ENABLE_SORT_AUCTIONS,
    CONFIG_AUTOBROADCAST_INTERVAL,
    CONFIG_GROUPLEADER_RECONNECT_PERIOD,
    CONFIG_INSTANCE_RESET_TIME_HOUR,
    CONFIG_INSTANCE_UNLOAD_DELAY,
    CONFIG_MAIL_DELIVERY_DELAY,
    CONFIG_EXTERNAL_MAIL,
    CONFIG_EXTERNAL_MAIL_INTERVAL,
    CONFIG_GM_MAIL,
    CONFIG_RETURNOLDMAILS_MODE,
    CONFIG_RETURNOLDMAILS_INTERVAL,
    CONFIG_GROUP_XP_DISTANCE,
    CONFIG_MAX_WHO,
    CONFIG_MIN_PETITION_SIGNS,
    CONFIG_NO_RESET_TALENT_COST,
    CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF,
    CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF,
    CONFIG_RABBIT_DAY,
    CONFIG_SKIP_CINEMATICS,
    CONFIG_SKILL_PROSPECTING,

    CONFIG_GUILD_ANN_INTERVAL,
    CONFIG_GUILD_ANN_COOLDOWN,
    CONFIG_GUILD_ANN_LENGTH,

    CONFIG_ENABLE_CUSTOM_XP_RATES,
    CONFIG_XP_RATE_MODIFY_ITEM_ENTRY,
    CONFIG_XP_RATE_MODIFY_ITEM_PCT,

    // Player interaction
    CONFIG_ALLOW_TWO_SIDE_ACCOUNTS,
    CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION,
    CONFIG_ALLOW_TWO_SIDE_INTERACTION_MAIL,
    CONFIG_ALLOW_TWO_SIDE_TRADE,
    CONFIG_ALLOW_TWO_SIDE_WHO_LIST,
    CONFIG_TALENTS_INSPECTING,
    
    // Chat settings
    CONFIG_PRIVATE_CHANNEL_LIMIT,
    CONFIG_GLOBAL_TRADE_CHANNEL,
    CONFIG_RESTRICTED_LFG_CHANNEL,
    CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL,
    CONFIG_CHAT_DENY_MASK,
    CONFIG_CHAT_MINIMUM_LVL,
    CONFIG_CHAT_FAKE_MESSAGE_PREVENTING,
    CONFIG_CHATFLOOD_MESSAGE_COUNT,
    CONFIG_CHATFLOOD_MESSAGE_DELAY,
    CONFIG_CHATFLOOD_MUTE_TIME,

    // Game master settings
    CONFIG_GM_LOGIN_STATE,
    CONFIG_GM_VISIBLE_STATE,
    CONFIG_GM_CHAT,
    CONFIG_GM_WISPERING_TO,
    CONFIG_GM_IN_GM_LIST,
    CONFIG_GM_IN_WHO_LIST,
    CONFIG_GM_LOG_TRADE,
    CONFIG_START_GM_LEVEL,
    CONFIG_ALLOW_GM_GROUP,
    CONFIG_ALLOW_GM_FRIEND,
    CONFIG_GM_TRUSTED_LEVEL,
    
    CONFIG_COMMAND_LOG_PERMISSION,
    CONFIG_INSTANT_LOGOUT,
    CONFIG_MIN_GM_TEXT_LVL,
    CONFIG_DISABLE_BREATHING,
    CONFIG_HIDE_GAMEMASTER_ACCOUNTS,

    // Server rates
    CONFIG_SKILL_GAIN_CRAFTING,
    CONFIG_SKILL_GAIN_DEFENSE,
    CONFIG_SKILL_GAIN_GATHERING,
    CONFIG_SKILL_GAIN_WEAPON,
    CONFIG_SKILL_CHANCE_ORANGE,
    CONFIG_SKILL_CHANCE_YELLOW,
    CONFIG_SKILL_CHANCE_GREEN,
    CONFIG_SKILL_CHANCE_GREY,
    CONFIG_SKILL_CHANCE_MINING_STEPS,
    CONFIG_SKILL_CHANCE_SKINNING_STEPS,

    CONFIG_DEATH_SICKNESS_LEVEL,
    CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP,
    CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE,
    CONFIG_DEATH_BONES_WORLD,
    CONFIG_DEATH_BONES_BG_OR_ARENA,

    // Creature settings
    CONFIG_CORPSE_DECAY_NORMAL,
    CONFIG_CORPSE_DECAY_RARE,
    CONFIG_CORPSE_DECAY_ELITE,
    CONFIG_CORPSE_DECAY_RAREELITE,
    CONFIG_CORPSE_DECAY_WORLDBOSS,
    CONFIG_LISTEN_RANGE_SAY,
    CONFIG_LISTEN_RANGE_TEXTEMOTE,
    CONFIG_LISTEN_RANGE_YELL,

    CONFIG_WAYPOINT_MOVEMENT_ACTIVE_ON_CONTINENTS,
    CONFIG_WAYPOINT_MOVEMENT_ACTIVE_IN_INSTANCES,
    CONFIG_COMBAT_ACTIVE_ON_CONTINENTS,
    CONFIG_COMBAT_ACTIVE_IN_INSTANCES,
    CONFIG_COMBAT_ACTIVE_FOR_PLAYERS_ONLY,

    CONFIG_SIGHT_GUARD,
    CONFIG_SIGHT_MONSTER,
    CONFIG_EVADE_HOMEDIST,
    CONFIG_EVADE_TARGETDIST,

    CONFIG_CREATURE_RESTORE_STATE,
    CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS,
    CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY,
    CONFIG_CREATURE_FAMILY_FLEE_RADIUS,
    CONFIG_CREATURE_FAMILY_FLEE_DELAY,
    CONFIG_WORLD_BOSS_LEVEL_DIFF,

    // Arena settings
    CONFIG_ARENA_MAX_RATING_DIFFERENCE,
    CONFIG_ARENA_RATING_DISCARD_TIMER,
    CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS,
    CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS,
    CONFIG_ENABLE_FAKE_WHO_ON_ARENA,
    CONFIG_ENABLE_FAKE_WHO_IN_GUILD,
    CONFIG_ARENA_LOG_EXTENDED_INFO,
    CONFIG_ARENA_READY_START_TIMER,

    CONFIG_ENABLE_HIDDEN_RATING,
    CONFIG_ENABLE_HIDDEN_RATING_PENALTY,
    CONFIG_HIDDEN_RATING_PENALTY,
    CONFIG_ENABLE_HIDDEN_RATING_LOWER_LOSS,

    CONFIG_ENABLE_ARENA_STEP_BY_STEP_MATCHING,
    CONFIG_ARENA_STEP_BY_STEP_TIME,
    CONFIG_ARENA_STEP_BY_STEP_VALUE,
    CONFIG_ARENA_END_AFTER_TIME,
    CONFIG_ARENA_END_AFTER_ALWAYS_DRAW,
    CONFIG_ARENA_STATUS_INFO,
    CONFIG_ARENA_ELO_COEFFICIENT,
    CONFIG_ARENA_DAILY_REQUIREMENT,
    CONFIG_ARENA_DAILY_AP_REWARD,

    // Battleground settings
    CONFIG_BATTLEGROUND_ANNOUNCE_START,
    CONFIG_BATTLEGROUND_CAST_DESERTER,
    CONFIG_BATTLEGROUND_INVITATION_TYPE,
    CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH,
    CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER,
    CONFIG_PREMATURE_BG_REWARD,
    CONFIG_BATTLEGROUND_QUEUE_INFO,
    CONFIG_BG_START_MUSIC,
    CONFIG_BATTLEGROUND_TIMER_INFO,
    CONFIG_BATTLEGROUND_WSG_END_AFTER_ENABLED,
    CONFIG_BATTLEGROUND_WSG_END_AFTER_TIME,
    CONFIG_BATTLEGROUND_WSG_END_AFTER_ALWAYS_DRAW,

    // vmaps/mmaps
    CONFIG_VMAP_LOS_ENABLED,
    CONFIG_VMAP_INDOOR_CHECK,
    CONFIG_PET_LOS,
    CONFIG_VMAP_TOTEM,
    CONFIG_MMAP_ENABLED,

    // visibility and radiuses
    CONFIG_GROUP_VISIBILITY,
    
    // movement
    CONFIG_TARGET_POS_RECALCULATION_RANGE,
    CONFIG_TARGET_POS_RECHECK_TIMER,
    CONFIG_WAYPOINT_MOVEMENT_PATHFINDING_ON_CONTINENTS,
    CONFIG_WAYPOINT_MOVEMENT_PATHFINDING_IN_INSTANCES,

    // CoreBalancer
    CONFIG_COREBALANCER_ENABLED,
    CONFIG_COREBALANCER_PLAYABLE_DIFF,
    CONFIG_COREBALANCER_INTERVAL,
    CONFIG_COREBALANCER_VISIBILITY_PENALTY,

    // VMSS system
    CONFIG_VMSS_ENABLE,
    CONFIG_VMSS_MAPFREEMETHOD,
    CONFIG_VMSS_FREEZECHECKPERIOD,
    CONFIG_VMSS_FREEZEDETECTTIME,
    
    // Warden/anticheat
    CONFIG_WARDEN_ENABLED,
    CONFIG_WARDEN_KICK,
    CONFIG_WARDEN_BAN,
    CONFIG_WARDEN_LOG_ONLY_CHECK,
    CONFIG_WARDEN_CHECK_INTERVAL_MIN,
    CONFIG_WARDEN_CHECK_INTERVAL_MAX,
    CONFIG_WARDEN_MEM_CHECK_MAX,
    CONFIG_WARDEN_RANDOM_CHECK_MAX,
    CONFIG_ENABLE_PASSIVE_ANTICHEAT,
    CONFIG_ANTICHEAT_CUMULATIVE_DELAY,

    // RaF
    CONFIG_UINT32_RAF_MAXGRANTLEVEL,
    CONFIG_UINT32_RAF_MAXREFERALS,
    CONFIG_UINT32_RAF_MAXREFERERS,

    // Ganking penalty
    CONFIG_ENABLE_GANKING_PENALTY,
    CONFIG_GANKING_PENALTY_EXPIRE,
    CONFIG_GANKING_KILLS_ALERT,

    // Network
    CONFIG_KICK_PLAYER_ON_BAD_PACKET,
    
    CONFIG_VALUE_COUNT
};

/// Server rates
enum Rates
{
    RATE_HEALTH=0,
    RATE_POWER_MANA,
    RATE_POWER_RAGE_INCOME,
    RATE_POWER_RAGE_LOSS,
    RATE_POWER_FOCUS,
    RATE_LOYALTY,
    RATE_SKILL_DISCOVERY,
    RATE_DROP_ITEM_POOR,
    RATE_DROP_ITEM_NORMAL,
    RATE_DROP_ITEM_UNCOMMON,
    RATE_DROP_ITEM_RARE,
    RATE_DROP_ITEM_EPIC,
    RATE_DROP_ITEM_LEGENDARY,
    RATE_DROP_ITEM_ARTIFACT,
    RATE_DROP_ITEM_REFERENCED,
    RATE_DROP_MONEY,
    RATE_XP_KILL,
    RATE_XP_QUEST,
    RATE_XP_EXPLORE,
    RATE_XP_PAST_70,

    RATE_REST_INGAME,
    RATE_REST_OFFLINE_IN_TAVERN_OR_CITY,
    RATE_REST_OFFLINE_IN_WILDERNESS,
    RATE_DAMAGE_FALL,
    RATE_AUCTION_TIME,
    RATE_AUCTION_DEPOSIT,
    RATE_AUCTION_CUT,
    RATE_HONOR,
    RATE_MINING_AMOUNT,
    RATE_MINING_NEXT,
    RATE_TALENT,
    RATE_REPUTATION_GAIN,
    RATE_REPUTATION_LOWLEVEL_KILL,
    RATE_REPUTATION_LOWLEVEL_QUEST,
    RATE_INSTANCE_RESET_TIME,
    RATE_DURABILITY_LOSS_DAMAGE,
    RATE_DURABILITY_LOSS_ABSORB,
    RATE_DURABILITY_LOSS_BLOCK,
    RATE_DURABILITY_LOSS_PARRY,

    RATE_CREATURE_AGGRO,
    RATE_CREATURE_GUARD_AGGRO,
    RATE_CORPSE_DECAY_LOOTED,
    RATE_CREATURE_NORMAL_DAMAGE,
    RATE_CREATURE_ELITE_ELITE_DAMAGE,
    RATE_CREATURE_ELITE_RAREELITE_DAMAGE,
    RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,
    RATE_CREATURE_ELITE_RARE_DAMAGE,
    RATE_CREATURE_NORMAL_SPELLDAMAGE,
    RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,
    RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE,
    RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE,
    RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,
    RATE_CREATURE_NORMAL_HP,
    RATE_CREATURE_ELITE_ELITE_HP,
    RATE_CREATURE_ELITE_RAREELITE_HP,
    RATE_CREATURE_ELITE_WORLDBOSS_HP,
    RATE_CREATURE_ELITE_RARE_HP,
    
    CONFIG_GANKING_PENALTY_PER_KILL,
    CONFIG_FLOAT_RATE_RAF_XP,
    CONFIG_FLOAT_RATE_RAF_LEVELPERLEVEL,
    
    CONFIG_ANTICHEAT_SPEEDHACK_TOLERANCE,
    MAX_RATES
};

/// Type of server
enum RealmType
{
    REALM_TYPE_NORMAL = 0,
    REALM_TYPE_PVP = 1,
    REALM_TYPE_NORMAL2 = 4,
    REALM_TYPE_RP = 6,
    REALM_TYPE_RPPVP = 8,
    REALM_TYPE_FFA_PVP = 16                                 // custom, free for all pvp mode like arena PvP in all zones except rest activated places and sanctuaries
                                                            // replaced by REALM_PVP in realm list
};

enum RealmZone
{
    REALM_ZONE_UNKNOWN       = 0,                           // any language
    REALM_ZONE_DEVELOPMENT   = 1,                           // any language
    REALM_ZONE_UNITED_STATES = 2,                           // extended-Latin
    REALM_ZONE_OCEANIC       = 3,                           // extended-Latin
    REALM_ZONE_LATIN_AMERICA = 4,                           // extended-Latin
    REALM_ZONE_TOURNAMENT_5  = 5,                           // basic-Latin at create, any at login
    REALM_ZONE_KOREA         = 6,                           // East-Asian
    REALM_ZONE_TOURNAMENT_7  = 7,                           // basic-Latin at create, any at login
    REALM_ZONE_ENGLISH       = 8,                           // extended-Latin
    REALM_ZONE_GERMAN        = 9,                           // extended-Latin
    REALM_ZONE_FRENCH        = 10,                          // extended-Latin
    REALM_ZONE_SPANISH       = 11,                          // extended-Latin
    REALM_ZONE_RUSSIAN       = 12,                          // Cyrillic
    REALM_ZONE_TOURNAMENT_13 = 13,                          // basic-Latin at create, any at login
    REALM_ZONE_TAIWAN        = 14,                          // East-Asian
    REALM_ZONE_TOURNAMENT_15 = 15,                          // basic-Latin at create, any at login
    REALM_ZONE_CHINA         = 16,                          // East-Asian
    REALM_ZONE_CN1           = 17,                          // basic-Latin at create, any at login
    REALM_ZONE_CN2           = 18,                          // basic-Latin at create, any at login
    REALM_ZONE_CN3           = 19,                          // basic-Latin at create, any at login
    REALM_ZONE_CN4           = 20,                          // basic-Latin at create, any at login
    REALM_ZONE_CN5           = 21,                          // basic-Latin at create, any at login
    REALM_ZONE_CN6           = 22,                          // basic-Latin at create, any at login
    REALM_ZONE_CN7           = 23,                          // basic-Latin at create, any at login
    REALM_ZONE_CN8           = 24,                          // basic-Latin at create, any at login
    REALM_ZONE_TOURNAMENT_25 = 25,                          // basic-Latin at create, any at login
    REALM_ZONE_TEST_SERVER   = 26,                          // any language
    REALM_ZONE_TOURNAMENT_27 = 27,                          // basic-Latin at create, any at login
    REALM_ZONE_QA_SERVER     = 28,                          // any language
    REALM_ZONE_CN9           = 29                           // basic-Latin at create, any at login
};

// DB scripting commands
#define SCRIPT_COMMAND_TALK                  0              // source = unit, target=any, datalong (0=say, 1=whisper, 2=yell, 3=emote text)
#define SCRIPT_COMMAND_EMOTE                 1              // source = unit, datalong = anim_id
#define SCRIPT_COMMAND_FIELD_SET             2              // source = any, datalong = field_id, datalog2 = value
#define SCRIPT_COMMAND_MOVE_TO               3              // source = Creature, datalog2 = time, x/y/z
#define SCRIPT_COMMAND_FLAG_SET              4              // source = any, datalong = field_id, datalog2 = bitmask
#define SCRIPT_COMMAND_FLAG_REMOVE           5              // source = any, datalong = field_id, datalog2 = bitmask
#define SCRIPT_COMMAND_TELEPORT_TO           6              // source or target with Player, datalong = map_id, x/y/z
#define SCRIPT_COMMAND_QUEST_EXPLORED        7              // one from source or target must be Player, another GO/Creature, datalong=quest_id, datalong2=distance or 0
#define SCRIPT_COMMAND_RESPAWN_GAMEOBJECT    9              // source = any (summoner), datalong=db_guid, datalong2=despawn_delay
#define SCRIPT_COMMAND_TEMP_SUMMON_CREATURE 10              // source = any (summoner), datalong=creature entry, datalong2=despawn_delay
#define SCRIPT_COMMAND_OPEN_DOOR            11              // source = unit, datalong=db_guid, datalong2=reset_delay
#define SCRIPT_COMMAND_CLOSE_DOOR           12              // source = unit, datalong=db_guid, datalong2=reset_delay
#define SCRIPT_COMMAND_ACTIVATE_OBJECT      13              // source = unit, target=GO
#define SCRIPT_COMMAND_REMOVE_AURA          14              // source (datalong2!=0) or target (datalong==0) unit, datalong = spell_id
#define SCRIPT_COMMAND_CAST_SPELL           15              // source (datalong2!=0) or target (datalong==0) unit, datalong = spell_id
#define SCRIPT_COMMAND_LOAD_PATH            16              // source = unit, path = datalong, repeatable datalong2
#define SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT   17              // datalong scriptid, lowguid datalong2, dataint table
#define SCRIPT_COMMAND_PLAY_SOUND           18              // source = any object, target=any/player, datalong (sound_id), datalong2 (bitmask: 0/1=anyone/target, 0/2=with distance dependent, so 1|2 = 3 is target with distance dependent)
#define SCRIPT_COMMAND_KILL                 19              // datalong removecorpse
#define SCRIPT_COMMAND_SET_INST_DATA        20              // source = any, datalong = type, datalong2 = data


/// Storage class for commands issued for delayed execution
struct CliCommandHolder
{
    typedef void Print(const char*);

    char *m_command;
    Print* m_print;

    CliCommandHolder(const char *command, Print* zprint)
        : m_print(zprint)
    {
        size_t len = strlen(command)+1;
        m_command = new char[len];
        memcpy(m_command, command, len);
    }

    ~CliCommandHolder() { delete[] m_command; }
};

// ye place for this sucks
#define MAX_PVP_RANKS 14

typedef tbb::concurrent_hash_map<uint32, std::list<uint64> > LfgContainerType;
typedef UNORDERED_MAP<uint32, WorldSession*> SessionMap;

enum CumulateMapDiff
{
    DIFF_SESSION_UPDATE          = 0,
    DIFF_PLAYER_UPDATE           = 1,
    DIFF_CREATURE_UPDATE         = 2,
    DIFF_PET_UPDATE              = 3,

    DIFF_PLAYER_GRID_VISIT       = 4,
    DIFF_ACTIVEUNIT_GRID_VISIT   = 5,

    DIFF_SEND_OBJECTS_UPDATE     = 6,
    DIFF_PROCESS_SCRIPTS         = 7,
    DIFF_MOVE_CREATURES_IN_LIST  = 8,

    DIFF_PROCESS_RELOCATION      = 9,

    DIFF_MAP_SPECIAL_DATA_UPDATE = 10,

    DIFF_MAX_CUMULATIVE_INFO     = 11
};

typedef ACE_Atomic_Op<ACE_Thread_Mutex, uint32> atomic_uint;

struct MapUpdateDiffInfo
{

    ~MapUpdateDiffInfo()
    {
        for (CumulativeDiffMap::iterator itr = _cumulativeDiffInfo.begin(); itr != _cumulativeDiffInfo.end(); ++itr)
            delete itr->second;
    }

    void InitializeMapData();

    void ClearDiffInfo()
    {
        for (CumulativeDiffMap::iterator itr = _cumulativeDiffInfo.begin(); itr != _cumulativeDiffInfo.end(); ++itr)
        {
            for (int i = DIFF_SESSION_UPDATE; i < DIFF_MAX_CUMULATIVE_INFO; i++)
                itr->second[i] = 0;
        }
    }

    void CumulateDiffFor(CumulateMapDiff type, uint32 diff, uint32 mapid)
    {
        _cumulativeDiffInfo[mapid][type] += diff;
    }

    void PrintCumulativeMapUpdateDiff();

    typedef std::map<uint32, atomic_uint*> CumulativeDiffMap;

    CumulativeDiffMap _cumulativeDiffInfo;
};

enum CBTresholds
{
    CB_DISABLE_NONE           = 0,
    CB_DISABLE_LOW_PRIORITY   = 1,
    CB_DISABLE_MID_PRIORITY   = 2,
    CB_DISABLE_HIGH_PRIORITY  = 3,
    CB_VISIBILITY_PENALTY     = 4,

    CB_TRESHOLD_MAX,
};


inline
bool operator>(FeaturePriority rhs, CBTresholds lhs)
{
    return static_cast<int>(rhs) > static_cast<int>(lhs);
}

inline
bool operator<=(FeaturePriority rhs, CBTresholds lhs)
{
    return !(rhs > lhs);
}

class CoreBalancer
{
    public:
        CoreBalancer();

        void Initialize();
        void Update(const uint32);
        void IncreaseTreshold();
        void DecreaseTreshold();

        CBTresholds GetTreshold() const { return _treshold; }

    private:
        uint32 _diffSum;
        uint32 _diffCount;
        CBTresholds _treshold;
        TimeTrackerSmall _balanceTimer;
};

/// The World
class HELLGROUND_EXPORT World
{
    friend class ACE_Singleton<World, ACE_Null_Mutex>;

    public:
        World();
        ~World();

        DelayExecutor m_ac;

        uint32 m_honorRanks[MAX_PVP_RANKS];

        static volatile uint32 m_worldLoopCounter;

        WorldSession* FindSession(uint32 id) const;
        void AddSession(WorldSession *s);
        bool RemoveSession(uint32 id);
        void AddSessionToRemove(SessionMap::iterator itr) { removedSessions.push_back(itr); }

        /// Get the number of current active sessions
        void UpdateMaxSessionCounters();
        uint32 GetActiveAndQueuedSessionCount() const { return m_sessions.size(); }
        uint32 GetActiveSessionCount() const { return m_sessions.size() - m_QueuedPlayer.size(); }
        uint32 GetQueuedSessionCount() const { return m_QueuedPlayer.size(); }
        const SessionMap& GetAllSessions() const { return m_sessions; }

        uint32 GetLoggedInCharsCount(TeamId team);
        uint32 ModifyLoggedInCharsCount(TeamId team, int val);
        void SetLoggedInCharsCount(TeamId team, uint32 val);

        /// Get the maximum number of parallel sessions on the server since last reboot
        uint32 GetMaxQueuedSessionCount() const { return m_maxQueuedSessionCount; }
        uint32 GetMaxActiveSessionCount() const { return m_maxActiveSessionCount; }
        Player* FindPlayerInZone(uint32 zone);

        Weather* FindWeather(uint32 id) const;
        Weather* AddWeather(uint32 zone_id);
        void RemoveWeather(uint32 zone_id);

        /// Get the active session server limit (or security level limitations)
        uint32 GetPlayerAmountLimit() const { return m_playerLimit; }
        uint64 GetMinimumPermissionMask() const { return m_requiredPermissionMask; }

        /// Set the active session server limit (or security level limitation)
        void SetPlayerLimit(int32 limit);

        //player Queue
        typedef std::list<WorldSession*> Queue;
        void AddQueuedPlayer(WorldSession*);
        bool RemoveQueuedPlayer(WorldSession* session);
        int32 GetQueuePos(WorldSession*);
        bool HasRecentlyDisconnected(WorldSession*);
        uint32 GetQueueSize() const { return m_QueuedPlayer.size(); }

        /// \todo Actions on m_allowMovement still to be implemented
        /// Is movement allowed?
        bool getAllowMovement() const { return m_allowMovement; }
        /// Allow/Disallow object movements
        void SetAllowMovement(bool allow) { m_allowMovement = allow; }

        /// Set a new Message of the Day
        void SetMotd(std::string motd) { m_motd = motd; }
        /// Get the current Message of the Day
        const char* GetMotd() const { return m_motd.c_str(); }

        /// Set the string for new characters (first login)
        void SetNewCharString(std::string str) { m_newCharString = str; }
        /// Get the string for new characters (first login)
        const std::string& GetNewCharString() const { return m_newCharString; }

        uint32 GetDefaultDbcLocale() const { return m_defaultDbcLocale; }

        /// Get the path where data (dbc, maps) are stored on disk
        std::string GetDataPath() const { return m_dataPath; }

        /// When server started?
        time_t const& GetStartTime() const { return m_startTime; }
        /// What time is it?
        time_t const& GetGameTime() const { return m_gameTime; }
        /// Uptime (in secs)
        uint32 GetUptime() const { return uint32(m_gameTime - m_startTime); }
        /// Update time
        uint32 GetUpdateTime() const { return m_updateTime; }
        void SetRecordDiffInterval(int32 t) { if (t >= 0) m_configs[CONFIG_INTERVAL_LOG_UPDATE] = (uint32)t; }

        /// Get the maximum skill level a player can reach
        uint16 GetConfigMaxSkillValue() const
        {
            uint32 lvl = getConfig(CONFIG_MAX_PLAYER_LEVEL);
            return lvl > 60 ? 300 + ((lvl - 60) * 75) / 10 : lvl*5;
        }

        void SetInitialWorldSettings();
        void LoadConfigSettings(bool reload = false);

        void QueueGuildAnnounce(uint32 guildid, uint32 team, std::string &msg);
        void SendGuildAnnounce(uint32 team, ...);

        void SendWorldText(int32 string_id, uint32 preventFlags, ...);
        void SendWorldTextForLevels(uint32 minLevel, uint32 maxLevel, uint32 preventFlags, int32 string_id, ...);
        void SendGlobalText(const char* text, WorldSession *self);
        void SendGMText(int32 string_id, ...);
        void SendGlobalMessage(WorldPacket *packet, WorldSession *self = 0, uint32 team = 0);
        void SendGlobalGMMessage(WorldPacket *packet, WorldSession *self = 0, uint32 team = 0);
        void SendZoneMessage(uint32 zone, WorldPacket *packet, WorldSession *self = 0, uint32 team = 0);
        void SendZoneText(uint32 zone, const char *text, WorldSession *self = 0, uint32 team = 0);
        void SendServerMessage(ServerMessageType type, const char *text = "", Player* player = NULL);

        /// Are we in the middle of a shutdown?
        bool IsShutdowning() const { return m_ShutdownTimer > 0; }
        uint32 GetShutdownMask() const { return m_ShutdownMask; }
        uint32 GetShutdownTimer() const { return m_ShutdownTimer; }
        char const* GetShutdownReason() { return m_ShutdownReason.c_str(); }

        void ShutdownServ(uint32 time, uint32 options, uint8 exitcode, char const* = "no reason");
        void ShutdownCancel();
        void ShutdownMsg(bool show = false, Player* player = NULL);
        static uint8 GetExitCode() { return m_ExitCode; }
        static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }
        static bool IsStopped() { return m_stopEvent; }

        void LoadAutobroadcasts();
        void Update(uint32 diff);
        void UpdateSessions(const uint32 & diff);

        void setConfig(WorldConfigs index, uint32 value)
        {
            if (index < CONFIG_VALUE_COUNT)
                m_configs[index] = value;
        }

        /* Unused
        void setConfig(Rates index, float value)
        {
            if (index < MAX_RATES)
                rate_values[index] = value;
        }*/

        uint32 getConfig(WorldConfigs index) const
        {
            return (index < CONFIG_VALUE_COUNT) ? m_configs[index] : 0;
        }

        float getConfig(Rates index) const
        {
            return (index < MAX_RATES) ? rate_values[index] : 0.0f ;
        }

        void loadConfig(WorldConfigs index, const char* name, int32 def = 0);
        void loadConfig(Rates index, const char* name, float def = 0);
        void loadConfig(WorldConfigs index, const char* name, bool def);

        /// Are we on a "Player versus Player" server?
        bool IsPvPRealm() { return (getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_PVP || getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_RPPVP || getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_FFA_PVP); }
        bool IsFFAPvPRealm() { return getConfig(CONFIG_GAME_TYPE) == REALM_TYPE_FFA_PVP; }

        bool KickPlayer(const std::string& playerName);
        void KickAll();
        void KickAllWithoutPermissions(uint64 perms);
        BanReturn BanAccount(BanMode mode, std::string nameIPOrMail, std::string duration, std::string reason, std::string author);
        bool RemoveBanAccount(BanMode mode, std::string nameIPOrMail);

        uint32 IncreaseScheduledScriptsCount() { return (uint32)++m_scheduledScripts; }
        uint32 DecreaseScheduledScriptCount() { return (uint32)--m_scheduledScripts; }
        uint32 DecreaseScheduledScriptCount(size_t count) { return (uint32)(m_scheduledScripts -= count); }
        bool IsScriptScheduled() const { return m_scheduledScripts > 0; }

        bool IsAllowedMap(uint32 mapid) { return m_forbiddenMapIds.count(mapid) == 0 ;}

        static float GetVisibleObjectGreyDistance()         { return m_VisibleObjectGreyDistance;     }

        static int32 GetActiveObjectUpdateDistanceOnContinents() { return m_activeObjectUpdateDistanceOnContinents; }
        static int32 GetActiveObjectUpdateDistanceInInstances() { return m_activeObjectUpdateDistanceInInstances; }

        void ProcessCliCommands();
        void QueueCliCommand(CliCommandHolder::Print* zprintf, char const* input) { cliCmdQueue.add(new CliCommandHolder(input, zprintf)); }

        void UpdateResultQueue();
        void InitResultQueue();

        void ForceGameEventUpdate();

        void UpdateRealmCharCount(uint32 accid);

        void UpdateRequiredPermissions();

        LocaleConstant GetAvailableDbcLocale(LocaleConstant locale) const { if (m_availableDbcLocaleMask & (1 << locale)) return locale; else return m_defaultDbcLocale; }

        //used World DB version
        void LoadDBVersion();
        char const* GetDBVersion() { return m_DBVersion.c_str(); }

        //used Script version
        void SetScriptsVersion(char const* version) { m_ScriptsVersion = version ? version : "unknown scripting library"; }
        char const* GetScriptsVersion() { return m_ScriptsVersion.c_str(); }

        void addDisconnectTime(std::pair<uint32,time_t> tPair){ m_disconnects.insert(tPair); }

        void CleanupDeletedChars();

        void SelectRandomHeroicDungeonDaily();
        void SelectRandomDungeonDaily();
        void SelectRandomCookingDaily();
        void SelectRandomFishingDaily();
        void SelectRandomPvPDaily();

        void SetMassMute(uint64 time, const char * reason)
        {
            m_massMuteTime = time;

            if (reason)
                m_massMuteReason = reason;
            else
                m_massMuteReason = "no-reason";
        }

        uint64 GetMassMuteTime() { return m_massMuteTime; }
        const char * GetMassMuteReason() { return m_massMuteReason.c_str(); }

        LfgContainerType & GetLfgContainer(uint32 team)
        {
            switch (team)
            {
                case ALLIANCE:
                    return lfgAllyContainer;
                default:
                    return lfgHordeContainer;
            }
        }

        // LFG container, lfg instance id to player guid list. Should be less lockable than prev implementation
        LfgContainerType lfgHordeContainer;
        LfgContainerType lfgAllyContainer;

        CBTresholds GetCoreBalancerTreshold();

        MAP_UPDATE_DIFF(MapUpdateDiffInfo& MapUpdateDiff() { return m_mapUpdateDiffInfo; })

    protected:
        void _UpdateGameTime();
        void InitDailyQuestResetTime();
        void ResetDailyQuests();

    private:
        static volatile bool m_stopEvent;
        static uint8 m_ExitCode;
        uint32 m_ShutdownTimer;
        uint32 m_ShutdownMask;
        std::string m_ShutdownReason;

        uint32 sessionThreads;

        std::list<SessionMap::iterator> removedSessions;

        //atomic op counter for active scripts amount
        ACE_Atomic_Op<ACE_Thread_Mutex, long> m_scheduledScripts;

        time_t m_startTime;
        time_t m_gameTime;
        IntervalTimer m_timers[WUPDATE_COUNT];
        uint32 mail_timer;
        uint32 mail_timer_expires;
        uint32 m_updateTime, m_updateTimeSum, m_avgUpdateTime, m_curAvgUpdateTime;

        uint32 m_updateTimeCount;

        MAP_UPDATE_DIFF(MapUpdateDiffInfo m_mapUpdateDiffInfo)
        uint64 m_serverUpdateTimeSum, m_serverUpdateTimeCount;

        CoreBalancer _coreBalancer;

        typedef UNORDERED_MAP<uint32, Weather*> WeatherMap;
        WeatherMap m_weathers;
        SessionMap m_sessions;
        typedef UNORDERED_MAP<uint32, time_t> DisconnectMap;
        DisconnectMap m_disconnects;
        uint32 m_maxActiveSessionCount;
        uint32 m_maxQueuedSessionCount;

        std::string m_newCharString;

        float rate_values[MAX_RATES];
        uint32 m_configs[CONFIG_VALUE_COUNT];
        
        uint32 m_playerLimit;
        uint64 m_requiredPermissionMask;
        LocaleConstant m_defaultDbcLocale;                     // from config for one from loaded DBC locales
        uint32 m_availableDbcLocaleMask;                       // by loaded DBC
        void DetectDBCLang();
        bool m_allowMovement;
        std::string m_motd;
        std::string m_dataPath;
        std::set<uint32> m_forbiddenMapIds;

        uint64 m_massMuteTime;
        std::string m_massMuteReason;

        static float m_VisibleObjectGreyDistance;

        static int32 m_activeObjectUpdateDistanceOnContinents;
        static int32 m_activeObjectUpdateDistanceInInstances;

        static uint32 m_relocationLowerLimitSq;
        static uint32 m_relocationAINotifyDelay;

        // CLI command holder to be thread safe
        ACE_Based::LockedQueue<CliCommandHolder*, ACE_Thread_Mutex> cliCmdQueue;

        // next daily quests reset time
        time_t m_NextDailyQuestReset;

        // Player Queue
        Queue m_QueuedPlayer;

        // characters count
        atomic_uint loggedInAlliances;
        atomic_uint loggedInHordes;

        // sessions that are added async
        void AddSession_(WorldSession* s);
        ACE_Based::LockedQueue<WorldSession*, ACE_Thread_Mutex> addSessQueue;

        std::list<std::string> m_Autobroadcasts;
        std::list<std::pair<uint64, std::string> > m_GuildAnnounces[2];

        //used versions
        std::string m_DBVersion;
        std::string m_ScriptsVersion;
};

extern uint32 realmID;

#define sWorld (*ACE_Singleton<World, ACE_Null_Mutex>::instance())

class SessionsUpdater
{
private:
    SessionMap * sessions;
    uint32 diff;

public:
    SessionsUpdater(SessionMap * sess, uint32 diff) : sessions(sess), diff(diff) {}

    void operator () (const tbb::blocked_range<int>& r) const
    {
        SessionMap::iterator itr = sessions->begin();
        advance(itr, r.begin());
        SessionMap::iterator itrEnd = sessions->begin();
        advance(itrEnd, r.end());

        for (; itr != itrEnd; ++itr)
        {
            if (!itr->second)
                continue;

            ///- and remove not active sessions from the list
            WorldSession * pSession = itr->second;
            WorldSessionFilter updater(pSession);
            if (!pSession->Update(diff, updater))    // As interval = 0
            {
                sWorld.RemoveQueuedPlayer(pSession);

                sWorld.AddSessionToRemove(itr);
            }
        }
    }
};

#endif
/// @}
