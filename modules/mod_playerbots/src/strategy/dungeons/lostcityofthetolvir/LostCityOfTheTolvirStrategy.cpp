/* Playerbot strategy for Lost City of the Tol'vir (normal and heroic). */
#include "LostCityOfTheTolvirStrategy.h"

#include "Creature.h"
#include "Playerbots.h"

namespace LostCityOfTheTolvirBot
{
namespace
{
Creature* FindCreature(Player* bot, uint32 entry, float range)
{
    Creature* creature = bot->FindNearestCreature(entry, range, true);
    return creature && creature->IsAlive() ? creature : nullptr;
}

Unit* SelectPriorityTarget(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_LOST_CITY_OF_THE_TOLVIR || !bot->IsInCombat() || !botAI->IsDps(bot))
        return nullptr;

    uint32 const priority[] =
    {
        NPC_SERVANT_OF_SIAMAT,
        NPC_HARBINGER_OF_DARKNESS,
        NPC_SOUL_FRAGMENT,
        NPC_FRENZIED_CROCOLISK,
        NPC_BLAZE_OF_THE_HEAVENS
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
        { NPC_MYSTIC_TRAP_TARGET, 7.0f },
        { NPC_TEMPEST_STORM, 8.0f },
        { NPC_CLOUD_BURST, 7.0f }
    };
    for (Hazard const& hazard : hazards)
        if (Creature* creature = FindCreature(bot, hazard.entry, hazard.radius))
            return creature;
    return nullptr;
}
}

void LostCityOfTheTolvirStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("lct avoid hazard",
        NextAction::array(0, new NextAction("lct avoid hazard", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("lct priority target",
        NextAction::array(0, new NextAction("lct attack priority target", ACTION_RAID + 5), nullptr)));
}

bool PriorityTargetTrigger::IsActive()
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && target != AI_VALUE(Unit*, "current target");
}

bool HazardTrigger::IsActive()
{
    return bot->GetMapId() == MAP_LOST_CITY_OF_THE_TOLVIR && bot->IsInCombat() && FindNearbyHazard(bot);
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

LostCityOfTheTolvirTriggerContext::LostCityOfTheTolvirTriggerContext()
{
    creators["lct priority target"] = [](PlayerbotAI* ai) -> Trigger* { return new PriorityTargetTrigger(ai); };
    creators["lct avoid hazard"] = [](PlayerbotAI* ai) -> Trigger* { return new HazardTrigger(ai); };
}

LostCityOfTheTolvirActionContext::LostCityOfTheTolvirActionContext()
{
    creators["lct attack priority target"] = [](PlayerbotAI* ai) -> Action* { return new AttackPriorityTargetAction(ai); };
    creators["lct avoid hazard"] = [](PlayerbotAI* ai) -> Action* { return new AvoidHazardAction(ai); };
}
}
