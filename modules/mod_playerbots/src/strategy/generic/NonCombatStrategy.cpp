#include "NonCombatStrategy.h"

#include "Playerbots.h"

void NonCombatStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    for (char const* dispel : {"pve dispel magic", "pve dispel curse", "pve dispel poison", "pve dispel disease"})
        triggers.push_back(new TriggerNode(dispel, NextAction::array(0,
            new NextAction(dispel, ACTION_DISPEL), nullptr)));
}

NextAction** MountStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("sync master mount", ACTION_DEFAULT + 1.0f),
        nullptr);
}
