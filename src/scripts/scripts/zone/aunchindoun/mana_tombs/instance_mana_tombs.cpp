/* 
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

#include "precompiled.h"
#include "def_mana_tombs.h"

#define ENCOUNTERS 4

struct instance_mana_tombs : public ScriptedInstance
{
    instance_mana_tombs(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];

    void Initialize()
    {
        for(uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < ENCOUNTERS; i++)
            if(Encounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
            case 185522:
                if(GetData(DATA_YOREVENT) == DONE)
                    go->Delete();
                break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_YOREVENT:
                if(Encounter[0] != DONE)
                    Encounter[0] = data;
                break;
            case DATA_PANDEMONIUSEVENT:
                if(Encounter[1] != DONE)
                    Encounter[1] = data;
                break;
            case DATA_NEXUSPRINCEEVENT:
                if(Encounter[2] != DONE)
                    Encounter[2] = data;
                break;
            case DATA_SHAHEENEVENT:
                if(Encounter[3] != DONE)
                    Encounter[3] = data;
                break;
        }

        if (data == DONE)
            SaveToDB();
        
    }

    uint32 GetData(uint32 type)
    {
        switch( type )
        {
            case DATA_YOREVENT:         return Encounter[0];
            case DATA_PANDEMONIUSEVENT: return Encounter[1];
            case DATA_NEXUSPRINCEEVENT: return Encounter[2];
            case DATA_SHAHEENEVENT:     return Encounter[3];
                        
        }
        return false;
    }

   std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream stream;
        stream << Encounter[0] << " "
                << Encounter[1] << " "
                << Encounter[2] << " "
                << Encounter[3];

        OUT_SAVE_INST_DATA_COMPLETE;

        return stream.str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2];

        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

};

InstanceData* GetInstanceData_instance_mana_tombs(Map* map)
{
    return new instance_mana_tombs(map);
}

void AddSC_instance_mana_tombs()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_mana_tombs";
    newscript->GetInstanceData = &GetInstanceData_instance_mana_tombs;
    newscript->RegisterSelf();
}

