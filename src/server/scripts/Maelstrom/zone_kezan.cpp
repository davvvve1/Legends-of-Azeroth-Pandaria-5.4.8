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

#include "ScriptPCH.h"
#include "Vehicle.h"

namespace Kezan
{
    enum GreatBankHeist
    {
        QUEST_GREAT_BANK_HEIST          = 14122,
        OBJECTIVE_BANK_VAULT            = 266678,
        ITEM_PERSONAL_RICHES            = 46858
    };

    enum FourthAndGoal
    {
        QUEST_NECESSARY_ROUGHNESS       = 24502,
        QUEST_FOURTH_AND_GOAL_HORDE     = 24503,
        QUEST_FOURTH_AND_GOAL_GOBLIN    = 28414,

        NPC_NECESSARY_ROUGHNESS_CREDIT  = 48271,
        NPC_NECESSARY_ROUGHNESS_VEHICLE = 37179,
        NPC_BILGEWATER_BUCCANEER        = 37213,
        NPC_FOURTH_AND_GOAL_TARGET      = 37203,

        SPELL_SUMMON_ROUGHNESS_VEHICLE  = 70015,
        SPELL_SUMMON_BUCCANEER          = 70075,
        SPELL_CONTROL_BUCCANEER         = 70065
    };

    bool IsFourthAndGoalActive(Player const* player)
    {
        return player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_HORDE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_GOBLIN) == QUEST_STATUS_INCOMPLETE;
    }

    bool IsNecessaryRoughnessActive(Player const* player)
    {
        return player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE;
    }

    void RestorePersonalRiches(Player* player)
    {
        QuestStatus const status = player->GetQuestStatus(
            QUEST_GREAT_BANK_HEIST);
        if ((status != QUEST_STATUS_INCOMPLETE &&
                status != QUEST_STATUS_COMPLETE) ||
            player->GetQuestObjectiveCounter(OBJECTIVE_BANK_VAULT) < 1 ||
            player->HasItemCount(ITEM_PERSONAL_RICHES))
            return;

        // The bank-vault SmartAI grants its kill credit before the delayed
        // item action. If that sequence is interrupted, the vault objective
        // remains 1/1 while Personal Riches is 0/1, so the quest can never
        // transition from incomplete to complete.
        player->AddItem(ITEM_PERSONAL_RICHES, 1);
    }

    Creature* FindAvailableBuccaneer(Player* player, uint32 entry)
    {
        std::list<Creature*> vehicles;
        GetCreatureListWithEntryInGrid(vehicles, player, entry, 50.0f);

        Creature* nearest = nullptr;
        float nearestDistance = 50.0f;
        for (Creature* candidate : vehicles)
        {
            Vehicle* vehicle = candidate->GetVehicleKit();
            if (!vehicle || vehicle->GetPassenger(0))
                continue;

            float distance = player->GetDistance(candidate);
            if (distance < nearestDistance)
            {
                nearest = candidate;
                nearestDistance = distance;
            }
        }

        return nearest;
    }

    bool BoardNecessaryRoughnessBuccaneer(Player* player)
    {
        if (!IsNecessaryRoughnessActive(player))
            return false;

        if (Unit* base = player->GetVehicleBase())
        {
            if (base->GetEntry() != NPC_NECESSARY_ROUGHNESS_VEHICLE)
                player->ExitVehicle();
            else
            {
                player->KilledMonsterCredit(NPC_NECESSARY_ROUGHNESS_CREDIT);
                player->VehicleSpellInitialize();
                return true;
            }
        }

        Creature* buccaneer = FindAvailableBuccaneer(player, NPC_NECESSARY_ROUGHNESS_VEHICLE);
        if (!buccaneer)
        {
            player->CastSpell(player, SPELL_SUMMON_ROUGHNESS_VEHICLE, true);
            buccaneer = FindAvailableBuccaneer(player, NPC_NECESSARY_ROUGHNESS_VEHICLE);
        }

        if (!buccaneer)
            return false;

        // The old SmartAI roots summoned copies even though vehicle 582 is a
        // player-controlled shredder.  Clear that stale state before boarding.
        buccaneer->SetControlled(false, UNIT_STATE_ROOT);
        player->EnterVehicle(buccaneer, 0);
        if (!player->GetVehicleBase() || player->GetVehicleBase()->GetEntry() != NPC_NECESSARY_ROUGHNESS_VEHICLE)
            return false;

        player->KilledMonsterCredit(NPC_NECESSARY_ROUGHNESS_CREDIT);
        player->VehicleSpellInitialize();
        return true;
    }

    Player* GetBuccaneerRider(Unit* caster, uint32 vehicleEntry)
    {
        if (!caster)
            return nullptr;

        if (Player* player = caster->ToPlayer())
            if (Unit* base = player->GetVehicleBase())
                return base->GetEntry() == vehicleEntry ? player : nullptr;

        if (caster->GetEntry() != vehicleEntry)
            return nullptr;

        if (Vehicle* vehicle = caster->GetVehicleKit())
            if (Unit* passenger = vehicle->GetPassenger(0))
                return passenger->ToPlayer();

        return nullptr;
    }
}

class quest_kezan_fourth_and_goal : public QuestScript
{
public:
    quest_kezan_fourth_and_goal() : QuestScript("quest_kezan_fourth_and_goal") { }

    void OnQuestStatusChange(Player* player, Quest const* quest, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (quest->GetQuestId() == Kezan::QUEST_NECESSARY_ROUGHNESS)
        {
            if (newStatus == QUEST_STATUS_INCOMPLETE)
                Kezan::BoardNecessaryRoughnessBuccaneer(player);
            return;
        }

        if (quest->GetQuestId() != Kezan::QUEST_FOURTH_AND_GOAL_HORDE &&
            quest->GetQuestId() != Kezan::QUEST_FOURTH_AND_GOAL_GOBLIN)
            return;

        if (newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->ExitVehicle();
            player->CastSpell(player, Kezan::SPELL_SUMMON_BUCCANEER, true);
        }
    }
};

class quest_kezan_great_bank_heist : public QuestScript
{
public:
    quest_kezan_great_bank_heist() : QuestScript("quest_kezan_great_bank_heist") { }

    void OnQuestStatusChange(Player* player, Quest const* quest, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (quest->GetQuestId() == Kezan::QUEST_GREAT_BANK_HEIST &&
            (newStatus == QUEST_STATUS_INCOMPLETE ||
                newStatus == QUEST_STATUS_COMPLETE))
            Kezan::RestorePersonalRiches(player);
    }

    void OnQuestObjectiveChange(Player* player, Quest const* quest,
        QuestObjective const* objective, int32 /*oldAmount*/,
        int32 newAmount) override
    {
        if (quest->GetQuestId() == Kezan::QUEST_GREAT_BANK_HEIST &&
            objective && objective->ID == Kezan::OBJECTIVE_BANK_VAULT &&
            newAmount >= 1)
            Kezan::RestorePersonalRiches(player);
    }
};

class player_kezan_great_bank_heist_recovery : public PlayerScript
{
public:
    player_kezan_great_bank_heist_recovery() : PlayerScript("player_kezan_great_bank_heist_recovery") { }

    void OnLogin(Player* player) override
    {
        Kezan::RestorePersonalRiches(player);
    }
};

class npc_kezan_coach_crosscheck : public CreatureScript
{
public:
    npc_kezan_coach_crosscheck() : CreatureScript("npc_kezan_coach_crosscheck") { }

    bool OnGossipHello(Player* player, Creature* /*creature*/) override
    {
        if (Kezan::IsNecessaryRoughnessActive(player))
            Kezan::BoardNecessaryRoughnessBuccaneer(player);
        else if (Kezan::IsFourthAndGoalActive(player) && !player->GetVehicle())
            player->CastSpell(player, Kezan::SPELL_SUMMON_BUCCANEER, true);

        return false;
    }
};

struct npc_kezan_fourth_and_goal_buccaneer : public ScriptedAI
{
    npc_kezan_fourth_and_goal_buccaneer(Creature* creature) : ScriptedAI(creature) { }

    void OnCharmed(bool /*apply*/) override { }

    void IsSummonedBy(Unit* summoner) override
    {
        Player* player = summoner->ToPlayer();
        if (!player || !Kezan::IsFourthAndGoalActive(player))
            return;

        // 70065 carries SPELL_AURA_CONTROL_VEHICLE and is conditioned to
        // select entry 37213.  It must be cast by the player; casting it from
        // the vehicle creates a self-control chain and breaks client movement.
        player->CastSpell(player, Kezan::SPELL_CONTROL_BUCCANEER, true);

        // Keep a defensive fallback for databases where the implicit-target
        // condition was not imported.
        if (!player->GetVehicleBase())
            player->EnterVehicle(me, 0);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger->ToPlayer();
        if (!player)
            return;

        if (apply)
            player->VehicleSpellInitialize();
        else
        {
            me->DespawnOrUnsummon(1000);
        }
    }
};

// 70052 - Kick Footbomb
class spell_kezan_fourth_and_goal_kick : public SpellScript
{
    PrepareSpellScript(spell_kezan_fourth_and_goal_kick);

    void HandleAfterCast()
    {
        Player* player = Kezan::GetBuccaneerRider(GetCaster(), Kezan::NPC_BILGEWATER_BUCCANEER);
        if (!player || !Kezan::IsFourthAndGoalActive(player))
            return;

        player->KilledMonsterCredit(Kezan::NPC_FOURTH_AND_GOAL_TARGET);
    }

    void Register() override
    {
        // A successful kick from the quest vehicle must always advance the
        // objective.  Client trajectory destinations are not reliable in 5.4.8.
        AfterCast += SpellCastFn(spell_kezan_fourth_and_goal_kick::HandleAfterCast);
    }
};

struct npc_sister_goldskimmer : public ScriptedAI
{
    npc_sister_goldskimmer(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && me->IsFriendlyTo(who) && me->isInFrontInMap(who, 7) && !who->HasAura(74973))
        {
            Talk(0, who);
            me->CastSpell(who, 74973, true);
        }
    }
};

void AddSC_kezan()
{
    new creature_script<npc_sister_goldskimmer>("npc_sister_goldskimmer");
    new quest_kezan_fourth_and_goal();
    new quest_kezan_great_bank_heist();
    new player_kezan_great_bank_heist_recovery();
    new npc_kezan_coach_crosscheck();
    new creature_script<npc_kezan_fourth_and_goal_buccaneer>("npc_kezan_fourth_and_goal_buccaneer");
    new spell_script<spell_kezan_fourth_and_goal_kick>("spell_kezan_fourth_and_goal_kick");
}
