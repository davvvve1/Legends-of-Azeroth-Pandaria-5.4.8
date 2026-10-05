/* Playerbot strategy for Halls of Origination (normal and heroic). */
#ifndef _PLAYERBOT_HALLS_OF_ORIGINATION_STRATEGY_H
#define _PLAYERBOT_HALLS_OF_ORIGINATION_STRATEGY_H

#include "AttackActions.h"
#include "MovementActions.h"
#include "Multiplier.h"
#include "NamedObjectContext.h"
#include "Strategy.h"
#include "Trigger.h"

namespace HallsOfOriginationBot
{
enum Ids : uint32
{
    MAP_HALLS_OF_ORIGINATION = 644,
    NPC_ANHUUR = 39425,
    NPC_QUICKSAND = 40503,
    NPC_DUSTBONE_HORROR = 40450,
    NPC_JEWELED_SCARAB = 40458,
    NPC_ALPHA_BEAM = 41144,
    NPC_OMEGA_STANCE = 41194,
    NPC_ASTRAL_RAIN = 39720,
    NPC_CELESTIAL_CALL = 39721,
    NPC_VEIL_OF_SKY = 39722,
    NPC_SEEDLING_POD = 40550,
    NPC_SEEDLING_POD_2 = 40716,
    NPC_SEEDLING_POD_3 = 40592,
    NPC_SPORE = 40585,
    NPC_CHAOS_PORTAL = 41055,
    NPC_VOID_SENTINEL = 41208,
    NPC_VOID_SEEKER = 41371,
    NPC_VOID_WURM = 41374,
    NPC_SOLAR_WINDS_1 = 39634,
    NPC_SOLAR_WINDS_2 = 39635,
    NPC_SOLAR_WINDS_3 = 47922,
    GO_BEACON_LEFT = 203133,
    GO_BEACON_RIGHT = 203136,
    SPELL_SHIELD_OF_LIGHT = 74938
};

class HallsOfOriginationStrategy : public Strategy
{
public:
    explicit HallsOfOriginationStrategy(PlayerbotAI* ai) : Strategy(ai) { }
    std::string const getName() override { return "cata-hoo"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class AnhuurBeaconTrigger : public Trigger
{
public:
    explicit AnhuurBeaconTrigger(PlayerbotAI* ai) : Trigger(ai, "hoo use anhuur beacon") { }
    bool IsActive() override;
};

class PriorityTargetTrigger : public Trigger
{
public:
    explicit PriorityTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "hoo priority target") { }
    bool IsActive() override;
};

class HazardTrigger : public Trigger
{
public:
    explicit HazardTrigger(PlayerbotAI* ai) : Trigger(ai, "hoo avoid hazard") { }
    bool IsActive() override;
};

class UseAnhuurBeaconAction : public MovementAction
{
public:
    explicit UseAnhuurBeaconAction(PlayerbotAI* ai) : MovementAction(ai, "hoo use anhuur beacon") { }
    bool Execute(Event event) override;
};

class AttackPriorityTargetAction : public AttackAction
{
public:
    explicit AttackPriorityTargetAction(PlayerbotAI* ai) : AttackAction(ai, "hoo attack priority target") { }
    bool Execute(Event event) override;
};

class AvoidHazardAction : public MovementAction
{
public:
    explicit AvoidHazardAction(PlayerbotAI* ai) : MovementAction(ai, "hoo avoid hazard") { }
    bool Execute(Event event) override;
};

class HallsOfOriginationStrategyContext : public NamedObjectContext<Strategy>
{
public:
    HallsOfOriginationStrategyContext() { creators["cata-hoo"] = [](PlayerbotAI* ai) -> Strategy* { return new HallsOfOriginationStrategy(ai); }; }
};

class HallsOfOriginationTriggerContext : public NamedObjectContext<Trigger>
{
public:
    HallsOfOriginationTriggerContext();
};

class HallsOfOriginationActionContext : public NamedObjectContext<Action>
{
public:
    HallsOfOriginationActionContext();
};
}
#endif
