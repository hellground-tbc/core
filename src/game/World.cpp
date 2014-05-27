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

/** \file
    \ingroup world
*/

#include "Common.h"
//#include "WorldSocket.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "SystemConfig.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "Weather.h"
#include "Player.h"
#include "SkillExtraItems.h"
#include "SkillDiscovery.h"
#include "World.h"
#include "AccountMgr.h"
#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "LootMgr.h"
#include "ItemEnchantmentMgr.h"
#include "MapManager.h"
#include "ScriptMgr.h"
#include "CreatureAIRegistry.h"
#include "BattleGroundMgr.h"
#include "OutdoorPvPMgr.h"
#include "TemporarySummon.h"
#include "WaypointMovementGenerator.h"
#include "VMapFactory.h"
#include "movemap/MoveMap.h"
#include "GameEvent.h"
#include "PoolManager.h"
#include "Database/DatabaseImpl.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "TicketMgr.h"
#include "Util.h"
#include "Language.h"
#include "CreatureGroups.h"
#include "Transports.h"
#include "CreatureEventAIMgr.h"
#include "WardenDataStorage.h"
#include "WorldEventProcessor.h"
#include "luaengine/HookMgr.h"
//#include "Timer.h"
#include "GuildMgr.h"
#include <tbb/parallel_for.h>

extern bool StartEluna();
volatile bool World::m_stopEvent = false;
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
volatile uint32 World::m_worldLoopCounter = 0;

float World::m_VisibleObjectGreyDistance      = 0;

int32 World::m_activeObjectUpdateDistanceOnContinents = DEFAULT_VISIBILITY_DISTANCE;
int32 World::m_activeObjectUpdateDistanceInInstances = DEFAULT_VISIBILITY_DISTANCE;

void MapUpdateDiffInfo::InitializeMapData()
{
    for(MapManager::MapMapType::const_iterator map = sMapMgr.Maps().begin(); map != sMapMgr.Maps().end(); ++map)
    {
        if (_cumulativeDiffInfo.find(map->first.nMapId) == _cumulativeDiffInfo.end())
        {
            _cumulativeDiffInfo[map->first.nMapId] = new atomic_uint[DIFF_MAX_CUMULATIVE_INFO];
            for (int i = DIFF_SESSION_UPDATE; i < DIFF_MAX_CUMULATIVE_INFO; i++)
                _cumulativeDiffInfo[map->first.nMapId][i] = 0;
        }
    }
}

void MapUpdateDiffInfo::PrintCumulativeMapUpdateDiff()
{
    for (CumulativeDiffMap::iterator itr = _cumulativeDiffInfo.begin(); itr != _cumulativeDiffInfo.end(); ++itr)
    {
        for (int i = DIFF_SESSION_UPDATE; i < DIFF_MAX_CUMULATIVE_INFO; i++)
        {
            uint32 diff = itr->second[i].value();
            if (diff >= sWorld.getConfig(CONFIG_MIN_LOG_UPDATE))
                sLog.outLog(LOG_DIFF, "Map[%u] diff for: %i - %u", itr->first, i, diff);
        }
    }
    ClearDiffInfo();
}

/// World constructor
World::World()
{
    m_playerLimit = 0;
    m_requiredPermissionMask = PERM_PLAYER;
    m_allowMovement = true;
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_gameTime=time(NULL);
    m_startTime=m_gameTime;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;
    m_NextDailyQuestReset = 0;
    m_scheduledScripts = 0;

    m_defaultDbcLocale = LOCALE_enUS;
    m_availableDbcLocaleMask = 0;

    m_updateTimeSum = 0;
    m_updateTimeCount = 0;

    m_massMuteTime = 0;

    loggedInAlliances = 0;
    loggedInHordes = 0;

    // TODO: move to config
    m_honorRanks[0] = 100;
    m_honorRanks[1] = 500;
    m_honorRanks[2] = 1000;
    m_honorRanks[3] = 2000;
    m_honorRanks[4] = 5000;
    m_honorRanks[5] = 10000;
    m_honorRanks[6] = 16000;
    m_honorRanks[7] = 23000;
    m_honorRanks[8] = 31000;
    m_honorRanks[9] = 40000;
    m_honorRanks[10] = 50000;
    m_honorRanks[11] = 60000;
    m_honorRanks[12] = 70000;
    m_honorRanks[13] = 100000;
}

/// World destructor
World::~World()
{
    ///- Empty the kicked session set
    while (!m_sessions.empty())
    {
        // not remove from queue, prevent loading new sessions
        WorldSession *temp = m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
        delete temp;
    }

    // Here event list SHOULD be already empty, but who knows what can happen :p
    sWorldEventProcessor.DestroyEvents();

    ///- Empty the WeatherMap
    for (WeatherMap::iterator itr = m_weathers.begin(); itr != m_weathers.end(); ++itr)
        delete itr->second;

    m_weathers.clear();

    CliCommandHolder* command;
    while (cliCmdQueue.next(command))
        delete command;


    VMAP::VMapFactory::clear();
    MMAP::MMapFactory::clear();

    //TODO free addSessQueue
}

/// Find a player in a specified zone
Player* World::FindPlayerInZone(uint32 zone)
{
    ///- circle through active sessions and return the first player found in the zone
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;
        Player *player = itr->second->GetPlayer();
        if (!player)
            continue;
        if (player->IsInWorld() && player->GetCachedZone() == zone)
        {
            // Used by the weather system. We return the player to broadcast the change weather message to him and all players in the zone.
            return player;
        }
    }
    return NULL;
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;                                 // also can return NULL for kicked session
    else
        return NULL;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;
        itr->second->KickPlayer();
    }

    return true;
}

void World::AddSession(WorldSession* s)
{
    addSessQueue.add(s);
}

void World::AddSession_ (WorldSession* s)
{
    if (!s)
        return;

    //NOTE - Still there is race condition in WorldSession* being used in the Sockets

    ///- kick already loaded player with same account (if any) and remove session
    ///- if player is in loading and want to load again, return
    if (!RemoveSession (s->GetAccountId ()))
    {
        s->KickPlayer ();
        delete s;                                           // session not added yet in session list, so not listed in queue
        return;
    }

    //m_ac_auth[s->GetRemoteAddress()] = 15000;

    // decrease session counts only at not reconnection case
    bool decrease_session = true;

    // if session already exist, prepare to it deleting at next world update
    // NOTE - KickPlayer() should be called on "old" in RemoveSession()
    {
        SessionMap::const_iterator old = m_sessions.find(s->GetAccountId ());

        if (old != m_sessions.end())
        {
            // prevent decrease sessions count if session queued
            if (RemoveQueuedPlayer(old->second))
                decrease_session = false;
            // not remove replaced session form queue if listed
            delete old->second;
        }
    }

    m_sessions[s->GetAccountId()] = s;

    uint32 Sessions = GetActiveAndQueuedSessionCount ();
    uint32 QueueSize = GetQueueSize (); //number of players in the queue

    //so we don't count the user trying to
    //login as a session and queue the socket that we are using
    if (decrease_session)
        --Sessions;

    if (m_playerLimit > 0 && Sessions >= m_playerLimit && !s->HasPermissions(PERM_GMT_HDEV))
    {
        if (!sObjectMgr.IsUnqueuedAccount(s->GetAccountId()) && !HasRecentlyDisconnected(s))
        {
            AddQueuedPlayer (s);
            UpdateMaxSessionCounters ();
            sLog.outDetail ("PlayerQueue: Account id %u is in Queue Position (%u).", s->GetAccountId (), ++QueueSize);
            return;
        }
    }

    WorldPacket packet(SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4 + 1);
    packet << uint8(AUTH_OK);
    packet << uint32(0);                        // unknown random value...
    packet << uint8(0);
    packet << uint32(0);
    packet << uint8(s->Expansion());            // 0 - normal, 1 - TBC, must be set in database manually for each account
    s->SendPacket (&packet);

    UpdateMaxSessionCounters();

    // Updates the population
    if (m_playerLimit > 0)
    {
        float popu = GetActiveSessionCount (); //updated number of users on the server
        popu /= m_playerLimit;
        popu *= 2;
        AccountsDatabase.PExecute ("UPDATE realms SET population = '%f' WHERE realm_id = '%u'", popu, realmID);
        sLog.outDetail ("Server Population (%f).", popu);
    }
}

bool World::HasRecentlyDisconnected(WorldSession* session)
{
    if (!session)
        return false;

    if (uint32 tolerance = getConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
    {
        for (DisconnectMap::iterator next, i = m_disconnects.begin(); i != m_disconnects.end(); i = next)
        {
            next = i;
            next++;

            if (i->first == session->GetAccountId())
            {
                if (difftime(i->second, time(NULL)) <= tolerance)
                    return true;
                else
                    m_disconnects.erase(i);
            }
        }
    }
    return false;
 }

int32 World::GetQueuePos(WorldSession* sess)
{
    uint32 position = 1;

    for (Queue::iterator iter = m_QueuedPlayer.begin(); iter != m_QueuedPlayer.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void World::AddQueuedPlayer(WorldSession* sess)
{
    sess->SetInQueue(true);
    m_QueuedPlayer.push_back (sess);

    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    WorldPacket packet (SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4 + 1);
    packet << uint8 (AUTH_WAIT_QUEUE);
    packet << uint32 (0); // unknown random value...
    packet << uint8 (0);
    packet << uint32 (0);
    packet << uint8 (sess->Expansion () ? 1 : 0); // 0 - normal, 1 - TBC, must be set in database manually for each account
    packet << uint32(GetQueuePos (sess));
    sess->SendPacket (&packet);

    //sess->SendAuthWaitQue (GetQueuePos (sess));
}

bool World::RemoveQueuedPlayer(WorldSession* sess)
{
    // sessions count including queued to remove (if removed_session set)
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    Queue::iterator iter = m_QueuedPlayer.begin();

    // search to remove and count skipped positions
    bool found = false;

    for (;iter != m_QueuedPlayer.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            iter = m_QueuedPlayer.erase(iter);
            found = true;                                   // removing queued session
            break;
        }
    }

    // iter point to next socked after removed or end()
    // position store position of removed socket and then new position next socket after removed

    // if session not queued then we need decrease sessions count
    if (!found && sessions)
        --sessions;

    // accept first in queue
    if ((!m_playerLimit || (sessions < m_playerLimit)) && !m_QueuedPlayer.empty())
    {
        WorldSession* pop_sess = m_QueuedPlayer.front();
        pop_sess->SetInQueue(false);
        pop_sess->SendAuthWaitQue(0);
        m_QueuedPlayer.pop_front();

        // update iter to point first queued socket or end() if queue is empty now
        iter = m_QueuedPlayer.begin();
        position = 1;
    }

    // update position from iter to end()
    // iter point to first not updated socket, position store new position
    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
        (*iter)->SendAuthWaitQue(position);

    if (!found && getConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
    {
        std::pair<uint32, time_t> tPair;
        tPair.first = sess->GetAccountId();
        tPair.second = time(NULL);

        addDisconnectTime(tPair);
    }
    return found;
}

/// Find a Weather object by the given zoneid
Weather* World::FindWeather(uint32 id) const
{
    WeatherMap::const_iterator itr = m_weathers.find(id);

    if (itr != m_weathers.end())
        return itr->second;
    else
        return 0;
}

/// Remove a Weather object for the given zoneid
void World::RemoveWeather(uint32 id)
{
    // not called at the moment. Kept for completeness
    WeatherMap::iterator itr = m_weathers.find(id);

    if (itr != m_weathers.end())
    {
        Weather *temp = itr->second;
        m_weathers.erase(itr);
        delete temp;
    }
}

/// Add a Weather object to the list
Weather* World::AddWeather(uint32 zone_id)
{
    WeatherZoneChances const* weatherChances = sObjectMgr.GetWeatherChances(zone_id);

    // zone not have weather, ignore
    if (!weatherChances)
        return NULL;

    Weather* w = new Weather(zone_id,weatherChances);
    m_weathers[w->GetZone()] = w;
    w->ReGenerate();
    w->UpdateWeather();
    return w;
}

/// Initialize config values
void World::LoadConfigSettings(bool reload)
{
    if (reload && !sConfig.Reload())
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: World settings reload fail: can't read settings from %s.",sConfig.GetFilename().c_str());
        return;
    }

    std::string dataPath = sConfig.GetStringDefault("DataDir", "./");
    if (dataPath.at(dataPath.length()-1)!='/' && dataPath.at(dataPath.length()-1)!='\\')
        dataPath.append("/");

    if (reload)
    {
        if (dataPath!=m_dataPath)
            sLog.outLog(LOG_DEFAULT, "ERROR: DataDir option can't be changed at .conf file reload, using current value (%s).",m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
        sLog.outString("Using DataDir %s",m_dataPath.c_str());
    }

    // === Load section ===
    // Performance settings
    SetPlayerLimit(sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT));
    loadConfig(CONFIG_ADDON_CHANNEL, "AddonChannel", false);
    loadConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY, "SaveRespawnTimeImmediately", true);
    loadConfig(CONFIG_GRID_UNLOAD, "GridUnload", true);

    loadConfig(CONFIG_INTERVAL_CHANGEWEATHER, "ChangeWeatherInterval", 600000);
    loadConfig(CONFIG_INTERVAL_SAVE, "PlayerSaveInterval", 900000);
    loadConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE, "DisconnectToleranceInterval", 0);

    loadConfig(CONFIG_NUMTHREADS, "MapUpdate.Threads", 1);
    if (m_configs[CONFIG_NUMTHREADS] < 1)
        m_configs[CONFIG_NUMTHREADS] = 1;
    loadConfig(CONFIG_MAPUPDATE_MAXVISITORS, "MapUpdate.UpdateVisitorsMax", 0);
    loadConfig(CONFIG_CUMULATIVE_LOG_METHOD, "MapUpdate.CumulativeLogMethod", 0);

    sessionThreads = sConfig.GetIntDefault("SessionUpdate.Threads", 0);
    loadConfig(CONFIG_SESSION_UPDATE_MAX_TIME, "SessionUpdate.MaxTime", 1000);
    loadConfig(CONFIG_SESSION_UPDATE_OVERTIME_METHOD, "SessionUpdate.Method", 3);
    loadConfig(CONFIG_SESSION_UPDATE_VERBOSE_LOG, "SessionUpdate.VerboseLog", 0);
    loadConfig(CONFIG_SESSION_UPDATE_IDLE_KICK, "SessionUpdate.IdleKickTimer", 15*MINUTE*IN_MILISECONDS);
    loadConfig(CONFIG_SESSION_UPDATE_MIN_LOG_DIFF, "SessionUpdate.MinLogDiff", 25);
    loadConfig(CONFIG_INTERVAL_LOG_UPDATE, "RecordUpdateTimeDiffInterval", 60000);
    loadConfig(CONFIG_MIN_LOG_UPDATE, "MinRecordUpdateTimeDiff", 10);

    // Server settings
    if (m_configs[CONFIG_REALM_ZONE] == REALM_ZONE_RUSSIAN)
        m_configs[CONFIG_DECLINED_NAMES_USED] = true;
    else
        loadConfig(CONFIG_DECLINED_NAMES_USED, "DeclinedNames", false);
    loadConfig(CONFIG_STRICT_PLAYER_NAMES, "StrictPlayerNames",  0);
    loadConfig(CONFIG_STRICT_CHARTER_NAMES, "StrictCharterNames", 0);
    loadConfig(CONFIG_STRICT_PET_NAMES, "StrictPetNames", 0);
    loadConfig(CONFIG_ACTIVE_BANS_UPDATE_TIME, "ActiveBansUpdateTime", 30000);
    loadConfig(CONFIG_ELUNA_ENABLED, "LuaEngine.Enabled", false);

    // Server customization basic
    loadConfig(CONFIG_CHARACTERS_CREATING_DISABLED, "CharactersCreatingDisabled", 0);
    loadConfig(CONFIG_START_ALL_TAXI_PATHS, "PlayerStart.AllFlightPaths", false);
    loadConfig(CONFIG_START_ALL_REP, "PlayerStart.AllReputation", false);
    loadConfig(CONFIG_START_ALL_SPELLS, "PlayerStart.AllSpells", false);
    loadConfig(CONFIG_START_ALL_EXPLORED, "PlayerStart.MapsExplored", false);
    SetNewCharString(sConfig.GetStringDefault("PlayerStart.String", ""));

    loadConfig(CONFIG_ALWAYS_MAXSKILL, "AlwaysMaxWeaponSkill", false);
    loadConfig(CONFIG_CAST_UNSTUCK, "CastUnstuck", true);
    loadConfig(CONFIG_DAILY_BLIZZLIKE, "DailyQuest.Blizzlike", true);
    loadConfig(CONFIG_DAILY_MAX_PER_DAY, "DailyQuest.MaxPerDay", 25);
    loadConfig(CONFIG_DISABLE_DUEL, "DisableDuel", false);
    loadConfig(CONFIG_DISABLE_PVP, "DisablePVP", false);
    loadConfig(CONFIG_EVENT_ANNOUNCE, "EventAnnounce", 0);
    loadConfig(CONFIG_FFA_DISALLOWGROUP, "FFA.DisallowGroup", false);
    loadConfig(CONFIG_HONOR_AFTER_DUEL, "HonorPointsAfterDuel", 0);
    if (m_configs[CONFIG_HONOR_AFTER_DUEL] < 0)
        m_configs[CONFIG_HONOR_AFTER_DUEL]= 0;
    loadConfig(CONFIG_INSTANCE_IGNORE_LEVEL, "Instance.IgnoreLevel", false);
    loadConfig(CONFIG_INSTANCE_IGNORE_RAID, "Instance.IgnoreRaid", false);
    loadConfig(CONFIG_MAX_PRIMARY_TRADE_SKILL, "MaxPrimaryTradeSkill", 2);
    SetMotd(sConfig.GetStringDefault("Motd", "Welcome to a HellgroundCore Server."));
    loadConfig(CONFIG_PVP_TOKEN_ENABLE, "PvPToken.Enable", false);
    loadConfig(CONFIG_PVP_TOKEN_ID, "PvPToken.ItemID", 29434);
    loadConfig(CONFIG_PVP_TOKEN_COUNT, "PvPToken.ItemCount", 1);
    if (m_configs[CONFIG_PVP_TOKEN_COUNT] < 1)
        m_configs[CONFIG_PVP_TOKEN_COUNT] = 1;
    loadConfig(CONFIG_PVP_TOKEN_MAP_TYPE, "PvPToken.MapAllowType", 4);
    loadConfig(CONFIG_ENABLE_SINFO_LOGIN, "Server.LoginInfo", 0);
    loadConfig(CONFIG_SHOW_KICK_IN_WORLD, "ShowKickInWorld", false);

    loadConfig(CONFIG_DONT_DELETE_CHARS, "DontDeleteChars", false);
    loadConfig(CONFIG_DONT_DELETE_CHARS_LVL, "DontDeleteCharsLvl", 40);
    loadConfig(CONFIG_KEEP_DELETED_CHARS_TIME, "KeepDeletedCharsTime", 31);

    // Server customization advanced
    loadConfig(CONFIG_WEATHER, "ActivateWeather",true);
    loadConfig(CONFIG_ENABLE_SORT_AUCTIONS, "Auction.EnableSort", true);
    loadConfig(CONFIG_AUTOBROADCAST_INTERVAL, "AutoBroadcast.Timer", 35*MINUTE*1000);
    loadConfig(CONFIG_GROUPLEADER_RECONNECT_PERIOD, "GroupLeaderReconnectPeriod", 180);
    loadConfig(CONFIG_INSTANCE_RESET_TIME_HOUR, "Instance.ResetTimeHour", 4);
    loadConfig(CONFIG_INSTANCE_UNLOAD_DELAY, "Instance.UnloadDelay", 1800000);
    loadConfig(CONFIG_MAIL_DELIVERY_DELAY, "Mail.DeliveryDelay", HOUR);
    loadConfig(CONFIG_EXTERNAL_MAIL, "Mail.External", 0);
    loadConfig(CONFIG_EXTERNAL_MAIL_INTERVAL, "Mail.ExternalInterval", 1);
    loadConfig(CONFIG_GM_MAIL, "Mail.GmInstantSend", 1);
    loadConfig(CONFIG_RETURNOLDMAILS_MODE, "Mail.OldReturnMode", 0);
    loadConfig(CONFIG_RETURNOLDMAILS_INTERVAL, "Mail.OldReturnTimer", 60);
    loadConfig(CONFIG_GROUP_XP_DISTANCE, "MaxGroupXPDistance", 74);
    loadConfig(CONFIG_MAX_WHO, "MaxWhoListReturns", 49);
    loadConfig(CONFIG_NO_RESET_TALENT_COST, "NoResetTalentsCost", false);
    loadConfig(CONFIG_RABBIT_DAY, "Rabbit.Day", 0);
    loadConfig(CONFIG_SKILL_PROSPECTING, "SkillChance.Prospecting",false);

    // note: disable value (-1) will assigned as 0xFFFFFFF, to prevent overflow at calculations limit it to max possible player level MAX_LEVEL(100)
    loadConfig(CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF, "Quests.LowLevelHideDiff", 4);
    if (m_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    loadConfig(CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF, "Quests.HighLevelHideDiff", 7);
    if (m_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = MAX_LEVEL;

    std::string forbiddenmaps = sConfig.GetStringDefault("ForbiddenMaps", "");
    char * forbiddenMaps = new char[forbiddenmaps.length() + 1];
    forbiddenMaps[forbiddenmaps.length()] = 0;
    strncpy(forbiddenMaps, forbiddenmaps.c_str(), forbiddenmaps.length());
    const char * delim = ", ";
    char * token = strtok(forbiddenMaps, delim);
    while (token != NULL)
    {
        int32 mapid = strtol(token, NULL, 10);
        m_forbiddenMapIds.insert(mapid);
        token = strtok(NULL,delim);
    }
    delete[] forbiddenMaps;

    loadConfig(CONFIG_GUILD_ANN_INTERVAL, "GuildAnnounce.Timer", 1*MINUTE*1000);
    loadConfig(CONFIG_GUILD_ANN_COOLDOWN, "GuildAnnounce.Cooldown", 60*MINUTE);
    loadConfig(CONFIG_GUILD_ANN_LENGTH, "GuildAnnounce.Length", 60);

    loadConfig(CONFIG_ENABLE_CUSTOM_XP_RATES, "EnableCustomXPRates", true);
    loadConfig(CONFIG_XP_RATE_MODIFY_ITEM_ENTRY, "XPRateModifyItem.Entry",0);
    loadConfig(CONFIG_XP_RATE_MODIFY_ITEM_PCT, "XPRateModifyItem.Pct",5);

    // Player interaction
    loadConfig(CONFIG_ALLOW_TWO_SIDE_ACCOUNTS, "AllowTwoSide.Accounts", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND, "AllowTwoSide.AddFriend", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT, "AllowTwoSide.Interaction.Chat", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL, "AllowTwoSide.Interaction.Channel", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP, "AllowTwoSide.Interaction.Group", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD, "AllowTwoSide.Interaction.Guild", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION, "AllowTwoSide.Interaction.Auction", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_MAIL, "AllowTwoSide.Interaction.Mail", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_TRADE, "AllowTwoSide.Trade", false);
    loadConfig(CONFIG_ALLOW_TWO_SIDE_WHO_LIST, "AllowTwoSide.WhoList", false);
    loadConfig(CONFIG_TALENTS_INSPECTING, "TalentsInspecting", true);

    // Chat settings
    loadConfig(CONFIG_GLOBAL_TRADE_CHANNEL, "Channel.GlobalTradeChannel",false);
    loadConfig(CONFIG_PRIVATE_CHANNEL_LIMIT, "Channel.PrivateLimitCount", 20);
    loadConfig(CONFIG_RESTRICTED_LFG_CHANNEL, "Channel.RestrictedLfg", true);
    loadConfig(CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL, "Channel.SilentlyGMJoin", false);
    loadConfig(CONFIG_CHAT_DENY_MASK, "Chat.DenyMask", 0);
    loadConfig(CONFIG_CHAT_MINIMUM_LVL, "Chat.MinimumLevel", 5);
    loadConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING, "ChatFakeMessagePreventing", false);
    loadConfig(CONFIG_CHATFLOOD_MESSAGE_COUNT, "ChatFlood.MessageCount",10);
    loadConfig(CONFIG_CHATFLOOD_MESSAGE_DELAY, "ChatFlood.MessageDelay",1);
    loadConfig(CONFIG_CHATFLOOD_MUTE_TIME, "ChatFlood.MuteTime",10);

    // Game master settings
    loadConfig(CONFIG_GM_LOGIN_STATE, "GM.LoginState",2);
    loadConfig(CONFIG_GM_VISIBLE_STATE, "GM.Visible", 2);
    loadConfig(CONFIG_GM_CHAT, "GM.Chat",2);
    loadConfig(CONFIG_GM_WISPERING_TO, "GM.WhisperingTo",2);
    loadConfig(CONFIG_GM_IN_GM_LIST, "GM.InGMList",false);
    loadConfig(CONFIG_GM_IN_WHO_LIST, "GM.InWhoList",false);
    loadConfig(CONFIG_GM_LOG_TRADE, "GM.LogTrade", false);
    loadConfig(CONFIG_ALLOW_GM_GROUP, "GM.AllowInvite", false);
    loadConfig(CONFIG_ALLOW_GM_FRIEND, "GM.AllowFriend", false);
    loadConfig(CONFIG_GM_TRUSTED_LEVEL, "GM.TrustedLevel", PERM_HIGH_GMT);

    loadConfig(CONFIG_COMMAND_LOG_PERMISSION, "CommandLogPermission", PERM_GMT_DEV);
    loadConfig(CONFIG_INSTANT_LOGOUT, "InstantLogout", PERM_GMT_DEV);
    loadConfig(CONFIG_MIN_GM_TEXT_LVL, "MinGMTextLevel", PERM_GMT_HDEV);
    loadConfig(CONFIG_DISABLE_BREATHING, "DisableWaterBreath", PERM_CONSOLE);
    loadConfig(CONFIG_HIDE_GAMEMASTER_ACCOUNTS, "HideGameMasterAccounts", true);

    // Server rates
    loadConfig(RATE_HEALTH, "Rate.Health", 1.0f);
    loadConfig(RATE_POWER_MANA, "Rate.Mana", 1.0f);
    loadConfig(RATE_POWER_RAGE_INCOME, "Rate.Rage.Income", 1.0f);
    loadConfig(RATE_POWER_RAGE_LOSS, "Rate.Rage.Loss", 1.0f);
    loadConfig(RATE_POWER_FOCUS, "Rate.Focus", 1.0f);
    loadConfig(RATE_LOYALTY, "Rate.Loyalty", 1.0f);
    loadConfig(RATE_SKILL_DISCOVERY, "Rate.Skill.Discovery", 1.0f);
    loadConfig(RATE_DROP_ITEM_POOR, "Rate.Drop.Item.Poor", 1.0f);
    loadConfig(RATE_DROP_ITEM_NORMAL, "Rate.Drop.Item.Normal", 1.0f);
    loadConfig(RATE_DROP_ITEM_UNCOMMON, "Rate.Drop.Item.Uncommon", 1.0f);
    loadConfig(RATE_DROP_ITEM_RARE, "Rate.Drop.Item.Rare", 1.0f);
    loadConfig(RATE_DROP_ITEM_EPIC, "Rate.Drop.Item.Epic", 1.0f);
    loadConfig(RATE_DROP_ITEM_LEGENDARY, "Rate.Drop.Item.Legendary", 1.0f);
    loadConfig(RATE_DROP_ITEM_ARTIFACT, "Rate.Drop.Item.Artifact", 1.0f);
    loadConfig(RATE_DROP_ITEM_REFERENCED, "Rate.Drop.Item.Referenced", 1.0f);
    loadConfig(RATE_DROP_MONEY, "Rate.Drop.Money", 1.0f);
    loadConfig(RATE_XP_KILL, "Rate.XP.Kill", 1.0f);
    loadConfig(RATE_XP_QUEST, "Rate.XP.Quest", 1.0f);
    loadConfig(RATE_XP_EXPLORE, "Rate.XP.Explore", 1.0f);
    loadConfig(RATE_XP_PAST_70, "Rate.XP.PastLevel70", 1.0f);

    loadConfig(RATE_REST_INGAME, "Rate.Rest.InGame", 1.0f);
    loadConfig(RATE_REST_OFFLINE_IN_TAVERN_OR_CITY, "Rate.Rest.Offline.InTavernOrCity", 1.0f);
    loadConfig(RATE_REST_OFFLINE_IN_WILDERNESS, "Rate.Rest.Offline.InWilderness", 1.0f);
    loadConfig(RATE_DAMAGE_FALL, "Rate.Damage.Fall", 1.0f);
    loadConfig(RATE_AUCTION_TIME, "Rate.Auction.Time", 1.0f);
    loadConfig(RATE_AUCTION_DEPOSIT, "Rate.Auction.Deposit", 1.0f);
    loadConfig(RATE_AUCTION_CUT, "Rate.Auction.Cut", 1.0f);
    loadConfig(RATE_HONOR, "Rate.Honor",1.0f);
    loadConfig(RATE_MINING_AMOUNT, "Rate.Mining.Amount",1.0f);
    loadConfig(RATE_MINING_NEXT, "Rate.Mining.Next",1.0f);
    loadConfig(RATE_TALENT, "Rate.Talent",1.0f);
    loadConfig(RATE_REPUTATION_GAIN, "Rate.Reputation.Gain", 1.0f);
    loadConfig(RATE_REPUTATION_LOWLEVEL_KILL, "Rate.Reputation.LowLevel.Kill", 0.2f);
    loadConfig(RATE_REPUTATION_LOWLEVEL_QUEST, "Rate.Reputation.LowLevel.Quest", 1.0f);
    loadConfig(RATE_INSTANCE_RESET_TIME, "Rate.InstanceResetTime", 1.0f);
    loadConfig(RATE_DURABILITY_LOSS_DAMAGE, "DurabilityLossChance.Damage", 0.5f);
    loadConfig(RATE_DURABILITY_LOSS_ABSORB, "DurabilityLossChance.Absorb", 0.5f);
    loadConfig(RATE_DURABILITY_LOSS_BLOCK, "DurabilityLossChance.Block", 0.05f);
    loadConfig(RATE_DURABILITY_LOSS_PARRY, "DurabilityLossChance.Parry", 0.05f);

    loadConfig(CONFIG_SKILL_GAIN_CRAFTING, "SkillGain.Crafting", 1);
    loadConfig(CONFIG_SKILL_GAIN_DEFENSE, "SkillGain.Defense", 1);
    loadConfig(CONFIG_SKILL_GAIN_GATHERING, "SkillGain.Gathering", 1);
    loadConfig(CONFIG_SKILL_GAIN_WEAPON, "SkillGain.Weapon", 1);
    loadConfig(CONFIG_SKILL_CHANCE_ORANGE, "SkillChance.Orange",100);
    loadConfig(CONFIG_SKILL_CHANCE_YELLOW, "SkillChance.Yellow",75);
    loadConfig(CONFIG_SKILL_CHANCE_GREEN, "SkillChance.Green",25);
    loadConfig(CONFIG_SKILL_CHANCE_GREY, "SkillChance.Grey",0);
    loadConfig(CONFIG_SKILL_CHANCE_MINING_STEPS, "SkillChance.MiningSteps",75);
    loadConfig(CONFIG_SKILL_CHANCE_SKINNING_STEPS, "SkillChance.SkinningSteps",75);

    loadConfig(CONFIG_DEATH_SICKNESS_LEVEL, "Death.SicknessLevel", 11);
    loadConfig(CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP, "Death.CorpseReclaimDelay.PvP", true);
    loadConfig(CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE, "Death.CorpseReclaimDelay.PvE", true);
    loadConfig(CONFIG_DEATH_BONES_WORLD, "Death.Bones.World", true);
    loadConfig(CONFIG_DEATH_BONES_BG_OR_ARENA, "Death.Bones.BattlegroundOrArena", true);

    // Creature settings
    loadConfig(RATE_CREATURE_AGGRO, "Rate.Creature.Aggro", 1.0f);
    loadConfig(RATE_CREATURE_GUARD_AGGRO, "Rate.Creature.Guard.Aggro", 1.5f);
    loadConfig(RATE_CORPSE_DECAY_LOOTED, "Rate.Corpse.Decay.Looted",0.5f);
    loadConfig(RATE_CREATURE_NORMAL_DAMAGE, "Rate.Creature.Normal.Damage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_ELITE_DAMAGE, "Rate.Creature.Elite.Elite.Damage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RAREELITE_DAMAGE, "Rate.Creature.Elite.RAREELITE.Damage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE, "Rate.Creature.Elite.WORLDBOSS.Damage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RARE_DAMAGE, "Rate.Creature.Elite.RARE.Damage", 1.0f);
    loadConfig(RATE_CREATURE_NORMAL_SPELLDAMAGE, "Rate.Creature.Normal.SpellDamage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE, "Rate.Creature.Elite.Elite.SpellDamage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE, "Rate.Creature.Elite.RAREELITE.SpellDamage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE, "Rate.Creature.Elite.WORLDBOSS.SpellDamage", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RARE_SPELLDAMAGE, "Rate.Creature.Elite.RARE.SpellDamage", 1.0f);
    loadConfig(RATE_CREATURE_NORMAL_HP, "Rate.Creature.Normal.HP", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_ELITE_HP, "Rate.Creature.Elite.Elite.HP", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RAREELITE_HP, "Rate.Creature.Elite.RAREELITE.HP", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_WORLDBOSS_HP, "Rate.Creature.Elite.WORLDBOSS.HP", 1.0f);
    loadConfig(RATE_CREATURE_ELITE_RARE_HP, "Rate.Creature.Elite.RARE.HP", 1.0f);

    loadConfig(CONFIG_CORPSE_DECAY_NORMAL, "Corpse.Decay.NORMAL", 60);
    loadConfig(CONFIG_CORPSE_DECAY_RARE, "Corpse.Decay.RARE", 300);
    loadConfig(CONFIG_CORPSE_DECAY_ELITE, "Corpse.Decay.ELITE", 300);
    loadConfig(CONFIG_CORPSE_DECAY_RAREELITE, "Corpse.Decay.RAREELITE", 300);
    loadConfig(CONFIG_CORPSE_DECAY_WORLDBOSS, "Corpse.Decay.WORLDBOSS", 3600);
    loadConfig(CONFIG_LISTEN_RANGE_SAY, "ListenRange.Say", 25);
    loadConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE, "ListenRange.TextEmote", 25);
    loadConfig(CONFIG_LISTEN_RANGE_YELL, "ListenRange.Yell", 300);

    loadConfig(CONFIG_WAYPOINT_MOVEMENT_ACTIVE_ON_CONTINENTS, "AutoActive.WaypointMovement.Continents", true);
    loadConfig(CONFIG_WAYPOINT_MOVEMENT_ACTIVE_IN_INSTANCES, "AutoActive.WaypointMovement.Instances", true);
    loadConfig(CONFIG_COMBAT_ACTIVE_ON_CONTINENTS, "AutoActive.Combat.Continents", true);
    loadConfig(CONFIG_COMBAT_ACTIVE_IN_INSTANCES, "AutoActive.Combat.Instances", true);
    loadConfig(CONFIG_COMBAT_ACTIVE_FOR_PLAYERS_ONLY, "AutoActive.Combat.PlayersOnly", false);

    loadConfig(CONFIG_SIGHT_GUARD, "GuarderSight", 50);
    loadConfig(CONFIG_SIGHT_MONSTER, "MonsterSight", 50);
    loadConfig(CONFIG_EVADE_HOMEDIST, "Creature.Evade.DistanceToHome", 50);
    loadConfig(CONFIG_EVADE_TARGETDIST, "Creature.Evade.DistanceToTarget", 45);

    loadConfig(CONFIG_CREATURE_RESTORE_STATE, "Creature.RestoreStateTimer", 5000);
    loadConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS, "CreatureFamilyAssistanceRadius", 10);
    loadConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY, "CreatureFamilyAssistanceDelay", 1500);
    loadConfig(CONFIG_CREATURE_FAMILY_FLEE_RADIUS, "CreatureFamilyFleeAssistanceRadius", 30);
    loadConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY, "CreatureFamilyFleeDelay", 7000);
    loadConfig(CONFIG_WORLD_BOSS_LEVEL_DIFF, "WorldBossLevelDiff",3);

    // Arena settings
    loadConfig(CONFIG_ARENA_MAX_RATING_DIFFERENCE, "Arena.MaxRatingDifference", 0);
    loadConfig(CONFIG_ARENA_RATING_DISCARD_TIMER, "Arena.RatingDiscardTimer",300000);
    loadConfig(CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS, "Arena.AutoDistributePoints", false);
    loadConfig(CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS, "Arena.AutoDistributeInterval", 7);
    loadConfig(CONFIG_ENABLE_FAKE_WHO_ON_ARENA, "Arena.EnableFakeWho", false);
    loadConfig(CONFIG_ENABLE_FAKE_WHO_IN_GUILD, "Arena.EnableFakeWho.ForGuild", false);
    loadConfig(CONFIG_ARENA_LOG_EXTENDED_INFO, "Arena.LogExtendedInfo", false);

    loadConfig(CONFIG_ENABLE_HIDDEN_RATING, "Arena.EnableMMR", false);
    loadConfig(CONFIG_ENABLE_HIDDEN_RATING_PENALTY, "Arena.EnableMMRPenalty", false);
    loadConfig(CONFIG_HIDDEN_RATING_PENALTY, "Arena.MMRPenalty", 150);
    loadConfig(CONFIG_ENABLE_HIDDEN_RATING_LOWER_LOSS, "Arena.MMRSpecialLossCalc", false);

    loadConfig(CONFIG_ENABLE_ARENA_STEP_BY_STEP_MATCHING, "Arena.StepByStep.Enable",false);
    loadConfig(CONFIG_ARENA_STEP_BY_STEP_TIME, "Arena.StepByStep.Time",60000);
    loadConfig(CONFIG_ARENA_STEP_BY_STEP_VALUE, "Arena.StepByStep.Value",100);
    loadConfig(CONFIG_ARENA_END_AFTER_TIME, "Arena.EndAfter.Time",0);
    loadConfig(CONFIG_ARENA_END_AFTER_ALWAYS_DRAW, "Arena.EndAfter.AlwaysDraw",false);
    loadConfig(CONFIG_ARENA_STATUS_INFO, "Arena.StatusInfo");
    loadConfig(CONFIG_ARENA_ELO_COEFFICIENT, "Arena.ELOCoefficient",32);
    loadConfig(CONFIG_ARENA_DAILY_REQUIREMENT, "Arena.DailyRequirement",0);
    loadConfig(CONFIG_ARENA_DAILY_AP_REWARD, "Arena.DailyAPReward",0);
    loadConfig(CONFIG_ARENA_READY_START_TIMER, "Arena.ReadyStartTimer", 5000);

    // Battleground settings
    loadConfig(CONFIG_BATTLEGROUND_ANNOUNCE_START, "BattleGround.AnnounceStart", 0);
    loadConfig(CONFIG_BATTLEGROUND_CAST_DESERTER, "Battleground.CastDeserter", true);
    loadConfig(CONFIG_BATTLEGROUND_INVITATION_TYPE, "Battleground.InvitationType", 1);
    loadConfig(CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH, "BattleGround.PremadeGroupWaitForMatch", 10 * MINUTE * IN_MILISECONDS);
    loadConfig(CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER, "BattleGround.PrematureFinishTimer", 0);
    loadConfig(CONFIG_PREMATURE_BG_REWARD, "Battleground.PrematureReward", true);
    loadConfig(CONFIG_BATTLEGROUND_QUEUE_INFO, "BattleGround.QueueInfo", 0);
    loadConfig(CONFIG_BG_START_MUSIC, "Battleground.StartMusic", false);
    loadConfig(CONFIG_BATTLEGROUND_TIMER_INFO, "BattleGround.TimerInfo");
    loadConfig(CONFIG_BATTLEGROUND_WSG_END_AFTER_ENABLED, "BattleGround.WSGEndAfter.Enabled",false);
    loadConfig(CONFIG_BATTLEGROUND_WSG_END_AFTER_TIME, "BatleGround.WSGEndAfter.Time",1800000);
    loadConfig(CONFIG_BATTLEGROUND_WSG_END_AFTER_ALWAYS_DRAW, "BattleGround.WSGEndAfter.AlwaysDraw",true);

    // Vmaps/mmaps
    loadConfig(CONFIG_VMAP_LOS_ENABLED, "vmap.enableLOS", true);
    sLog.outString("WORLD: vmap los %sabled", getConfig(CONFIG_VMAP_LOS_ENABLED) ? "en" : "dis");

    std::string ignoreSpellIds = sConfig.GetStringDefault("vmap.ignoreSpellIds", "");
    VMAP::VMapFactory::preventSpellsFromBeingTestedForLoS(ignoreSpellIds.c_str());

    loadConfig(CONFIG_VMAP_INDOOR_CHECK, "vmap.enableIndoorCheck", true);
    loadConfig(CONFIG_PET_LOS, "vmap.petLOS", false);
    loadConfig(CONFIG_VMAP_TOTEM, "vmap.totem", false);

    loadConfig(CONFIG_MMAP_ENABLED, "mmap.enabled", true);
    sLog.outString("WORLD: mmap pathfinding %sabled", getConfig(CONFIG_MMAP_ENABLED) ? "en" : "dis");

    // visibility and radiuses
    loadConfig(CONFIG_GROUP_VISIBILITY, "Visibility.GroupMode", 0);
    m_activeObjectUpdateDistanceOnContinents = sConfig.GetIntDefault("Visibility.Distance.ActiveObjectUpdate.Continents", DEFAULT_VISIBILITY_DISTANCE);
    m_activeObjectUpdateDistanceInInstances = sConfig.GetIntDefault("Visibility.Distance.ActiveObjectUpdate.Instances", DEFAULT_VISIBILITY_DISTANCE);

    // movement
    loadConfig(CONFIG_TARGET_POS_RECALCULATION_RANGE, "Movement.RecalculateRange", 2);
    if (m_configs[CONFIG_TARGET_POS_RECALCULATION_RANGE] < 0)
        m_configs[CONFIG_TARGET_POS_RECALCULATION_RANGE] = 0;
    if (m_configs[CONFIG_TARGET_POS_RECALCULATION_RANGE] > 5)
        m_configs[CONFIG_TARGET_POS_RECALCULATION_RANGE] = 5;
    loadConfig(CONFIG_TARGET_POS_RECHECK_TIMER, "Movement.RecheckTimer", 100);
    loadConfig(CONFIG_WAYPOINT_MOVEMENT_PATHFINDING_ON_CONTINENTS, "Movement.WaypointPathfinding.Continents", true);
    loadConfig(CONFIG_WAYPOINT_MOVEMENT_PATHFINDING_IN_INSTANCES, "Movement.WaypointPathfinding.Instances", true);

    // CoreBalancer
    loadConfig(CONFIG_COREBALANCER_ENABLED, "CoreBalancer.Enable", false);
    loadConfig(CONFIG_COREBALANCER_PLAYABLE_DIFF, "CoreBalancer.PlayableDiff", 200);
    loadConfig(CONFIG_COREBALANCER_INTERVAL, "CoreBalancer.BalanceInterval", 300000);
    loadConfig(CONFIG_COREBALANCER_VISIBILITY_PENALTY, "CoreBalancer.VisibilityPenalty", 25);

    // VMSS system
    loadConfig(CONFIG_VMSS_ENABLE, "VMSS.Enable", false);
    loadConfig(CONFIG_VMSS_MAPFREEMETHOD, "VMSS.MapFreeMethod", 0);
    loadConfig(CONFIG_VMSS_FREEZECHECKPERIOD, "VMSS.FreezeCheckPeriod", 1000);
    loadConfig(CONFIG_VMSS_FREEZEDETECTTIME, "VMSS.MapFreezeDetectTime", 1000);

    // Warden/anticheat
    loadConfig(CONFIG_WARDEN_ENABLED, "Warden.Enabled", true);
    loadConfig(CONFIG_WARDEN_KICK, "Warden.Kick", true);
    loadConfig(CONFIG_WARDEN_BAN, "Warden.Ban", true);
    loadConfig(CONFIG_WARDEN_LOG_ONLY_CHECK, "Warden.LogOnlyCheck",0);
    loadConfig(CONFIG_WARDEN_CHECK_INTERVAL_MIN, "Warden.CheckIntervalMin",25000);
    loadConfig(CONFIG_WARDEN_CHECK_INTERVAL_MAX, "Warden.CheckIntervalMax",35000);
    loadConfig(CONFIG_WARDEN_MEM_CHECK_MAX, "Warden.MemCheckMax",3);
    loadConfig(CONFIG_WARDEN_RANDOM_CHECK_MAX, "Warden.RandomCheckMax",5);
    loadConfig(CONFIG_ENABLE_PASSIVE_ANTICHEAT, "AntiCheat.Enable", 1); 
    loadConfig(CONFIG_ANTICHEAT_CUMULATIVE_DELAY, "AntiCheat.CumulativeDelay",5* IN_MILISECONDS);
    loadConfig(CONFIG_ANTICHEAT_SPEEDHACK_TOLERANCE, "AntiCheat.SpeedhackTolerance",1.00f);

    // RaF
    loadConfig(CONFIG_UINT32_RAF_MAXGRANTLEVEL, "RAF.MaxGrantLevel", 60);
    loadConfig(CONFIG_UINT32_RAF_MAXREFERALS, "RAF.MaxReferals", 5);
    loadConfig(CONFIG_UINT32_RAF_MAXREFERERS, "RAF.MaxReferers", 5);
    loadConfig(CONFIG_FLOAT_RATE_RAF_XP, "Rate.RAF.XP", 3.0f);
    loadConfig(CONFIG_FLOAT_RATE_RAF_LEVELPERLEVEL, "Rate.RAF.LevelPerLevel", 0.5f);
    
    // Ganking penalty
    loadConfig(CONFIG_ENABLE_GANKING_PENALTY, "PVP.EnableGankingPenalty", false);
    loadConfig(CONFIG_GANKING_PENALTY_EXPIRE, "PVP.GankingPenaltyExpireTime", 600000);
    loadConfig(CONFIG_GANKING_KILLS_ALERT, "PVP.GankingPenaltyKillsAlert", 10);
    loadConfig(CONFIG_GANKING_PENALTY_PER_KILL , "PVP.GankingPenaltyPerKill", 0.1);

    // Network
    loadConfig(CONFIG_KICK_PLAYER_ON_BAD_PACKET, "Network.KickOnBadPacket", true);

        // === Reload only section === 
    if (reload)
    {
        if (sConfig.GetIntDefault("WorldServerPort", DEFAULT_WORLDSERVER_PORT) != m_configs[CONFIG_PORT_WORLD])
            sLog.outLog(LOG_DEFAULT, "ERROR: WorldServerPort option can't be changed at .conf file reload, using current value (%u).",m_configs[CONFIG_PORT_WORLD]);

        // Performance settings
        if (sConfig.GetIntDefault("SocketSelectTime", DEFAULT_SOCKET_SELECT_TIME) != m_configs[CONFIG_SOCKET_SELECTTIME])
            sLog.outLog(LOG_DEFAULT, "ERROR: SocketSelectTime option can't be changed at .conf file reload, using current value (%u).",m_configs[CONFIG_SOCKET_SELECTTIME]);
        sMapMgr.SetGridCleanUpDelay(m_configs[CONFIG_INTERVAL_GRIDCLEAN]);
        m_timers[WUPDATE_UPTIME].SetInterval(m_configs[CONFIG_UPTIME_UPDATE]*MINUTE*1000);
        m_timers[WUPDATE_UPTIME].Reset();

        // Server settings
        if (sConfig.GetIntDefault("GameType", 0) != m_configs[CONFIG_GAME_TYPE])
            sLog.outLog(LOG_DEFAULT, "ERROR: GameType option can't be changed at .conf file reload, using current value (%u).",m_configs[CONFIG_GAME_TYPE]);
        if (sConfig.GetIntDefault("RealmZone", REALM_ZONE_DEVELOPMENT) != m_configs[CONFIG_REALM_ZONE])
            sLog.outLog(LOG_DEFAULT, "ERROR: RealmZone option can't be changed at .conf file reload, using current value (%u).",m_configs[CONFIG_REALM_ZONE]);
        if ( sConfig.GetIntDefault("Expansion",1) != m_configs[CONFIG_EXPANSION])
            sLog.outLog(LOG_DEFAULT, "ERROR: Expansion option can't be changed at .conf file reload, using current value (%u).",m_configs[CONFIG_EXPANSION]);
        
        // Server customization basic
        if (sConfig.GetIntDefault("MaxPlayerLevel", 70) != m_configs[CONFIG_MAX_PLAYER_LEVEL])
            sLog.outLog(LOG_DEFAULT, "ERROR: MaxPlayerLevel option can't be changed at config reload, using current value (%u).",m_configs[CONFIG_MAX_PLAYER_LEVEL]);

    }
    // === Not-reload only section ===
    else
    {
        loadConfig(CONFIG_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT);
        // Performance settings
        loadConfig(CONFIG_SOCKET_SELECTTIME, "SocketSelectTime", DEFAULT_SOCKET_SELECT_TIME);

        // Server settings
        loadConfig(CONFIG_GAME_TYPE, "GameType", 0);
        loadConfig(CONFIG_REALM_ZONE, "RealmZone", REALM_ZONE_DEVELOPMENT);
        loadConfig(CONFIG_EXPANSION, "Expansion",1);

        // Server customization basic
        loadConfig(CONFIG_MAX_PLAYER_LEVEL, "MaxPlayerLevel", 70);
        if (m_configs[CONFIG_MAX_PLAYER_LEVEL] > MAX_LEVEL)
        {
            sLog.outLog(LOG_DEFAULT, "ERROR: MaxPlayerLevel (%i) must be in range 1..%u. Set to %u.",m_configs[CONFIG_MAX_PLAYER_LEVEL],MAX_LEVEL,MAX_LEVEL);
            m_configs[CONFIG_MAX_PLAYER_LEVEL] = MAX_LEVEL;
        }
    }

    // === Warns section ===
    // Performance settings
    loadConfig(CONFIG_COMPRESSION, "Compression", 1);
    if (m_configs[CONFIG_COMPRESSION] < 1 || m_configs[CONFIG_COMPRESSION] > 9)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: Compression level (%i) must be in range 1..9. Using default compression level (1).",m_configs[CONFIG_COMPRESSION]);
        m_configs[CONFIG_COMPRESSION] = 1;
    }
        
    loadConfig(CONFIG_MAX_OVERSPEED_PINGS, "MaxOverspeedPings",2);
    if (m_configs[CONFIG_MAX_OVERSPEED_PINGS] != 0 && m_configs[CONFIG_MAX_OVERSPEED_PINGS] < 2)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check. Set to 2.",m_configs[CONFIG_MAX_OVERSPEED_PINGS]);
        m_configs[CONFIG_MAX_OVERSPEED_PINGS] = 2;
    }

    loadConfig(CONFIG_INTERVAL_GRIDCLEAN, "GridCleanUpDelay", 300000);
    if (m_configs[CONFIG_INTERVAL_GRIDCLEAN] < MIN_GRID_DELAY)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: GridCleanUpDelay (%i) must be greater %u. Use this minimal value.",m_configs[CONFIG_INTERVAL_GRIDCLEAN],MIN_GRID_DELAY);
        m_configs[CONFIG_INTERVAL_GRIDCLEAN] = MIN_GRID_DELAY;
    }

    loadConfig(CONFIG_INTERVAL_MAPUPDATE, "MapUpdateInterval", 100);
    if (m_configs[CONFIG_INTERVAL_MAPUPDATE] < MIN_MAP_UPDATE_DELAY)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: MapUpdateInterval (%i) must be greater %u. Use this minimal value.",m_configs[CONFIG_INTERVAL_MAPUPDATE],MIN_MAP_UPDATE_DELAY);
        m_configs[CONFIG_INTERVAL_MAPUPDATE] = MIN_MAP_UPDATE_DELAY;
    }

    loadConfig(CONFIG_UPTIME_UPDATE, "UpdateUptimeInterval", 10);
    if (m_configs[CONFIG_UPTIME_UPDATE]<=0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: UpdateUptimeInterval (%i) must be > 0, set to default 10.",m_configs[CONFIG_UPTIME_UPDATE]);
        m_configs[CONFIG_UPTIME_UPDATE] = 10;
    }
    // Server settings    
    loadConfig(CONFIG_CHARACTERS_PER_REALM, "CharactersPerRealm", 10);
    if (m_configs[CONFIG_CHARACTERS_PER_REALM] < 1 || m_configs[CONFIG_CHARACTERS_PER_REALM] > 10)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: CharactersPerRealm (%i) must be in range 1..10. Set to 10.",m_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_configs[CONFIG_CHARACTERS_PER_REALM] = 10;
    }

    loadConfig(CONFIG_CHARACTERS_PER_ACCOUNT, "CharactersPerAccount", 50);
    if (m_configs[CONFIG_CHARACTERS_PER_ACCOUNT] < m_configs[CONFIG_CHARACTERS_PER_REALM])
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: CharactersPerAccount (%i) can't be less than CharactersPerRealm (%i).",m_configs[CONFIG_CHARACTERS_PER_ACCOUNT],m_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = m_configs[CONFIG_CHARACTERS_PER_REALM];
    }
    // Server customization basic
    loadConfig(CONFIG_START_PLAYER_LEVEL, "StartPlayerLevel", 1);
    if (m_configs[CONFIG_START_PLAYER_LEVEL] < 1)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 1.",m_configs[CONFIG_START_PLAYER_LEVEL],m_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_configs[CONFIG_START_PLAYER_LEVEL] = 1;
    }
    else if (m_configs[CONFIG_START_PLAYER_LEVEL] > m_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.",m_configs[CONFIG_START_PLAYER_LEVEL],m_configs[CONFIG_MAX_PLAYER_LEVEL],m_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_configs[CONFIG_START_PLAYER_LEVEL] = m_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    loadConfig(CONFIG_START_PLAYER_MONEY,"StartPlayerMoney", 0);
    if (m_configs[CONFIG_START_PLAYER_MONEY] < 0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartPlayerMoney (%i) must be in range 0..%u. Set to %u.",m_configs[CONFIG_START_PLAYER_MONEY],MAX_MONEY_AMOUNT,0);
        m_configs[CONFIG_START_PLAYER_MONEY] = 0;
    }
    else if (m_configs[CONFIG_START_PLAYER_MONEY] > MAX_MONEY_AMOUNT)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartPlayerMoney (%i) must be in range 0..%u. Set to %u.",
            m_configs[CONFIG_START_PLAYER_MONEY],MAX_MONEY_AMOUNT,MAX_MONEY_AMOUNT);
        m_configs[CONFIG_START_PLAYER_MONEY] = MAX_MONEY_AMOUNT;
    }

    loadConfig(CONFIG_MAX_HONOR_POINTS, "MaxHonorPoints", 75000);
    if (m_configs[CONFIG_MAX_HONOR_POINTS] < 0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: MaxHonorPoints (%i) can't be negative. Set to 0.",m_configs[CONFIG_MAX_HONOR_POINTS]);
        m_configs[CONFIG_MAX_HONOR_POINTS] = 0;
    }

    loadConfig(CONFIG_START_HONOR_POINTS, "StartHonorPoints", 0);
    if (m_configs[CONFIG_START_HONOR_POINTS] < 0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_configs[CONFIG_START_HONOR_POINTS],m_configs[CONFIG_MAX_HONOR_POINTS],0);
        m_configs[CONFIG_START_HONOR_POINTS] = 0;
    }
    else if (m_configs[CONFIG_START_HONOR_POINTS] > m_configs[CONFIG_MAX_HONOR_POINTS])
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_configs[CONFIG_START_HONOR_POINTS],m_configs[CONFIG_MAX_HONOR_POINTS],m_configs[CONFIG_MAX_HONOR_POINTS]);
        m_configs[CONFIG_START_HONOR_POINTS] = m_configs[CONFIG_MAX_HONOR_POINTS];
    }

    loadConfig(CONFIG_MAX_ARENA_POINTS, "MaxArenaPoints", 5000);
    if (m_configs[CONFIG_MAX_ARENA_POINTS] < 0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: MaxArenaPoints (%i) can't be negative. Set to 0.",m_configs[CONFIG_MAX_ARENA_POINTS]);
        m_configs[CONFIG_MAX_ARENA_POINTS] = 0;
    }

    loadConfig(CONFIG_START_ARENA_POINTS, "StartArenaPoints", 0);
    if (m_configs[CONFIG_START_ARENA_POINTS] < 0)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_configs[CONFIG_START_ARENA_POINTS],m_configs[CONFIG_MAX_ARENA_POINTS],0);
        m_configs[CONFIG_START_ARENA_POINTS] = 0;
    }
    else if (m_configs[CONFIG_START_ARENA_POINTS] > m_configs[CONFIG_MAX_ARENA_POINTS])
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_configs[CONFIG_START_ARENA_POINTS],m_configs[CONFIG_MAX_ARENA_POINTS],m_configs[CONFIG_MAX_ARENA_POINTS]);
        m_configs[CONFIG_START_ARENA_POINTS] = m_configs[CONFIG_MAX_ARENA_POINTS];
    }

    // Server customization advanced
    loadConfig(CONFIG_MIN_PETITION_SIGNS, "MinPetitionSigns", 9);
    if (m_configs[CONFIG_MIN_PETITION_SIGNS] > 9)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: MinPetitionSigns (%i) must be in range 0..9. Set to 9.",m_configs[CONFIG_MIN_PETITION_SIGNS]);
        m_configs[CONFIG_MIN_PETITION_SIGNS] = 9;
    }

    loadConfig(CONFIG_SKIP_CINEMATICS, "SkipCinematics", 0);
    if (m_configs[CONFIG_SKIP_CINEMATICS] < 0 || m_configs[CONFIG_SKIP_CINEMATICS] > 2)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: SkipCinematics (%i) must be in range 0..2. Set to 0.",m_configs[CONFIG_SKIP_CINEMATICS]);
        m_configs[CONFIG_SKIP_CINEMATICS] = 0;
    }

    // Game master settings
    loadConfig(CONFIG_START_GM_LEVEL, "GM.StartLevel", 1);
    if (m_configs[CONFIG_START_GM_LEVEL] < m_configs[CONFIG_START_PLAYER_LEVEL])
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: GM.StartLevel (%i) must be in range StartPlayerLevel(%u)..%u. Set to %u.",
            m_configs[CONFIG_START_GM_LEVEL],m_configs[CONFIG_START_PLAYER_LEVEL], MAX_LEVEL, m_configs[CONFIG_START_PLAYER_LEVEL]);
        m_configs[CONFIG_START_GM_LEVEL] = m_configs[CONFIG_START_PLAYER_LEVEL];
    }
    else if (m_configs[CONFIG_START_GM_LEVEL] > MAX_LEVEL)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: GM.StartLevel (%i) must be in range 1..%u. Set to %u.", m_configs[CONFIG_START_GM_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_configs[CONFIG_START_GM_LEVEL] = MAX_LEVEL;
    }

    // visibility and radiuses
    m_VisibleObjectGreyDistance = sConfig.GetIntDefault("Visibility.Distance.Grey.Object", 10);
    if (m_VisibleObjectGreyDistance >  MAX_VISIBILITY_DISTANCE)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: Visibility.Distance.Grey.Object can't be greater %f",MAX_VISIBILITY_DISTANCE);
        m_VisibleObjectGreyDistance = MAX_VISIBILITY_DISTANCE;
    }

}

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Initialize the random number generator
    srand((unsigned int)time(NULL));

    dtAllocSetCustom(dtCustomAlloc, dtCustomFree);

    ///- Initialize config settings
    LoadConfigSettings();

    ///- Init highest guids before any table loading to prevent using not initialized guids in some code.
    sObjectMgr.SetHighestGuids();

    ///- Check the existence of the map files for all races' startup areas.
    if ( !MapManager::ExistMapAndVMap(0,-6240.32f, 331.033f)
        ||!MapManager::ExistMapAndVMap(0,-8949.95f,-132.493f)
        ||!MapManager::ExistMapAndVMap(0,-8949.95f,-132.493f)
        ||!MapManager::ExistMapAndVMap(1,-618.518f,-4251.67f)
        ||!MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f)
        ||!MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f)
        ||!MapManager::ExistMapAndVMap(1,-2917.58f,-257.98f)
        ||m_configs[CONFIG_EXPANSION] && (
        !MapManager::ExistMapAndVMap(530,10349.6f,-6357.29f) || !MapManager::ExistMapAndVMap(530,-3961.64f,-13931.2f)))
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: Correct *.map files not found in path '%smaps' or *.vmap/*vmdir files in '%svmaps'. Please place *.map/*.vmap/*.vmdir files in appropriate directories or correct the DataDir value in the .conf file.",m_dataPath.c_str(),m_dataPath.c_str());
        exit(1);
    }

    ///- Loading strings. Getting no records means core load has to be canceled because no error message can be output.
    sLog.outString("");
    sLog.outString("Loading Hellground strings...");
    if (!sObjectMgr.LoadHellgroundStrings())
        exit(1);                                            // Error message displayed in function already

    ///- Update the realm entry in the database with the realm type from the config file
    //No SQL injection as values are treated as integers

    // not send custom type REALM_FFA_PVP to realm list
    uint32 server_type = IsFFAPvPRealm() ? REALM_TYPE_PVP : getConfig(CONFIG_GAME_TYPE);
    uint32 realm_zone = getConfig(CONFIG_REALM_ZONE);
    AccountsDatabase.PExecute("UPDATE realms SET icon = %u, timezone = %u WHERE realm_id = '%u'", server_type, realm_zone, realmID);

    ///- Remove the bones after a restart
    RealmDataDatabase.PExecute("DELETE FROM corpse WHERE corpse_type = '0'");

    ///- Load the DBC files
    sLog.outString("Initialize data stores...");
    LoadDBCStores(m_dataPath);
    DetectDBCLang();

    sLog.outString("Loading Terrain specific data...");
    sTerrainMgr.LoadTerrainSpecifics();

    sLog.outString("Loading Script Names...");
    sScriptMgr.LoadScriptNames();

    sLog.outString("Loading InstanceTemplate");
    sObjectMgr.LoadInstanceTemplate();

    sLog.outString("Loading SkillLineAbilityMultiMap Data...");
    sSpellMgr.LoadSkillLineAbilityMap();

    ///- Clean up and pack instances
    sLog.outString("Cleaning up instances...");
    sInstanceSaveManager.CleanupInstances();                              // must be called before `creature_respawn`/`gameobject_respawn` tables

    //sLog.outString("Packing instances...");
    //sInstanceSaveManager.PackInstances();

    sLog.outString("Loading Localization strings...");
    sObjectMgr.LoadCreatureLocales();
    sObjectMgr.LoadGameObjectLocales();
    sObjectMgr.LoadItemLocales();
    sObjectMgr.LoadQuestLocales();
    sObjectMgr.LoadNpcTextLocales();
    sObjectMgr.LoadPageTextLocales();
    sObjectMgr.LoadNpcOptionLocales();
    sObjectMgr.SetDBCLocaleIndex(GetDefaultDbcLocale());        // Get once for all the locale index of DBC language (console/broadcasts)

    sLog.outString("Loading Page Texts...");
    sObjectMgr.LoadPageTexts();

    sLog.outString("Loading Game Object Templates...");   // must be after LoadPageTexts
    sObjectMgr.LoadGameobjectInfo();

    sLog.outString("Loading Spell Chain Data...");
    sSpellMgr.LoadSpellChains();

    sLog.outString("Loading Spell Required Data...");
    sSpellMgr.LoadSpellRequired();

    sLog.outString("Loading Spell Elixir types...");
    sSpellMgr.LoadSpellElixirs();

    sLog.outString("Loading Spell Learn Skills...");
    sSpellMgr.LoadSpellLearnSkills();                        // must be after LoadSpellChains

    sLog.outString("Loading Spell Learn Spells...");
    sSpellMgr.LoadSpellLearnSpells();

    sLog.outString("Loading Spell Proc Event conditions...");
    sSpellMgr.LoadSpellProcEvents();

    sLog.outString("Loading Aggro Spells Definitions...");
    sSpellMgr.LoadSpellThreats();

    sLog.outString("Loading Unqueued Account List...");
    sObjectMgr.LoadUnqueuedAccountList();

    sLog.outString("Loading NPC Texts...");
    sObjectMgr.LoadGossipText();

    sLog.outString("Loading Enchant Spells Proc datas...");
    sSpellMgr.LoadSpellEnchantProcData();

    sLog.outString("Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();

    sLog.outString("Loading Items...");                   // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr.LoadItemPrototypes();

    sLog.outString("Loading Item Texts...");
    sObjectMgr.LoadItemTexts();

    sLog.outString("Loading Creature Model Based Info Data...");
    sObjectMgr.LoadCreatureModelInfo();

    sLog.outString("Loading Equipment templates...");
    sObjectMgr.LoadEquipmentTemplates();

    sLog.outString("Loading Creature templates...");
    sObjectMgr.LoadCreatureTemplates();

    sLog.outString("Loading SpellsScriptTarget...");
    sSpellMgr.LoadSpellScriptTarget();                       // must be after LoadCreatureTemplates and LoadGameobjectInfo

    sLog.outString( "Loading Reputation Reward Rates...");
    sObjectMgr.LoadReputationRewardRate();

    sLog.outString("Loading Creature Reputation OnKill Data...");
    sObjectMgr.LoadReputationOnKill();

    sLog.outString( "Loading Reputation Spillover Data..." );
    sObjectMgr.LoadReputationSpilloverTemplate();

    sLog.outString("Loading Pet Create Spells...");
    sObjectMgr.LoadPetCreateSpells();

    sLog.outString("Loading Creature Data...");
    sObjectMgr.LoadCreatures();

    sLog.outString("Loading Creature Linked Respawn...");
    sObjectMgr.LoadCreatureLinkedRespawn();                     // must be after LoadCreatures()

    sLog.outString("Loading Creature Addon Data...");
    sObjectMgr.LoadCreatureAddons();                            // must be after LoadCreatureTemplates() and LoadCreatures()

    sLog.outString("Loading Creature Respawn Data...");   // must be after PackInstances()
    sObjectMgr.LoadCreatureRespawnTimes();

    sLog.outString("Loading Gameobject Data...");
    sObjectMgr.LoadGameobjects();

    sLog.outString("Loading Gameobject Respawn Data..."); // must be after PackInstances()
    sObjectMgr.LoadGameobjectRespawnTimes();

    sLog.outString("Loading Objects Pooling Data...");
    sPoolMgr.LoadFromDB();

    sLog.outString("Loading Game Event Data...");
    sGameEventMgr.LoadFromDB();

    sLog.outString("Loading Weather Data...");
    sObjectMgr.LoadWeatherZoneChances();

    sLog.outString("Loading Quests...");
    sObjectMgr.LoadQuests();                                    // must be loaded after DBCs, creature_template, item_template, gameobject tables

    sLog.outString("Loading Quests Relations...");
    sObjectMgr.LoadQuestRelations();                            // must be after quest load

    sLog.outString("Loading AreaTrigger definitions...");
    sObjectMgr.LoadAreaTriggerTeleports();

    sLog.outString("Loading Access Requirements...");
    sObjectMgr.LoadAccessRequirements();                        // must be after item template load

    sLog.outString("Loading Quest Area Triggers...");
    sObjectMgr.LoadQuestAreaTriggers();                         // must be after LoadQuests

    sLog.outString("Loading Tavern Area Triggers...");
    sObjectMgr.LoadTavernAreaTriggers();

    sLog.outString("Loading AreaTrigger script names...");
    sScriptMgr.LoadAreaTriggerScripts();

    sLog.outString("Loading CompletedCinematic script names...");
    sScriptMgr.LoadCompletedCinematicScripts();

    sLog.outString("Loading event id script names...");
    sScriptMgr.LoadEventIdScripts();

    sLog.outString("Loading spell id script names...");
    sScriptMgr.LoadSpellIdScripts();

    sLog.outString("Loading Graveyard-zone links...");
    sObjectMgr.LoadGraveyardZones();

    sLog.outString("Loading Spell target coordinates...");
    sSpellMgr.LoadSpellTargetPositions();

    sLog.outString("Loading SpellAffect definitions...");
    sSpellMgr.LoadSpellAffects();

    sLog.outString("Loading spell pet auras...");
    sSpellMgr.LoadSpellPetAuras();

    sLog.outString("Loading spell extra attributes...(TODO)");
    sSpellMgr.LoadSpellCustomAttr();

    sLog.outString("Loading linked spells...");
    sSpellMgr.LoadSpellLinked();

    sLog.outString("Loading player Create Info & Level Stats...");
    sObjectMgr.LoadPlayerInfo();

    sLog.outString("Loading Exploration BaseXP Data...");
    sObjectMgr.LoadExplorationBaseXP();

    sLog.outString("Loading Pet Name Parts...");
    sObjectMgr.LoadPetNames();

    sLog.outString("Loading the max pet number...");
    sObjectMgr.LoadPetNumber();

    sLog.outString("Loading pet level stats...");
    sObjectMgr.LoadPetLevelInfo();

    sLog.outString("Loading Player Corpses...");
    sObjectMgr.LoadCorpses();

    sLog.outString("Loading Disabled Spells...");
    sObjectMgr.LoadSpellDisabledEntrys();

    sLog.outString("Loading Loot Tables...");
    LoadLootTables();

    sLog.outString("Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();

    sLog.outString("Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();

    sLog.outString("Loading Skill Fishing base level requirements...");
    sObjectMgr.LoadFishingBaseSkillLevel();

    ///- Load dynamic data tables from the database
    sLog.outString("Loading Auctions...");
    sAuctionMgr.LoadAuctionItems();
    sAuctionMgr.LoadAuctions();

    sLog.outString("Loading Guilds...");
    sGuildMgr.LoadGuilds();

    sLog.outString("Loading ArenaTeams...");
    sObjectMgr.LoadArenaTeams();

    sLog.outString("Loading Groups...");
    sObjectMgr.LoadGroups();

    sLog.outString("Loading ReservedNames...");
    sObjectMgr.LoadReservedPlayersNames();

    //sLog.outString("Loading GameObject for quests...");
    //sObjectMgr.LoadGameObjectForQuests();

    sLog.outString("Loading BattleMasters...");
    sBattleGroundMgr.LoadBattleMastersEntry();

    sLog.outString("Loading GameTeleports...");
    sObjectMgr.LoadGameTele();

    sLog.outString("Loading Npc Text Id...");
    sObjectMgr.LoadNpcTextId();                                 // must be after load Creature and NpcText

    sLog.outString("Loading Npc Options...");
    sObjectMgr.LoadNpcOptions();

    sLog.outString("Loading vendors...");
    sObjectMgr.LoadVendors();                                   // must be after load CreatureTemplate and ItemPrototype

    sLog.outString("Loading trainers...");
    sObjectMgr.LoadTrainerSpell();                              // must be after load CreatureTemplate

    sLog.outString("Loading opcodes cooldown...");
    sObjectMgr.LoadOpcodesCooldown();

    sLog.outString("Loading Waypoints...");
    sWaypointMgr.Load();

    sLog.outString("Loading Creature Formations...");
    CreatureGroupManager::LoadCreatureFormations();

    sLog.outString("Loading GM tickets...");
    sTicketMgr.LoadGMTickets();

    ///- Handle outdated emails (delete/return)
    sLog.outString("Returning old mails...");
    sObjectMgr.ReturnOrDeleteOldMails(false);

    sLog.outString("Loading Autobroadcasts...");
    LoadAutobroadcasts();

    ///- Load and initialize scripts
    sLog.outString("Loading Scripts...");
    sScriptMgr.LoadQuestStartScripts();                         // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sScriptMgr.LoadQuestEndScripts();                           // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sScriptMgr.LoadSpellScripts();                              // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadGameObjectScripts();                         // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadEventScripts();                              // must be after load Creature/Gameobject(Template/Data)
    sScriptMgr.LoadWaypointScripts();

    sLog.outString("Loading Scripts text locales...");    // must be after Load*Scripts calls
    sScriptMgr.LoadDbScriptStrings();

    sLog.outString("Loading CreatureEventAI Texts...");
    sCreatureEAIMgr.LoadCreatureEventAI_Texts(false);       // false, will checked in LoadCreatureEventAI_Scripts

    sLog.outString("Loading CreatureEventAI Summons...");
    sCreatureEAIMgr.LoadCreatureEventAI_Summons(false);     // false, will checked in LoadCreatureEventAI_Scripts

    sLog.outString("Loading CreatureEventAI Scripts...");
    sCreatureEAIMgr.LoadCreatureEventAI_Scripts();

    sLog.outString("Initializing Scripts...");
    sScriptMgr.LoadScriptLibrary(HELLGROUND_SCRIPT_NAME);

    ///- Initialize Lua Engine
    sLog.outString("Initialize Eluna Lua Engine...");
    StartEluna();

    ///- Initialize game time and timers
    sLog.outDebug("DEBUG:: Initialize game time and timers");
    m_gameTime = time(NULL);
    m_startTime=m_gameTime;

    tm local;
    time_t curr;
    time(&curr);
    local=*(localtime(&curr));                              // dereference and assign
    char isoDate[128];
    sprintf(isoDate, "%04d-%02d-%02d %02d:%02d:%02d",
        local.tm_year+1900, local.tm_mon+1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);

    RealmDataDatabase.PExecute("INSERT INTO uptime (startstring, starttime, uptime) VALUES('%s', " UI64FMTD ", 0)",
        isoDate, uint64(m_startTime));

    m_timers[WUPDATE_OBJECTS].SetInterval(0);
    m_timers[WUPDATE_SESSIONS].SetInterval(0);
    m_timers[WUPDATE_WEATHERS].SetInterval(1000);
    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE*1000);    //set auction update interval to 1 minute
    m_timers[WUPDATE_UPTIME].SetInterval(m_configs[CONFIG_UPTIME_UPDATE]*MINUTE*1000);
                                                            //Update "uptime" table based on configuration entry in minutes.
    m_timers[WUPDATE_CORPSES].SetInterval(20*MINUTE*1000);  //erase corpses every 20 minutes

    m_timers[WUPDATE_AUTOBROADCAST].SetInterval(getConfig(CONFIG_AUTOBROADCAST_INTERVAL));
    m_timers[WUPDATE_GUILD_ANNOUNCES].SetInterval(getConfig(CONFIG_GUILD_ANN_INTERVAL));
    m_timers[WUPDATE_DELETECHARS].SetInterval(DAY*IN_MILISECONDS); // check for chars to delete every day
    m_timers[WUPDATE_OLDMAILS].SetInterval(getConfig(CONFIG_RETURNOLDMAILS_INTERVAL)*1000);
    m_timers[WUPDATE_ACTIVE_BANS].SetInterval(getConfig(CONFIG_ACTIVE_BANS_UPDATE_TIME));

    //to set mailtimer to return mails every day between 4 and 5 am
    //mailtimer is increased when updating auctions
    //one second is 1000 -(tested on win system)
    mail_timer = ((((localtime(&m_gameTime)->tm_hour + 20) % 24)* HOUR * 1000) / m_timers[WUPDATE_OLDMAILS].GetInterval());
                                                            //1440
    mail_timer_expires = ((DAY * 1000) / (m_timers[WUPDATE_OLDMAILS].GetInterval()));
    sLog.outDebug("Mail timer set to: %u, mail return is called every %u minutes", mail_timer, mail_timer_expires);

    ///- Initilize static helper structures
    AIRegistry::Initialize();
    Player::InitVisibleBits();

    ///- Initialize MapManager
    sLog.outString("Starting Map System");
    sMapMgr.Initialize();

    ///- Initialize Battlegrounds
    sLog.outString("Starting BattleGround System");
    sBattleGroundMgr.CreateInitialBattleGrounds();
    sBattleGroundMgr.InitAutomaticArenaPointDistribution();

    //Not sure if this can be moved up in the sequence (with static data loading) as it uses MapManager
    sLog.outString("Loading Transports...");
    sMapMgr.LoadTransports();

    sLog.outString("Loading Transports Events...");
    sObjectMgr.LoadTransportEvents();

    ///- Initialize outdoor pvp
    sLog.outString("Starting Outdoor PvP System");
    sOutdoorPvPMgr.InitOutdoorPvP();

    sLog.outString("Deleting expired IP bans...");
    AccountsDatabase.Execute("DELETE FROM ip_banned WHERE unban_date <= UNIX_TIMESTAMP() AND unban_date <> ban_date");

    sLog.outString("Starting objects Pooling system...");
    sPoolMgr.Initialize();

    sLog.outString("Calculate next daily quest reset time...");
    InitDailyQuestResetTime();

    sLog.outString("Starting Game Event system...");
    uint32 nextGameEvent = sGameEventMgr.Initialize();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);    //depend on next event

    sLog.outString("Loading Warden Data..." );
    sWardenDataStorage.Init();

    sLog.outString("Cleanup deleted characters");
    CleanupDeletedChars();

    sLog.outString("Activating AntiCheat");
    if (getConfig(CONFIG_ENABLE_PASSIVE_ANTICHEAT) && m_ac.activate() == -1)
        sLog.outString("Couldn't activate AntiCheat");

    if (getConfig(CONFIG_COREBALANCER_ENABLED))
        _coreBalancer.Initialize();

    sLog.outString("WORLD: World initialized");
}

void World::DetectDBCLang()
{
    uint32 m_lang_confid = sConfig.GetIntDefault("Locale", 255);

    if (m_lang_confid != 255 && m_lang_confid >= MAX_LOCALE)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)",MAX_LOCALE);
        m_lang_confid = LOCALE_enUS;
    }

    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(1);

    std::string availableLocalsStr;

    int default_locale = MAX_LOCALE;
    for (int i = MAX_LOCALE-1; i >= 0; --i)
    {
        if (strlen(race->name[i]) > 0)                     // check by race names
        {
            default_locale = i;
            m_availableDbcLocaleMask |= (1 << i);
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if (default_locale != m_lang_confid && m_lang_confid < MAX_LOCALE &&
        (m_availableDbcLocaleMask & (1 << m_lang_confid)))
    {
        default_locale = m_lang_confid;
    }

    if (default_locale >= MAX_LOCALE)
    {
        sLog.outLog(LOG_DEFAULT, "ERROR: Unable to determine your DBC Locale! (corrupt DBC?)");
        exit(1);
    }

    m_defaultDbcLocale = LocaleConstant(default_locale);

    sLog.outString("Using %s DBC Locale as default. All available DBC locales: %s",localeNames[m_defaultDbcLocale],availableLocalsStr.empty() ? "<none>" : availableLocalsStr.c_str());
}

void World::LoadAutobroadcasts()
{
    m_Autobroadcasts.clear();

    QueryResultAutoPtr result = GameDataDatabase.Query("SELECT text FROM autobroadcast");

    if (!result)
    {
        sLog.outString();
        sLog.outString(">> Loaded 0 autobroadcasts definitions");
        return;
    }

    uint32 count = 0;
    do
    {
        Field *fields = result->Fetch();
        std::string message = fields[0].GetCppString();
        m_Autobroadcasts.push_back(message);
        count++;
    } while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u autobroadcast definitions", count);
}

/// Update the World !
void World::Update(uint32 diff)
{
    m_updateTime = uint32(diff);

    if (getConfig(CONFIG_COREBALANCER_ENABLED))
        _coreBalancer.Update(diff);

    bool accumulateMapDiff = getConfig(CONFIG_CUMULATIVE_LOG_METHOD) == 1 ? true : false;
    if (getConfig(CONFIG_INTERVAL_LOG_UPDATE))
    {
        if (m_updateTimeSum > getConfig(CONFIG_INTERVAL_LOG_UPDATE))
        {
            accumulateMapDiff = true;

            m_curAvgUpdateTime = m_updateTimeSum/m_updateTimeCount;   // from last log time
            m_serverUpdateTimeSum += m_updateTimeSum;
            m_serverUpdateTimeCount += m_updateTimeCount;

            m_avgUpdateTime = m_serverUpdateTimeSum/m_serverUpdateTimeCount; // from server start

            sLog.outLog(LOG_DEFAULT, "[Diff]: Update time diff: %u, avg: %u. Players online: %u.", m_curAvgUpdateTime, m_avgUpdateTime, GetActiveSessionCount());
            sLog.outLog(LOG_DIFF, "Update time diff: %u, avg: %u. Players online: %u.", m_curAvgUpdateTime, m_avgUpdateTime, GetActiveSessionCount());
            sLog.outLog(LOG_STATUS, "%u %u %u %u %u %u %s %u %u %u %u %u",
                        GetUptime(), GetActiveSessionCount(), GetMaxActiveSessionCount(), GetQueuedSessionCount(), GetMaxQueuedSessionCount(),
                        m_playerLimit, _REVISION, m_curAvgUpdateTime, m_avgUpdateTime, loggedInAlliances.value(), loggedInHordes.value(), sWorld.GetGameTime());

            m_updateTimeSum = m_updateTime;
            m_updateTimeCount = 1;
        }
        else
        {
            m_updateTimeSum += m_updateTime;
            ++m_updateTimeCount;
        }
    }

    DiffRecorder diffRecorder(__FUNCTION__, getConfig(CONFIG_MIN_LOG_UPDATE));

    ///- Update the different timers
    for (int i = 0; i < WUPDATE_COUNT; i++)
    {
        if (m_timers[i].GetCurrent()>=0)
            m_timers[i].Update(diff);
        else
            m_timers[i].SetCurrent(0);
    }

    diffRecorder.RecordTimeFor("UpdateTimers");

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    diffRecorder.RecordTimeFor("UpdateGameTime");

    /// Handle daily quests reset time
    if (m_gameTime > m_NextDailyQuestReset)
    {
        ResetDailyQuests();
        m_NextDailyQuestReset += DAY;

        diffRecorder.RecordTimeFor("ResetDailyQuests");
    }
    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_OLDMAILS].Passed())
    {
        m_timers[WUPDATE_OLDMAILS].Reset();

        ///- Update mails (return old mails with item, or delete them)
        switch (m_configs[CONFIG_RETURNOLDMAILS_MODE])
        {
            case 1:
                sObjectMgr.ReturnOrDeleteOldMails(true);
                break;
            case 0:
            default:
                if (++mail_timer > mail_timer_expires)
                {
                    mail_timer = 0;
                    sObjectMgr.ReturnOrDeleteOldMails(true);
                }
                break;
        }

        diffRecorder.RecordTimeFor("ReturnOldMails");
    }

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_AUCTIONS].Passed())
    {
        m_timers[WUPDATE_AUCTIONS].Reset();

        ///-Handle expired auctions
        sAuctionMgr.Update();
        diffRecorder.RecordTimeFor("UpdateAuctions");
    }

    /// <li> Handle session updates when the timer has passed
    if (m_timers[WUPDATE_SESSIONS].Passed())
    {
        m_timers[WUPDATE_SESSIONS].Reset();

        UpdateSessions(diff);

        diffRecorder.RecordTimeFor("UpdateSessions");

        // Update groups
        for (ObjectMgr::GroupSet::iterator itr = sObjectMgr.GetGroupSetBegin(); itr != sObjectMgr.GetGroupSetEnd(); ++itr)
            (*itr)->Update(diff);

        diffRecorder.RecordTimeFor("UpdateGroups");
    }

    sWorldEventProcessor.ExecuteEvents();
    diffRecorder.RecordTimeFor("ExecuteWorldEvents");

    /// <li> Handle weather updates when the timer has passed
    if (m_timers[WUPDATE_WEATHERS].Passed())
    {
        m_timers[WUPDATE_WEATHERS].Reset();

        ///- Send an update signal to Weather objects
        WeatherMap::iterator itr, next;
        for (itr = m_weathers.begin(); itr != m_weathers.end(); itr = next)
        {
            next = itr;
            ++next;

            ///- and remove Weather objects for zones with no player as interval > WorldTick
            if (!itr->second->Update(m_timers[WUPDATE_WEATHERS].GetInterval()))
            {
                Weather *temp = itr->second;
                m_weathers.erase(itr);
                delete temp;
            }
        }

        diffRecorder.RecordTimeFor("UpdateWeathers");
    }

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff = (m_gameTime - m_startTime);
        uint32 maxClientsNum = sWorld.GetMaxActiveSessionCount();

        m_timers[WUPDATE_UPTIME].Reset();
        RealmDataDatabase.PExecute("UPDATE uptime SET uptime = %d, maxplayers = %d WHERE starttime = " UI64FMTD, tmpDiff, maxClientsNum, uint64(m_startTime));
    }

    diffRecorder.ResetDiff();

    if (sWorld.getConfig(CONFIG_AUTOBROADCAST_INTERVAL))
    {
        if (m_timers[WUPDATE_AUTOBROADCAST].Passed())
        {
            m_timers[WUPDATE_AUTOBROADCAST].Reset();
            if (m_Autobroadcasts.empty())
                return;

            std::string msg;

            std::list<std::string>::const_iterator itr = m_Autobroadcasts.begin();
            std::advance(itr, rand() % m_Autobroadcasts.size());
            msg = *itr;

            sWorld.SendWorldText(LANG_AUTO_ANN, ACC_DISABLED_BROADCAST, msg.c_str());
        }

        diffRecorder.RecordTimeFor("Send Autobroadcast");
    }

    ///- send guild announces every one minute
    if (m_timers[WUPDATE_GUILD_ANNOUNCES].Passed())
    {
        m_timers[WUPDATE_GUILD_ANNOUNCES].Reset();
        if (!m_GuildAnnounces[0].empty())
        {
            std::list<std::pair<uint64, std::string> >::iterator itr = m_GuildAnnounces[0].begin();
            std::string guildName = sGuildMgr.GetGuildNameById(PAIR64_LOPART(itr->first));

            sWorld.SendGuildAnnounce(PAIR64_HIPART(itr->first), guildName.c_str(), itr->second.c_str());
            m_GuildAnnounces[0].pop_front();
        }

        if (!m_GuildAnnounces[1].empty())
        {
            std::list<std::pair<uint64, std::string> >::iterator itr = m_GuildAnnounces[1].begin();
            std::string guildName = sGuildMgr.GetGuildNameById(PAIR64_LOPART(itr->first));

            sWorld.SendGuildAnnounce(PAIR64_HIPART(itr->first), guildName.c_str(), itr->second.c_str());
            m_GuildAnnounces[1].pop_front();
        }

        diffRecorder.RecordTimeFor("Send Guild announce");
    }

    /// <li> Handle all other objects
    ///- Update objects when the timer has passed (maps, transport, creatures,...)
    MAP_UPDATE_DIFF(MapUpdateDiff().InitializeMapData())

    sMapMgr.Update(diff);                // As interval = 0

    diffRecorder.RecordTimeFor("MapManager::update");

    if (accumulateMapDiff)
    {
        MAP_UPDATE_DIFF(MapUpdateDiff().PrintCumulativeMapUpdateDiff())
    }

    sBattleGroundMgr.Update(diff);
    diffRecorder.RecordTimeFor("UpdateBattleGroundMgr");

    sOutdoorPvPMgr.Update(diff);
    diffRecorder.RecordTimeFor("UpdateOutdoorPvPMgr");

    ///- Delete all characters which have been deleted X days before
    if (m_timers[WUPDATE_DELETECHARS].Passed())
    {
        m_timers[WUPDATE_DELETECHARS].Reset();
        CleanupDeletedChars();
        diffRecorder.RecordTimeFor("CleanupDeletedChars");
    }

    // execute callbacks from sql queries that were queued recently
    UpdateResultQueue();
    diffRecorder.RecordTimeFor("UpdateResultQueue");

    ///- Erase corpses once every 20 minutes
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();

        sObjectAccessor.RemoveOldCorpses();
        diffRecorder.RecordTimeFor("RemoveOldCorpses");
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
        uint32 nextGameEvent = sGameEventMgr.Update();
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
        diffRecorder.RecordTimeFor("UpdateGameEvents");
    }
    
    if (m_timers[WUPDATE_ACTIVE_BANS].Passed())
    {
        m_timers[WUPDATE_ACTIVE_BANS].Reset();

        static SqlStatementID updateBansStmt;
        SqlStatement stmt = AccountsDatabase.CreateStatement(updateBansStmt, "UPDATE account_punishment "
            "SET active = 0 WHERE expiration_date <= UNIX_TIMESTAMP() AND expiration_date <> punishment_date "
            "AND punishment_type_id IN (?,?,?)");
        stmt.addUInt8(PUNISHMENT_BAN);
        stmt.addUInt8(PUNISHMENT_MUTE);
        stmt.addUInt8(PUNISHMENT_TROLLMUTE);
        stmt.Execute();
    }
    /// </ul>

    // update the instance reset times
    sInstanceSaveManager.Update();
    diffRecorder.RecordTimeFor("UpdateSaveMGR");

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();
    diffRecorder.RecordTimeFor("UpdateProcessCLI");

    //cleanup unused GridMap objects as well as VMaps
    sTerrainMgr.Update(diff);
    diffRecorder.RecordTimeFor("UpdateTerrainMGR");

    ///- used by eluna
    sHookMgr->OnWorldUpdate(diff);
}

void World::UpdateSessions(const uint32 & diff)
{
    ///- Add new sessions
    WorldSession* sess;
    while (addSessQueue.next(sess))
        AddSession_ (sess);

    if (sessionThreads)
        tbb::parallel_for(tbb::blocked_range<int>(0, m_sessions.size(), m_sessions.size()/sessionThreads), SessionsUpdater(&m_sessions, diff));
    else
    {
        ///- Then send an update signal to remaining ones
        for (SessionMap::iterator itr = m_sessions.begin(), next; itr != m_sessions.end(); itr = next)
        {
            next = itr;
            ++next;

            if (!itr->second)
                continue;

            ///- and remove not active sessions from the list
            WorldSession * pSession = itr->second;
            WorldSessionFilter updater(pSession);
            if (!pSession->Update(diff, updater))   // As interval = 0
            {
                RemoveQueuedPlayer(pSession);
                AddSessionToRemove(itr);
            }
        }
    }

    for (std::list<SessionMap::iterator>::iterator itr = removedSessions.begin(); itr != removedSessions.end(); ++itr)
    {
        sess = (*itr)->second;
        m_sessions.erase(*itr);
        delete sess;
        sess = NULL;
    }

    removedSessions.clear();
}

void World::ForceGameEventUpdate()
{
    m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
    uint32 nextGameEvent = sGameEventMgr.Update();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
    m_timers[WUPDATE_EVENTS].Reset();
}

/// Send a packet to all players (except self if mentioned)
void World::SendGlobalMessage(WorldPacket *packet, WorldSession *self, uint32 team)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second != self &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

void World::QueueGuildAnnounce(uint32 guildid, uint32 team, std::string &msg)
{
    std::pair<uint64, std::string> temp;
    //                           low, high
    temp.first = MAKE_PAIR64(guildid, team);
    temp.second = msg;
    m_GuildAnnounces[team == ALLIANCE ? 0 : 1].push_back(temp);
}

void World::SendGuildAnnounce(uint32 team, ...)
{
    std::vector<std::vector<WorldPacket*> > data_cache;     // 0 = default, i => i-1 locale index

    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->GetPlayer()->GetTeam() != team || itr->second->IsAccountFlagged(ACC_DISABLED_GANN))
            continue;

        uint32 loc_idx = itr->second->GetSessionDbLocaleIndex();
        uint32 cache_idx = loc_idx+1;

        std::vector<WorldPacket*>* data_list;

        // create if not cached yet
        if (data_cache.size() < cache_idx+1 || data_cache[cache_idx].empty())
        {
            if (data_cache.size() < cache_idx+1)
                data_cache.resize(cache_idx+1);

            data_list = &data_cache[cache_idx];

            char const* text = sObjectMgr.GetHellgroundString(LANG_GUILD_ANNOUNCE,loc_idx);

            char buf[1000];

            va_list argptr;
            va_start(argptr, team);
            vsnprintf(buf,1000, text, argptr);
            va_end(argptr);

            char* pos = &buf[0];

            while (char* line = ChatHandler::LineFromMessage(pos))
            {
                WorldPacket* data = new WorldPacket();
                ChatHandler::FillMessageData(data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
                data_list->push_back(data);
            }
        }
        else
            data_list = &data_cache[cache_idx];

        for (int i = 0; i < data_list->size(); ++i)
            itr->second->SendPacket((*data_list)[i]);
    }

    // free memory
    for (int i = 0; i < data_cache.size(); ++i)
        for (int j = 0; j < data_cache[i].size(); ++j)
            delete data_cache[i][j];
}

void World::SendGlobalGMMessage(WorldPacket *packet, WorldSession *self, uint32 team)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second != self &&
            itr->second->HasPermissions(sWorld.getConfig(CONFIG_MIN_GM_TEXT_LVL)) &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

/// Send a System Message to all players (except self if mentioned)
void World::SendWorldText(int32 string_id, uint32 preventFlags, ...)
{
    std::vector<std::vector<WorldPacket*> > data_cache;     // 0 = default, i => i-1 locale index

    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->IsAccountFlagged(AccountFlags(preventFlags)))
            continue;

        uint32 loc_idx = itr->second->GetSessionDbLocaleIndex();
        uint32 cache_idx = loc_idx+1;

        std::vector<WorldPacket*>* data_list;

        // create if not cached yet
        if (data_cache.size() < cache_idx+1 || data_cache[cache_idx].empty())
        {
            if (data_cache.size() < cache_idx+1)
                data_cache.resize(cache_idx+1);

            data_list = &data_cache[cache_idx];

            char const* text = sObjectMgr.GetHellgroundString(string_id,loc_idx);

            char buf[1000];

            va_list argptr;
            va_start(argptr, preventFlags);
            vsnprintf(buf,1000, text, argptr);
            va_end(argptr);

            char* pos = &buf[0];

            while (char* line = ChatHandler::LineFromMessage(pos))
            {
                WorldPacket* data = new WorldPacket();
                ChatHandler::FillMessageData(data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
                data_list->push_back(data);
            }
        }
        else
            data_list = &data_cache[cache_idx];

        for (int i = 0; i < data_list->size(); ++i)
            itr->second->SendPacket((*data_list)[i]);
    }

    // free memory
    for (int i = 0; i < data_cache.size(); ++i)
        for (int j = 0; j < data_cache[i].size(); ++j)
            delete data_cache[i][j];
}

// send global message for players in range <minLevel, maxLevel> which don't have account flags
// setted in 'preventFlags'
void World::SendWorldTextForLevels(uint32 minLevel, uint32 maxLevel, uint32 preventFlags, int32 string_id, ...)
{
    std::vector<std::vector<WorldPacket*> > data_cache;     // 0 = default, i => i-1 locale index

    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        if (itr->second->GetPlayer()->getLevel() < minLevel || itr->second->GetPlayer()->getLevel() > maxLevel)
            continue;

        if (itr->second->IsAccountFlagged(AccountFlags(preventFlags)))
            continue;

        uint32 loc_idx = itr->second->GetSessionDbLocaleIndex();
        uint32 cache_idx = loc_idx+1;

        std::vector<WorldPacket*>* data_list;

        // create if not cached yet
        if (data_cache.size() < cache_idx+1 || data_cache[cache_idx].empty())
        {
            if (data_cache.size() < cache_idx+1)
                data_cache.resize(cache_idx+1);

            data_list = &data_cache[cache_idx];

            char const* text = sObjectMgr.GetHellgroundString(string_id,loc_idx);

            char buf[1000];

            va_list argptr;
            va_start(argptr, string_id);
            vsnprintf(buf,1000, text, argptr);
            va_end(argptr);

            char* pos = &buf[0];

            while (char* line = ChatHandler::LineFromMessage(pos))
            {
                WorldPacket* data = new WorldPacket();
                ChatHandler::FillMessageData(data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
                data_list->push_back(data);
            }
        }
        else
            data_list = &data_cache[cache_idx];

        for (int i = 0; i < data_list->size(); ++i)
            itr->second->SendPacket((*data_list)[i]);
    }

    // free memory
    for (int i = 0; i < data_cache.size(); ++i)
        for (int j = 0; j < data_cache[i].size(); ++j)
            delete data_cache[i][j];
}

void World::SendGMText(int32 string_id, ...)
{
    std::vector<std::vector<WorldPacket*> > data_cache;     // 0 = default, i => i-1 locale index

    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        uint32 loc_idx = itr->second->GetSessionDbLocaleIndex();
        uint32 cache_idx = loc_idx+1;

        std::vector<WorldPacket*>* data_list;

        // create if not cached yet
        if (data_cache.size() < cache_idx+1 || data_cache[cache_idx].empty())
        {
            if (data_cache.size() < cache_idx+1)
                data_cache.resize(cache_idx+1);

            data_list = &data_cache[cache_idx];

            char const* text = sObjectMgr.GetHellgroundString(string_id,loc_idx);

            char buf[1000];

            va_list argptr;
            va_start(argptr, string_id);
            vsnprintf(buf,1000, text, argptr);
            va_end(argptr);

            char* pos = &buf[0];

            while (char* line = ChatHandler::LineFromMessage(pos))
            {
                WorldPacket* data = new WorldPacket();
                ChatHandler::FillMessageData(data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
                data_list->push_back(data);
            }
        }
        else
            data_list = &data_cache[cache_idx];

        for (int i = 0; i < data_list->size(); ++i)
            if (itr->second->HasPermissions(sWorld.getConfig(CONFIG_MIN_GM_TEXT_LVL)))
                itr->second->SendPacket((*data_list)[i]);
    }

    // free memory
    for (int i = 0; i < data_cache.size(); ++i)
        for (int j = 0; j < data_cache[i].size(); ++j)
            delete data_cache[i][j];
}

/// Send a System Message to all players (except self if mentioned)
void World::SendGlobalText(const char* text, WorldSession *self)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = mangos_strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::FillMessageData(&data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
        SendGlobalMessage(&data, self);
    }

    delete [] buf;
}

/// Send a packet to all players (or players selected team) in the zone (except self if mentioned)
void World::SendZoneMessage(uint32 zone, WorldPacket *packet, WorldSession *self, uint32 team)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second->GetPlayer()->GetCachedZone() == zone &&
            itr->second != self &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

/// Send a System Message to all players in the zone (except self if mentioned)
void World::SendZoneText(uint32 zone, const char* text, WorldSession *self, uint32 team)
{
    WorldPacket data;
    ChatHandler::FillMessageData(&data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, text, NULL);
    SendZoneMessage(zone, &data, self,team);
}

/// Kick (and save) all players
void World::KickAll()
{
    m_QueuedPlayer.clear();                                 // prevent send queue update packet and login queued sessions

    // session not removed at kick and will removed in next update tick
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        itr->second->KickPlayer();
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllWithoutPermissions(uint64 perms)
{
    // session not removed at kick and will removed in next update tick
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->HasPermissions(perms))
            itr->second->KickPlayer();
}

/// Kick (and save) the designated player
bool World::KickPlayer(const std::string& playerName)
{
    SessionMap::iterator itr;

    // session not removed at kick and will removed in next update tick
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;
        Player *player = itr->second->GetPlayer();
        if (!player)
            continue;
        if (player->IsInWorld())
        {
            if (playerName == player->GetName())
            {
                itr->second->KickPlayer();
                return true;
            }
        }
    }
    return false;
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string nameIPOrMail, std::string duration, std::string reason, std::string author)
{
    AccountsDatabase.escape_string(nameIPOrMail);
    AccountsDatabase.escape_string(reason);
    std::string safe_author=author;
    AccountsDatabase.escape_string(safe_author);

    uint32 duration_secs = 0;
    if (mode != BAN_EMAIL)
        duration_secs = TimeStringToSecs(duration);

    QueryResultAutoPtr resultAccounts = QueryResultAutoPtr(NULL);   //used for kicking

    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            //No SQL injection as strings are escaped
            resultAccounts = AccountsDatabase.PQuery("SELECT account_id FROM account WHERE last_ip = '%s'", nameIPOrMail.c_str());
            AccountsDatabase.PExecute("INSERT INTO ip_banned VALUES ('%s', UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+%u, '%s', '%s')", nameIPOrMail.c_str(), duration_secs, safe_author.c_str(), reason.c_str());
            break;
        case BAN_ACCOUNT:
            //No SQL injection as string is escaped
            resultAccounts = AccountsDatabase.PQuery("SELECT account_id FROM account WHERE username = '%s'", nameIPOrMail.c_str());
            break;
        case BAN_CHARACTER:
            //No SQL injection as string is escaped
            resultAccounts = RealmDataDatabase.PQuery("SELECT account FROM characters WHERE name = '%s'", nameIPOrMail.c_str());
            break;
        case BAN_EMAIL:
            resultAccounts = AccountsDatabase.PQuery("SELECT account_id FROM account WHERE email = '%s'", nameIPOrMail.c_str());
            AccountsDatabase.PExecute("INSERT INTO email_banned VALUES ('%s', UNIX_TIMESTAMP(), '%s', '%s')", nameIPOrMail.c_str(), safe_author.c_str(), reason.c_str());
            break;
        default:
            return BAN_SYNTAX_ERROR;
    }

    if (!resultAccounts)
    {
        switch (mode)
        {
            case BAN_EMAIL:
            case BAN_IP:
                return BAN_SUCCESS;
            default:
                return BAN_NOTFOUND;                            // Nobody to ban
        }
    }

    ///- Disconnect all affected players (for IP it can be several)
    do
    {
        Field* fieldsAccount = resultAccounts->Fetch();
        uint32 account = fieldsAccount[0].GetUInt32();
        uint32 permissions = PERM_PLAYER;

        QueryResultAutoPtr resultAccPerm = AccountsDatabase.PQuery("SELECT permission_mask FROM account_permissions WHERE account_id = '%u' AND realm_id = '%u')", account, realmID);
        if (resultAccPerm)
        {
            Field* fieldsAccId = resultAccPerm->Fetch();
            if (fieldsAccId)
                permissions = fieldsAccId[0].GetUInt32();
        }

        if (permissions & PERM_GMT)
            continue;

        if (mode != BAN_IP && mode != BAN_EMAIL)
        {
            //No SQL injection as strings are escaped
            AccountsDatabase.PExecute("INSERT INTO account_punishment VALUES ('%u', '%u', UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+%u, '%s', '%s', '1')",
                                    account, PUNISHMENT_BAN, duration_secs, safe_author.c_str(), reason.c_str());
        }

        if (WorldSession* sess = FindSession(account))
            if (std::string(sess->GetPlayerName()) != author)
                sess->KickPlayer();
    }
    while (resultAccounts->NextRow());

    return BAN_SUCCESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, std::string nameIPOrMail)
{
    switch (mode)
    {
        case BAN_IP:
            AccountsDatabase.escape_string(nameIPOrMail);
            AccountsDatabase.PExecute("DELETE FROM ip_banned WHERE ip = '%s'",nameIPOrMail.c_str());
            break;
        case BAN_EMAIL:
            AccountsDatabase.escape_string(nameIPOrMail);
            AccountsDatabase.PExecute("DELETE FROM email_banned WHERE email = '%s'",nameIPOrMail.c_str());
            break;
        case BAN_ACCOUNT:
        case BAN_CHARACTER:
            uint32 account = 0;
            if (mode == BAN_ACCOUNT)
                account = AccountMgr::GetId (nameIPOrMail);
            else if (mode == BAN_CHARACTER)
                account = sObjectMgr.GetPlayerAccountIdByPlayerName (nameIPOrMail);

            if (!account)
                return false;

            //NO SQL injection as account is uint32
            AccountsDatabase.PExecute("UPDATE account_punishment SET active = '0' WHERE account_id = '%u' AND punishment_type_id = '%u'", account, PUNISHMENT_BAN);

            break;
    }
    return true;
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(NULL);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;

    ///- if there is a shutdown timer
    if (!m_stopEvent && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount()==0)
                m_stopEvent = true;                         // exist code already set
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode, char const* exitmsg)
{
    // ignore if server shutdown at next tick
    if (m_stopEvent)
        return;

    m_ShutdownMask = options;
    m_ExitCode = exitcode;

    m_ShutdownReason = exitmsg;

    m_ShutdownTimer = time;
    ShutdownMsg(true);
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show, Player* player)
{
    // not show messages for idle shutdown mode
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    ///- Display a message every 12 hours, hours, 5 minutes, minute, 5 seconds and finally seconds
    if (show ||
        (m_ShutdownTimer < 10) ||
                                                            // < 30 sec; every 5 sec
        (m_ShutdownTimer<30        && (m_ShutdownTimer % 5        )==0) ||
                                                            // < 5 min ; every 1 min
        (m_ShutdownTimer<5*MINUTE  && (m_ShutdownTimer % MINUTE   )==0) ||
                                                            // < 30 min ; every 5 min
        (m_ShutdownTimer<30*MINUTE && (m_ShutdownTimer % (5*MINUTE))==0) ||
                                                            // < 12 h ; every 1 h
        (m_ShutdownTimer<12*HOUR   && (m_ShutdownTimer % HOUR     )==0) ||
                                                            // > 12 h ; every 12 h
        (m_ShutdownTimer>12*HOUR   && (m_ShutdownTimer % (12*HOUR))==0))
    {
        std::string str = secsToTimeString(m_ShutdownTimer);
        str += " Reason: " + m_ShutdownReason + ".";

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid,str.c_str(),player);
        DEBUG_LOG("Server is %s in %s",(m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"),str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    // nothing cancel or too later
    if (!m_ShutdownTimer || m_stopEvent)
        return;

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_ShutdownReason = "no-reason";

    m_ExitCode = SHUTDOWN_EXIT_CODE;                       // to default value
    SendServerMessage(msgid);

    DEBUG_LOG("Server %s cancelled.",(m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"));
}

/// Send a server message to the user(s)
void World::SendServerMessage(ServerMessageType type, const char *text, Player* player)
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50);              // guess size
    data << uint32(type);
    if (type <= SERVER_MSG_STRING)
        data << text;

    if (player)
        player->SendPacketToSelf(&data);
    else
        SendGlobalMessage(&data);
}

// This handles the issued and queued CLI commands
void World::ProcessCliCommands()
{
    CliCommandHolder::Print* zprint = NULL;

    CliCommandHolder* command;
    while (cliCmdQueue.next(command))
    {
        sLog.outDebug("CLI command under processing...");

        zprint = command->m_print;

        CliHandler(zprint).ParseCommands(command->m_command);

        delete command;
    }

    // print the console message here so it looks right
    if (zprint)
        zprint("TC> ");
}

void World::InitResultQueue()
{

}

void World::UpdateResultQueue()
{
    //process async result queues
    RealmDataDatabase.ProcessResultQueue();
    GameDataDatabase.ProcessResultQueue();
    AccountsDatabase.ProcessResultQueue();
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    QueryResultAutoPtr resultCharCount = RealmDataDatabase.PQuery("SELECT COUNT(guid) FROM characters WHERE account = '%u'", accountId);

    if (!resultCharCount)
        return;

    Field *fields = resultCharCount->Fetch();

    static SqlStatementID updateRealmChars;
    SqlStatement stmt = AccountsDatabase.CreateStatement(updateRealmChars, "UPDATE realm_characters SET characters_count = ? WHERE account_id = ? AND realm_id = ?");
    stmt.addUInt8(fields[0].GetUInt8());
    stmt.addUInt32(accountId);
    stmt.addUInt32(realmID);
    stmt.DirectExecute();
}

void World::InitDailyQuestResetTime()
{
    time_t mostRecentQuestTime;

    QueryResultAutoPtr result = RealmDataDatabase.Query("SELECT MAX(time) FROM character_queststatus_daily");
    if (result)
    {
        Field *fields = result->Fetch();

        mostRecentQuestTime = (time_t)fields[0].GetUInt64();
    }
    else
        mostRecentQuestTime = 0;

    // client built-in time for reset is 6:00 AM
    // FIX ME: client not show day start time
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = 6;
    localTm.tm_min  = 0;
    localTm.tm_sec  = 0;

    // current day reset time
    time_t curDayResetTime = mktime(&localTm);

    // last reset time before current moment
    time_t resetTime = (curTime < curDayResetTime) ? curDayResetTime - DAY : curDayResetTime;

    // need reset (if we have quest time before last reset time (not processed by some reason)
    if (mostRecentQuestTime && mostRecentQuestTime <= resetTime)
        m_NextDailyQuestReset = mostRecentQuestTime;
    else
    {
        // plan next reset time
        m_NextDailyQuestReset = (curTime >= curDayResetTime) ? curDayResetTime + DAY : curDayResetTime;
    }
}

void World::UpdateRequiredPermissions()
{
     QueryResultAutoPtr result = AccountsDatabase.PQuery("SELECT required_permission_mask from realms WHERE realm_id = '%u'", realmID);
     if (result)
     {
        m_requiredPermissionMask = result->Fetch()->GetUInt64();
        sLog.outDebug("Required permission mask: %u", m_requiredPermissionMask);
     }
}

void World::SelectRandomHeroicDungeonDaily()
{
    if (sGameEventMgr.GetEventMap().empty())
        return;

    const uint32 HeroicEventStart = 100;
    const uint32 HeroicEventEnd   = 115;

    uint8 currentId = 0;
    for (uint8 eventId = HeroicEventStart; eventId <= HeroicEventEnd; ++eventId)
    {
        if (sGameEventMgr.IsActiveEvent(eventId))
        {
            currentId = eventId;
            sGameEventMgr.StopEvent(eventId, true);
        }
        if (sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 5184000 WHERE entry = %u", eventId);
        else{
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", eventId);
            sGameEventMgr.StartEvent(eventId, true);
        }
    }

    if (!sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
        return;

    uint8 random = urand(HeroicEventStart, HeroicEventEnd);
    while (random == currentId)
        random = urand(HeroicEventStart, HeroicEventEnd);

    sGameEventMgr.GetEventMap()[currentId].occurence = 5184000;
    sGameEventMgr.GetEventMap()[random].occurence = 1400;

    sGameEventMgr.StartEvent(random, true);
    GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", random);
}

void World::SelectRandomDungeonDaily()
{
    if (sGameEventMgr.GetEventMap().empty())
        return;

    const uint32 DungeonEventStart = 116;
    const uint32 DungeonEventEnd   = 123;

    uint8 currentId = 0;
    for (uint8 eventId = DungeonEventStart; eventId <= DungeonEventEnd; ++eventId)
    {
        if (sGameEventMgr.IsActiveEvent(eventId))
        {
            currentId = eventId;
            sGameEventMgr.StopEvent(eventId, true);
        }
        if (sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 5184000 WHERE entry = %u", eventId);
        else{
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", eventId);
            sGameEventMgr.StartEvent(eventId, true);
        }
    }

    if (!sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
        return;

    uint8 random = urand(DungeonEventStart, DungeonEventEnd);
    while (random == currentId)
        random = urand(DungeonEventStart, DungeonEventEnd);

    sGameEventMgr.GetEventMap()[currentId].occurence = 5184000;
    sGameEventMgr.GetEventMap()[random].occurence = 1400;

    sGameEventMgr.StartEvent(random, true);
    GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", random);
}

void World::SelectRandomCookingDaily()
{
    if (sGameEventMgr.GetEventMap().empty())
        return;

    const uint32 CookingEventStart = 124;
    const uint32 CookingEventEnd   = 127;

    uint8 currentId = 0;
    for (uint8 eventId = CookingEventStart; eventId <= CookingEventEnd; ++eventId)
    {
        if (sGameEventMgr.IsActiveEvent(eventId))
        {
            currentId = eventId;
            sGameEventMgr.StopEvent(eventId, true);
        }
        if (sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 5184000 WHERE entry = %u", eventId);
        else{
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", eventId);
            sGameEventMgr.StartEvent(eventId, true);
        }
    }

    if (!sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
        return;

    uint8 random = urand(CookingEventStart, CookingEventEnd);
    while (random == currentId)
        random = urand(CookingEventStart, CookingEventEnd);

    sGameEventMgr.GetEventMap()[currentId].occurence = 5184000;
    sGameEventMgr.GetEventMap()[random].occurence = 1400;

    sGameEventMgr.StartEvent(random, true);
    GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", random);
}

void World::SelectRandomFishingDaily()
{
    if (sGameEventMgr.GetEventMap().empty())
        return;

    const uint32 FishingEventStart = 128;
    const uint32 FishingEventEnd   = 132;

    uint8 currentId = 0;
    for (uint8 eventId = FishingEventStart; eventId <= FishingEventEnd; ++eventId)
    {
        if (sGameEventMgr.IsActiveEvent(eventId))
        {
            currentId = eventId;
            sGameEventMgr.StopEvent(eventId, true);
        }
        if (sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 5184000 WHERE entry = %u", eventId);
        else{
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", eventId);
            sGameEventMgr.StartEvent(eventId, true);
        }
    }

    if (!sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
        return;

    uint8 random = urand(FishingEventStart, FishingEventEnd);
    while (random == currentId)
        random = urand(FishingEventStart, FishingEventEnd);

    sGameEventMgr.GetEventMap()[currentId].occurence = 5184000;
    sGameEventMgr.GetEventMap()[random].occurence = 1400;

    sGameEventMgr.StartEvent(random, true);
    GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", random);
}

void World::SelectRandomPvPDaily()
{
    if (sGameEventMgr.GetEventMap().empty())
        return;

    const uint32 PvPEventStart = 133;
    const uint32 PvPEventEnd   = 136;

    uint8 currentId = 0;
    for (uint8 eventId = PvPEventStart; eventId <= PvPEventEnd; ++eventId)
    {
        if (sGameEventMgr.IsActiveEvent(eventId))
        {
            currentId = eventId;
            sGameEventMgr.StopEvent(eventId);
        }
        if (sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 5184000 WHERE entry = %u", eventId);
        else{
            GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", eventId);
            sGameEventMgr.StartEvent(eventId, true);
        }
    }

    if (!sWorld.getConfig(CONFIG_DAILY_BLIZZLIKE))
        return;

    uint8 random = urand(PvPEventStart, PvPEventEnd);;
    while (random == currentId)
        random = urand(PvPEventStart, PvPEventEnd);

    sGameEventMgr.GetEventMap()[currentId].occurence = 5184000;
    sGameEventMgr.GetEventMap()[random].occurence = 1400;

    sGameEventMgr.StartEvent(random);
    GameDataDatabase.PExecute("UPDATE game_event SET occurence = 1400 WHERE entry = %u", random);
}

void World::ResetDailyQuests()
{
    sLog.outDetail("Daily quests reset for all characters.");
    RealmDataDatabase.Execute("DELETE FROM character_queststatus_daily");
    RealmDataDatabase.Execute("UPDATE character_stats_ro SET dailyarenawins = 0");
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetDailyQuestStatus();

    SelectRandomHeroicDungeonDaily();
    SelectRandomDungeonDaily();
    SelectRandomCookingDaily();
    SelectRandomFishingDaily();
    SelectRandomPvPDaily();

    //sGameEventMgr.LoadFromDB();
}

void World::SetPlayerLimit(int32 limit)
{
    if(limit >= 0)
    {
        m_playerLimit = limit;
        m_requiredPermissionMask = PERM_PLAYER;
        return;
    }
    
    if(limit == -1)
        m_requiredPermissionMask = PERM_GMT_DEV;
    if(limit == -2)
        m_requiredPermissionMask = PERM_HIGH_GMT | PERM_HEAD_DEVELOPER;
    if(limit == -3)
        m_requiredPermissionMask = PERM_ADM;
}

void World::UpdateMaxSessionCounters()
{
    m_maxActiveSessionCount = std::max(m_maxActiveSessionCount,uint32(m_sessions.size()-m_QueuedPlayer.size()));
    m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount,uint32(m_QueuedPlayer.size()));
}

void World::LoadDBVersion()
{
    QueryResultAutoPtr result = GameDataDatabase.Query("SELECT db_version FROM version LIMIT 1");
    if (result)
    {
        Field* fields = result->Fetch();

        m_DBVersion = fields[0].GetString();
    }
    else
        m_DBVersion = "unknown world database";
}

void World::CleanupDeletedChars()
{
    int keepDays = getConfig(CONFIG_KEEP_DELETED_CHARS_TIME);

    if (keepDays < 1)
        return;

    QueryResultAutoPtr result = RealmDataDatabase.PQuery("SELECT char_guid FROM deleted_chars WHERE DATEDIFF(now(), date) >= %u", keepDays);
    if (result)
    {
        do
        {
            Field *fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            Player::DeleteCharacterInfoFromDB(guid);
        }
        while (result->NextRow());
    }
}

uint32 World::GetLoggedInCharsCount(TeamId team)
{
    switch (team)
    {
        case TEAM_HORDE:
            return loggedInHordes.value();
        case TEAM_ALLIANCE:
            return loggedInAlliances.value();
        default:
            return loggedInAlliances.value() + loggedInHordes.value();
    }
}

uint32 World::ModifyLoggedInCharsCount(TeamId team, int val)
{
    switch (team)
    {
        case TEAM_HORDE:
            return loggedInHordes += val;
        case TEAM_ALLIANCE:
            return loggedInAlliances += val;
        default:
            break;
    }

    return 0;
}

void World::SetLoggedInCharsCount(TeamId team, uint32 val)
{
    switch (team)
    {
        case TEAM_HORDE:
            loggedInHordes = val;
            break;
        case TEAM_ALLIANCE:
            loggedInAlliances = val;
            break;
        default:
            break;
    }
}

void World::loadConfig(WorldConfigs index, const char* name, int32 def)
{
    if (index < CONFIG_VALUE_COUNT)
        m_configs[index] = sConfig.GetIntDefault(name, def);
}

void World::loadConfig(Rates index, const char* name, float def)
{
    if (index < MAX_RATES)
        rate_values[index] = sConfig.GetFloatDefault(name, def);
}

void World::loadConfig(WorldConfigs index, const char* name, bool def)
{
    if (index < CONFIG_VALUE_COUNT)
        m_configs[index] = sConfig.GetBoolDefault(name, def);
}

CBTresholds World::GetCoreBalancerTreshold()
{
    return _coreBalancer.GetTreshold();
}

CoreBalancer::CoreBalancer() : _diffSum(0), _diffCount(0), _balanceTimer(0)
{
    _treshold = CB_DISABLE_NONE;
}

void CoreBalancer::Initialize()
{
    _balanceTimer.Reset(sWorld.getConfig(CONFIG_COREBALANCER_INTERVAL));
}

void CoreBalancer::Update(const uint32 diff)
{
    _diffSum += diff;
    ++_diffCount;

    _balanceTimer.Update(diff);
    if (_balanceTimer.Passed())
    {
        uint32 diffAvg = _diffSum / _diffCount;;
        if (diffAvg > sWorld.getConfig(CONFIG_COREBALANCER_PLAYABLE_DIFF))
            IncreaseTreshold();
        else
        {
            if (diffAvg +20 < sWorld.getConfig(CONFIG_COREBALANCER_PLAYABLE_DIFF))
                DecreaseTreshold();
        }

        _diffSum = diff;
        _diffCount = 1;

        _balanceTimer.Reset(sWorld.getConfig(CONFIG_COREBALANCER_INTERVAL));
    }
}

void CoreBalancer::IncreaseTreshold()
{
    uint32 t = _treshold;
    if (t >= CB_TRESHOLD_MAX)
        return;

    _treshold = CBTresholds(++t);
}

void CoreBalancer::DecreaseTreshold()
{
    uint32 t = _treshold;
    if (t <= CB_DISABLE_NONE)
        return;

    _treshold = CBTresholds(--t);
}
