/* Playerbot strategy for Grim Batol (normal and heroic). */
#include "GrimBatolStrategy.h"

#include "Creature.h"
#include "Playerbots.h"

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

float GrimBatolMultiplier::GetValue(Action* action)
{
    if (bot->GetMapId() != MAP_GRIM_BATOL || !FindActiveShadowGale(bot))
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
}

GrimBatolActionContext::GrimBatolActionContext()
{
    creators["gb attack priority target"] = [](PlayerbotAI* ai) -> Action* { return new AttackPriorityTargetAction(ai); };
    creators["gb avoid hazard"] = [](PlayerbotAI* ai) -> Action* { return new AvoidHazardAction(ai); };
    creators["gb move to shadow gale"] = [](PlayerbotAI* ai) -> Action* { return new MoveToShadowGaleAction(ai); };
}
}
