/*
 * Shared playerbot strategy for every Mists of Pandaria dungeon and raid.
 *
 * Normal and heroic difficulties use the same map and encounter scripts, so
 * this layer deliberately keys on map id and works for both modes.  Specific
 * encounter strategies can override it with a higher-priority action.
 */

#ifndef _PLAYERBOT_MOP_INSTANCE_STRATEGY_H
#define _PLAYERBOT_MOP_INSTANCE_STRATEGY_H

#include "AttackActions.h"
#include "NamedObjectContext.h"
#include "Strategy.h"
#include "Trigger.h"

namespace MopInstanceBot
{
bool IsMopInstanceMap(uint32 mapId);

class MopInstanceStrategy : public Strategy
{
public:
    explicit MopInstanceStrategy(PlayerbotAI* ai) : Strategy(ai) { }
    std::string const getName() override { return "mop-instance"; }
    NextAction** getDefaultActions() override;
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class CoordinatedTargetTrigger : public Trigger
{
public:
    explicit CoordinatedTargetTrigger(PlayerbotAI* ai)
        : Trigger(ai, "mop coordinated target") { }
    bool IsActive() override;
};

class AttackCoordinatedTargetAction : public AttackAction
{
public:
    explicit AttackCoordinatedTargetAction(PlayerbotAI* ai)
        : AttackAction(ai, "mop attack coordinated target") { }
    bool Execute(Event event) override;
};

class MopInstanceStrategyContext : public NamedObjectContext<Strategy>
{
public:
    MopInstanceStrategyContext()
    {
        creators["mop-instance"] = [](PlayerbotAI* ai) -> Strategy*
        {
            return new MopInstanceStrategy(ai);
        };
    }
};

class MopInstanceTriggerContext : public NamedObjectContext<Trigger>
{
public:
    MopInstanceTriggerContext();
};

class MopInstanceActionContext : public NamedObjectContext<Action>
{
public:
    MopInstanceActionContext();
};
}

#endif
