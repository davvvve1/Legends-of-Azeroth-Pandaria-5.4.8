/* Playerbot strategy for Halls of Origination (normal and heroic). */
#include "HallsOfOriginationStrategy.h"

#include "Creature.h"
#include "GameObject.h"
#include "Playerbots.h"

namespace HallsOfOriginationBot
{
namespace
{
Creature* FindCreature(Player* bot, uint32 entry, float range)
{
    Creature* creature = bot->FindNearestCreature(entry, range, true);
    return creature && creature->IsAlive() ? creature : nullptr;
}

GameObject* FindUsableBeacon(Player* bot)
{
    GameObject* left = bot->FindNearestGameObject(GO_BEACON_LEFT, 120.0f);
    GameObject* right = bot->FindNearestGameObject(GO_BEACON_RIGHT, 120.0f);
    if (left && left->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND))
        left = nullptr;
    if (right && right->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND))
        right = nullptr;
    if (!left)
        return right;
    if (!right)
        return left;
    return bot->GetExactDist2d(left) <= bot->GetExactDist2d(right) ? left : right;
}

Creature* FindShieldedAnhuur(Player* bot)
{
    Creature* boss = FindCreature(bot, NPC_ANHUUR, 120.0f);
    return boss && boss->IsInCombat() && boss->HasAura(SPELL_SHIELD_OF_LIGHT) ? boss : nullptr;
}

Unit* SelectPriorityTarget(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_HALLS_OF_ORIGINATION || !bot->IsInCombat() || !botAI->IsDps(bot))
        return nullptr;

    uint32 const priority[] =
    {
        NPC_CHAOS_PORTAL,
        NPC_SEEDLING_POD,
        NPC_SEEDLING_POD_2,
        NPC_SEEDLING_POD_3,
        NPC_ASTRAL_RAIN,
        NPC_CELESTIAL_CALL,
        NPC_VEIL_OF_SKY,
        NPC_VOID_SENTINEL,
        NPC_VOID_SEEKER,
        NPC_VOID_WURM,
        NPC_DUSTBONE_HORROR,
        NPC_JEWELED_SCARAB
    };

    GuidVector const& targets = botAI->GetAiObjectContext()->GetValue<GuidVector>("possible targets")->Get();
    for (uint32 entry : priority)
        for (ObjectGuid const& guid : targets)
            if (Unit* unit = botAI->GetUnit(guid))
                if (unit->GetEntry() == entry && unit->IsAlive() && bot->IsValidAttackTarget(unit) &&
                    bot->GetExactDist2d(unit) < 70.0f)
                    return unit;
    return nullptr;
}

Creature* FindNearbyHazard(Player* bot)
{
    struct Hazard { uint32 entry; float radius; };
    Hazard const hazards[] =
    {
        { NPC_QUICKSAND, 8.0f },
        { NPC_ALPHA_BEAM, 7.0f },
        { NPC_OMEGA_STANCE, 9.0f },
        { NPC_SPORE, 7.0f },
        { NPC_SOLAR_WINDS_1, 8.0f },
        { NPC_SOLAR_WINDS_2, 8.0f },
        { NPC_SOLAR_WINDS_3, 8.0f }
    };
    for (Hazard const& hazard : hazards)
        if (Creature* creature = FindCreature(bot, hazard.entry, hazard.radius))
            return creature;
    return nullptr;
}
}

void HallsOfOriginationStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("hoo use anhuur beacon",
        NextAction::array(0, new NextAction("hoo use anhuur beacon", ACTION_MOVE + 20), nullptr)));
    triggers.push_back(new TriggerNode("hoo avoid hazard",
        NextAction::array(0, new NextAction("hoo avoid hazard", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("hoo priority target",
        NextAction::array(0, new NextAction("hoo attack priority target", ACTION_RAID + 5), nullptr)));
}

bool AnhuurBeaconTrigger::IsActive()
{
    return bot->GetMapId() == MAP_HALLS_OF_ORIGINATION && FindShieldedAnhuur(bot) && FindUsableBeacon(bot);
}

bool PriorityTargetTrigger::IsActive()
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && target != AI_VALUE(Unit*, "current target");
}

bool HazardTrigger::IsActive()
{
    return bot->GetMapId() == MAP_HALLS_OF_ORIGINATION && bot->IsInCombat() && !FindShieldedAnhuur(bot) &&
        FindNearbyHazard(bot);
}

bool UseAnhuurBeaconAction::Execute(Event /*event*/)
{
    if (!FindShieldedAnhuur(bot))
        return false;
    GameObject* beacon = FindUsableBeacon(bot);
    if (!beacon)
        return false;
    if (bot->GetExactDist2d(beacon) > 1.5f)
    {
        bot->AttackStop();
        return MoveTo(bot->GetMapId(), beacon->GetPositionX(), beacon->GetPositionY(), beacon->GetPositionZ(),
            false, false, false, false, MovementPriority::MOVEMENT_FORCED, true, false);
    }
    bot->StopMoving();
    bot->SetFacingToObject(beacon);
    beacon->Use(bot);
    return true;
}

bool AttackPriorityTargetAction::Execute(Event /*event*/)
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && Attack(target);
}

bool AvoidHazardAction::Execute(Event /*event*/)
{
    Creature* hazard = FindNearbyHazard(bot);
    return hazard && MoveAway(hazard, 12.0f);
}

HallsOfOriginationTriggerContext::HallsOfOriginationTriggerContext()
{
    creators["hoo use anhuur beacon"] = [](PlayerbotAI* ai) -> Trigger* { return new AnhuurBeaconTrigger(ai); };
    creators["hoo priority target"] = [](PlayerbotAI* ai) -> Trigger* { return new PriorityTargetTrigger(ai); };
    creators["hoo avoid hazard"] = [](PlayerbotAI* ai) -> Trigger* { return new HazardTrigger(ai); };
}

HallsOfOriginationActionContext::HallsOfOriginationActionContext()
{
    creators["hoo use anhuur beacon"] = [](PlayerbotAI* ai) -> Action* { return new UseAnhuurBeaconAction(ai); };
    creators["hoo attack priority target"] = [](PlayerbotAI* ai) -> Action* { return new AttackPriorityTargetAction(ai); };
    creators["hoo avoid hazard"] = [](PlayerbotAI* ai) -> Action* { return new AvoidHazardAction(ai); };
}
}
