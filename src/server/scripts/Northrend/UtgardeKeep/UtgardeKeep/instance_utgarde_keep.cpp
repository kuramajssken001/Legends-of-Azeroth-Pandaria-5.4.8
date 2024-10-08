/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ScriptData
SDName: Instance_Utgarde_Keep
SD%Complete: 90
SDComment: Instance Data Scripts and functions to acquire mobs and set encounter status for use in various Utgarde Keep Scripts
SDCategory: Utgarde Keep
EndScriptData */

#include "ScriptPCH.h"
#include "utgarde_keep.h"

#define MAX_ENCOUNTER     3

/* Utgarde Keep encounters:
 0 - Prince kelesethGUID
 1 - skarvaldGUID dalronnGUID
 2 - ingvarGUID the Plunderer
*/

enum GameObjects
{
    GO_BELLOW_1              = 186688,
    GO_BELLOW_2              = 186689,
    GO_BELLOW_3              = 186690,

    GO_FORGEFIRE_1           = 186692,
    GO_FORGEFIRE_2           = 186693,
    GO_FORGEFIRE_3           = 186691,

    GO_GLOWING_ANVIL_1       = 186609,
    GO_GLOWING_ANVIL_2       = 186610,
    GO_GLOWING_ANVIL_3       = 186611,
    
    GO_GIANT_PORTCULLIS_1    = 186756,
    GO_GIANT_PORTCULLIS_2    = 186694,
};

class instance_utgarde_keep : public InstanceMapScript
{
    public:
        instance_utgarde_keep() : InstanceMapScript("instance_utgarde_keep", 574) { }

        struct instance_utgarde_keep_InstanceMapScript : public InstanceScript
        {
            instance_utgarde_keep_InstanceMapScript(Map* map) : InstanceScript(map) { }

            ObjectGuid kelesethGUID;
            ObjectGuid skarvaldGUID;
            ObjectGuid dalronnGUID;
            ObjectGuid ingvarGUID;

            ObjectGuid forge_bellow[3];
            ObjectGuid forge_fire[3];
            ObjectGuid forge_anvil[3];
            ObjectGuid portcullis[2];

            uint32 m_auiEncounter[MAX_ENCOUNTER];
            uint32 forge_event[3];
            std::string str_data;

           void Initialize() override
           {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                kelesethGUID = ObjectGuid::Empty;
                skarvaldGUID = ObjectGuid::Empty;
                dalronnGUID = ObjectGuid::Empty;
                ingvarGUID = ObjectGuid::Empty;

                for (uint8 i = 0; i < 3; ++i)
                {
                    forge_bellow[i] = ObjectGuid::Empty;
                    forge_fire[i] = ObjectGuid::Empty;
                    forge_anvil[i] = ObjectGuid::Empty;
                    forge_event[i] = NOT_STARTED;
                }

                portcullis[0] = ObjectGuid::Empty;
                portcullis[1] = ObjectGuid::Empty;
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            Player* GetPlayerInMap()
            {
                Map::PlayerList const& players = instance->GetPlayers();

                if (!players.isEmpty())
                {
                    for (auto&& ref : players)
                    {
                        if (Player* player = ref.GetSource())
                        return player;
                    }
                }

                TC_LOG_DEBUG("scripts", "TSCR: Instance Utgarde Keep: GetPlayerInMap, but PlayerList is empty!");
                return NULL;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case 23953:
                        kelesethGUID = creature->GetGUID();
                        break;
                    case 24201:
                        dalronnGUID = creature->GetGUID();
                        break;
                    case 24200:
                        skarvaldGUID = creature->GetGUID();
                        break;
                    case 23954:
                        ingvarGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_BELLOW_1: 
                        forge_bellow[0] = go->GetGUID();
                        if (forge_event[0] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                            break;
                    case GO_BELLOW_2:
                        forge_bellow[1] = go->GetGUID();
                        if (forge_event[1] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_BELLOW_3: forge_bellow[2] = go->GetGUID();
                        if (forge_event[2] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_FORGEFIRE_1: forge_fire[0] = go->GetGUID();
                        if (forge_event[0] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                        break;
                    case GO_FORGEFIRE_2: forge_fire[1] = go->GetGUID();
                        if (forge_event[1] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_FORGEFIRE_3: forge_fire[2] = go->GetGUID();
                        if (forge_event[2] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GLOWING_ANVIL_1: forge_anvil[0] = go->GetGUID();
                        if (forge_event[0] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GLOWING_ANVIL_2: forge_anvil[1] = go->GetGUID();
                        if (forge_event[1] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GLOWING_ANVIL_3: forge_anvil[2] = go->GetGUID();
                        if (forge_event[2] != NOT_STARTED)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GIANT_PORTCULLIS_1: portcullis[0] = go->GetGUID();
                        if (m_auiEncounter[2] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GIANT_PORTCULLIS_2:
                        portcullis[1] = go->GetGUID();
                        if (m_auiEncounter[2] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_PRINCEKELESETH: return kelesethGUID;
                    case DATA_DALRONN:        return dalronnGUID;
                    case DATA_SKARVALD:       return skarvaldGUID;
                    case DATA_INGVAR:         return ingvarGUID;
                }

                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_PRINCEKELESETH_EVENT:
                        m_auiEncounter[0] = data;
                        break;
                    case DATA_SKARVALD_DALRONN_EVENT:
                        m_auiEncounter[1] = data;
                        break;
                    case DATA_INGVAR_EVENT:
                        if (data == DONE)
                        {
                            HandleGameObject(portcullis[0], true);
                            HandleGameObject(portcullis[1], true);
                        }
                        m_auiEncounter[2] = data;
                        break;
                    case EVENT_FORGE_1:
                        if (data != DONE)
                        {
                            HandleGameObject(forge_bellow[0], false);
                            HandleGameObject(forge_fire[0], false);
                            HandleGameObject(forge_anvil[0], false);
                        }
                        else
                        {
                            HandleGameObject(forge_bellow[0], true);
                            HandleGameObject(forge_fire[0], true);
                            HandleGameObject(forge_anvil[0], true);
                        }
                        forge_event[0] = data;
                        break;
                    case EVENT_FORGE_2:
                        if (data != DONE)
                        {
                            HandleGameObject(forge_bellow[1], false);
                            HandleGameObject(forge_fire[1], false);
                            HandleGameObject(forge_anvil[1], false);
                        }
                        else
                        {
                            HandleGameObject(forge_bellow[1], true);
                            HandleGameObject(forge_fire[1], true);
                            HandleGameObject(forge_anvil[1], true);
                        }
                        forge_event[1] = data;
                        break;
                    case EVENT_FORGE_3:
                        if (data != DONE)
                        {
                            HandleGameObject(forge_bellow[2], false);
                            HandleGameObject(forge_fire[2], false);
                            HandleGameObject(forge_anvil[2], false);
                        }
                        else
                        {
                            HandleGameObject(forge_bellow[2], true);
                            HandleGameObject(forge_fire[2], true);
                            HandleGameObject(forge_anvil[2], true);
                        }
                        forge_event[2] = data;
                        break;
                }

                if (data == DONE)
                {
                    SaveToDB();
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_PRINCEKELESETH_EVENT:   return m_auiEncounter[0];
                    case DATA_SKARVALD_DALRONN_EVENT: return m_auiEncounter[1];
                    case DATA_INGVAR_EVENT:           return m_auiEncounter[2];
                    case EVENT_FORGE_1:               return forge_event[0];
                    case EVENT_FORGE_2:               return forge_event[1];
                    case EVENT_FORGE_3:               return forge_event[2];
                }

                return 0;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "U K " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                    << m_auiEncounter[2] << " " << forge_event[0] << " " << forge_event[1] << " " << forge_event[2];

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;
                uint16 data0, data1, data2, data3, data4, data5;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5;

                if (dataHead1 == 'U' && dataHead2 == 'K')
                {
                    m_auiEncounter[0] = data0;
                    m_auiEncounter[1] = data1;
                    m_auiEncounter[2] = data2;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)
                            m_auiEncounter[i] = NOT_STARTED;

                    forge_event[0] = data3;
                    forge_event[1] = data4;
                    forge_event[2] = data5;

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
           return new instance_utgarde_keep_InstanceMapScript(map);
        }
};


void AddSC_instance_utgarde_keep()
{
    new instance_utgarde_keep();
}
