/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Instance_Molten_Core
SD%Complete: 0
SDComment: Place Holder
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "def_molten_core.h"

#define ENCOUNTERS      10

#define ID_LUCIFRON     12118
#define ID_MAGMADAR     11982
#define ID_GEHENNAS     12259
#define ID_GARR         12057
#define ID_GEDDON       12056
#define ID_SHAZZRAH     12264
#define ID_GOLEMAGG     11988
#define ID_SULFURON     12098
#define ID_DOMO         12018
#define ID_RAGNAROS     11502
#define ID_FLAMEWAKERPRIEST     11662

struct TRINITY_DLL_DECL instance_molten_core : public ScriptedInstance
{
    instance_molten_core(Map *map) : ScriptedInstance(map) {Initialize();};

    uint64 Lucifron, Magmadar, Gehennas, Garr, Geddon, Shazzrah, Sulfuron, Golemagg, Domo, Ragnaros, FlamewakerPriest;
    uint64 RuneKoro, RuneZeth, RuneMazj, RuneTheri, RuneBlaz, RuneKress, RuneMohn;

    uint32 Encounter[ENCOUNTERS];

    void Initialize()
    {
        Lucifron = 0;
        Magmadar = 0;
        Gehennas = 0;
        Garr = 0;
        Geddon = 0;
        Shazzrah = 0;
        Sulfuron = 0;
        Golemagg = 0;
        Domo = 0;
        Ragnaros = 0;
        FlamewakerPriest = 0;

        RuneKoro = 0;
        RuneZeth = 0;
        RuneMazj = 0;
        RuneTheri = 0;
        RuneBlaz = 0;
        RuneKress = 0;
        RuneMohn = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] != DONE && Encounter[i] != NOT_STARTED)
                return true;

        return false;
    };


   void OnObjectCreate(GameObject *go)
    {
         switch(go->GetEntry())
         {
         case 176951:                                    //Sulfuron
             RuneKoro = go->GetGUID();
             break;
         case 176952:                                    //Geddon
             RuneZeth = go->GetGUID();
             break;
         case 176953:                                    //Shazzrah
             RuneMazj = go->GetGUID();
             break;
         case 176954:                                    //Golemagg
             RuneTheri = go->GetGUID();
             break;
         case 176955:                                    //Garr
             RuneBlaz = go->GetGUID();
             break;
         case 176956:                                    //Magmadar
             RuneKress = go->GetGUID();
             break;
         case 176957:                                    //Gehennas
             RuneMohn = go->GetGUID();
             break;
             }
    }

    uint32 GetEncounterForEntry(uint32 entry)
    {
        switch(entry)
        {
            case ID_LUCIFRON:
                return DATA_LUCIFRON_EVENT;
            case ID_MAGMADAR:
                return DATA_MAGMADAR_EVENT;
            case ID_GEHENNAS:
                return DATA_GEHENNAS_EVENT;
            case ID_GARR:
                return DATA_GARR_EVENT;
            case ID_GEDDON:
                return DATA_BARON_GEDDON_EVENT;
            case ID_SHAZZRAH:
                return DATA_SHAZZRAH_EVENT;
            case ID_SULFURON:
                return DATA_SULFURON_HARBRINGER_EVENT;
            case ID_GOLEMAGG:
                return DATA_GOLEMAGG_THE_INCINERATOR_EVENT;
            case ID_DOMO:
                return DATA_MAJORDOMO_EXECUTUS_EVENT;
            case ID_RAGNAROS:
                return DATA_RAGNAROS_EVENT;
            default:
                return 0;
        }
    }

    void OnCreatureCreate(Creature *creature, uint32 creature_entry)
    {
        switch (creature_entry)
        {
            case ID_LUCIFRON:
                Lucifron = creature->GetGUID();
                break;

            case ID_MAGMADAR:
                Magmadar = creature->GetGUID();
                break;

            case ID_GEHENNAS:
                Gehennas = creature->GetGUID();
                break;

            case ID_GARR:
                Garr = creature->GetGUID();
                break;

            case ID_GEDDON:
                Geddon = creature->GetGUID();
                break;

            case ID_SHAZZRAH:
                Shazzrah = creature->GetGUID();
                break;

            case ID_SULFURON:
                Sulfuron = creature->GetGUID();
                break;

            case ID_GOLEMAGG:
                Golemagg = creature->GetGUID();
                break;

            case ID_DOMO:
                Domo = creature->GetGUID();
                break;

            case ID_RAGNAROS:
                Ragnaros = creature->GetGUID();
                break;

            case ID_FLAMEWAKERPRIEST:
                FlamewakerPriest = creature->GetGUID();
                break;
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

    uint64 GetData64 (uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_SULFURON:
                return Sulfuron;
            case DATA_GOLEMAGG:
                return Sulfuron;

            case DATA_FLAMEWAKERPRIEST:
                return FlamewakerPriest;
        }

        return 0;
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_LUCIFRON_EVENT:
                return Encounter[0];
            case DATA_MAGMADAR_EVENT:
                return Encounter[1];
            case DATA_GEHENNAS_EVENT:
                return Encounter[2];
            case DATA_GARR_EVENT:
                return Encounter[3];
            case DATA_SHAZZRAH_EVENT:
                return Encounter[4];
            case DATA_BARON_GEDDON_EVENT:
                return Encounter[5];
            case DATA_GOLEMAGG_THE_INCINERATOR_EVENT:
                return Encounter[6];
            case DATA_SULFURON_HARBRINGER_EVENT:
                return Encounter[7];
            case DATA_MAJORDOMO_EXECUTUS_EVENT:
                return Encounter[8];
            case DATA_RAGNAROS_EVENT:
                return Encounter[9];
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_LUCIFRON_EVENT:
                if(Encounter[0] != DONE)
                    Encounter[0] = data;
                break;
            case DATA_MAGMADAR_EVENT:
                if(Encounter[1] != DONE)
                    Encounter[1] = data;
                break;
            case DATA_GEHENNAS_EVENT:
                if(Encounter[2] != DONE)
                    Encounter[2] = data;
                break;
            case DATA_GARR_EVENT:
                if(Encounter[3] != DONE)
                    Encounter[3] = data;
                break;
            case DATA_SHAZZRAH_EVENT:
                if(Encounter[4] != DONE)
                    Encounter[4] = data;
                break;
            case DATA_BARON_GEDDON_EVENT:
                if(Encounter[5] != DONE)
                    Encounter[5] = data;
                break;
            case DATA_GOLEMAGG_THE_INCINERATOR_EVENT:
                if(Encounter[6] != DONE)
                    Encounter[6] = data;
                break;
            case DATA_SULFURON_HARBRINGER_EVENT:
                if(Encounter[7] != DONE)
                    Encounter[7] = data;
                break;
            case DATA_MAJORDOMO_EXECUTUS_EVENT:
                if(Encounter[8] != DONE)
                    Encounter[8] = data;
                break;
            case DATA_RAGNAROS_EVENT:
                if(Encounter[9] != DONE)
                    Encounter[9] = data;
                break;
        }

        if (data == DONE)
            SaveToDB();
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream stream;
        stream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3] << " " << Encounter[4] << " "
             << Encounter[5] << " " << Encounter[6] << " " << Encounter[7] << " " << Encounter[8] << " " << Encounter[9];
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
        stream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3] >> Encounter[4]
             >> Encounter[5] >> Encounter[6] >> Encounter[7] >> Encounter[8] >> Encounter[9];
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if(Encounter[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                Encounter[i] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstance_instance_molten_core(Map *map)
{
    return new instance_molten_core (map);
}

void AddSC_instance_molten_core()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="instance_molten_core";
    newscript->GetInstanceData = &GetInstance_instance_molten_core;
    newscript->RegisterSelf();
}

