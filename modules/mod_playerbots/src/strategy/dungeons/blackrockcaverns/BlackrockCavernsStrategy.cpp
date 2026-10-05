/*
 * Playerbot strategy for Blackrock Caverns (normal and heroic).
 */

#include "BlackrockCavernsStrategy.h"

#include "Creature.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"

namespace BlackrockCavernsBot
{
namespace
{
// The boss script heats Karsh within six yards of 237.84, 784.76.  These
// endpoints lie six yards either side of that point on the line from Karsh's
// home position through the centre of the Molten Stream.
Position const StreamHomeSide = { 235.47f, 790.27f, 95.67f, 0.0f };
Position const StreamFarSide  = { 240.21f, 779.25f, 95.67f, 0.0f };

Creature* FindTankEngagedKarsh(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_BLACKROCK_CAVERNS || !bot->IsInCombat() ||
        !PlayerBotSpec::IsTank(bot, true))
        return nullptr;

    Creature* karsh = bot->FindNearestCreature(NPC_KARSH_STEELBENDER, 120.0f, true);
    if (!karsh || !karsh->IsAlive() || !karsh->IsInCombat() || karsh->GetVictim() != bot)
        return nullptr;

    return karsh;
}
}

void BlackrockCavernsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("brc karsh molten stream",
        NextAction::array(0, new NextAction("brc kite karsh through stream", ACTION_MOVE + 20), nullptr)));
}

void BlackrockCavernsStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new BlackrockCavernsMultiplier(botAI));
}

bool KarshMoltenStreamTrigger::IsActive()
{
    return FindTankEngagedKarsh(botAI) != nullptr;
}

bool KiteKarshThroughStreamAction::Execute(Event /*event*/)
{
    Creature* karsh = FindTankEngagedKarsh(botAI);
    if (!karsh)
    {
        encounterGuid = ObjectGuid::Empty;
        return false;
    }

    // Start every pull by crossing to the side opposite the tank.  Afterwards
    // reverse at each endpoint, producing a straight 6-yard pendulum through
    // the stream until Karsh dies or changes victim.
    if (encounterGuid != karsh->GetGUID())
    {
        encounterGuid = karsh->GetGUID();
        movingToHomeSide = bot->GetExactDist2d(StreamHomeSide) > bot->GetExactDist2d(StreamFarSide);
    }

    Position const* destination = movingToHomeSide ? &StreamHomeSide : &StreamFarSide;
    if (bot->GetExactDist2d(*destination) <= 0.8f)
    {
        movingToHomeSide = !movingToHomeSide;
        destination = movingToHomeSide ? &StreamHomeSide : &StreamFarSide;
    }

    return MoveTo(bot->GetMapId(), destination->GetPositionX(), destination->GetPositionY(),
        destination->GetPositionZ(), false, false, false, true,
        MovementPriority::MOVEMENT_FORCED, true);
}

float BlackrockCavernsMultiplier::GetValue(Action* action)
{
    if (!FindTankEngagedKarsh(botAI))
        return 1.0f;

    // Formation, chase and generic positioning must not interrupt the
    // pendulum. Combat, mitigation, taunts and attacks remain available.
    if (dynamic_cast<KiteKarshThroughStreamAction*>(action))
        return 1.0f;

    return dynamic_cast<MovementAction*>(action) ? 0.0f : 1.0f;
}

BlackrockCavernsTriggerContext::BlackrockCavernsTriggerContext()
{
    creators["brc karsh molten stream"] = [](PlayerbotAI* ai) -> Trigger*
    {
        return new KarshMoltenStreamTrigger(ai);
    };
}

BlackrockCavernsActionContext::BlackrockCavernsActionContext()
{
    creators["brc kite karsh through stream"] = [](PlayerbotAI* ai) -> Action*
    {
        return new KiteKarshThroughStreamAction(ai);
    };
}
}
