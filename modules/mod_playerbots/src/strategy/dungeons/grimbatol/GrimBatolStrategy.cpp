/* Playerbot strategy for Grim Batol (normal and heroic). */
#include "GrimBatolStrategy.h"

#include "Creature.h"
#include "Playerbots.h"
#include "Vehicle.h"

#include <algorithm>
#include <vector>

namespace GrimBatolBot
{
namespace
{
Creature* FindCreature(Player* bot, uint32 entry, float range)
{
    Creature* creature = bot->FindNearestCreature(entry, range, true);
    return creature && creature->IsAlive() ? creature : nullptr;
}

Creature* FindEngagedBoss(Player* bot, uint32 entry)
{
    Creature* boss = FindCreature(bot, entry, 120.0f);
    return boss && boss->IsInCombat() ? boss : nullptr;
}

bool IsAvailableBombingDrake(Creature* drake)
{
    Vehicle* vehicle = drake ? drake->GetVehicleKit() : nullptr;
    return drake && drake->IsAlive() && vehicle && !vehicle->IsVehicleInUse() &&
        !drake->HasAura(79377) &&
        drake->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK) &&
        !drake->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
}

Creature* AssignedBombingDrake(PlayerbotAI* botAI)
{
    Player* bot = botAI ? botAI->GetBot() : nullptr;
    if (!bot || bot->GetMapId() != MAP_GRIM_BATOL || bot->GetVehicleBase())
        return nullptr;

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    Player* master = botAI->GetMaster();
    if (!group || !master)
        return nullptr;

    std::list<Creature*> found;
    bot->GetCreatureListWithEntryInGrid(found, NPC_BATTERED_RED_DRAKE, 100.0f);
    std::vector<Creature*> drakes;
    for (Creature* drake : found)
        if (IsAvailableBombingDrake(drake))
            drakes.push_back(drake);
    std::sort(drakes.begin(), drakes.end(), [](Creature* left, Creature* right)
    {
        return left->GetGUID() < right->GetGUID();
    });

    std::vector<Player*> bots;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        if (Player* member = ref->GetSource())
        {
            PlayerbotAI* memberAI = GET_PLAYERBOT_AI(member);
            if (member != master && memberAI && !memberAI->IsRealPlayer() &&
                member->IsAlive() && member->IsInWorld() &&
                member->GetMap() == bot->GetMap() && !member->GetVehicleBase())
                bots.push_back(member);
        }
    std::sort(bots.begin(), bots.end(), [](Player* left, Player* right)
    {
        return left->GetGUID() < right->GetGUID();
    });

    auto const position = std::find(bots.begin(), bots.end(), bot);
    if (position == bots.end())
        return nullptr;
    size_t const index = std::distance(bots.begin(), position);
    return index < drakes.size() ? drakes[index] : nullptr;
}

bool IsRidingBombingDrake(Player* bot)
{
    Unit* vehicle = bot ? bot->GetVehicleBase() : nullptr;
    return vehicle && vehicle->GetEntry() == NPC_BATTERED_RED_DRAKE;
}

Creature* FindBombingTarget(Player* bot)
{
    Unit* vehicle = bot ? bot->GetVehicleBase() : nullptr;
    if (!vehicle || vehicle->GetEntry() != NPC_BATTERED_RED_DRAKE)
        return nullptr;

    uint32 const trashEntries[] =
    {
        39381, 39405, 39414, 39415, 39450, 39626, 39854, 39870,
        39873, 39890, 39909, 39954, 39956, 39962, 40166, 40167,
        40268, 40270, 40272, 40273, 40290, 40291, 40306, 40448, 41073
    };

    Creature* nearest = nullptr;
    for (uint32 entry : trashEntries)
        if (Creature* target = vehicle->FindNearestCreature(entry, 100.0f, true))
            if (vehicle->IsValidAttackTarget(target) &&
                (!nearest || vehicle->GetExactDist(target) < vehicle->GetExactDist(nearest)))
                nearest = target;
    return nearest;
}

Unit* SelectPriorityTarget(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_GRIM_BATOL || !bot->IsInCombat() || !botAI->IsDps(bot))
        return nullptr;

    uint32 const priority[] =
    {
        NPC_INVOKED_FLAMING_SPIRIT,
        NPC_FACELESS_CORRUPTOR_H,
        NPC_FACELESS_CORRUPTOR,
        NPC_MALIGNANT_TROGG
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
        { NPC_BLITZ_STALKER, 9.0f },
        { NPC_GROUND_SIEGE_STALKER, 8.0f },
        { NPC_CAVE_IN_STALKER, 7.0f },
        { NPC_FIRE_PATCH, 7.0f },
        { NPC_SEEPING_TWILIGHT, 7.0f },
        { NPC_DEVOURING_FLAMES, 9.0f }
    };

    for (Hazard const& hazard : hazards)
        if (Creature* creature = FindCreature(bot, hazard.entry, hazard.radius))
            return creature;
    return nullptr;
}

Creature* FindActiveShadowGale(Player* bot)
{
    if (!FindEngagedBoss(bot, NPC_ERUDAX))
        return nullptr;
    return FindCreature(bot, NPC_SHADOW_GALE_STALKER, 150.0f);
}
}

void GrimBatolStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("gb mount bombing drake",
        NextAction::array(0, new NextAction("gb mount bombing drake", ACTION_RAID + 20), nullptr)));
    triggers.push_back(new TriggerNode("gb bomb from drake",
        NextAction::array(0, new NextAction("gb bomb from drake", ACTION_RAID + 19), nullptr)));
    triggers.push_back(new TriggerNode("gb shadow gale",
        NextAction::array(0, new NextAction("gb move to shadow gale", ACTION_MOVE + 20), nullptr)));
    triggers.push_back(new TriggerNode("gb avoid hazard",
        NextAction::array(0, new NextAction("gb avoid hazard", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("gb priority target",
        NextAction::array(0, new NextAction("gb attack priority target", ACTION_RAID + 5), nullptr)));
}

void GrimBatolStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new GrimBatolMultiplier(botAI));
}

bool PriorityTargetTrigger::IsActive()
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && target != AI_VALUE(Unit*, "current target");
}

bool HazardTrigger::IsActive()
{
    return bot->GetMapId() == MAP_GRIM_BATOL && bot->IsInCombat() && !FindActiveShadowGale(bot) &&
        FindNearbyHazard(bot);
}

bool ShadowGaleTrigger::IsActive()
{
    Creature* eye = bot->GetMapId() == MAP_GRIM_BATOL ? FindActiveShadowGale(bot) : nullptr;
    return eye && bot->GetExactDist2d(eye) > 3.5f;
}

bool MountBombingDrakeTrigger::IsActive()
{
    return AssignedBombingDrake(botAI) != nullptr;
}

bool BombFromDrakeTrigger::IsActive()
{
    Creature* target = FindBombingTarget(bot);
    return target && botAI->CanCastVehicleSpell(SPELL_ENGULFING_FLAMES, target);
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

bool MoveToShadowGaleAction::Execute(Event /*event*/)
{
    Creature* eye = FindActiveShadowGale(bot);
    if (!eye)
        return false;
    if (bot->GetExactDist2d(eye) <= 3.0f)
    {
        bot->StopMoving();
        bot->GetMotionMaster()->Clear();
        return true;
    }
    return MoveTo(eye, 2.0f, MovementPriority::MOVEMENT_FORCED);
}

bool MountBombingDrakeAction::Execute(Event /*event*/)
{
    Creature* drake = AssignedBombingDrake(botAI);
    if (!drake)
        return false;

    if (bot->GetDistance(drake) > INTERACTION_DISTANCE)
        return MoveTo(drake, INTERACTION_DISTANCE - 0.5f,
            MovementPriority::MOVEMENT_FORCED);

    botAI->RemoveShapeshift();
    if (bot->IsMounted())
    {
        WorldPacket packet;
        bot->GetSession()->HandleCancelMountAuraOpcode(packet);
    }
    bot->GetMotionMaster()->Clear(false);
    bot->StopMoving();
    return drake->HandleSpellClick(bot);
}

bool BombFromDrakeAction::Execute(Event /*event*/)
{
    Creature* target = FindBombingTarget(bot);
    return target && botAI->CastVehicleSpell(SPELL_ENGULFING_FLAMES, target);
}

float GrimBatolMultiplier::GetValue(Action* action)
{
    if (bot->GetMapId() != MAP_GRIM_BATOL)
        return 1.0f;

    if (AssignedBombingDrake(botAI) || IsRidingBombingDrake(bot))
    {
        if (dynamic_cast<MountBombingDrakeAction*>(action))
            return 1.0f;
        return dynamic_cast<MovementAction*>(action) ? 0.0f : 1.0f;
    }

    if (!FindActiveShadowGale(bot))
        return 1.0f;
    if (dynamic_cast<MoveToShadowGaleAction*>(action))
        return 1.0f;
    return dynamic_cast<MovementAction*>(action) ? 0.0f : 1.0f;
}

GrimBatolTriggerContext::GrimBatolTriggerContext()
{
    creators["gb priority target"] = [](PlayerbotAI* ai) -> Trigger* { return new PriorityTargetTrigger(ai); };
    creators["gb avoid hazard"] = [](PlayerbotAI* ai) -> Trigger* { return new HazardTrigger(ai); };
    creators["gb shadow gale"] = [](PlayerbotAI* ai) -> Trigger* { return new ShadowGaleTrigger(ai); };
    creators["gb mount bombing drake"] = [](PlayerbotAI* ai) -> Trigger* { return new MountBombingDrakeTrigger(ai); };
    creators["gb bomb from drake"] = [](PlayerbotAI* ai) -> Trigger* { return new BombFromDrakeTrigger(ai); };
}

GrimBatolActionContext::GrimBatolActionContext()
{
    creators["gb attack priority target"] = [](PlayerbotAI* ai) -> Action* { return new AttackPriorityTargetAction(ai); };
    creators["gb avoid hazard"] = [](PlayerbotAI* ai) -> Action* { return new AvoidHazardAction(ai); };
    creators["gb move to shadow gale"] = [](PlayerbotAI* ai) -> Action* { return new MoveToShadowGaleAction(ai); };
    creators["gb mount bombing drake"] = [](PlayerbotAI* ai) -> Action* { return new MountBombingDrakeAction(ai); };
    creators["gb bomb from drake"] = [](PlayerbotAI* ai) -> Action* { return new BombFromDrakeAction(ai); };
}
}
