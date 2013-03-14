/*
 * Copyright (C) 2008-2009 Trinity <http://www.trinitycore.org/>
 *
 * Thanks to the original authors: MaNGOS <http://getmangos.com/>
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

#include "precompiled.h"
#include "system.h"
#include "ProgressBar.h"
#include "ObjectMgr.h"
#include "Database/DatabaseEnv.h"

#if PLATFORM == PLATFORM_WINDOWS
HELLGROUND_IMPORT_EXPORT DatabaseType GameDataDatabase;                              ///< Accessor to the world database
HELLGROUND_IMPORT_EXPORT DatabaseType RealmDataDatabase;                             ///< Accessor to the character database
HELLGROUND_IMPORT_EXPORT DatabaseType AccountsDatabase;                              ///< Accessor to the realm/login database
#endif

SystemMgr::SystemMgr()
{
}

SystemMgr& SystemMgr::Instance()
{
    static SystemMgr pSysMgr;
    return pSysMgr;
}

void SystemMgr::LoadVersion()
{
    //Get Version information
    QueryResultAutoPtr pResult = GameDataDatabase.PQuery("SELECT script_version FROM version LIMIT 1");

    if (pResult)
    {
        Field* pFields = pResult->Fetch();

        outstring_log("TSCR: Database version is: %s", pFields[0].GetString());
        outstring_log("");
    }
    else
    {
        error_log("TSCR: Missing `version`.`script_version` information.");
        outstring_log("");
    }
}

void SystemMgr::LoadScriptTexts()
{
    outstring_log("TSCR: Loading Script Texts...");
    LoadHellgroundStrings(GameDataDatabase,"script_texts",TEXT_SOURCE_RANGE,1+(TEXT_SOURCE_RANGE*2));

    QueryResultAutoPtr pResult = GameDataDatabase.PQuery("SELECT entry, sound, type, language, emote FROM script_texts");

    outstring_log("TSCR: Loading Script Texts additional data...");

    if (pResult)
    {
        BarGoLink bar(pResult->GetRowCount());
        uint32 uiCount = 0;

        do
        {
            bar.step();
            Field* pFields = pResult->Fetch();
            StringTextData pTemp;

            int32 iId           = pFields[0].GetInt32();
            pTemp.uiSoundId     = pFields[1].GetUInt32();
            pTemp.uiType        = pFields[2].GetUInt32();
            pTemp.uiLanguage    = pFields[3].GetUInt32();
            pTemp.uiEmote       = pFields[4].GetUInt32();

            if (iId >= 0)
            {
                error_db_log("TSCR: Entry %i in table `script_texts` is not a negative value.", iId);
                continue;
            }

            if (iId > TEXT_SOURCE_RANGE || iId <= TEXT_SOURCE_RANGE*2)
            {
                error_db_log("TSCR: Entry %i in table `script_texts` is out of accepted entry range for table.", iId);
                continue;
            }

            if (pTemp.uiSoundId)
            {
                if (!GetSoundEntriesStore()->LookupEntry(pTemp.uiSoundId))
                    error_db_log("TSCR: Entry %i in table `script_texts` has soundId %u but sound does not exist.", iId, pTemp.uiSoundId);
            }

            if (!GetLanguageDescByID(pTemp.uiLanguage))
                error_db_log("TSCR: Entry %i in table `script_texts` using Language %u but Language does not exist.", iId, pTemp.uiLanguage);

            if (pTemp.uiType > CHAT_TYPE_ZONE_YELL)
                error_db_log("TSCR: Entry %i in table `script_texts` has Type %u but this Chat Type does not exist.", iId, pTemp.uiType);

            m_mTextDataMap[iId] = pTemp;
            ++uiCount;
        } while (pResult->NextRow());

        outstring_log("");
        outstring_log(">> Loaded %u additional Script Texts data.", uiCount);
    }
    else
    {
        BarGoLink bar(1);
        bar.step();
        outstring_log("");
        outstring_log(">> Loaded 0 additional Script Texts data. DB table `script_texts` is empty.");
    }
}

void SystemMgr::LoadScriptTextsCustom()
{
    outstring_log("TSCR: Loading Custom Texts...");
    LoadHellgroundStrings(GameDataDatabase,"custom_texts",TEXT_SOURCE_RANGE*2,1+(TEXT_SOURCE_RANGE*3));

    QueryResultAutoPtr pResult = GameDataDatabase.PQuery("SELECT entry, sound, type, language, emote FROM custom_texts");

    outstring_log("TSCR: Loading Custom Texts additional data...");

    if (pResult)
    {
        BarGoLink bar(pResult->GetRowCount());
        uint32 uiCount = 0;

        do
        {
            bar.step();
            Field* pFields = pResult->Fetch();
            StringTextData pTemp;

            int32 iId              = pFields[0].GetInt32();
            pTemp.uiSoundId        = pFields[1].GetUInt32();
            pTemp.uiType           = pFields[2].GetUInt32();
            pTemp.uiLanguage       = pFields[3].GetUInt32();
            pTemp.uiEmote          = pFields[4].GetUInt32();

            if (iId >= 0)
            {
                error_db_log("TSCR: Entry %i in table `custom_texts` is not a negative value.", iId);
                continue;
            }

            if (iId > TEXT_SOURCE_RANGE*2 || iId <= TEXT_SOURCE_RANGE*3)
            {
                error_db_log("TSCR: Entry %i in table `custom_texts` is out of accepted entry range for table.", iId);
                continue;
            }

            if (pTemp.uiSoundId)
            {
                if (!GetSoundEntriesStore()->LookupEntry(pTemp.uiSoundId))
                    error_db_log("TSCR: Entry %i in table `custom_texts` has soundId %u but sound does not exist.", iId, pTemp.uiSoundId);
            }

            if (!GetLanguageDescByID(pTemp.uiLanguage))
                error_db_log("TSCR: Entry %i in table `custom_texts` using Language %u but Language does not exist.", iId, pTemp.uiLanguage);

            if (pTemp.uiType > CHAT_TYPE_ZONE_YELL)
                error_db_log("TSCR: Entry %i in table `custom_texts` has Type %u but this Chat Type does not exist.", iId, pTemp.uiType);

            m_mTextDataMap[iId] = pTemp;
            ++uiCount;
        } while (pResult->NextRow());

        outstring_log("");
        outstring_log(">> Loaded %u additional Custom Texts data.", uiCount);
    }
    else
    {
        BarGoLink bar(1);
        bar.step();
        outstring_log("");
        outstring_log(">> Loaded 0 additional Custom Texts data. DB table `custom_texts` is empty.");
    }
}
