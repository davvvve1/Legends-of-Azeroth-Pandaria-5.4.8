#include "AhnQirajStrategy.h"
#include "FollowActions.h"

#include <cstddef>
#include <cmath>

#include "Event.h"
#include "Formations.h"
#include "GroupPveCombat.h"
#include "GroupFollowFormation.h"
#include "PlayerbotSpec.h"
#include "LastMovementValue.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "ServerFacade.h"
#include "SharedDefines.h"

Player* FollowAction::GetFollowTarget()
{
    // The elected tank is an independent navigation anchor. It must never
    // resolve any ordinary follow action back to the human master (the tank
    // formation slot is in front, which made this look like leadership).
    if (botAI->IsInstanceTankLeader())
        return nullptr;

    Player* master = GetMaster();
    if (!botAI->IsInstanceTankLeadershipActive())
        return master;

    // While gotank is active every bot follows the elected tank, never the
    // human master. This also preserves the tank corpse as the recovery point
    // so healers can resurrect it and the same leader can resume the route.
    if (Player* leader = botAI->GetInstanceTankLeader())
        return leader;
    return nullptr;
}

bool FollowAction::UseGroupFollowFormation()
{
    Player* master = GetFollowTarget();
    return master && master != bot && bot->GetGroup() &&
        master->GetGroup() == bot->GetGroup() && master->IsInWorld() &&
        master->GetMap() == bot->GetMap() && master->IsAlive() && bot->IsAlive() &&
        !bot->InBattleground() && !master->IsInCombat() && !bot->IsInCombat() &&
        botAI->GetState() == BOT_STATE_NON_COMBAT;
}

WorldLocation FollowAction::GetGroupFollowLocation()
{
    Player* master = GetFollowTarget();
    bool const tank = PlayerBotSpec::IsTank(bot, true);
    bool const healer = PlayerBotSpec::IsHeal(bot, true);
    std::size_t slot = 0;
    for (GroupReference* ref = bot->GetGroup()->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && member != master && member->IsInWorld() && member->IsAlive() &&
            member->GetMap() == bot->GetMap() &&
            PlayerBotSpec::IsTank(member, true) == tank &&
            (!healer || PlayerBotSpec::IsHeal(member, true)) &&
            (tank || healer || !PlayerBotSpec::IsHeal(member, true)) &&
            member->GetGUID() < bot->GetGUID())
            ++slot;
    }

    auto const offset = healer ?
        GroupFollowFormation::GetHealerOffset(slot) :
        (tank && botAI->IsInstanceTankLeadershipActive() ?
            GroupFollowFormation::GetOffTankOffset(slot) :
            GroupFollowFormation::GetOffset(tank, slot));
    float const orientation = master->GetOrientation();
    // Narrow the formation if a wall blocks a slot; never use unchecked coordinates.
    for (float scale : {1.0f, 0.5f, 0.25f})
    {
        float x = master->GetPositionX() + scale *
            (std::cos(orientation) * offset.forward - std::sin(orientation) * offset.sideways);
        float y = master->GetPositionY() + scale *
            (std::sin(orientation) * offset.forward + std::cos(orientation) * offset.sideways);
        float z = master->GetPositionZ();
        if (master->GetMap()->CheckCollisionAndGetValidCoords(master,
            master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), x, y, z) &&
            std::fabs(z - master->GetPositionZ()) <= 6.0f)
            return WorldLocation(master->GetMapId(), x, y, z);
    }
    // Multi-level instances can expose another walkable floor at the same
    // X/Y. Never accept that lower floor as a formation offset; stacking on
    // the tank is safer than sending a follower through the visible floor.
    return WorldLocation(master->GetMapId(), master->GetPositionX(),
        master->GetPositionY(), master->GetPositionZ());
}

bool FollowAction::MoveToCombatFollowPoint(Player* leader, float distance)
{
    if (!leader || !leader->IsInWorld() ||
        leader->GetMap() != bot->GetMap())
        return false;

    // Targeted follow generators keep a raw Unit pointer for their lifetime.
    // During a combat-to-non-combat transition the old enemy movement and a
    // newly elected tank follow can be replaced in the same map update.  Use
    // a copied point for combat catch-up so no movement generator retains a
    // group-member pointer across that transition.
    float const leaderX = leader->GetPositionX();
    float const leaderY = leader->GetPositionY();
    float const leaderZ = leader->GetPositionZ();
    float deltaX = bot->GetPositionX() - leaderX;
    float deltaY = bot->GetPositionY() - leaderY;
    float const separation = std::hypot(deltaX, deltaY);
    if (separation > 0.1f)
    {
        deltaX /= separation;
        deltaY /= separation;
    }
    else
    {
        deltaX = -std::cos(leader->GetOrientation());
        deltaY = -std::sin(leader->GetOrientation());
    }

    return MoveTo(leader->GetMapId(), leaderX + deltaX * distance,
        leaderY + deltaY * distance, leaderZ, false, false, true, false,
        MovementPriority::MOVEMENT_COMBAT, true);
}

bool FollowAction::Execute(Event event)
{
    if (AhnQirajStrategy::IsActive(bot)) return false;
    if (botAI->IsInstanceTankLeader()) return false;
    if (botAI->IsInstanceTankLeadershipActive())
    {
        Player* leader = GetFollowTarget();
        bool const groupCombat = GroupPveCombat::GroupHasActiveCombat(bot);
        bool const healerCatchup = leader && leader->IsAlive() &&
            PlayerBotSpec::IsHeal(bot, true) && groupCombat &&
            bot->GetDistance(leader) > 32.0f;
        bool const idleCombatCatchup = leader && leader->IsAlive() &&
            !PlayerBotSpec::IsHeal(bot, true) && groupCombat &&
            !bot->GetVictim() && bot->GetDistance(leader) > 8.0f;
        if (!leader || leader == bot || !leader->IsInWorld() ||
            leader->GetMap() != bot->GetMap() ||
            (groupCombat && !healerCatchup && !idleCombatCatchup) ||
            bot->IsNonMeleeSpellCasted(true, false, true))
            return false;

        // Dragon Soul's wings are separate pieces of map 967 joined by
        // scripted portals, the Skyfire and the Spine jump. A follower that
        // missed the leader's interaction has no mmap path at all. Rejoin
        // only out of combat and only across an unmistakable island-sized
        // gap; ordinary room movement remains path-driven.
        if (bot->GetMapId() == 967 && !groupCombat &&
            bot->GetDistance(leader) > 500.0f)
        {
            float const angle = leader->GetOrientation() + float(M_PI);
            bot->NearTeleportTo(leader->GetPositionX() + std::cos(angle) * 3.0f,
                leader->GetPositionY() + std::sin(angle) * 3.0f,
                leader->GetPositionZ(), leader->GetOrientation());
            return true;
        }

        // A healer displaced by mechanics or a fast chain pull closes to a
        // stable 20-yard casting position before selecting its next heal.
        if (healerCatchup)
            return MoveToCombatFollowPoint(leader, 20.0f);
        if (idleCombatCatchup)
            return MoveToCombatFollowPoint(leader, 6.0f);

        if (UseGroupFollowFormation())
        {
            WorldLocation const loc = GetGroupFollowLocation();
            if (bot->GetExactDist2d(loc.GetPositionX(), loc.GetPositionY()) <= 1.0f)
                return false;
            return MoveTo(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(),
                loc.GetPositionZ(), false, false, true, false,
                MovementPriority::MOVEMENT_NORMAL, true);
        }
        return bot->GetDistance(leader) > 4.0f && Follow(leader, 3.0f,
            static_cast<float>(M_PI));
    }
    if (bot->HasWorldBossStagingAccess() &&
        !bot->IsWorldBossStagingCleanup() &&
        !bot->IsWorldBossStagingEncounterStarted())
    {
        Player* master = GetMaster();
        if (!master || !master->IsInWorld() ||
            master->GetMap() != bot->GetMap() || !CanDeadFollow(master) ||
            bot->GetDistance(master) <= 4.0f ||
            bot->IsNonMeleeSpellCasted(true, false, true))
            return false;

        // All pre-pull bots share one compact point behind the requester.
        // This avoids the roster-wide angular formation and remains still
        // while the requester is standing and the raid is buffing.
        return Follow(master, 2.0f, static_cast<float>(M_PI));
    }

    if (UseGroupFollowFormation())
    {
        if (GetMaster()->HasUnitState(UNIT_STATE_IN_FLIGHT) ||
            bot->IsNonMeleeSpellCasted(true, false, true) ||
            botAI->HasStrategy("move from group", BOT_STATE_NON_COMBAT))
            return false;

        WorldLocation const loc = GetGroupFollowLocation();
        if (bot->GetExactDist2d(loc.GetPositionX(), loc.GetPositionY()) <= 1.0f)
            return false;
        return MoveTo(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(),
            loc.GetPositionZ(), false, false, true, false, MovementPriority::MOVEMENT_NORMAL, true);
    }

    if (botAI->IsLfgAutoQueueControlled() && botAI->IsGroupPveActivity())
    {
        Player* master = GetMaster();
        if (!master || !master->IsInWorld() ||
            master->GetMap() != bot->GetMap() || !CanDeadFollow(master))
            return false;

        // No Chaos/Circle/Arrow offsets in a managed instance. A stationary
        // requester must not make followers rotate into an untouched pack.
        // Stay close outside combat, but allow enough room to reach a ranged
        // requester's enemy without the follow action pulling us back.
        float const followRadius = botAI->GetState() == BOT_STATE_COMBAT ?
            60.0f : 4.0f;
        if (bot->GetDistance(master) <= followRadius ||
            bot->IsNonMeleeSpellCasted(true, false, true))
            return false;
        return Follow(master, 2.0f, static_cast<float>(M_PI));
    }

    Formation* formation = AI_VALUE(Formation*, "formation");
    std::string const target = formation->GetTargetName();

    bool moved = false;
    if (!target.empty())
    {
        moved = Follow(AI_VALUE(Unit*, target));
    }
    else
    {
        WorldLocation loc = formation->GetLocation();
        if (Formation::IsNullLocation(loc) || loc.GetMapId() == -1)
            return false;

        MovementPriority priority = botAI->GetState() == BOT_STATE_COMBAT ? MovementPriority::MOVEMENT_COMBAT : MovementPriority::MOVEMENT_NORMAL;
        moved = MoveTo(loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), false, false, false,
            true, priority, true);
    }

    if (Pet* pet = bot->GetPet())
    {
        //botAI->PetFollow();
    }

    return moved;
}

bool FollowAction::isUseful()
{
    if (AhnQirajStrategy::IsActive(bot)) return false;
    if (botAI->IsInstanceTankLeader()) return false;
    if (botAI->IsInstanceTankLeadershipActive())
    {
        Player* leader = GetFollowTarget();
        bool const groupCombat = GroupPveCombat::GroupHasActiveCombat(bot);
        bool const healerCatchup = leader && leader->IsAlive() &&
            PlayerBotSpec::IsHeal(bot, true) && groupCombat &&
            bot->GetDistance(leader) > 32.0f;
        bool const idleCombatCatchup = leader && leader->IsAlive() &&
            !PlayerBotSpec::IsHeal(bot, true) && groupCombat &&
            !bot->GetVictim() && bot->GetDistance(leader) > 8.0f;
        if (!leader || leader == bot || !leader->IsInWorld() ||
            leader->GetMap() != bot->GetMap() ||
            (groupCombat && !healerCatchup && !idleCombatCatchup) ||
            leader->HasUnitState(UNIT_STATE_IN_FLIGHT) ||
            bot->IsNonMeleeSpellCasted(true, false, true))
            return false;
        if (bot->GetMapId() == 967 && !groupCombat &&
            bot->GetDistance(leader) > 500.0f)
            return true;
        if (healerCatchup || idleCombatCatchup)
            return true;
        if (UseGroupFollowFormation())
        {
            WorldLocation const loc = GetGroupFollowLocation();
            return bot->GetExactDist2d(loc.GetPositionX(), loc.GetPositionY()) > 1.0f;
        }
        return bot->GetDistance(leader) > 4.0f;
    }
    if (bot->HasWorldBossStagingAccess() &&
        !bot->IsWorldBossStagingCleanup() &&
        !bot->IsWorldBossStagingEncounterStarted())
    {
        Player* master = GetMaster();
        return master && master != bot && master->IsInWorld() &&
            master->GetMap() == bot->GetMap() && CanDeadFollow(master) &&
            !master->HasUnitState(UNIT_STATE_IN_FLIGHT) &&
            !bot->IsNonMeleeSpellCasted(true, false, true) &&
            bot->GetDistance(master) > 4.0f;
    }

    if (UseGroupFollowFormation())
    {
        if (GetMaster()->HasUnitState(UNIT_STATE_IN_FLIGHT) ||
            bot->IsNonMeleeSpellCasted(true, false, true) ||
            botAI->HasStrategy("move from group", BOT_STATE_NON_COMBAT))
            return false;

        WorldLocation const loc = GetGroupFollowLocation();
        return bot->GetExactDist2d(loc.GetPositionX(), loc.GetPositionY()) > 1.0f;
    }

    if (botAI->IsLfgAutoQueueControlled() && botAI->IsGroupPveActivity())
    {
        Player* master = GetMaster();
        // Combat positioning/healing owns normal in-fight movement. Do not
        // oscillate between ranged formation and a two-yard follow point.
        float const followRadius = botAI->GetState() == BOT_STATE_COMBAT ?
            60.0f : 4.0f;
        return master && master != bot && master->IsInWorld() &&
            master->GetMap() == bot->GetMap() && CanDeadFollow(master) &&
            !master->HasUnitState(UNIT_STATE_IN_FLIGHT) &&
            !bot->IsNonMeleeSpellCasted(true, false, true) &&
            bot->GetDistance(master) > followRadius;
    }

    // move from group takes priority over follow as it's added and removed automatically
    // (without removing/adding follow)
    if (botAI->HasStrategy("move from group", BOT_STATE_COMBAT) ||
        botAI->HasStrategy("move from group", BOT_STATE_NON_COMBAT))
        return false;

    if (bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != nullptr)
        return false;

    Formation* formation = AI_VALUE(Formation*, "formation");
    if (!formation)
        return false;

    std::string const target = formation->GetTargetName();

    Unit* fTarget = nullptr;
    if (!target.empty())
        fTarget = AI_VALUE(Unit*, target);
    else
        fTarget = AI_VALUE(Unit*, "master target");

    if (fTarget)
    {
        if (fTarget->HasUnitState(UNIT_STATE_IN_FLIGHT))
            return false;

        if (!CanDeadFollow(fTarget))
            return false;

        if (fTarget->GetGUID() == bot->GetGUID())
            return false;
    }

    float distance = 0.f;
    if (!target.empty())
    {
        distance = AI_VALUE2(float, "distance", target);
    }
    else
    {
        WorldLocation loc = formation->GetLocation();
        if (Formation::IsNullLocation(loc) || bot->GetMapId() != loc.GetMapId())
            return false;

        distance = bot->GetDistance(loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
    }

    return sServerFacade->IsDistanceGreaterThan(distance, formation->GetMaxDistance());
}

bool FollowAction::CanDeadFollow(Unit* target)
{
    if (!target || !target->IsInWorld())
        return false;

    // Move to corpse when dead and player is alive or not a ghost.
    if (!bot->IsAlive() && (target->IsAlive() || !target->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST)))
        return false;

    return true;
}

bool FleeToMasterAction::Execute(Event event)
{
    if (botAI->IsInstanceTankLeader())
        return false;

    Unit* fTarget = AI_VALUE(Unit*, "master target");
    if (!fTarget || fTarget == bot || !fTarget->IsInWorld() ||
        fTarget->GetMap() != bot->GetMap())
        return false;

    bool canFollow = Follow(fTarget);
    if (!canFollow)
    {
        // botAI->SetNextCheckDelay(5000);
        return false;
    }

    WorldPosition targetPos(fTarget);
    WorldPosition bosPos(bot);
    float distance = bosPos.fDist(targetPos);

    if (distance < sPlayerbotAIConfig->reactDistance * 3)
    {
        if (!urand(0, 3))
            botAI->TellMaster("I am close, wait for me!");
    }
    else if (distance < 1000)
    {
        if (!urand(0, 10))
            botAI->TellMaster("I heading to your position.");
    }
    else if (!urand(0, 20))
        botAI->TellMaster("I am traveling to your position.");

    botAI->SetNextCheckDelay(3000);

    return true;
}

bool FleeToMasterAction::isUseful()
{
    if (botAI->IsInstanceTankLeader())
        return false;

    Player* groupMaster = botAI->GetGroupMaster();
    if (!groupMaster || groupMaster == bot || !groupMaster->IsInWorld())
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");
    if (target && groupMaster->GetTarget() == target->GetGUID())
        return false;

    if (!botAI->HasStrategy("follow", BOT_STATE_NON_COMBAT))
        return false;

    Unit* fTarget = AI_VALUE(Unit*, "master target");

    if (!fTarget || fTarget != groupMaster ||
        fTarget->GetMap() != bot->GetMap() || !CanDeadFollow(fTarget))
        return false;

    return true;
}
