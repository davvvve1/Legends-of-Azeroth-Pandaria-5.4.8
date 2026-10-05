/*
 * Playerbot strategy for Blackrock Caverns (normal and heroic).
 */

#ifndef _PLAYERBOT_BLACKROCK_CAVERNS_STRATEGY_H
#define _PLAYERBOT_BLACKROCK_CAVERNS_STRATEGY_H

#include "MovementActions.h"
#include "Multiplier.h"
#include "NamedObjectContext.h"
#include "ObjectGuid.h"
#include "Strategy.h"
#include "Trigger.h"

namespace BlackrockCavernsBot
{
enum Ids : uint32
{
    MAP_BLACKROCK_CAVERNS = 645,
    NPC_KARSH_STEELBENDER = 39698
};

class BlackrockCavernsStrategy : public Strategy
{
public:
    explicit BlackrockCavernsStrategy(PlayerbotAI* ai) : Strategy(ai) { }
    std::string const getName() override { return "cata-brc"; }
    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

class KarshMoltenStreamTrigger : public Trigger
{
public:
    explicit KarshMoltenStreamTrigger(PlayerbotAI* ai) : Trigger(ai, "brc karsh molten stream") { }
    bool IsActive() override;
};

class KiteKarshThroughStreamAction : public MovementAction
{
public:
    explicit KiteKarshThroughStreamAction(PlayerbotAI* ai)
        : MovementAction(ai, "brc kite karsh through stream") { }

    bool Execute(Event event) override;

private:
    ObjectGuid encounterGuid = ObjectGuid::Empty;
    bool movingToHomeSide = false;
};

class BlackrockCavernsMultiplier : public Multiplier
{
public:
    explicit BlackrockCavernsMultiplier(PlayerbotAI* ai)
        : Multiplier(ai, "blackrock caverns mechanics") { }

    float GetValue(Action* action) override;
};

class BlackrockCavernsStrategyContext : public NamedObjectContext<Strategy>
{
public:
    BlackrockCavernsStrategyContext()
    {
        creators["cata-brc"] = [](PlayerbotAI* ai) -> Strategy* { return new BlackrockCavernsStrategy(ai); };
    }
};

class BlackrockCavernsTriggerContext : public NamedObjectContext<Trigger>
{
public:
    BlackrockCavernsTriggerContext();
};

class BlackrockCavernsActionContext : public NamedObjectContext<Action>
{
public:
    BlackrockCavernsActionContext();
};
}

#endif
