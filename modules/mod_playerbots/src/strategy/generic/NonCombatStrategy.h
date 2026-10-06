#ifndef _PLAYERBOT_NONCOMBATSTRATEGY_H
#define _PLAYERBOT_NONCOMBATSTRATEGY_H

#include "PassTroughStrategy.h"

class PlayerbotAI;
class NonCombatStrategy : public Strategy
{
public:
    NonCombatStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    uint32 GetType() const override { return STRATEGY_TYPE_NONCOMBAT; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

class MountStrategy : public NonCombatStrategy
{
public:
    MountStrategy(PlayerbotAI* botAI) : NonCombatStrategy(botAI) {}

    std::string const getName() override { return "mount"; }
    NextAction** getDefaultActions() override;
};

#endif
