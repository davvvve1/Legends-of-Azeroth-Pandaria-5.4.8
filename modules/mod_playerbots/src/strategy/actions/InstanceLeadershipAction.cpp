#include "InstanceLeadershipAction.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "Group.h"
#include "GameObject.h"
#include "GroupPveCombat.h"
#include "InstanceScript.h"
#include "LastMovementValue.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "PathGenerator.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "PossibleTargetsValue.h"
#include "ServerFacade.h"
#include "Timer.h"
#include "Transport.h"

namespace
{
constexpr uint32 MogushanPalaceMap = 994;
constexpr uint32 MogushanElevator = 212162;

struct RoutePoint
{
    float x;
    float y;
    float z;
    float radius;
};

// Mogu'shan Palace is layered vertically. A proximity-only pull algorithm
// sees enemies through the floors and sends the tank toward the wrong wall.
// These points follow the actual dungeon order and include the hidden stairs
// opened after Trial of the King and the lift used after Gekkan.
constexpr RoutePoint TrialRoute[] =
{
    { -3969.4f, -2560.0f, 26.0f, 7.0f },
    { -3995.3f, -2602.7f, 22.4f, 7.0f },
    { -4032.0f, -2609.6f, 22.4f, 7.0f },
    { -4074.7f, -2610.5f, 22.4f, 7.0f },
    { -4117.3f, -2611.4f, 22.4f, 7.0f },
    { -4147.7f, -2612.1f, 22.4f, 7.0f },
    { -4160.0f, -2612.4f, 17.9f, 7.0f },
    { -4202.7f, -2613.3f, 16.5f, 7.0f },
    { -4215.8f, -2613.6f, 16.5f, 9.0f }
};

constexpr RoutePoint GekkanRoute[] =
{
    // The secret stair does not continue south.  It curls north below the
    // Trial room, follows the treasure galleries west, and only then turns
    // south-west toward Gekkan.  These points are sampled from map 994's
    // actual mmap; the old route accidentally reversed the cosmetic fleeing
    // Saurok spline and therefore led the tank into a wall.
    { -4215.8f, -2648.0f, 17.6f, 7.0f },
    { -4224.0f, -2666.0f, 17.6f, 7.0f },
    { -4229.1f, -2678.7f, 17.6f, 6.0f },
    { -4229.0f, -2681.0f, 15.2f, 6.0f },
    { -4218.0f, -2685.0f, 12.5f, 6.0f },
    { -4207.0f, -2683.0f, 9.6f, 6.0f },
    { -4197.0f, -2674.0f, 5.5f, 6.0f },
    { -4196.0f, -2664.0f, 2.5f, 6.0f },
    { -4196.0f, -2645.0f, -2.6f, 7.0f },
    { -4196.0f, -2622.0f, -8.9f, 7.0f },
    { -4196.0f, -2581.0f, -9.4f, 7.0f },
    { -4199.0f, -2539.0f, -28.3f, 7.0f },
    { -4239.2f, -2523.5f, -28.3f, 7.0f },
    { -4266.7f, -2523.5f, -39.0f, 7.0f },
    { -4309.3f, -2523.5f, -36.6f, 7.0f },
    { -4330.7f, -2523.5f, -28.3f, 7.0f },
    { -4373.3f, -2525.8f, -28.3f, 7.0f },
    { -4394.1f, -2548.0f, -28.3f, 7.0f },
    { -4373.3f, -2571.6f, -28.3f, 7.0f },
    { -4356.5f, -2596.0f, -28.3f, 7.0f },
    { -4358.8f, -2626.6f, -28.3f, 7.0f },
    { -4373.3f, -2642.6f, -40.1f, 7.0f },
    { -4389.1f, -2660.0f, -43.2f, 7.0f },
    { -4394.7f, -2645.3f, -51.5f, 7.0f },
    { -4395.8f, -2624.0f, -54.7f, 7.0f },
    { -4397.9f, -2583.0f, -54.5f, 9.0f }
};

constexpr uint16 MogushanUpperRouteIndex = 5;
constexpr RoutePoint XinRoute[] =
{
    { -4397.9f, -2583.0f, -54.5f, 8.0f },
    { -4398.3f, -2624.0f, -54.6f, 7.0f },
    { -4398.6f, -2664.3f, -43.8f, 7.0f },
    { -4399.0f, -2709.3f, -42.0f, 7.0f },
    { -4399.3f, -2743.0f, -40.0f, 7.0f },
    // Same X/Y as the lower landing, but on the upper floor.  The moving
    // transport is the only valid connection between these adjacent nodes.
    { -4399.0f, -2743.0f, 22.6f, 7.0f },
    { -4399.0f, -2707.6f, 22.4f, 7.0f },
    { -4404.3f, -2657.6f, 22.4f, 7.0f },
    { -4416.3f, -2645.4f, 22.4f, 7.0f },
    { -4437.3f, -2624.0f, 22.4f, 7.0f },
    { -4461.3f, -2617.5f, 22.4f, 7.0f },
    { -4501.3f, -2616.8f, 22.4f, 7.0f },
    { -4544.0f, -2616.1f, 22.4f, 7.0f },
    { -4586.7f, -2615.4f, 22.4f, 7.0f },
    { -4629.3f, -2614.7f, 22.1f, 7.0f },
    { -4672.0f, -2614.0f, 22.1f, 7.0f },
    { -4694.0f, -2613.6f, 28.0f, 8.0f }
};

constexpr float RouteDistanceSquared(RoutePoint const& point, float x,
    float y, float z)
{
    float const dx = point.x - x;
    float const dy = point.y - y;
    float const dz = point.z - z;
    return dx * dx + dy * dy + dz * dz;
}

template <size_t N>
constexpr size_t ClosestRoutePoint(RoutePoint const (&route)[N], float x,
    float y, float z)
{
    size_t closestIndex = 0;
    float closestDistance = RouteDistanceSquared(route[0], x, y, z);
    for (size_t i = 1; i < N; ++i)
    {
        float const distance = RouteDistanceSquared(route[i], x, y, z);
        if (distance < closestDistance)
        {
            closestIndex = i;
            closestDistance = distance;
        }
    }
    return closestIndex;
}

template <size_t N>
constexpr bool RouteSegmentsAreContinuous(RoutePoint const (&route)[N],
    size_t verticalTransportIndex = N)
{
    for (size_t i = 1; i < N; ++i)
    {
        if (i == verticalTransportIndex)
            continue;
        if (RouteDistanceSquared(route[i], route[i - 1].x,
            route[i - 1].y, route[i - 1].z) > 55.0f * 55.0f)
            return false;
    }
    return true;
}

// Source-level regressions for enabling gotank in the middle of each section.
// In particular, the lower gallery must resume on its northern mmap corridor,
// never on the unrelated cosmetic spline south of the Trial room.
static_assert(RouteSegmentsAreContinuous(TrialRoute));
static_assert(RouteSegmentsAreContinuous(GekkanRoute));
static_assert(RouteSegmentsAreContinuous(XinRoute, MogushanUpperRouteIndex));
static_assert(RouteDistanceSquared(GekkanRoute[0], TrialRoute[
    std::size(TrialRoute) - 1].x, TrialRoute[std::size(TrialRoute) - 1].y,
    TrialRoute[std::size(TrialRoute) - 1].z) < 40.0f * 40.0f);
static_assert(RouteDistanceSquared(XinRoute[0], GekkanRoute[
    std::size(GekkanRoute) - 1].x, GekkanRoute[std::size(GekkanRoute) - 1].y,
    GekkanRoute[std::size(GekkanRoute) - 1].z) < 1.0f);
static_assert(ClosestRoutePoint(TrialRoute, -4030.0f, -2610.0f, 22.4f) == 2);
static_assert(ClosestRoutePoint(GekkanRoute, -4196.0f, -2620.0f, -9.0f) == 9);
static_assert(ClosestRoutePoint(GekkanRoute, -4358.0f, -2625.0f, -28.3f) == 20);
static_assert(ClosestRoutePoint(XinRoute, -4399.0f, -2742.0f, -40.0f) == 4);
static_assert(ClosestRoutePoint(XinRoute, -4399.0f, -2742.0f, 22.5f) ==
    MogushanUpperRouteIndex);

uint8 MogushanEncounterStage(Player* bot)
{
    InstanceScript* instance = bot ? bot->GetInstanceScript() : nullptr;
    if (!instance || instance->GetBossState(0) != DONE)
        return 0;
    if (instance->GetBossState(1) != DONE)
        return 1;
    if (instance->GetBossState(2) != DONE)
        return 2;
    return 3;
}

void GetMogushanRoute(uint8 stage, RoutePoint const*& points, size_t& count)
{
    if (stage == 0)
    {
        points = TrialRoute;
        count = std::size(TrialRoute);
    }
    else if (stage == 1)
    {
        points = GekkanRoute;
        count = std::size(GekkanRoute);
    }
    else if (stage == 2)
    {
        points = XinRoute;
        count = std::size(XinRoute);
    }
    else
    {
        points = nullptr;
        count = 0;
    }
}
}

bool InstanceLeadershipAction::GroupHasActiveCombat() const
{
    return GroupPveCombat::GroupHasActiveCombat(bot);
}

bool InstanceLeadershipAction::HasGenericDestination() const
{
    if (!bot || bot->GetMapId() == MogushanPalaceMap ||
        !bot->GetMap() || !bot->GetMap()->IsDungeon())
        return false;

    DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(
        bot->GetMapId(), bot->GetMap()->GetDifficulty());
    InstanceScript* instance = bot->GetInstanceScript();
    if (!encounters || encounters->empty() || !instance)
        return false;

    uint32 const completed = instance->GetCompletedEncounterMask();
    for (DungeonEncounter const* encounter : *encounters)
        if (encounter && encounter->dbcEntry &&
            encounter->dbcEntry->encounterIndex < 32 &&
            !(completed & (1u << encounter->dbcEntry->encounterIndex)))
            return true;

    return false;
}

bool InstanceLeadershipAction::FindGenericDestination(Position& destination) const
{
    DungeonEncounterList const* encounterList = sObjectMgr->GetDungeonEncounterList(
        bot->GetMapId(), bot->GetMap()->GetDifficulty());
    InstanceScript* instance = bot->GetInstanceScript();
    if (!encounterList || !instance)
        return false;

    std::vector<DungeonEncounter const*> encounters(encounterList->begin(),
        encounterList->end());
    std::stable_sort(encounters.begin(), encounters.end(),
        [](DungeonEncounter const* left, DungeonEncounter const* right)
        {
            if (!left || !left->dbcEntry)
                return false;
            if (!right || !right->dbcEntry)
                return true;
            return left->dbcEntry->encounterIndex <
                right->dbcEntry->encounterIndex;
        });

    uint32 const completed = instance->GetCompletedEncounterMask();
    CellObjectGuidsMap const& cells = sObjectMgr->GetMapObjectGuids(
        bot->GetMapId(), bot->GetMap()->GetSpawnMode());

    for (DungeonEncounter const* encounter : encounters)
    {
        if (!encounter || !encounter->dbcEntry ||
            encounter->dbcEntry->encounterIndex >= 32 ||
            (completed & (1u << encounter->dbcEntry->encounterIndex)) ||
            encounter->creditType != ENCOUNTER_CREDIT_KILL_CREATURE)
            continue;

        CreatureData const* best = nullptr;
        float bestDistance = std::numeric_limits<float>::max();
        for (auto const& cell : cells)
            for (uint32 spawnId : cell.second.creatures)
            {
                CreatureData const* data = sObjectMgr->GetCreatureData(spawnId);
                if (!data || data->id != encounter->creditEntry ||
                    !(data->phaseMask & bot->GetPhaseMask()))
                    continue;

                float const dx = data->posX - bot->GetPositionX();
                float const dy = data->posY - bot->GetPositionY();
                float const dz = data->posZ - bot->GetPositionZ();
                float const distance = dx * dx + dy * dy + dz * dz;
                if (distance < bestDistance)
                {
                    best = data;
                    bestDistance = distance;
                }
            }

        if (best)
        {
            destination.Relocate(best->posX, best->posY, best->posZ,
                best->orientation);
            return true;
        }
    }

    return false;
}

bool InstanceLeadershipAction::AdvanceGenericRoute()
{
    Position destination;
    if (!FindGenericDestination(destination))
        return false;

    if (bot->GetExactDist2d(destination.GetPositionX(),
        destination.GetPositionY()) <= 15.0f)
    {
        bot->StopMoving();
        return true;
    }

    // Use the live mmap to turn the next incomplete encounter into short
    // corridor steps. Each step loads the next grid and exposes intervening
    // trash to SelectNextTarget without requiring per-instance trash routes.
    PathGenerator path(bot);
    path.SetPathLengthLimit(4000.0f);
    if (!path.CalculatePath(destination.GetPositionX(),
        destination.GetPositionY(), destination.GetPositionZ(), false))
        return false;

    PathType const pathType = path.GetPathType();
    if (pathType & (PATHFIND_NOPATH | PATHFIND_SHORTCUT |
        PATHFIND_NOT_USING_PATH))
        return false;

    Movement::PointsArray const& points = path.GetPath();
    if (points.size() < 2)
        return false;

    G3D::Vector3 waypoint = points.back();
    float walked = 0.0f;
    for (size_t i = 1; i < points.size(); ++i)
    {
        walked += (points[i] - points[i - 1]).length();
        waypoint = points[i];
        if (walked >= 35.0f)
            break;
    }

    bool const moved = MoveTo(bot->GetMapId(), waypoint.x, waypoint.y,
        waypoint.z, false, false, false, true,
        RouteMovementPriority(), true);
    if (moved)
        TC_LOG_INFO("server",
            "gotank generic route leader=%s map=%u instance=%u waypoint=%.2f,%.2f,%.2f",
            bot->GetName().c_str(), bot->GetMapId(), bot->GetInstanceId(),
            waypoint.x, waypoint.y, waypoint.z);
    return moved;
}

Unit* InstanceLeadershipAction::GetLockedPullTarget() const
{
    ObjectGuid const pullGuid =
        context->GetValue<ObjectGuid>("pull target")->Get();
    Unit* pull = pullGuid ? botAI->GetUnit(pullGuid) : nullptr;
    if (!pull || !pull->ToCreature() || !pull->IsAlive() ||
        !pull->IsInWorld() || pull->GetMap() != bot->GetMap() ||
        !bot->IsValidAttackTarget(pull) ||
        pull->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE) ||
        pull->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE) ||
        bot->GetExactDist(pull) > 240.0f)
        return nullptr;

    return pull;
}

void InstanceLeadershipAction::ResetCompletedPull()
{
    Value<ObjectGuid>* pullValue = context->GetValue<ObjectGuid>("pull target");
    ObjectGuid const pullGuid = pullValue->Get();
    if (!pullGuid)
        return;

    if (GetLockedPullTarget())
        return;

    pullValue->Set(ObjectGuid::Empty);
    _forwardSearchStarted = getMSTime();
    _approachTargetGuid = 0;
    _approachProgressAt = 0;
    _approachBestDistance = 0.0f;
    Unit* current = context->GetValue<Unit*>("current target")->Get();
    if (!current || !current->IsAlive() || current->GetGUID() == pullGuid)
        context->GetValue<Unit*>("current target")->Set(nullptr);
    context->GetValue<LastMovement&>("last movement")->Get().clear();
    bot->SetTarget(ObjectGuid::Empty);
    bot->SetSelection(ObjectGuid::Empty);
    bot->GetMotionMaster()->Clear(false);
    bot->StopMoving();

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group && group->GetTargetIcon(7) == pullGuid)
        group->SetTargetIcon(7, bot->GetGUID(), ObjectGuid::Empty);

    botAI->SetNextCheckDelay(0);
}

Unit* InstanceLeadershipAction::SelectNextTarget() const
{
    // Once a pull has been selected, own it until it dies or becomes invalid.
    // Re-scoring every visible creature while approaching a pack makes the
    // nearest candidate change as the tank moves; that repeatedly reverses
    // MoveTo, makes the tank appear to rubber-band, and moves the skull faster
    // than the client can present a useful kill order.
    if (Unit* pull = GetLockedPullTarget())
        return pull;

    GuidVector const& targets = context->GetValue<GuidVector>(
        "possible targets")->Get();
    Unit* best = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    auto consider = [&](ObjectGuid const& guid)
    {
        auto const unreachable = _unreachableTargets.find(guid.GetCounter());
        if (unreachable != _unreachableTargets.end() &&
            getMSTimeDiff(unreachable->second, getMSTime()) < 20000)
            return;

        Unit* target = botAI->GetUnit(guid);
        Creature* creature = target ? target->ToCreature() : nullptr;
        if (!creature || !creature->IsAlive() || !creature->IsInWorld() ||
            creature->GetMap() != bot->GetMap() || creature->IsInCombat() ||
            !bot->IsValidAttackTarget(creature) ||
            creature->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE) ||
            creature->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE))
            return;

        bool const palace = bot->GetMapId() == MogushanPalaceMap;
        float const distance = bot->GetExactDist(creature);
        float const maximumDistance = palace ? 48.0f : 160.0f;
        float const maximumHeight = palace ? 8.0f : 35.0f;
        if (distance > maximumDistance ||
            std::fabs(bot->GetPositionZ() - creature->GetPositionZ()) > maximumHeight ||
            (palace && !bot->IsWithinLOSInMap(creature)))
            return;

        // Compare actual navigation distance rather than straight-line
        // distance. This is the generic route guard for every dungeon and
        // raid: a mob behind a wall or on another floor must not win merely
        // because its world coordinates look close.
        float score = distance;
        PathGenerator path(bot);
        path.SetPathLengthLimit(240.0f);
        bool const calculated = path.CalculatePath(creature->GetPositionX(),
            creature->GetPositionY(), creature->GetPositionZ(), false);
        PathType const pathType = path.GetPathType();
        bool const cleanPath = calculated && (pathType & PATHFIND_NORMAL) &&
            !(pathType & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE |
                PATHFIND_SHORTCUT | PATHFIND_NOT_USING_PATH |
                PATHFIND_FARFROMPOLY_END));
        if (cleanPath && !path.GetPath().empty())
        {
            G3D::Vector3 const& end = path.GetPath().back();
            float const dx = end.x - creature->GetPositionX();
            float const dy = end.y - creature->GetPositionY();
            float const dz = end.z - creature->GetPositionZ();
            if (dx * dx + dy * dy > 16.0f || std::fabs(dz) > 8.0f)
                return;
            score = path.getPathLength();
        }
        else
        {
            // Retain a conservative fallback for old instances which have no
            // mmap tile. It requires direct sight and almost the same floor.
            if (!bot->IsWithinLOSInMap(creature) ||
                std::fabs(bot->GetPositionZ() - creature->GetPositionZ()) > 8.0f)
                return;
            score += 30.0f;
        }

        // Bosses win close ties so the tank does not circle an arena looking
        // for an irrelevant leftover.
        if (creature->IsDungeonBoss() || creature->isWorldBoss())
            score -= 12.0f;
        if (!bot->IsWithinLOSInMap(creature))
            score += 18.0f;

        if (score < bestScore)
        {
            best = creature;
            bestScore = score;
        }
    };

    for (ObjectGuid const& guid : targets)
        consider(guid);

    // The ordinary combat target cache intentionally uses the configured
    // 75-yard sight distance.  That is too short for many empty corridors
    // between instance packs and made generic gotank leadership stop even
    // though a clean mmap route existed just beyond the cache.  Only the one
    // elected leader performs this wider scan, only while out of combat and
    // only when the cheap normal scan found nothing.  Mogu'shan Palace keeps
    // its explicit floor-safe route because a radius scan there sees several
    // vertically overlapping galleries.
    if (!best && bot->GetMapId() != MogushanPalaceMap)
    {
        GuidVector const extended = PossibleTargetsValue(botAI,
            "instance leadership targets", 160.0f, true).Calculate();
        for (ObjectGuid const& guid : extended)
            consider(guid);
    }

    return best;
}

bool InstanceLeadershipAction::HasMogushanPalaceDestination() const
{
    return bot->GetMapId() == MogushanPalaceMap &&
        MogushanEncounterStage(bot) < 3;
}

bool InstanceLeadershipAction::AdvanceMogushanPalaceRoute()
{
    uint32 const generation =
        botAI->GetInstanceTankLeadershipGeneration();
    if (_leadershipGeneration != generation)
    {
        _leadershipGeneration = generation;
        _mogushanRouteStage = 0xFF;
        _mogushanRouteIndex = 0;
    }

    uint8 const stage = MogushanEncounterStage(bot);
    RoutePoint const* route = nullptr;
    size_t count = 0;
    GetMogushanRoute(stage, route, count);
    if (!route || !count)
        return false;

    if (_mogushanRouteStage != stage)
    {
        _mogushanRouteStage = stage;
        _mogushanRouteIndex = 0;

        // gotank can be enabled halfway through the dungeon. Resume at the
        // closest three-dimensional route point rather than walking back to
        // its beginning. Z is part of the distance so overlapping floors do
        // not select one another; the stage remains authoritative.
        float closest = std::numeric_limits<float>::max();
        for (uint16 i = 0; i < count; ++i)
        {
            float const distance = RouteDistanceSquared(route[i],
                bot->GetPositionX(), bot->GetPositionY(),
                bot->GetPositionZ());
            if (distance < closest)
            {
                closest = distance;
                _mogushanRouteIndex = i;
            }
        }
    }

    // Combat can pull the tank off the route before it reaches the current
    // point's small arrival radius. If the next point is now closer, the tank
    // has already passed the old point along the ordered route; advance one
    // node instead of repeatedly trying to run back through the cleared room.
    if (_mogushanRouteIndex + 1 < count)
    {
        float const currentDistance = RouteDistanceSquared(
            route[_mogushanRouteIndex], bot->GetPositionX(),
            bot->GetPositionY(), bot->GetPositionZ());
        float const nextDistance = RouteDistanceSquared(
            route[_mogushanRouteIndex + 1], bot->GetPositionX(),
            bot->GetPositionY(), bot->GetPositionZ());
        if (nextDistance < currentDistance)
            ++_mogushanRouteIndex;
    }

    while (_mogushanRouteIndex < count)
    {
        RoutePoint const& point = route[_mogushanRouteIndex];
        if (bot->GetExactDist2d(point.x, point.y) > point.radius ||
            std::fabs(bot->GetPositionZ() - point.z) > 18.0f)
            break;
        ++_mogushanRouteIndex;
    }

    if (_mogushanRouteIndex >= count)
        return false;

    // The lower and upper Palace floors are connected by a moving transport.
    // Wait for it at the lower landing, board only while level with the bot,
    // and let normal transport passenger movement carry the tank upward.
    if (stage == 2 && _mogushanRouteIndex == MogushanUpperRouteIndex &&
        bot->GetPositionZ() < 8.0f)
    {
        GameObject* elevator = bot->FindNearestGameObject(
            MogushanElevator, 180.0f);
        if (!elevator)
            return false;

        if (bot->GetTransport() == elevator ||
            bot->GetExactDist2d(elevator) <= 2.2f)
        {
            bot->StopMoving();
            return true;
        }

        if (bot->GetExactDist2d(elevator) <= 12.0f &&
            std::fabs(bot->GetPositionZ() - elevator->GetPositionZ()) <= 4.0f)
            return MoveTo(elevator, 0.5f,
                RouteMovementPriority());

        RoutePoint const& landing = route[MogushanUpperRouteIndex - 1];
        if (bot->GetExactDist2d(landing.x, landing.y) > landing.radius)
            return MoveTo(bot->GetMapId(), landing.x, landing.y, landing.z,
                false, false, false, true,
                RouteMovementPriority(), true);

        bot->StopMoving();
        return true;
    }

    RoutePoint const& point = route[_mogushanRouteIndex];
    bool const moved = MoveTo(bot->GetMapId(), point.x, point.y, point.z,
        false, false, false, true, RouteMovementPriority(), true);
    if (moved)
        TC_LOG_INFO("server",
            "gotank Mogu'shan route leader=%s instance=%u stage=%u index=%u waypoint=%.2f,%.2f,%.2f",
            bot->GetName().c_str(), bot->GetInstanceId(), uint32(stage),
            uint32(_mogushanRouteIndex), point.x, point.y, point.z);
    return moved;
}

bool InstanceLeadershipAction::EngageTarget(Unit* target)
{
    if (!target)
        return false;

    // The forward-search window ends as soon as the next real pull is found.
    _forwardSearchStarted = 0;

    uint32 const now = getMSTime();
    uint32 const targetGuid = target->GetGUID().GetCounter();
    float const distance = bot->GetExactDist(target);
    if (_approachTargetGuid != targetGuid)
    {
        _approachTargetGuid = targetGuid;
        _approachProgressAt = now;
        _approachBestDistance = distance;
    }
    else if (distance + 1.5f < _approachBestDistance)
    {
        _approachProgressAt = now;
        _approachBestDistance = distance;
    }

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group && group->GetTargetIcon(7) != target->GetGUID())
    {
        group->SetTargetIcon(7, bot->GetGUID(), target->GetGUID(), 0);
        TC_LOG_INFO("server",
            "gotank pull locked leader=%s target=%s entry=%u target-guid=%u map=%u instance=%u",
            bot->GetName().c_str(), target->GetName().c_str(),
            target->GetEntry(), target->GetGUID().GetCounter(),
            bot->GetMapId(), bot->GetInstanceId());
    }

    context->GetValue<Unit*>("current target")->Set(target);
    context->GetValue<ObjectGuid>("pull target")->Set(target->GetGUID());
    bot->SetTarget(target->GetGUID());

    bool issued = false;
    if (distance > 22.0f || !bot->IsWithinLOSInMap(target))
        issued = MoveTo(target, 18.0f, MovementPriority::MOVEMENT_FORCED);
    else
        issued = Attack(target);

    // Attack() puts the bot in combat and assigns its victim before the first
    // swing/threat event reaches the creature. Give that opening attack more
    // time than a pure movement approach; otherwise the watchdog cancels a
    // valid pull and selects another marked mob just before the first hit.
    bool const openingAttack = bot->GetVictim() == target &&
        botAI->GetState() == BOT_STATE_COMBAT;
    uint32 const stalledFor = openingAttack ? 6000 : 4000;
    if (_approachProgressAt &&
        getMSTimeDiff(_approachProgressAt, now) >= stalledFor)
    {
        AbandonUnreachableTarget(target);
        return true;
    }

    return issued;
}

void InstanceLeadershipAction::AbandonUnreachableTarget(Unit* target)
{
    if (!target)
        return;

    ObjectGuid const guid = target->GetGUID();
    uint32 const now = getMSTime();
    for (auto it = _unreachableTargets.begin();
        it != _unreachableTargets.end();)
    {
        if (getMSTimeDiff(it->second, now) >= 20000)
            it = _unreachableTargets.erase(it);
        else
            ++it;
    }
    _unreachableTargets[guid.GetCounter()] = now;
    _approachTargetGuid = 0;
    _approachProgressAt = 0;
    _approachBestDistance = 0.0f;
    _forwardSearchStarted = now;

    context->GetValue<ObjectGuid>("pull target")->Set(ObjectGuid::Empty);
    if (context->GetValue<Unit*>("current target")->Get() == target)
        context->GetValue<Unit*>("current target")->Set(nullptr);
    context->GetValue<LastMovement&>("last movement")->Get().clear();
    bot->AttackStop();
    bot->SetTarget(ObjectGuid::Empty);
    bot->SetSelection(ObjectGuid::Empty);
    bot->GetMotionMaster()->Clear(false);
    bot->StopMoving();

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group && group->GetTargetIcon(7) == guid)
        group->SetTargetIcon(7, bot->GetGUID(), ObjectGuid::Empty);

    TC_LOG_WARN("server",
        "gotank abandoned unreachable pull leader=%s target=%s entry=%u target-guid=%u map=%u instance=%u position=%.2f,%.2f,%.2f",
        bot->GetName().c_str(), target->GetName().c_str(),
        target->GetEntry(), guid.GetCounter(), bot->GetMapId(),
        bot->GetInstanceId(), bot->GetPositionX(), bot->GetPositionY(),
        bot->GetPositionZ());
    botAI->SetNextCheckDelay(0);
}

MovementPriority InstanceLeadershipAction::RouteMovementPriority() const
{
    if (_forwardSearchStarted &&
        getMSTimeDiff(_forwardSearchStarted, getMSTime()) < 20000)
        return MovementPriority::MOVEMENT_HAZARD;

    return MovementPriority::MOVEMENT_NORMAL;
}

bool InstanceLeadershipAction::isUseful()
{
    if (!bot || !bot->IsAlive() || !bot->GetMap() ||
        !bot->GetMap()->IsDungeon() || !botAI->IsInstanceTankLeader() ||
        !PlayerBotSpec::IsTank(bot, true) || GroupHasActiveCombat())
        return false;

    ResetCompletedPull();

    return SelectNextTarget() || HasMogushanPalaceDestination() ||
        HasGenericDestination();
}

bool InstanceLeadershipAction::Execute(Event /*event*/)
{
    if (!isUseful())
        return false;

    ResetCompletedPull();

    if (Unit* target = SelectNextTarget())
        return EngageTarget(target);

    if (bot->GetMapId() == MogushanPalaceMap)
        return AdvanceMogushanPalaceRoute();

    return AdvanceGenericRoute();
}
