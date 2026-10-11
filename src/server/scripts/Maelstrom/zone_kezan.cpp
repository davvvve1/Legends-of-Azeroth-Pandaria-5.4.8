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
    enum FourthAndGoal
    {
        QUEST_FOURTH_AND_GOAL_HORDE     = 24503,
        QUEST_FOURTH_AND_GOAL_GOBLIN    = 28414,

        NPC_BILGEWATER_BUCCANEER        = 37213,
        NPC_FOURTH_AND_GOAL_TARGET      = 37203,

        SPELL_SUMMON_BUCCANEER          = 70075,
        SPELL_GOAL_DETECTION            = 70065,
        SPELL_GROUND_RUMBLE             = 78607,
        SPELL_SUMMON_DEATHWING          = 66322
    };

    bool IsFourthAndGoalActive(Player const* player)
    {
        return player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_HORDE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_GOBLIN) == QUEST_STATUS_INCOMPLETE;
    }
}

class quest_kezan_fourth_and_goal : public QuestScript
{
public:
    quest_kezan_fourth_and_goal() : QuestScript("quest_kezan_fourth_and_goal") { }

    void OnQuestStatusChange(Player* player, Quest const* quest, QuestStatus /*oldStatus*/, QuestStatus newStatus) override
    {
        if (quest->GetQuestId() != Kezan::QUEST_FOURTH_AND_GOAL_HORDE &&
            quest->GetQuestId() != Kezan::QUEST_FOURTH_AND_GOAL_GOBLIN)
            return;

        if (newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->ExitVehicle();
            player->CastSpell(player, Kezan::SPELL_SUMMON_BUCCANEER, true);
        }
        else if (newStatus == QUEST_STATUS_COMPLETE)
        {
            player->CastSpell(player, Kezan::SPELL_GROUND_RUMBLE, true);
            player->CastSpell(player, Kezan::SPELL_SUMMON_DEATHWING, true);
        }
        else if (newStatus == QUEST_STATUS_REWARDED)
            player->CastSpell(player, Kezan::SPELL_GROUND_RUMBLE, true);
    }
};

class npc_kezan_coach_crosscheck : public CreatureScript
{
public:
    npc_kezan_coach_crosscheck() : CreatureScript("npc_kezan_coach_crosscheck") { }

    bool OnGossipHello(Player* player, Creature* /*creature*/) override
    {
        if (Kezan::IsFourthAndGoalActive(player) && !player->GetVehicle())
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

        me->SetSpeed(MOVE_RUN, 0.001f);
        player->EnterVehicle(me, 0);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger->ToPlayer();
        if (!player)
            return;

        if (apply)
            me->CastSpell(me, Kezan::SPELL_GOAL_DETECTION, true);
        else
        {
            me->RemoveAurasDueToSpell(Kezan::SPELL_GOAL_DETECTION);
            me->DespawnOrUnsummon(1000);
        }
    }
};

// 70052 - Kick Footbomb
class spell_kezan_fourth_and_goal_kick : public SpellScript
{
    PrepareSpellScript(spell_kezan_fourth_and_goal_kick);

    void HandleBeforeCast()
    {
        Unit* caster = GetCaster();
        Player* player = caster->ToPlayer();

        if (!player && caster->GetVehicleKit())
            if (Unit* passenger = caster->GetVehicleKit()->GetPassenger(0))
                player = passenger->ToPlayer();

        if (!player || !Kezan::IsFourthAndGoalActive(player))
            return;

        WorldLocation const* destination = GetExplTargetDest();
        if (!destination || destination->GetMapId() != 648 ||
            destination->GetPositionY() < 1450.0f || destination->GetPositionY() > 1525.0f ||
            destination->GetPositionZ() < 100.0f || destination->GetPositionZ() > 300.0f)
            return;

        player->KilledMonsterCredit(Kezan::NPC_FOURTH_AND_GOAL_TARGET);
    }

    void Register() override
    {
        // The explicit destination still belongs to the spell during
        // BeforeCast.  It is not guaranteed to survive until AfterCast.
        BeforeCast += SpellCastFn(spell_kezan_fourth_and_goal_kick::HandleBeforeCast);
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
    new npc_kezan_coach_crosscheck();
    new creature_script<npc_kezan_fourth_and_goal_buccaneer>("npc_kezan_fourth_and_goal_buccaneer");
    new spell_script<spell_kezan_fourth_and_goal_kick>("spell_kezan_fourth_and_goal_kick");
}
