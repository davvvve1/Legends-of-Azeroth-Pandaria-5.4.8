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

#include "lost_city_of_the_tolvir.h"
#include "Pet.h"
#include "ScriptPCH.h"

#define MAX_ENCOUNTER 5

enum eScriptText
{
    YELL_FREE                    = 4,
};

namespace
{
constexpr float SiamatPlatformMinX = -11004.5f;
constexpr float SiamatPlatformMaxX = -10897.4f;
constexpr float SiamatPlatformMinY = -1449.6f;
constexpr float SiamatPlatformMaxY = -1343.1f;
constexpr float SiamatPlatformActivationZ = 27.0f;
constexpr float SiamatPlatformFallZ = 30.0f;
constexpr float SiamatPlatformRescueZ = 36.0f;

bool IsInsideSiamatPlatform(WorldObject const* object)
{
    return object && object->GetPositionX() >= SiamatPlatformMinX &&
        object->GetPositionX() <= SiamatPlatformMaxX &&
        object->GetPositionY() >= SiamatPlatformMinY &&
        object->GetPositionY() <= SiamatPlatformMaxY;
}

bool RescueFromSiamatPlatform(Unit* unit)
{
    if (!unit || !unit->IsAlive() || !IsInsideSiamatPlatform(unit) ||
        unit->GetPositionZ() >= SiamatPlatformFallZ)
        return false;

    unit->GetMotionMaster()->Clear();
    unit->StopMoving();
    unit->NearTeleportTo(unit->GetPositionX(), unit->GetPositionY(),
        SiamatPlatformRescueZ, unit->GetOrientation());
    return true;
}
}

class instance_lost_city_of_the_tolvir : public InstanceMapScript
{
    public:
        instance_lost_city_of_the_tolvir() : InstanceMapScript("instance_lost_city_of_the_tolvir", 755) { }

        struct instance_lost_city_of_the_tolvir_InstanceMapScript : public InstanceScript
        {
            instance_lost_city_of_the_tolvir_InstanceMapScript(InstanceMap* map) : InstanceScript(map) 
            {
                SetBossNumber(MAX_ENCOUNTER);

                memset(&uiTunnelGUID, 0, sizeof(uiTunnelGUID));
                uiTunnelFlag = 0;
                uiHusamGUID = ObjectGuid::Empty;
                uiLockmawGUID = ObjectGuid::Empty;
                uiAughGUID = ObjectGuid::Empty;
                uiBarimGUID = ObjectGuid::Empty;
                uiBlazeGUID = ObjectGuid::Empty;
                uiSiamatGUID = ObjectGuid::Empty;
                uiHarbingerGUID = ObjectGuid::Empty;
                uiSiamatPlatformGUID = ObjectGuid::Empty;
                uiUpdateTimer = 7000;
                uiPlatformSafetyTimer = 100;
                BosesIsDone = false;
                archaeologyQuestAura = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (archaeologyQuestAura)
                    if (!player->HasAura(archaeologyQuestAura))
                        player->CastSpell(player, archaeologyQuestAura, true);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                bool siamatAvailable = (GetBossState(DATA_GENERAL_HUSAM)==DONE) && (GetBossState(DATA_LOCKMAW)==DONE) && (GetBossState(DATA_HIGH_PROPHET_BARIM)==DONE);

                if (IsInsideSiamatPlatform(creature) &&
                    creature->GetPositionZ() > SiamatPlatformActivationZ)
                    platformCreatureGUIDs.insert(creature->GetGUID());
            
                switch (creature->GetEntry())
                {
                    case BOSS_GENERAL_HUSAM:
                        uiHusamGUID = creature->GetGUID();
                        break;
                    case BOSS_LOCKMAW:
                        uiLockmawGUID = creature->GetGUID();
                        break;
                    case BOSS_AUGH:
                        uiAughGUID = creature->GetGUID();
                        break;
                    case BOSS_HIGH_PROPHET_BARIM:
                        uiBarimGUID = creature->GetGUID();
                        break;
                    case BOSS_SIAMAT:
                        uiSiamatGUID = creature->GetGUID();
                        // Siamat always belongs on the upper platform, even if
                        // a bad height was already selected before this hook.
                        platformCreatureGUIDs.insert(creature->GetGUID());
                        if (siamatAvailable)
                            BosesIsDone = true;
                        break;
                    case NPC_WIND_TUNNEL:
                        {
                            creature->SetVisible(false);
                            creature->SetCanFly(true);
                            uiTunnelGUID[uiTunnelFlag] = creature->GetGUID();
                            ++uiTunnelFlag;

                            if (uiTunnelFlag >= 6)
                                uiTunnelFlag = 0;
                        }
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                if (go->GetEntry() == SIAMAT_PLATFORM)
                {
                    go->setActive(true);
                    uiSiamatPlatformGUID = go->GetGUID();

                    if (GetBossState(DATA_GENERAL_HUSAM) == DONE &&
                        GetBossState(DATA_LOCKMAW) == DONE &&
                        GetBossState(DATA_HIGH_PROPHET_BARIM) == DONE)
                        OpenSiamatPlatform(go);
                }
            }

            void OpenSiamatPlatform(GameObject* platform)
            {
                platform->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                platform->EnableCollision(true);
            }

            void SiamatFree()
            {
                if (GameObject* platform = instance->GetGameObject(uiSiamatPlatformGUID))
                    OpenSiamatPlatform(platform);

                for (int i = 0; i < 6; ++i)
                    if (Creature* tunnel = instance->GetCreature(uiTunnelGUID[i]))
                        tunnel->SetVisible(true);
            }

            void Update(uint32 diff) override
            {
                if (uiPlatformSafetyTimer <= diff)
                {
                    uiPlatformSafetyTimer = 100;

                    for (auto const& reference : instance->GetPlayers())
                        if (Player* player = reference.GetSource())
                        {
                            if (IsInsideSiamatPlatform(player) &&
                                player->GetPositionZ() > SiamatPlatformActivationZ)
                                platformPlayerGUIDs.insert(player->GetGUID());

                            if (platformPlayerGUIDs.count(player->GetGUID()))
                            {
                                RescueFromSiamatPlatform(player);
                                if (Pet* pet = player->GetPet())
                                    if (RescueFromSiamatPlatform(pet) &&
                                        !pet->GetVictim())
                                        pet->GetMotionMaster()->MoveFollow(player,
                                            PET_FOLLOW_DIST,
                                            pet->GetFollowAngle());
                            }
                        }

                    for (auto itr = platformCreatureGUIDs.begin();
                        itr != platformCreatureGUIDs.end();)
                    {
                        if (Creature* creature = instance->GetCreature(*itr))
                        {
                            RescueFromSiamatPlatform(creature);
                            ++itr;
                        }
                        else
                            itr = platformCreatureGUIDs.erase(itr);
                    }
                }
                else
                    uiPlatformSafetyTimer -= diff;

                if (BosesIsDone)
                {
                    if (uiUpdateTimer <= diff)
                    {
                        BosesIsDone = false;
                        SiamatFree();

                        if (Creature* siamat = instance->GetCreature(uiSiamatGUID))
                            siamat->AI()->Talk(YELL_FREE);
                    }
                    else
                        uiUpdateTimer -= diff;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == uint32(-1))
                {
                    archaeologyQuestAura = data;
                    SaveToDB();
                    return;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GENERAL_HUSAM:
                        return uiSiamatGUID;
                    case DATA_LOCKMAW:
                        return uiLockmawGUID;
                    case DATA_AUGH:
                        return uiAughGUID;
                    case DATA_HIGH_PROPHET_BARIM:
                        return uiBarimGUID;
                    case DATA_BLAZE:
                        return uiBlazeGUID;
                    case DATA_HARBINGER:
                        return uiHarbingerGUID;
                    case DATA_SIAMAT:
                        return uiSiamatGUID;
                }
                return ObjectGuid::Empty;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_HARBINGER:
                        uiHarbingerGUID = data;
                        break;
                    case DATA_BLAZE:
                        uiBlazeGUID = data;
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                bool siamatAvailable = (GetBossState(DATA_GENERAL_HUSAM)==DONE) && (GetBossState(DATA_LOCKMAW)==DONE) && (GetBossState(DATA_HIGH_PROPHET_BARIM)==DONE);

                switch (type)
                {
                    case DATA_GENERAL_HUSAM:
                    case DATA_LOCKMAW:
                    case DATA_HIGH_PROPHET_BARIM:
                        if (state == DONE && siamatAvailable)
                            BosesIsDone = true;
                        break;
                }

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "L S " << GetBossSaveData() << archaeologyQuestAura;

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

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'L' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                    loadStream >> archaeologyQuestAura;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
            ObjectGuid uiTunnelGUID[6];
            ObjectGuid uiHusamGUID;
            ObjectGuid uiLockmawGUID;
            ObjectGuid uiAughGUID;
            ObjectGuid uiBarimGUID;
            ObjectGuid uiBlazeGUID;
            ObjectGuid uiHarbingerGUID;
            ObjectGuid uiSiamatGUID;
            ObjectGuid uiSiamatPlatformGUID;
            uint32 uiUpdateTimer;
            uint32 uiPlatformSafetyTimer;
            uint32 archaeologyQuestAura;
            uint8 uiTunnelFlag;
            bool BosesIsDone;
            std::set<ObjectGuid> platformPlayerGUIDs;
            std::set<ObjectGuid> platformCreatureGUIDs;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_lost_city_of_the_tolvir_InstanceMapScript(map);
        }
};

void AddSC_instance_lost_city_of_the_tolvir()
{
    new instance_lost_city_of_the_tolvir();
}
