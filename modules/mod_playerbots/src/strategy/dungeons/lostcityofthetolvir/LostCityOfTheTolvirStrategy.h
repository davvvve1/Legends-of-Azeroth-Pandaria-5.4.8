/* Playerbot strategy for Lost City of the Tol'vir (normal and heroic). */
#ifndef _PLAYERBOT_LOST_CITY_OF_THE_TOLVIR_STRATEGY_H
#define _PLAYERBOT_LOST_CITY_OF_THE_TOLVIR_STRATEGY_H

#include "AttackActions.h"
#include "MovementActions.h"
#include "Multiplier.h"
#include "NamedObjectContext.h"
#include "Strategy.h"
#include "Trigger.h"

namespace LostCityOfTheTolvirBot
{
enum Ids : uint32
{
    MAP_LOST_CITY_OF_THE_TOLVIR = 755,
    NPC_SIAMAT = 44819,
    NPC_WIND_TUNNEL = 48092,
    NPC_WIND_TUNNEL_LANDING = 48097,
    NPC_MYSTIC_TRAP_TARGET = 44840,
    NPC_FRENZIED_CROCOLISK = 43658,
    NPC_HARBINGER_OF_DARKNESS = 43927,
    NPC_SOUL_FRAGMENT = 43934,
    NPC_BLAZE_OF_THE_HEAVENS = 48904,
    NPC_SERVANT_OF_SIAMAT = 45269,
    NPC_TEMPEST_STORM = 44713,
    NPC_CLOUD_BURST = 44541
};

class LostCityOfTheTolvirStrategy : public Strategy
{
public:
    explicit LostCityOfTheTolvirStrategy(PlayerbotAI* ai) : Strategy(ai) { }
    std::string const getName() override { return "cata-lct"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

class WindTunnelTrigger : public Trigger
{
public:
    explicit WindTunnelTrigger(PlayerbotAI* ai) : Trigger(ai, "lct use wind tunnel") { }
    bool IsActive() override;
};

class PriorityTargetTrigger : public Trigger
{
public:
    explicit PriorityTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "lct priority target") { }
    bool IsActive() override;
};

class HazardTrigger : public Trigger
{
public:
    explicit HazardTrigger(PlayerbotAI* ai) : Trigger(ai, "lct avoid hazard") { }
    bool IsActive() override;
};

class AttackPriorityTargetAction : public AttackAction
{
public:
    explicit AttackPriorityTargetAction(PlayerbotAI* ai) : AttackAction(ai, "lct attack priority target") { }
    bool Execute(Event event) override;
};

class AvoidHazardAction : public MovementAction
{
public:
    explicit AvoidHazardAction(PlayerbotAI* ai) : MovementAction(ai, "lct avoid hazard") { }
    bool Execute(Event event) override;
};

class UseWindTunnelAction : public MovementAction
{
public:
    explicit UseWindTunnelAction(PlayerbotAI* ai) : MovementAction(ai, "lct use wind tunnel") { }
    bool Execute(Event event) override;
};

class LostCityOfTheTolvirMultiplier : public Multiplier
{
public:
    explicit LostCityOfTheTolvirMultiplier(PlayerbotAI* ai)
        : Multiplier(ai, "lost city of the tolvir mechanics") { }
    float GetValue(Action* action) override;
};

class LostCityOfTheTolvirStrategyContext : public NamedObjectContext<Strategy>
{
public:
    LostCityOfTheTolvirStrategyContext() { creators["cata-lct"] = [](PlayerbotAI* ai) -> Strategy* { return new LostCityOfTheTolvirStrategy(ai); }; }
};

class LostCityOfTheTolvirTriggerContext : public NamedObjectContext<Trigger>
{
public:
    LostCityOfTheTolvirTriggerContext();
};

class LostCityOfTheTolvirActionContext : public NamedObjectContext<Action>
{
public:
    LostCityOfTheTolvirActionContext();
};
}
#endif
