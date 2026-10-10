#include "InstanceLeadershipAction.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "Group.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Map.h"
#include "PathGenerator.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "ServerFacade.h"
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
    { -3969.4f, -2560.0f, 22.4f, 5.0f },
    { -3969.4f, -2602.0f, 22.4f, 5.0f },
    { -4025.0f, -2613.6f, 22.4f, 6.0f },
    { -4072.0f, -2613.6f, 22.4f, 6.0f },
    { -4140.0f, -2613.6f, 22.4f, 6.0f },
    { -4185.0f, -2613.6f, 17.6f, 7.0f },
    { -4215.8f, -2613.6f, 17.6f, 9.0f }
};

constexpr RoutePoint GekkanRoute[] =
{
    { -4215.8f, -2648.0f, 17.6f, 6.0f },
    { -4216.1f, -2682.0f, 23.2f, 6.0f },
    { -4217.5f, -2710.0f, 4.0f, 7.0f },
    { -4219.5f, -2750.0f, -32.0f, 7.0f },
    { -4220.9f, -2794.2f, -68.9f, 7.0f },
    { -4254.5f, -2768.5f, -67.0f, 7.0f },
    { -4283.5f, -2759.4f, -64.0f, 7.0f },
    { -4309.5f, -2739.9f, -59.0f, 7.0f },
    { -4300.3f, -2721.9f, -55.3f, 7.0f },
    { -4300.6f, -2683.1f, -50.3f, 7.0f },
    { -4322.0f, -2677.0f, -46.8f, 7.0f },
    { -4339.7f, -2675.1f, -37.1f, 7.0f },
    { -4348.0f, -2677.3f, -30.5f, 7.0f },
    { -4372.0f, -2675.0f, -40.0f, 7.0f },
    { -4398.0f, -2662.0f, -43.5f, 7.0f },
    { -4398.0f, -2620.0f, -54.5f, 7.0f },
    { -4397.9f, -2583.0f, -54.5f, 9.0f }
};

constexpr uint16 MogushanUpperRouteIndex = 5;
constexpr RoutePoint XinRoute[] =
{
    { -4397.9f, -2583.0f, -54.5f, 8.0f },
    { -4398.0f, -2622.0f, -54.5f, 7.0f },
    { -4398.5f, -2664.0f, -43.5f, 7.0f },
    { -4399.0f, -2705.0f, -43.0f, 7.0f },
    { -4399.3f, -2743.0f, -43.0f, 7.0f },
    { -4399.3f, -2700.0f, 22.4f, 7.0f },
    { -4428.0f, -2652.0f, 22.3f, 7.0f },
    { -4431.0f, -2613.6f, 22.3f, 7.0f },
    { -4462.0f, -2613.6f, 22.4f, 7.0f },
    { -4505.0f, -2613.6f, 22.4f, 7.0f },
    { -4532.0f, -2613.6f, 22.4f, 7.0f },
    { -4576.0f, -2613.6f, 22.2f, 7.0f },
    { -4618.0f, -2613.6f, 21.9f, 7.0f },
    { -4658.0f, -2613.6f, 22.0f, 7.0f },
    { -4694.0f, -2613.6f, 28.0f, 8.0f }
};

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

        bool const palace = bot->GetMapId() == MogushanPalaceMap;
        float const distance = bot->GetExactDist(creature);
        float const maximumDistance = palace ? 48.0f : 160.0f;
        float const maximumHeight = palace ? 8.0f : 35.0f;
        if (distance > maximumDistance ||
            std::fabs(bot->GetPositionZ() - creature->GetPositionZ()) > maximumHeight ||
            (palace && !bot->IsWithinLOSInMap(creature)))
            continue;

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
                continue;
            score = path.getPathLength();
        }
        else
        {
            // Retain a conservative fallback for old instances which have no
            // mmap tile. It requires direct sight and almost the same floor.
            if (!bot->IsWithinLOSInMap(creature) ||
                std::fabs(bot->GetPositionZ() - creature->GetPositionZ()) > 8.0f)
                continue;
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
        // closest same-floor route point rather than walking back to its
        // beginning. The stage itself is authoritative and prevents skips.
        float closest = std::numeric_limits<float>::max();
        for (uint16 i = 0; i < count; ++i)
        {
            float const dz = std::fabs(bot->GetPositionZ() - route[i].z);
            float const distance = bot->GetExactDist(
                route[i].x, route[i].y, route[i].z);
            if (dz <= 20.0f && distance < closest)
            {
                closest = distance;
                _mogushanRouteIndex = i;
            }
        }
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
                MovementPriority::MOVEMENT_NORMAL);

        RoutePoint const& landing = route[MogushanUpperRouteIndex - 1];
        if (bot->GetExactDist2d(landing.x, landing.y) > landing.radius)
            return MoveTo(bot->GetMapId(), landing.x, landing.y, landing.z,
                false, false, false, false,
                MovementPriority::MOVEMENT_NORMAL, true);

        bot->StopMoving();
        return true;
    }

    RoutePoint const& point = route[_mogushanRouteIndex];
    return MoveTo(bot->GetMapId(), point.x, point.y, point.z,
        false, false, false, false, MovementPriority::MOVEMENT_NORMAL, true);
}

bool InstanceLeadershipAction::EngageTarget(Unit* target)
{
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

bool InstanceLeadershipAction::isUseful()
{
    return bot && bot->IsAlive() && !bot->IsInCombat() &&
        bot->GetMap() && bot->GetMap()->IsDungeon() &&
        botAI->IsInstanceTankLeader() &&
        PlayerBotSpec::IsTank(bot, true) && GroupIsReady() &&
        (SelectNextTarget() || HasMogushanPalaceDestination());
}

bool InstanceLeadershipAction::Execute(Event /*event*/)
{
    if (!isUseful())
        return false;

    if (Unit* target = SelectNextTarget())
        return EngageTarget(target);

    if (bot->GetMapId() == MogushanPalaceMap)
        return AdvanceMogushanPalaceRoute();

    return false;
}
