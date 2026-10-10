#include "InstanceLeadershipAction.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "Group.h"
#include "Map.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"

bool InstanceLeadershipAction::GroupIsReady() const
{
    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsAlive() ||
            !member->IsInWorld() || member->GetMap() != bot->GetMap())
            continue;

        // The tank advances in short, safe stages. This also makes the toggle
        // useful with a real player in the group: it waits instead of pulling
        // another pack while somebody is looting or recovering mana.
        if (bot->GetDistance(member) > 42.0f)
            return false;
    }
    return true;
}

Unit* InstanceLeadershipAction::SelectNextTarget() const
{
    GuidVector const& targets = context->GetValue<GuidVector>(
        "possible targets")->Get();
    Unit* best = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    for (ObjectGuid const& guid : targets)
    {
        Unit* target = botAI->GetUnit(guid);
        Creature* creature = target ? target->ToCreature() : nullptr;
        if (!creature || !creature->IsAlive() || !creature->IsInWorld() ||
            creature->GetMap() != bot->GetMap() || creature->IsInCombat() ||
            !bot->IsValidAttackTarget(creature) ||
            creature->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE) ||
            creature->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE))
            continue;

        float const distance = bot->GetExactDist(creature);
        if (distance > 160.0f ||
            std::fabs(bot->GetPositionZ() - creature->GetPositionZ()) > 35.0f)
            continue;

        // Prefer the nearest reachable pack. Bosses win close ties so the
        // tank does not circle an arena looking for an irrelevant leftover.
        float score = distance;
        if (creature->IsDungeonBoss() || creature->isWorldBoss())
            score -= 12.0f;
        if (!bot->IsWithinLOSInMap(creature))
            score += 18.0f;

        if (score < bestScore)
        {
            best = creature;
            bestScore = score;
        }
    }
    return best;
}

bool InstanceLeadershipAction::isUseful()
{
    return bot && bot->IsAlive() && !bot->IsInCombat() &&
        bot->GetMap() && bot->GetMap()->IsDungeon() &&
        botAI->IsInstanceTankLeader() &&
        PlayerBotSpec::IsTank(bot, true) && GroupIsReady() &&
        SelectNextTarget();
}

bool InstanceLeadershipAction::Execute(Event /*event*/)
{
    if (!isUseful())
        return false;

    Unit* target = SelectNextTarget();
    if (!target)
        return false;

    if (Group* group = bot->GetGroup(GroupSlot::Instance))
        group->SetTargetIcon(7, bot->GetGUID(), target->GetGUID(), 0);
    else if (Group* group = bot->GetGroup())
        group->SetTargetIcon(7, bot->GetGUID(), target->GetGUID(), 0);

    context->GetValue<Unit*>("current target")->Set(target);
    context->GetValue<ObjectGuid>("pull target")->Set(target->GetGUID());
    bot->SetTarget(target->GetGUID());

    float const distance = bot->GetExactDist(target);
    if (distance > 22.0f || !bot->IsWithinLOSInMap(target))
        return MoveTo(target, 18.0f, MovementPriority::MOVEMENT_NORMAL);

    return Attack(target);
}
