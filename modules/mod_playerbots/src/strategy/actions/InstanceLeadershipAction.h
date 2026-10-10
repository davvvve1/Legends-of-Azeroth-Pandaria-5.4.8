#ifndef _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H
#define _PLAYERBOT_INSTANCE_LEADERSHIP_ACTION_H

#include "AttackActions.h"

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
    Unit* SelectNextTarget() const;
    bool GroupIsReady() const;
    bool HasMogushanPalaceDestination() const;
    bool AdvanceMogushanPalaceRoute();
    bool EngageTarget(Unit* target);

    uint8 _mogushanRouteStage = 0xFF;
    uint16 _mogushanRouteIndex = 0;
};

#endif
