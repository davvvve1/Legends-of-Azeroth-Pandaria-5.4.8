/* Playerbot strategy for Grim Batol (normal and heroic). */
#ifndef _PLAYERBOT_GRIM_BATOL_STRATEGY_H
#define _PLAYERBOT_GRIM_BATOL_STRATEGY_H

#include "AttackActions.h"
#include "MovementActions.h"
#include "Multiplier.h"
#include "NamedObjectContext.h"
#include "Strategy.h"
#include "Trigger.h"

namespace GrimBatolBot
{
enum Ids : uint32
{
    MAP_GRIM_BATOL = 670,
    NPC_GENERAL_UMBRISS = 39625,
    NPC_BATTERED_RED_DRAKE = 39294,
    SPELL_ENGULFING_FLAMES = 74039,
    NPC_FORGEMASTER_THRONGUS = 40177,
    NPC_DRAHGA_SHADOWBURNER = 40319,
    NPC_ERUDAX = 40484,
    NPC_GROUND_SIEGE_STALKER = 40030,
    NPC_BLITZ_STALKER = 40040,
    NPC_MALIGNANT_TROGG = 39984,
    NPC_INVOKED_FLAMING_SPIRIT = 40357,
    NPC_SEEPING_TWILIGHT = 40365,
    NPC_DEVOURING_FLAMES = 48798,
    NPC_CAVE_IN_STALKER = 40228,
    NPC_FIRE_PATCH = 48711,
    NPC_FACELESS_CORRUPTOR = 40600,
    NPC_FACELESS_CORRUPTOR_H = 48844,
    NPC_SHADOW_GALE_STALKER = 40567
};

class GrimBatolStrategy : public Strategy
{
public:
    explicit GrimBatolStrategy(PlayerbotAI* ai) : Strategy(ai) { }
    std::string const getName() override { return "cata-gb"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

class PriorityTargetTrigger : public Trigger
{
public:
    explicit PriorityTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "gb priority target") { }
    bool IsActive() override;
};

class HazardTrigger : public Trigger
{
public:
    explicit HazardTrigger(PlayerbotAI* ai) : Trigger(ai, "gb avoid hazard") { }
    bool IsActive() override;
};

class ShadowGaleTrigger : public Trigger
{
public:
    explicit ShadowGaleTrigger(PlayerbotAI* ai) : Trigger(ai, "gb shadow gale") { }
    bool IsActive() override;
};

class MountBombingDrakeTrigger : public Trigger
{
public:
    explicit MountBombingDrakeTrigger(PlayerbotAI* ai) : Trigger(ai, "gb mount bombing drake") { }
    bool IsActive() override;
};

class BombFromDrakeTrigger : public Trigger
{
public:
    explicit BombFromDrakeTrigger(PlayerbotAI* ai) : Trigger(ai, "gb bomb from drake") { }
    bool IsActive() override;
};

class AttackPriorityTargetAction : public AttackAction
{
public:
    explicit AttackPriorityTargetAction(PlayerbotAI* ai) : AttackAction(ai, "gb attack priority target") { }
    bool Execute(Event event) override;
};

class AvoidHazardAction : public MovementAction
{
public:
    explicit AvoidHazardAction(PlayerbotAI* ai) : MovementAction(ai, "gb avoid hazard") { }
    bool Execute(Event event) override;
};

class MoveToShadowGaleAction : public MovementAction
{
public:
    explicit MoveToShadowGaleAction(PlayerbotAI* ai) : MovementAction(ai, "gb move to shadow gale") { }
    bool Execute(Event event) override;
};

class MountBombingDrakeAction : public MovementAction
{
public:
    explicit MountBombingDrakeAction(PlayerbotAI* ai) : MovementAction(ai, "gb mount bombing drake") { }
    bool Execute(Event event) override;
};

class BombFromDrakeAction : public Action
{
public:
    explicit BombFromDrakeAction(PlayerbotAI* ai) : Action(ai, "gb bomb from drake") { }
    bool Execute(Event event) override;
};

class GrimBatolMultiplier : public Multiplier
{
public:
    explicit GrimBatolMultiplier(PlayerbotAI* ai) : Multiplier(ai, "grim batol mechanics") { }
    float GetValue(Action* action) override;
};

class GrimBatolStrategyContext : public NamedObjectContext<Strategy>
{
public:
    GrimBatolStrategyContext() { creators["cata-gb"] = [](PlayerbotAI* ai) -> Strategy* { return new GrimBatolStrategy(ai); }; }
};

class GrimBatolTriggerContext : public NamedObjectContext<Trigger>
{
public:
    GrimBatolTriggerContext();
};

class GrimBatolActionContext : public NamedObjectContext<Action>
{
public:
    GrimBatolActionContext();
};
}
#endif
