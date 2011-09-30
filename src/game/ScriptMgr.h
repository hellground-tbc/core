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

#ifndef _SCRIPTMGR_H
#define _SCRIPTMGR_H

#include "Common.h"
#include "Policies/Singleton.h"

#define MIN_DB_SCRIPT_STRING_ID        MAX_TRINITY_STRING_ID // 'db_script_string'
#define MAX_DB_SCRIPT_STRING_ID        2000010000

struct ScriptInfo
{
    uint32 id;
    uint32 delay;
    uint32 command;
    uint32 datalong;
    uint32 datalong2;
    int32  dataint;
    float x;
    float y;
    float z;
    float o;
};

typedef std::multimap<uint32, ScriptInfo> ScriptMap;
typedef std::map<uint32, ScriptMap > ScriptMapMap;
extern ScriptMapMap sQuestEndScripts;
extern ScriptMapMap sQuestStartScripts;
extern ScriptMapMap sSpellScripts;
extern ScriptMapMap sGameObjectScripts;
extern ScriptMapMap sEventScripts;
extern ScriptMapMap sWaypointScripts;

class ScriptMgr
{
public:
    ScriptMgr();
    ~ScriptMgr();

    typedef std::vector<std::string> ScriptNameMap;

    void LoadGameObjectScripts();
    void LoadQuestEndScripts();
    void LoadQuestStartScripts();
    void LoadEventScripts();
    void LoadSpellScripts();
    void LoadWaypointScripts();

    void LoadDbScriptStrings();

    void LoadScriptNames();
    void LoadAreaTriggerScripts();

    uint32 GetAreaTriggerScriptId(uint32 triggerId) const;

    ScriptNameMap &GetScriptNames() { return m_scriptNames; }
    const char * GetScriptName(uint32 id) { return id < m_scriptNames.size() ? m_scriptNames[id].c_str() : ""; }
    uint32 GetScriptId(const char *name);

private:
    void LoadScripts(ScriptMapMap& scripts, const char* tablename);
    void CheckScripts(ScriptMapMap const& scripts,std::set<int32>& ids);

    typedef UNORDERED_MAP<uint32, uint32> AreaTriggerScriptMap;
    typedef UNORDERED_MAP<uint32, uint32> EventIdScriptMap;
    typedef std::vector<std::string> ScriptNameMap;

    AreaTriggerScriptMap    m_AreaTriggerScripts;

    ScriptNameMap           m_scriptNames;
};

#define sScriptMgr Trinity::Singleton<ScriptMgr>::Instance()

TRINITY_DLL_SPEC uint32 GetAreaTriggerScriptId(uint32 triggerId);
TRINITY_DLL_SPEC uint32 GetScriptId(const char *name);
TRINITY_DLL_SPEC ScriptMgr::ScriptNameMap& GetScriptNames();

#endif
