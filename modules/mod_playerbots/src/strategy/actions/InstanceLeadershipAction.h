#ifndef _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H
#define _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H

#include "AttackActions.h"

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
    bool AdvanceGenericRoute();
    void ResetCompletedPull();
    bool HasMogushanPalaceDestination() const;
    bool AdvanceMogushanPalaceRoute();
    bool EngageTarget(Unit* target);
    MovementPriority RouteMovementPriority() const;

    uint8 _mogushanRouteStage = 0xFF;
    uint16 _mogushanRouteIndex = 0;
    uint32 _leadershipGeneration = 0;
    uint32 _forwardSearchStarted = 0;
};

#endif
