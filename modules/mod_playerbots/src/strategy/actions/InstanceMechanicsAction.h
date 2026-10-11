#ifndef _PLAYERBOT_INSTANCE_MECHANICS_ACTION_H
#define _PLAYERBOT_INSTANCE_MECHANICS_ACTION_H

#include "AttackActions.h"

namespace InstanceMechanics
{
// Shared with PlayerbotAI's taunt safety gate so scripted tank swaps can use
// the ordinary class taunt without enabling arbitrary off-tank taunts.
bool ShouldTankSwap(Player* bot, Unit* boss);

// Trial actors become attackable/aggressive shortly before the core regards
// them as hostile or fully engaged. Leadership and mechanics must use this
// same activation test so a selected target is not immediately discarded.
bool IsActiveMogushanTrialTarget(Player* bot, Creature* creature);

// Returns the highest-priority engaged add for the shared kill order. Tank
// marker ownership and combat mechanics use the same result so skull cannot
// remain on a boss while damage dealers correctly switch to a critical add.
Unit* PriorityTarget(PlayerbotAI* botAI, Player* bot, Unit* boss = nullptr);
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
        AvoidUnitHazard,
        WiseMariDryPlatform,
        CircleWiseMari,
        Spread,
        Stack,
        Kite,
        AvoidFrontal,
        FocusAdd,
        BreakMindControl,
        StopAttack,
        TankSwap,
        Defensive,
        HealEncounterUnit,
        HeroicWill,
        InterceptOrb
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
