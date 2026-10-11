#ifndef _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H
#define _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H

#include "AttackActions.h"

#include <unordered_map>

struct Position;

// Out-of-combat half of the party/instance "gotank" toggle. The selected
// tank advances to the next reachable hostile pack while the ordinary follow
// action makes the rest of the bots use that tank as their formation anchor.
class InstanceLeadershipAction : public AttackAction
{
public:
    explicit InstanceLeadershipAction(PlayerbotAI* ai)
        : AttackAction(ai, "lead instance") { }

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    Unit* GetLockedPullTarget() const;
    Unit* SelectNextTarget() const;
    bool GroupHasActiveCombat() const;
    bool HasGenericDestination() const;
    bool FindGenericDestination(Position& destination) const;
    bool HasTempleOfJadeSerpentDestination() const;
    bool FindTempleOfJadeSerpentDestination(Position& destination) const;
    bool AdvanceTempleOfJadeSerpentRoute();
    bool AdvanceTempleLorewalkerRoute();
    bool AdvanceRouteTo(Position const& destination, char const* routeName);
    bool AdvanceValidatedWaypoint(float x, float y, float z);
    bool AdvanceGenericRoute();
    void ResetCompletedPull();
    bool HasMogushanPalaceDestination() const;
    bool AdvanceMogushanPalaceRoute();
    bool HasDragonSoulDestination() const;
    bool AdvanceDragonSoulRoute();
    bool UseDragonSoulNpc(uint32 entry, Position const& position,
        uint32 sender = 0, uint32 action = 0);
    bool EngageTarget(Unit* target);
    void AbandonUnreachableTarget();
    MovementPriority RouteMovementPriority() const;

    uint8 _mogushanRouteStage = 0xFF;
    uint16 _mogushanRouteIndex = 0;
    uint16 _templeLorewalkerRouteIndex = 0;
    uint32 _dragonSoulInteractionStage = 0;
    uint32 _dragonSoulInteractionAt = 0;
    uint32 _leadershipGeneration = 0;
    uint32 _forwardSearchStarted = 0;
    uint32 _approachTargetGuid = 0;
    uint32 _pullMarkedAt = 0;
    uint32 _approachProgressAt = 0;
    float _approachBestDistance = 0.0f;
    // Keep every recently failed candidate suppressed independently. A
    // single remembered GUID makes two unreachable creatures continually
    // replace one another, so the tank just moves the skull back and forth.
    std::unordered_map<uint32, uint32> _unreachableTargets;
};

#endif
