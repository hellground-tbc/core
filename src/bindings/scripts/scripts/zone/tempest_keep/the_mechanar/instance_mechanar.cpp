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

#define ENCOUNTERS      5

#define GO_MOARGDOOR1           184632
#define GO_MOARGDOOR2           184322
#define GO_NETHERMANCERDOOR     184449

#define NPC_IRONHAND            19710
#define NPC_GYROKILL            19218

struct TRINITY_DLL_DECL instance_mechanar : public ScriptedInstance
{
    instance_mechanar(Map *map) : ScriptedInstance(map) {Initialize();};

    bool Heroic;
    uint32 Encounters[ENCOUNTERS];
    uint64 MoargDoor1;
    uint64 MoargDoor2;
    uint64 NethermancerDoor;
    uint32 CheckTimer;
    bool CleanupCharges;

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

        Heroic = instance->IsHeroic();
        MoargDoor1 = 0;
        MoargDoor2 = 0;
        NethermancerDoor = 0;
        CheckTimer = 3000;
        CleanupCharges = false;
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
        case DATA_NETHERMANCER_EVENT:       return Encounters[0];
        case DATA_IRONHAND_EVENT:           return Encounters[1];
        case DATA_GYROKILL_EVENT:           return Encounters[2];
        case DATA_CACHE_OF_LEGION_EVENT:    return Encounters[3];
        case DATA_MECHANO_LORD_EVENT:       return Encounters[4];
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
            case DATA_CACHE_OF_LEGION_EVENT:
                if(Encounters[3] != DONE)
                    Encounters[3] = data;
                break;
            case DATA_MECHANO_LORD_EVENT:
                if(Encounters[4] != DONE)
                    Encounters[4] = data;
                if(data == DONE)
                    CleanupCharges = true;
                break;
        }
        if(data == DONE)
            SaveToDB();
    }

    void Update(uint32 diff)
    {
        if(Heroic && GetData(DATA_MECHANO_LORD_EVENT) == IN_PROGRESS)
        {
            if(CheckTimer < diff)
            {
                const Map::PlayerList& players = instance->GetPlayers();
                for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                {
                    Player *sourcePlayer = i->getSource();
                    if(sourcePlayer->isGameMaster())
                        continue;

                    sourcePlayer->RemoveAurasDueToSpell(39089);
                    sourcePlayer->RemoveAurasDueToSpell(39092);

                    int chargeid = GET_CHARGE_ID(sourcePlayer);
                    if(!chargeid)
                        continue;
                    int counter = 0;
                    for(Map::PlayerList::const_iterator j = players.begin(); j != players.end(); ++j)
                    {
                        Player *checkPlayer = j->getSource();
                        if(checkPlayer->isGameMaster() || sourcePlayer == checkPlayer)
                            continue;

                        if(chargeid == GET_CHARGE_ID(checkPlayer) && sourcePlayer->IsWithinDist(checkPlayer, 10))
                            counter++;
                    }
                    while(counter--)
                        sourcePlayer->CastSpell(sourcePlayer, chargeid == 1 ? 39089 : 39092, true);
                }

                CheckTimer = 3000;
            } else
                CheckTimer -= diff;
        }
        if(Heroic && CleanupCharges)
        {
            const Map::PlayerList& players = instance->GetPlayers();
            for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                Player *sourcePlayer = i->getSource();
                if(sourcePlayer->isGameMaster())
                    continue;

                sourcePlayer->RemoveAurasDueToSpell(39089);
                sourcePlayer->RemoveAurasDueToSpell(39092);
            }
            CleanupCharges = false;
        }

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

bool GOUse_go_cache_of_the_legion(Player *player, GameObject* _GO)
{
    Map* m = player->GetMap();
    if (!m->IsHeroic())
        return false;

    if (ScriptedInstance* pInstance = (_GO->GetInstanceData()))
    {
        if (pInstance->GetData(DATA_CACHE_OF_LEGION_EVENT) == NOT_STARTED)
        {
            const Map::PlayerList& players = _GO->GetMap()->GetPlayers();

            for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                Player *player = i->getSource();
                if (player->isGameMaster())
                    continue;

                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 29434, 1);
                if (msg == EQUIP_ERR_OK )
                {
                    Item* item = player->StoreNewItem(dest,29434,true);
                    if (item)
                        player->SendNewItem(item,1,false,true);
                    else
                        player->SendEquipError(msg,NULL,NULL);
                }
            }

            ((InstanceMap*)m)->PermBindAllPlayers(player);
            pInstance->SetData(DATA_CACHE_OF_LEGION_EVENT, DONE);
            return false;
        }
    }
    return true;
}

void AddSC_instance_mechanar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_mechanar";
    newscript->GetInstanceData = &GetInstanceData_instance_mechanar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_cache_of_the_legion";
    newscript->pGOUse = &GOUse_go_cache_of_the_legion;
    newscript->RegisterSelf();
}
