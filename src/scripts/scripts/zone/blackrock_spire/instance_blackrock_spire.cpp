/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "precompiled.h"
#include "def_blackrock_spire.h"

#define EMBERSEER_IN    175244
#define EMBERSEER_OUT   175153

#define ENCOUNTERS 3

struct instance_blackrock_spire : public ScriptedInstance
{
    instance_blackrock_spire(Map *map) : ScriptedInstance(map){};

    uint32 Encounters[ENCOUNTERS];
    uint64 emberseerOut;

    uint32 runesTimer;
    std::set<uint64> emberseerInDoorsGUID;
    std::set<uint64> runesDoorGUID;
    std::set<uint64> runesEmberGUID;

    void Initialize()
    {
        for(uint8 i=0; i < ENCOUNTERS; ++i)
            Encounters[i] = NOT_STARTED;

        runesDoorGUID.clear();
        runesEmberGUID.clear();
        emberseerInDoorsGUID.clear();
        emberseerOut = 0;
        runesTimer = 3000;
    }


    void OnObjectCreate (GameObject* go)
    {
        switch(go->GetEntry())
        {
            case 175194:
            case 175195:
            case 175196:
            case 175197:
            case 175198:
            case 175199:
            case 175200:
                runesDoorGUID.insert(go->GetGUID());
                break;

            case 175187:
            case 175267:
            case 175268:
            case 175269:
            case 175270:
            case 175271:
            case 175272:
                runesEmberGUID.insert(go->GetGUID());
                break;

            case EMBERSEER_IN:
                emberseerInDoorsGUID.insert(go->GetGUID());
                if(GetData(DATA_RUNE_DOOR) == DONE)
                    HandleGameObject(go->GetGUID(), true);
                break;
            case EMBERSEER_OUT:
                emberseerOut = go->GetGUID();
                if(GetData(DATA_EMBERSEER) == DONE)
                    HandleGameObject(emberseerOut, true);
                break;
            default:
                break;
        }
    }

    void Update(uint32 diff)
    {
        if(runesTimer <= diff && GetData(DATA_RUNE_DOOR) == NOT_STARTED)
        {
            bool runesUsed = false;
            for(std::set<uint64>::iterator i = runesDoorGUID.begin(); i != runesDoorGUID.end(); ++i)
            {
                if (GameObject* rune = instance->GetGameObject(*i))
                {
                    if (!(runesUsed = !(rune->GetGoState() == GO_STATE_ACTIVE)))
                        break;
                }
            }

            if(runesUsed) 
            {
                for(std::set<uint64>::iterator i = runesDoorGUID.begin(); i != runesDoorGUID.end(); ++i)
                {
                    HandleGameObject(instance->GetGameObject(*i)->GetGUID(), true);
                }
                SetData(DATA_RUNE_DOOR, DONE);
            }
            runesTimer = 3000;
        }
        else runesTimer -= diff;
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_RUNE_DOOR:
                return Encounters[0];
            case DATA_EMBERSEER:
                return Encounters[1];
        }

        return 0;
    }

    void SetData (uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_RUNE_DOOR:
                if(Encounters[0] != DONE)
                    Encounters[0] = data;
                break;
            case DATA_EMBERSEER:
                if(Encounters[1] != DONE)
                    Encounters[1] = data;
                break;
        }

        if (data == DONE)
            SaveToDB();
    }

    
    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream stream;
        stream << Encounters[0] << " ";
        stream << Encounters[1] << " ";
        stream << Encounters[2];

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
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2];

        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounters[i] == IN_PROGRESS)
                Encounters[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;

    }

};



InstanceData* GetInstanceData_instance_blackrock_spire(Map* map)
{
    return new instance_blackrock_spire(map);
}

void AddSC_instance_blackrock_spire()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_blackrock_spire";
    newscript->GetInstanceData = &GetInstanceData_instance_blackrock_spire;
    newscript->RegisterSelf();
}

