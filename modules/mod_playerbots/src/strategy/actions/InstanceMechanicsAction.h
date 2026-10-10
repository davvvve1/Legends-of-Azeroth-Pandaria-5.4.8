#ifndef _PLAYERBOT_INSTANCE_MECHANICS_ACTION_H
#define _PLAYERBOT_INSTANCE_MECHANICS_ACTION_H

#include "AttackActions.h"

namespace InstanceMechanics
{
// Shared with PlayerbotAI's taunt safety gate so scripted tank swaps can use
// the ordinary class taunt without enabling arbitrary off-tank taunts.
bool ShouldTankSwap(Player* bot, Unit* boss);
}

// A data-driven encounter layer used in every dungeon and raid. It handles
// mechanics that can be expressed consistently across encounters (frontal
// casts, carrier spread/stack, fixates, immunity/add switches, tank swaps,
// defensive casts and dangerous adds). Dedicated legacy strategies may still
// override it with their higher-priority encounter actions.
class InstanceMechanicsAction : public AttackAction
{
public:
    explicit InstanceMechanicsAction(PlayerbotAI* ai)
        : AttackAction(ai, "instance mechanics") { }

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    enum class Reaction : uint8
    {
        None,
        Spread,
        Stack,
        Kite,
        AvoidFrontal,
        FocusAdd,
        BreakMindControl,
        StopAttack,
        TankSwap,
        Defensive,
        HealEncounterUnit
    };

    struct Plan
    {
        Reaction reaction = Reaction::None;
        Unit* anchor = nullptr;
        Unit* target = nullptr;
        float distance = 0.0f;
    };

    Plan BuildPlan() const;
    Unit* FindEncounterBoss() const;
    Unit* FindPriorityAdd(Unit* boss) const;
    Unit* FindMindControlledMember() const;
    Unit* FindFriendlyEncounterUnit() const;
    bool UseDefensive();
    bool HealEncounterUnit(Unit* target);
};

#endif
