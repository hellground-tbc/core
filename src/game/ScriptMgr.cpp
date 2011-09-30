/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
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

#include "ScriptMgr.h"
#include "Policies/SingletonImp.h"
#include "Log.h"
#include "ProgressBar.h"
#include "ObjectMgr.h"
#include "WaypointManager.h"
#include "World.h"

ScriptMapMap sQuestEndScripts;
ScriptMapMap sQuestStartScripts;
ScriptMapMap sSpellScripts;
ScriptMapMap sGameObjectScripts;
ScriptMapMap sEventScripts;
ScriptMapMap sGossipScripts;
ScriptMapMap sWaypointScripts;

INSTANTIATE_SINGLETON_1(ScriptMgr);

ScriptMgr::ScriptMgr()
{
}

ScriptMgr::~ScriptMgr()
{
}

void ScriptMgr::LoadScripts(ScriptMapMap& scripts, char const* tablename)
{
    if (sWorld.IsScriptScheduled())                          // function don't must be called in time scripts use.
        return;

    sLog.outString("%s :", tablename);

    scripts.clear();                                        // need for reload support

    QueryResultAutoPtr result = WorldDatabase.PQuery("SELECT id,delay,command,datalong,datalong2,dataint, x, y, z, o FROM %s", tablename);

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded %u script definitions", count);
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        Field *fields = result->Fetch();
        ScriptInfo tmp;
        tmp.id        = fields[0].GetUInt32();
        tmp.delay     = fields[1].GetUInt32();
        tmp.command   = fields[2].GetUInt32();
        tmp.datalong  = fields[3].GetUInt32();
        tmp.datalong2 = fields[4].GetUInt32();
        tmp.dataint   = fields[5].GetInt32();
        tmp.x         = fields[6].GetFloat();
        tmp.y         = fields[7].GetFloat();
        tmp.z         = fields[8].GetFloat();
        tmp.o         = fields[9].GetFloat();

        // generic command args check
        switch (tmp.command)
        {
            case SCRIPT_COMMAND_TALK:
            {
                if (tmp.datalong > 3)
                {
                    sLog.outErrorDb("Table `%s` has invalid talk type (datalong = %u) in SCRIPT_COMMAND_TALK for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }

                if (tmp.dataint==0)
                {
                    sLog.outErrorDb("Table `%s` has invalid talk text id (dataint = %i) in SCRIPT_COMMAND_TALK for script id %u",tablename,tmp.dataint,tmp.id);
                    continue;
                }

                if (tmp.dataint < MIN_DB_SCRIPT_STRING_ID || tmp.dataint >= MAX_DB_SCRIPT_STRING_ID)
                {
                    sLog.outErrorDb("Table `%s` has out of range text id (dataint = %i expected %u-%u) in SCRIPT_COMMAND_TALK for script id %u",tablename,tmp.dataint,MIN_DB_SCRIPT_STRING_ID,MAX_DB_SCRIPT_STRING_ID,tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_EMOTE:
            {
                if (!sEmotesStore.LookupEntry(tmp.datalong))
                {
                    sLog.outErrorDb("Table `%s` has invalid emote id (datalong = %u) in SCRIPT_COMMAND_EMOTE for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TELEPORT_TO:
            {
                if (!sMapStore.LookupEntry(tmp.datalong))
                {
                    sLog.outErrorDb("Table `%s` has invalid map (Id: %u) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }

                if (!Trinity::IsValidMapCoord(tmp.x,tmp.y,tmp.z,tmp.o))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",tablename,tmp.x,tmp.y,tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!Trinity::IsValidMapCoord(tmp.x,tmp.y,tmp.z,tmp.o))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",tablename,tmp.x,tmp.y,tmp.id);
                    continue;
                }

                if (!sObjectMgr.GetCreatureTemplate(tmp.datalong))
                {
                    sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                GameObjectData const* data = sObjectMgr.GetGOData(tmp.datalong);
                if (!data)
                {
                    sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }

                GameObjectInfo const* info = GetGameObjectInfo(data->id);
                if (!info)
                {
                    sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",tablename,tmp.datalong,data->id,tmp.id);
                    continue;
                }

                if (info->type==GAMEOBJECT_TYPE_FISHINGNODE ||
                    info->type==GAMEOBJECT_TYPE_FISHINGHOLE ||
                    info->type==GAMEOBJECT_TYPE_DOOR        ||
                    info->type==GAMEOBJECT_TYPE_BUTTON      ||
                    info->type==GAMEOBJECT_TYPE_TRAP)
                {
                    sLog.outErrorDb("Table `%s` have gameobject type (%u) unsupported by command SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",tablename,info->id,tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_OPEN_DOOR:
            case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                GameObjectData const* data = sObjectMgr.GetGOData(tmp.datalong);
                if (!data)
                {
                    sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in %s for script id %u",tablename,tmp.datalong,(tmp.command==SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"),tmp.id);
                    continue;
                }

                GameObjectInfo const* info = GetGameObjectInfo(data->id);
                if (!info)
                {
                    sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in %s for script id %u",tablename,tmp.datalong,data->id,(tmp.command==SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"),tmp.id);
                    continue;
                }

                if (info->type!=GAMEOBJECT_TYPE_DOOR)
                {
                    sLog.outErrorDb("Table `%s` has gameobject type (%u) non supported by command %s for script id %u",tablename,info->id,(tmp.command==SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"),tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                Quest const* quest = sObjectMgr.GetQuestTemplate(tmp.datalong);
                if (!quest)
                {
                    sLog.outErrorDb("Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",tablename,tmp.datalong,tmp.id);
                    continue;
                }

                if (!quest->HasFlag(QUEST_TRINITY_FLAGS_EXPLORATION_OR_EVENT))
                {
                    sLog.outErrorDb("Table `%s` has quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, but quest not have flag QUEST_TRINITY_FLAGS_EXPLORATION_OR_EVENT in quest flags. Script command or quest flags wrong. Quest modified to require objective.",tablename,tmp.datalong,tmp.id);

                    // this will prevent quest completing without objective
                    const_cast<Quest*>(quest)->SetFlag(QUEST_TRINITY_FLAGS_EXPLORATION_OR_EVENT);

                    // continue; - quest objective requirement set and command can be allowed
                }

                if (float(tmp.datalong2) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                        tablename,tmp.datalong2,tmp.id);
                    continue;
                }

                if (tmp.datalong2 && float(tmp.datalong2) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, max distance is %f or 0 for disable distance check",
                        tablename,tmp.datalong2,tmp.id,DEFAULT_VISIBILITY_DISTANCE);
                    continue;
                }

                if (tmp.datalong2 && float(tmp.datalong2) < INTERACTION_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too small distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, min distance is %f or 0 for disable distance check",
                        tablename,tmp.datalong2,tmp.id,INTERACTION_DISTANCE);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_REMOVE_AURA:
            case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!sSpellStore.LookupEntry(tmp.datalong))
                {
                    sLog.outErrorDb("Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_REMOVE_AURA or SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tablename,tmp.datalong,tmp.id);
                    continue;
                }
                break;
            }
        }

        if (scripts.find(tmp.id) == scripts.end())
        {
            ScriptMap emptyMap;
            scripts[tmp.id] = emptyMap;
        }
        scripts[tmp.id].insert(std::pair<uint32, ScriptInfo>(tmp.delay, tmp));

        ++count;
    }
    while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u script definitions", count);
}

void ScriptMgr::CheckScripts(ScriptMapMap const& scripts,std::set<int32>& ids)
{
    for (ScriptMapMap::const_iterator itrMM = scripts.begin(); itrMM != scripts.end(); ++itrMM)
    {
        for (ScriptMap::const_iterator itrM = itrMM->second.begin(); itrM != itrMM->second.end(); ++itrM)
        {
            if (itrM->second.dataint)
            {
                if (!sObjectMgr.GetTrinityStringLocale (itrM->second.dataint))
                    sLog.outErrorDb("Table `db_script_string` has not existed string id  %u", itrM->first);

                if (ids.count(itrM->second.dataint))
                    ids.erase(itrM->second.dataint);
            }
        }
    }
}

void ScriptMgr::LoadGameObjectScripts()
{
    LoadScripts(sGameObjectScripts, "gameobject_scripts");

    // check ids
    for (ScriptMapMap::const_iterator itr = sGameObjectScripts.begin(); itr != sGameObjectScripts.end(); ++itr)
    {
        if (!sObjectMgr.GetGOData(itr->first))
            sLog.outErrorDb("Table `gameobject_scripts` has not existing gameobject (GUID: %u) as script id", itr->first);
    }
}

void ScriptMgr::LoadQuestEndScripts()
{
    LoadScripts(sQuestEndScripts, "quest_end_scripts");

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestEndScripts.begin(); itr != sQuestEndScripts.end(); ++itr)
    {
        if (!sObjectMgr.GetQuestTemplate(itr->first))
            sLog.outErrorDb("Table `quest_end_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ScriptMgr::LoadQuestStartScripts()
{
    LoadScripts(sQuestStartScripts,"quest_start_scripts");

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestStartScripts.begin(); itr != sQuestStartScripts.end(); ++itr)
    {
        if (!sObjectMgr.GetQuestTemplate(itr->first))
            sLog.outErrorDb("Table `quest_start_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ScriptMgr::LoadSpellScripts()
{
    LoadScripts(sSpellScripts, "spell_scripts");

    // check ids
    for (ScriptMapMap::const_iterator itr = sSpellScripts.begin(); itr != sSpellScripts.end(); ++itr)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
        if (!spellInfo)
        {
            sLog.outErrorDb("Table `spell_scripts` has not existing spell (Id: %u) as script id", itr->first);
            continue;
        }

        //check for correct spellEffect
        bool found = false;
        for (int i=0; i<3; ++i)
        {
            // skip empty effects
            if (!spellInfo->Effect[i])
                continue;

            if (spellInfo->Effect[i] == SPELL_EFFECT_SCRIPT_EFFECT)
            {
                found =  true;
                break;
            }
        }

        if (!found)
            sLog.outErrorDb("Table `spell_scripts` has unsupported spell (Id: %u) without SPELL_EFFECT_SCRIPT_EFFECT (%u) spell effect",itr->first,SPELL_EFFECT_SCRIPT_EFFECT);
    }
}

void ScriptMgr::LoadEventScripts()
{
    LoadScripts(sEventScripts, "event_scripts");

    std::set<uint32> evt_scripts;
    // Load all possible script entries from gameobjects
    for (uint32 i = 1; i < sGOStorage.MaxEntry; ++i)
    {
        if (GameObjectInfo const * goInfo = sGOStorage.LookupEntry<GameObjectInfo>(i))
            if (uint32 eventId = goInfo->GetEventScriptId())
                evt_scripts.insert(eventId);
    }

    // Load all possible script entries from spells
    for (uint32 i = 1; i < sSpellStore.GetNumRows(); ++i)
    {
        SpellEntry const * spell = sSpellStore.LookupEntry(i);
        if (spell)
        {
            for (int j=0; j<3; ++j)
            {
                if (spell->Effect[j] == SPELL_EFFECT_SEND_EVENT)
                {
                    if (spell->EffectMiscValue[j])
                        evt_scripts.insert(spell->EffectMiscValue[j]);
                }
            }
        }
    }

    for (size_t path_idx = 0; path_idx < sTaxiPathNodesByPath.size(); ++path_idx)
    {
        for (size_t node_idx = 0; node_idx < sTaxiPathNodesByPath[path_idx].size(); ++node_idx)
        {
            TaxiPathNodeEntry const& node = sTaxiPathNodesByPath[path_idx][node_idx];

            if (node.arrivalEventID)
                evt_scripts.insert(node.arrivalEventID);

            if (node.departureEventID)
                evt_scripts.insert(node.departureEventID);
        }
    }

    // Then check if all scripts are in above list of possible script entries
    for (ScriptMapMap::const_iterator itr = sEventScripts.begin(); itr != sEventScripts.end(); ++itr)
    {
        std::set<uint32>::const_iterator itr2 = evt_scripts.find(itr->first);
        if (itr2 == evt_scripts.end())
            sLog.outErrorDb("Table `event_scripts` has script (Id: %u) not referring to any gameobject_template type 10 data2 field or type 3 data6 field or any spell effect %u", itr->first, SPELL_EFFECT_SEND_EVENT);
    }
}

//Load WP Scripts
void ScriptMgr::LoadWaypointScripts()
{
    LoadScripts(sWaypointScripts, "waypoint_scripts");

    for (ScriptMapMap::const_iterator itr = sWaypointScripts.begin(); itr != sWaypointScripts.end(); ++itr)
    {
        QueryResultAutoPtr query = WorldDatabase.PQuery("SELECT * FROM `waypoint_scripts` WHERE `id` = %u", itr->first);
        if (!query || !query->GetRowCount())
            sLog.outErrorDb("There is no waypoint which links to the waypoint script %u", itr->first);
    }
}

void ScriptMgr::LoadDbScriptStrings()
{
    LoadTrinityStrings(WorldDatabase,"db_script_string",MIN_DB_SCRIPT_STRING_ID,MAX_DB_SCRIPT_STRING_ID);

    std::set<int32> ids;

    for (int32 i = MIN_DB_SCRIPT_STRING_ID; i < MAX_DB_SCRIPT_STRING_ID; ++i)
        if (sObjectMgr.GetTrinityStringLocale(i))
            ids.insert(i);

    CheckScripts(sQuestEndScripts,ids);
    CheckScripts(sQuestStartScripts,ids);
    CheckScripts(sSpellScripts,ids);
    CheckScripts(sGameObjectScripts,ids);
    CheckScripts(sEventScripts,ids);

    CheckScripts(sWaypointScripts,ids);

    for (std::set<int32>::const_iterator itr = ids.begin(); itr != ids.end(); ++itr)
        sLog.outErrorDb("Table `db_script_string` has unused string id  %u", *itr);
}

void ScriptMgr::LoadScriptNames()
{
    m_scriptNames.push_back("");
    QueryResultAutoPtr result = WorldDatabase.Query(
      "SELECT DISTINCT(ScriptName) FROM creature_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM gameobject_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM item_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM areatrigger_scripts WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(script) FROM instance_template WHERE script <> ''");
    if (result)
    {
        do
        {
            m_scriptNames.push_back((*result)[0].GetString());
        } while (result->NextRow());
    }

    std::sort(m_scriptNames.begin(), m_scriptNames.end());
}

void ScriptMgr::LoadAreaTriggerScripts()
{
    m_AreaTriggerScripts.clear();                            // need for reload case
    QueryResultAutoPtr result = WorldDatabase.Query("SELECT entry, ScriptName FROM areatrigger_scripts");

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded %u areatrigger scripts", count);
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        ++count;
        bar.step();

        Field *fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();
        const char *scriptName = fields[1].GetString();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.",Trigger_ID);
            continue;
        }
        m_AreaTriggerScripts[Trigger_ID] = GetScriptId(scriptName);
    } while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u areatrigger scripts", count);
}

ScriptMgr::ScriptNameMap & GetScriptNames()
{
    return sScriptMgr.GetScriptNames();
}

uint32 ScriptMgr::GetScriptId(const char *name)
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if (!name) return 0;
    ScriptNameMap::const_iterator itr =
        std::lower_bound(m_scriptNames.begin(), m_scriptNames.end(), name);
    if (itr == m_scriptNames.end() || *itr != name) return 0;
    return itr - m_scriptNames.begin();
}

uint32 ScriptMgr::GetAreaTriggerScriptId(uint32 trigger_id) const
{
    AreaTriggerScriptMap::const_iterator i = m_AreaTriggerScripts.find(trigger_id);
    if (i!= m_AreaTriggerScripts.end())
        return i->second;
    return 0;
}

// Functions for scripting access
uint32 GetAreaTriggerScriptId(uint32 trigger_id)
{
    return sScriptMgr.GetAreaTriggerScriptId(trigger_id);
}

uint32 GetScriptId(const char *name)
{
    return sScriptMgr.GetScriptId(name);
}
