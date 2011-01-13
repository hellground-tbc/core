/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Instance_Mechanar
SD%Complete: 100
SDComment:
SDCategory: Mechanar
EndScriptData */

#include "precompiled.h"
#include "def_mechanar.h"

#define ENCOUNTERS      3

#define GO_MOARGDOOR1           184632
#define GO_MOARGDOOR2           184322
#define GO_NETHERMANCERDOOR     184449

#define NPC_IRONHAND            19710
#define NPC_GYROKILL            19218

struct TRINITY_DLL_DECL instance_mechanar : public ScriptedInstance
{
    instance_mechanar(Map *map) : ScriptedInstance(map) {Initialize();};


    uint32 Encounters[ENCOUNTERS];
    uint64 MoargDoor1;
    uint64 MoargDoor2;
    uint64 NethermancerDoor;

    void OnCreatureCreate (Creature *creature, uint32 creature_entry)
    {
    }

    void OnObjectCreate(GameObject* go)
    {
        switch(go->GetEntry())
        {
            case GO_MOARGDOOR1:
                MoargDoor1 = go->GetGUID();
                if(GetData(DATA_IRONHAND_EVENT) == DONE)
                    HandleGameObject(NULL, true, go);
                break;
            case GO_MOARGDOOR2:
                MoargDoor2 = go->GetGUID();
                if(GetData(DATA_GYROKILL_EVENT) == DONE)
                    HandleGameObject(NULL, true, go);
                break;
            case GO_NETHERMANCERDOOR:
                NethermancerDoor = go->GetGUID();
                HandleGameObject(NULL, true, go);
                break;
        }
    }

    void Initialize()
    {
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            Encounters[i] = NOT_STARTED;

        MoargDoor1 = 0;
        MoargDoor2 = 0;
        NethermancerDoor = 0;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if(Encounters[i] == IN_PROGRESS)
                return true;

        return false;
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
        case DATA_NETHERMANCER_EVENT:   return Encounters[0];
        case DATA_IRONHAND_EVENT:       return Encounters[1];
        case DATA_GYROKILL_EVENT:       return Encounters[2];
        }

        return false;
    }

    uint64 GetData64 (uint32 identifier)
    {
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_NETHERMANCER_EVENT:
                if(Encounters[0] != DONE)
                {
                    Encounters[0] = data;
                    if(data == IN_PROGRESS)
                        HandleGameObject(NethermancerDoor, false);
                    else
                        HandleGameObject(NethermancerDoor, true);
                }
                break;
            case DATA_IRONHAND_EVENT:
                if(Encounters[1] != DONE)
                {
                    Encounters[1] = data;
                    if(data == DONE)
                        HandleGameObject(MoargDoor1, true);
                }
                break;
            case DATA_GYROKILL_EVENT:
                if(Encounters[2] != DONE)
                {
                    Encounters[2] = data;
                    if(data == DONE)
                        HandleGameObject(MoargDoor2, true);
                }
                break;
        }
        if(data == DONE)
            SaveToDB();
    }

    const char* Save()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream stream;
        stream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2];
        char* out = new char[stream.str().length() + 1];
        strcpy(out, stream.str().c_str());
        if(out)
        {
            OUT_SAVE_INST_DATA_COMPLETE;
            return out;
        }
        return NULL;
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
        stream >> Encounters[0] >> Encounters[1] >> Encounters[2];
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if(Encounters[i] == IN_PROGRESS)
                Encounters[i] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_mechanar(Map* map)
{
    return new instance_mechanar(map);
}

void AddSC_instance_mechanar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_mechanar";
    newscript->GetInstanceData = &GetInstanceData_instance_mechanar;
    newscript->RegisterSelf();
}
