/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Instance_Blood_Furnace
SD%Complete: 85
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "precompiled.h"
#include "def_blood_furnace.h"

#define ENTRY_SEWER1                 181823
#define ENTRY_SEWER2                 181766

struct TRINITY_DLL_DECL instance_blood_furnace : public ScriptedInstance
{
    instance_blood_furnace(Map *map) : ScriptedInstance(map) {Initialize();};


    uint64 Sewer1GUID;
    uint64 Sewer2GUID;

    uint32 BroggokEncounter;
    uint32 MakerEncounter;

    uint64 BroggokDoor;
    uint64 MakerDoorGUID;

    uint64 CellDoor[4];

    void Initialize()
    {
        Sewer1GUID = 0;
        Sewer2GUID = 0;
        MakerDoorGUID = 0;
        BroggokEncounter = NOT_STARTED;
        MakerEncounter = NOT_STARTED;

        for(int i = 0; i < 4; i++)
            CellDoor[i] = 0;

        BroggokDoor = 0;
    }

    void OnObjectCreate(GameObject *go)
    {
        switch (go->GetEntry())
        {
            case ENTRY_SEWER1: Sewer1GUID = go->GetGUID(); break;
            case ENTRY_SEWER2: Sewer2GUID = go->GetGUID(); break;
            case 181819:
                BroggokDoor = go->GetGUID();
                if (BroggokEncounter == DONE)
                    HandleGameObject(BroggokDoor, 0);
                break;
            case 181821: CellDoor[0] = go->GetGUID(); break;
            case 181820: CellDoor[1] = go->GetGUID(); break;
            case 181818: CellDoor[2] = go->GetGUID(); break;
            case 181817: CellDoor[3] = go->GetGUID(); break;
            case 181812:
                MakerDoorGUID = go->GetGUID();
                if (MakerEncounter == DONE)
                    HandleGameObject(MakerDoorGUID,0);
                break;
        }
    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    return plr;
            }
        }

        debug_log("TSCR: Instance Blood Furnace: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void HandleGameObject(uint64 guid, uint32 state)
    {
        Player *player = GetPlayerInMap();

        if (!player || !guid)
        {
            debug_log("TSCR: Blood Furnace: HandleGameObject fail");
            return;
        }

        if (GameObject *go = GameObject::GetGameObject(*player,guid))
            go->SetGoState(GOState(state));
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
            case DATA_KELIDANEVENT:
                if (data == DONE)
                {
                    HandleGameObject(Sewer1GUID,0);
                    HandleGameObject(Sewer2GUID,0);
                }
                break;
            case DATA_BROGGOKEVENT:
                if (data == DONE)
                    HandleGameObject(BroggokDoor, 0);

                BroggokEncounter = data;
                break;
            case DATA_MAKEREVENT:
                if (data == DONE)
                    HandleGameObject(MakerDoorGUID,0);

                MakerEncounter = data;
                break;

            if (data == DONE)
                SaveToDB();
        }
    }

    uint32 GetData(uint32 type)
    {
        if (type == DATA_BROGGOKEVENT)
            return BroggokEncounter;

        return NULL;
    }

    uint64 GetData64(uint32 type)
    {
        switch (type)
        {
            case 1:
            case 2:
            case 3:
            case 4:
                return CellDoor[type-1];
            case 5:
                return BroggokDoor;
            default:
                return NULL;
        }
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream stream;
        stream << MakerEncounter << " ";
        stream << BroggokEncounter;

        OUT_SAVE_INST_DATA_COMPLETE;

        return stream.str();
    }

    void Load(const char* in)
    {
        if(!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }
        OUT_LOAD_INST_DATA(in);
        std::istringstream stream(in);
        stream >> MakerEncounter >> BroggokEncounter;

        if (MakerEncounter == DONE)
            HandleGameObject(MakerDoorGUID,0);

        if (BroggokEncounter == DONE)
            HandleGameObject(BroggokDoor, 0);

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_blood_furnace(Map* map)
{
    return new instance_blood_furnace(map);
}

void AddSC_instance_blood_furnace()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_blood_furnace";
    newscript->GetInstanceData = &GetInstanceData_instance_blood_furnace;
    newscript->RegisterSelf();
}

