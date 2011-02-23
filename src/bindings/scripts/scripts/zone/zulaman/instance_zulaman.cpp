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
SDName: instance_zulaman
SD%Complete: 80
SDComment:
SDCategory: Zul'Aman
EndScriptData */

#include "precompiled.h"
#include "def_zulaman.h"

#define ENCOUNTERS     8
#define RAND_VENDOR    2

//187021 //Harkor's Satchel
//186648 //Tanzar's Trunk
//186672 //Ashli's Bag
//186667 //Kraz's Package
// Chests spawn at bear/eagle/dragonhawk/lynx bosses
// The loots depend on how many bosses have been killed, but not the entries of the chests
// But we cannot add loots to gameobject, so we have to use the fixed loot_template
struct SHostageInfo
{
    uint32 npc, go;
    float x, y, z, o;
};

static SHostageInfo HostageInfo[] =
{
    {23790, 186648, -57, 1343, 40.77, 3.2}, // bear
    {23999, 187021, 400, 1414, 74.36, 3.3}, // eagle
    {24001, 186672, -35, 1134, 18.71, 1.9}, // dragonhawk
    {24024, 186667, 413, 1117,  6.32, 3.1}  // lynx

};

struct TRINITY_DLL_DECL instance_zulaman : public ScriptedInstance
{
    instance_zulaman(Map *map) : ScriptedInstance(map) {Initialize();};

    uint64 HarkorsSatchelGUID;
    uint64 TanzarsTrunkGUID;
    uint64 AshlisBagGUID;
    uint64 KrazsPackageGUID;
    uint64 StrangeGongGUID;
    uint64 MassiveGateGUID;

    uint64 HarrisonGUID;
    uint64 AkilzonGUID;
    uint64 HexLordGateGUID;
    uint64 ZulJinGateGUID;
    uint64 AkilzonDoorGUID;
    uint64 ZulJinDoorGUID;
    uint64 HalazziDoorGUID;

    uint32 QuestTimer;
    uint16 BossKilled;
    uint16 QuestMinute;
    uint16 ChestLooted;
    uint32 AkilzonGauntlet;

    uint32 Encounters[ENCOUNTERS];
    uint32 RandVendor[RAND_VENDOR];

    void Initialize()
    {
        HarkorsSatchelGUID = 0;
        TanzarsTrunkGUID = 0;
        AshlisBagGUID = 0;
        KrazsPackageGUID = 0;

        StrangeGongGUID = 0;
        MassiveGateGUID = 0;
        HarrisonGUID = 0;
        HexLordGateGUID = 0;
        ZulJinGateGUID = 0;
        AkilzonDoorGUID = 0;
        HalazziDoorGUID = 0;
        ZulJinDoorGUID = 0;

        QuestTimer = 0;
        QuestMinute = 0;
        BossKilled = 0;
        ChestLooted = 0;
        AkilzonGauntlet = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            Encounters[i] = NOT_STARTED;
        for(uint8 i = 0; i < RAND_VENDOR; i++)
            RandVendor[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < ENCOUNTERS; i++)
            if(Encounters[i] == IN_PROGRESS) return true;

        return false;
    }

    uint32 GetEncounterForEntry(uint32 entry)
    {
        switch(entry)
        {
            case 23574:
                return DATA_AKILZONEVENT;
            case 23576:
                return DATA_NALORAKKEVENT;
            case 23578:
                return DATA_JANALAIEVENT;
            case 23577:
                return DATA_HALAZZIEVENT;
            case 24239:
                return DATA_HEXLORDEVENT;
            case 23863:
                return DATA_ZULJINEVENT;
            default:
                return 0;
        }
    }

    void OnCreatureCreate(Creature *creature, uint32 creature_entry)
    {
        switch(creature_entry)
        {
        case 24375://harrison jones
            HarrisonGUID = creature->GetGUID();
            break;
        case 23574:
            AkilzonGUID = creature->GetGUID();
            break;
        case 23578://janalai
        case 23863://zuljin
        case 24239://hexlord
        case 23577://halazzi
        case 23576://nalorakk
        default: break;
        }

        const CreatureData *tmp = creature->GetLinkedRespawnCreatureData();
        if (!tmp)
            return;

        if (GetEncounterForEntry(tmp->id) && creature->isAlive() && GetData(GetEncounterForEntry(tmp->id)) == DONE)
        {
            creature->Kill(creature, false);
            creature->RemoveCorpse();
        }
    }

    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
        case 187359: StrangeGongGUID = go->GetGUID(); break;
        case 186728: 
            MassiveGateGUID = go->GetGUID();
            if (Encounters[0] != NOT_STARTED)
                go->SetGoState(GO_STATE_ACTIVE);  //opened
            else
                go->SetGoState(GO_STATE_READY);   //closed
            break;
        case 186303: HalazziDoorGUID = go->GetGUID(); break;
        case 186304: ZulJinGateGUID  = go->GetGUID(); break;
        case 186305: HexLordGateGUID = go->GetGUID(); break;
        case 186858: AkilzonDoorGUID = go->GetGUID(); break;
        case 186859: ZulJinDoorGUID  = go->GetGUID(); break;

        case 187021: HarkorsSatchelGUID  = go->GetGUID(); break;
        case 186648: TanzarsTrunkGUID = go->GetGUID(); break;
        case 186672: AshlisBagGUID = go->GetGUID(); break;
        case 186667: KrazsPackageGUID  = go->GetGUID(); break;
        default: break;

        }
        CheckInstanceStatus();
    }

    void OpenDoor(uint64 DoorGUID, bool open)
    {
        if(GameObject *Door = instance->GetGameObject(DoorGUID))
            Door->SetUInt32Value(GAMEOBJECT_STATE, open ? 0 : 1);
    }

    void SummonHostage(uint8 num)
    {
        if(!QuestMinute)
            return;

        Map::PlayerList const &PlayerList = instance->GetPlayers();
        if (PlayerList.isEmpty())
            return;

        Map::PlayerList::const_iterator i = PlayerList.begin();
        if(Player* i_pl = i->getSource())
        {
            if(Unit* Hostage = i_pl->SummonCreature(HostageInfo[num].npc, HostageInfo[num].x, HostageInfo[num].y, HostageInfo[num].z, HostageInfo[num].o, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                Hostage->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                Hostage->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }
    }

    void CheckInstanceStatus()
    {
        if(BossKilled >= 4)
            OpenDoor(HexLordGateGUID, true);

        if(BossKilled >= 5)
            OpenDoor(ZulJinGateGUID, true);
    }

    void UpdateWorldState(uint32 field, uint32 value)
    {
        WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);
        data << field << value;
        instance->SendToPlayers(&data);
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream stream;
        stream << Encounters[0] << " ";
        stream << Encounters[1] << " ";
        stream << Encounters[2] << " ";
        stream << Encounters[3] << " ";
        stream << Encounters[4] << " ";
        stream << Encounters[5] << " ";
        stream << Encounters[6] << " ";

        stream << BossKilled << " ";
        stream << ChestLooted << " ";
        stream << QuestMinute;

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
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2] >> Encounters[3] >> Encounters[4] >>
                Encounters[5] >> Encounters[6] >> BossKilled >> ChestLooted >> QuestMinute;

        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounters[i] == IN_PROGRESS)
                Encounters[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
        case TYPE_EVENT_RUN:
            if (data == SPECIAL)
            {
                OpenDoor(MassiveGateGUID, true);
                QuestMinute = 21;
                UpdateWorldState(WORLD_STATE_COUNTER, QuestMinute);
                UpdateWorldState(WORLD_STATE_ID,1);
                Encounters[0] = data;
            }
            break;
        case DATA_NALORAKKEVENT:
            if (Encounters[1] != DONE)
                Encounters[1] = data;
            if(data == DONE)
            {
                if(QuestMinute)
                {
                    QuestMinute += 15;
                    UpdateWorldState(3106, QuestMinute);
                }
                SummonHostage(0);
            }
            break;
        case DATA_AKILZONEVENT:
            if (Encounters[2] != DONE)
                Encounters[2] = data;

            OpenDoor(AkilzonDoorGUID, data != IN_PROGRESS);

            if(data == DONE)
            {
                if(QuestMinute)
                {
                    QuestMinute += 10;
                    UpdateWorldState(3106, QuestMinute);
                }
                SummonHostage(1);
            }
            break;
        case DATA_JANALAIEVENT:
            if (Encounters[3] != DONE)
                Encounters[3] = data;

            if(data == DONE)
                SummonHostage(2);
            break;
        case DATA_HALAZZIEVENT:
            if (Encounters[4] != DONE)
                Encounters[4] = data;

            OpenDoor(HalazziDoorGUID, data != IN_PROGRESS);

            if(data == DONE)
                SummonHostage(3);
            break;
        case DATA_HEXLORDEVENT:
            if (Encounters[5] != DONE)
                Encounters[5] = data;

            if(data == IN_PROGRESS)
                OpenDoor(HexLordGateGUID, false);
            else if(data == NOT_STARTED)
                CheckInstanceStatus();
            break;
        case DATA_ZULJINEVENT:
            if (Encounters[6] != DONE)
                Encounters[6] = data;

            OpenDoor(ZulJinDoorGUID, data != IN_PROGRESS);
            break;
        case DATA_CHESTLOOTED:
            ChestLooted++;
            SaveToDB();
            break;
        case TYPE_RAND_VENDOR_1:
            RandVendor[0] = data;
            break;
        case TYPE_RAND_VENDOR_2:
            RandVendor[1] = data;
            break;
        case DATA_AKILZONGAUNTLET:
            AkilzonGauntlet = data;
            break;
        }

        if(data == DONE)
        {
            BossKilled++;
            if(QuestMinute && BossKilled >= 4)
            {
                QuestMinute = 0;
                UpdateWorldState(3104, 0);
            }
            CheckInstanceStatus();
            SaveToDB();
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_EVENT_RUN:     return Encounters[0];
            case DATA_NALORAKKEVENT: return Encounters[1];
            case DATA_AKILZONEVENT:  return Encounters[2];
            case DATA_JANALAIEVENT:  return Encounters[3];
            case DATA_HALAZZIEVENT:  return Encounters[4];
            case DATA_HEXLORDEVENT:  return Encounters[5];
            case DATA_ZULJINEVENT:   return Encounters[6];
            case DATA_CHESTLOOTED:   return ChestLooted;
            case TYPE_RAND_VENDOR_1: return RandVendor[0];
            case TYPE_RAND_VENDOR_2: return RandVendor[1];
            case DATA_AKILZONGAUNTLET:  return AkilzonGauntlet;
            default:                 return 0;
        }
    }

    uint64 GetData64(uint32 data)
    {
        switch(data)
        {
            case DATA_AKILZONEVENT:
                return AkilzonGUID;
                /*
            case DATA_NALORAKKEVENT:
                return NalorakkGUID;
            case DATA_JANALAIEVENT:
                return JanalaiGUID;
            case DATA_HALAZZIEVENT:
                return HalazziGUID;
            case DATA_SPIRIT_LYNX:
                return SpiritLynxGUID;
            case DATA_ZULJINEVENT:
                return ZuljinGUID;
            case DATA_HEXLORDEVENT:
                return HexLordGateGUID;*/
            case DATA_HARRISON:
                return HarrisonGUID;
            case DATA_GO_GONG:
                return StrangeGongGUID;
            case DATA_GO_ENTRANCE:
                return MassiveGateGUID;
            /*case DATA_GO_MALACRASS_GATE:
                return MalacrassEntranceGUID;*/
        }
        return 0;
    }

    void Update(uint32 diff)
    {
        if(QuestMinute)
        {
            if(QuestTimer < diff)
            {
                QuestMinute--;
                SaveToDB();
                QuestTimer += 60000;
                if(QuestMinute)
                {
                    UpdateWorldState(3104, 1);
                    UpdateWorldState(3106, QuestMinute);
                }
                else UpdateWorldState(3104, 0);
            }
            QuestTimer -= diff;
        }
    }
};

InstanceData* GetInstanceData_instance_zulaman(Map* map)
{
    return new instance_zulaman(map);
}

void AddSC_instance_zulaman()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_zulaman";
    newscript->GetInstanceData = &GetInstanceData_instance_zulaman;
    newscript->RegisterSelf();
}

