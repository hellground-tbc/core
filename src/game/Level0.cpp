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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "AccountMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Util.h"
#include "GameEvent.h"
#include "BattleGroundMgr.h"

bool ChatHandler::HandleAccountXPToggleCommand(const char* args)
{
    if (uint32 account_id = m_session->GetAccountId())
    {
        if (WorldSession *session = sWorld.FindSession(account_id))
        {
            if (session->IsAccountFlagged(ACC_BLIZZLIKE_RATES))
            {
                session->RemoveAccountFlag(ACC_BLIZZLIKE_RATES);
                PSendSysMessage("Now your rates are serverlike: x2.");
            }
            else
            {
                session->AddAccountFlag(ACC_BLIZZLIKE_RATES);
                PSendSysMessage("Now your rates are blizzlike: x1.");
            }
        }
    }
    else
    {
        PSendSysMessage("Specified account not found.");
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAccountBonesHideCommand(const char* args)
{
    if (uint32 account_id = m_session->GetAccountId())
    {
        if (WorldSession *session = sWorld.FindSession(account_id))
        {
            if (session->IsAccountFlagged(ACC_HIDE_BONES))
            {
                session->RemoveAccountFlag(ACC_HIDE_BONES);
                PSendSysMessage("Client will show bones for this account now.");
            }
            else
            {
                session->AddAccountFlag(ACC_HIDE_BONES);
                PSendSysMessage("Client won't show bones for this account now.");
            }
        }
    }
    else
    {
        PSendSysMessage("Specified account not found.");
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAccountGuildAnnToggleCommand(const char* args)
{
    if (uint32 account_id = m_session->GetAccountId())
    {
        if (WorldSession *session = sWorld.FindSession(account_id))
        {
            if (session->IsAccountFlagged(ACC_DISABLED_GANN))
            {
                session->RemoveAccountFlag(ACC_DISABLED_GANN);
                PSendSysMessage("Guild announces have been enabled for this account.");
            }
            else
            {
                session->AddAccountFlag(ACC_DISABLED_GANN);
                PSendSysMessage("Guild announces have been disabled for this account.");
            }
        }
    }
    else
    {
        PSendSysMessage("Specified account not found.");
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAccountBattleGroundAnnCommand(const char* args)
{
    if (uint32 account_id = m_session->GetAccountId())
    {
        if (WorldSession *session = sWorld.FindSession(account_id))
        {
            if (session->IsAccountFlagged(ACC_DISABLED_BGANN))
            {
                session->RemoveAccountFlag(ACC_DISABLED_BGANN);
                PSendSysMessage("BattleGround announces have been enabled for this account.");
            }
            else
            {
                session->AddAccountFlag(ACC_DISABLED_BGANN);
                PSendSysMessage("BattleGround announces have been disabled for this account.");
            }
        }
    }
    else
    {
        PSendSysMessage("Specified account not found.");
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAccountAnnounceBroadcastCommand(const char* args)
{
    if (m_session->IsAccountFlagged(ACC_DISABLED_BROADCAST))
    {
        m_session->RemoveAccountFlag(ACC_DISABLED_BROADCAST);
        PSendSysMessage("AutoBroadcast announces have been enabled for this account.");
    }
    else
    {
        m_session->AddAccountFlag(ACC_DISABLED_BROADCAST);
        PSendSysMessage("AutoBroadcast announces have been disabled for this account.");
    }

    return true;
}

bool ChatHandler::HandleHelpCommand(const char* args)
{
    char* cmd = strtok((char*)args, " ");
    if (!cmd)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), cmd))
            SendSysMessage(LANG_NO_HELP_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(const char* args)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(const char* /*args*/)
{
    uint64 permissions = m_session->GetPermissions();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, permissions);
    return true;
}

bool ChatHandler::HandleStartCommand(const char* /*args*/)
{
    Player *chr = m_session->GetPlayer();

    if (chr->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (chr->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr,7355,false);
    return true;
}

bool ChatHandler::HandleAccountWeatherCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    std::string argstr = (char*)args;
    if (argstr == "on")
        m_session->AddOpcodeDisableFlag(OPC_DISABLE_WEATHER);
    else if (argstr == "off")
        m_session->RemoveOpcodeDisableFlag(OPC_DISABLE_WEATHER);
    else
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    PSendSysMessage(LANG_SET_WEATHER, argstr.c_str());
    return true;
}

bool ChatHandler::HandleArenaReadyCommand(const char* args)
{
    Player* player = m_session->GetPlayer();
    if ( !player->InArena() )
    {
        PSendSysMessage("You can use this command only in arena.");
        return false;
    }

    BattleGround* bg = player->GetBattleGround();
    if ( bg == nullptr )
        return false;

    bool result = bg->SetPlayerReady(player->GetGUID());
    if (result)
    {
        PSendSysMessage("You have been NOT marked as ready due to some problems.");
        return false;
    }

    PSendSysMessage("You have been marked as ready.");
    return true;
}

bool ChatHandler::HandleServerInfoCommand(const char* /*args*/)
{
    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string str = secsToTimeString(sWorld.GetUptime());
    uint32 updateTime = sWorld.GetUpdateTime();
    std::string str2 = TimeToTimestampStr(sWorld.GetGameTime());

    PSendSysMessage("HellGround.net - rev: %s",_REVISION);
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
    PSendSysMessage(LANG_UPTIME, str.c_str());
    PSendSysMessage("Current time: %s", str2.c_str());
    PSendSysMessage("Update time diff: %u.", updateTime);

    if (sWorld.IsShutdowning())
    {
        PSendSysMessage("");
        PSendSysMessage("Server will %s in: %s", (sWorld.GetShutdownMask() & SHUTDOWN_MASK_RESTART ? "restart" : "be shutteddown"), secsToTimeString(sWorld.GetShutdownTimer()).c_str());
        PSendSysMessage("Reason: %s.", sWorld.GetShutdownReason());
    }

    return true;
}

bool ChatHandler::HandleServerEventsCommand(const char*)
{
    std::string active_events = sGameEventMgr.getActiveEventsString();
    PSendSysMessage(active_events.c_str());//ChatHandler::FillMessageData(&data, this, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, GetPlayer()->GetGUID(), active_events, NULL);
    if(sWorld.getConfig(CONFIG_ARENA_DAILY_REQUIREMENT))
    {
        PSendSysMessage("Daily Arenas! Get %u AP for winning %u rated arenas",
            sWorld.getConfig(CONFIG_ARENA_DAILY_AP_REWARD),sWorld.getConfig(CONFIG_ARENA_DAILY_REQUIREMENT));
    }
    return true;
}

bool ChatHandler::HandleDismountCommand(const char* /*args*/)
{
    //If player is not mounted, so go out :)
    if (!m_session->GetPlayer()->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->Unmount();
    m_session->GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(const char* /*args*/)
{
    Player *player=m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if (m_session->HasPermissions(PERM_GMT))
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_INTERVAL_SAVE);
    if (save_interval==0 || save_interval > 20*1000 && player->GetSaveTimer() <= save_interval - 20*1000)
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandlePasswordCommand(const char* args)
{
    if (!*args)
        return false;

    char *old_pass = strtok ((char*)args, " ");
    char *new_pass = strtok (NULL, " ");
    char *new_pass_c  = strtok (NULL, " ");

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (strcmp(new_pass, new_pass_c) != 0)
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    if (!AccountMgr::CheckPassword (m_session->GetAccountId(), password_old))
    {
        SendSysMessage (LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = AccountMgr::ChangePassword(m_session->GetAccountId(), password_new);

    switch (result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandleLockAccountCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    std::string argstr = (char*)args;
    if (argstr == "on")
    {
        AccountsDatabase.PExecute("UPDATE account SET account_state_id = '2' WHERE account_id = '%u'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
        return true;
    }

    if (argstr == "off")
    {
        AccountsDatabase.PExecute("UPDATE account SET account_state_id = '1' WHERE account_id = '%u'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(const char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}

bool ChatHandler::HandleServerPVPCommand(const char* /*args*/)
{
    Player *player = m_session->GetPlayer();

    if (!sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_INFO))
        PSendSysMessage("Battleground queue info is disabled");
    else
    {
        if (!(player->InBattleGroundQueue()))
            PSendSysMessage("You aren't in any battleground queue");
        else
        {
            BattleGroundQueueTypeId qtype;
            BattleGroundTypeId bgtype;
            bool isbg;
            for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                qtype = player->GetBattleGroundQueueTypeId(i);
                isbg = false;
                switch (qtype)
                {
                case BATTLEGROUND_QUEUE_AB:
                    {
                        PSendSysMessage("You are queued for Arathi Basin");
                        isbg = true;
                        bgtype = BATTLEGROUND_AB;
                        break;
                    }
                case BATTLEGROUND_QUEUE_AV:
                    {
                        PSendSysMessage("You are queued for Alterac Valley");
                        isbg = true;
                        bgtype = BATTLEGROUND_AV;
                        break;
                    }
                case BATTLEGROUND_QUEUE_WS:
                    {
                        PSendSysMessage("You are queued for Warsong Gulch");
                        isbg = true;
                        bgtype = BATTLEGROUND_WS;
                        break;
                    }
                case BATTLEGROUND_QUEUE_EY:
                    {
                        PSendSysMessage("You are queued for Eye of the storm");
                        isbg = true;
                        bgtype = BATTLEGROUND_EY;
                        break;
                    }
                default:
                    break;
                }

                if (isbg)
                {
                    uint32 minPlayers = sBattleGroundMgr.GetBattleGroundTemplate(bgtype)->GetMinPlayersPerTeam();
                    uint32 queuedHorde = sBattleGroundMgr.m_BattleGroundQueues[qtype].GetQueuedPlayersCount(BG_TEAM_HORDE, player->GetBattleGroundBracketIdFromLevel(bgtype));
                    uint32 queuedAlliance = sBattleGroundMgr.m_BattleGroundQueues[qtype].GetQueuedPlayersCount(BG_TEAM_ALLIANCE, player->GetBattleGroundBracketIdFromLevel(bgtype));
                    PSendSysMessage("Horde queued: %u, Alliance queued: %u. Minimum per team: %u", queuedHorde, queuedAlliance, minPlayers);
                }
            }
        }
    }

    if (sWorld.getConfig(CONFIG_ARENA_STATUS_INFO))
        PSendSysMessage("Arena status: %u players in 2v2, %u in 3v3, %u in 5v5",
            sBattleGroundMgr.inArenasCount[0],sBattleGroundMgr.inArenasCount[1],sBattleGroundMgr.inArenasCount[2]);
    else
        PSendSysMessage("Arena status is disabled");

    if(sWorld.getConfig(CONFIG_ARENA_DAILY_REQUIREMENT))
        PSendSysMessage("Today you won %u rated arenas (%u required for reward)",
            player->m_DailyArenasWon,sWorld.getConfig(CONFIG_ARENA_DAILY_REQUIREMENT));

    return true;
}
