#!/usr/bin/env python3
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


def read(relative: str) -> str:
    return (ROOT / relative).read_text(encoding="utf-8")


factory = read("modules/mod_playerbots/src/Factory/AiFactory.cpp")
assert 'if (sPlayerbotAIConfig->autoSaveMana)' in factory
assert 'engine->addStrategy("save mana", false);' in factory

loadout = read("modules/mod_playerbots/src/Factory/BotFactory.cpp")
loadout_start = loadout.index("bool BotFactory::PrepareManagedLoadout(")
loadout_end = loadout.index("uint32 BotFactory::GetWeaponReferenceItemLevel", loadout_start)
loadout_body = loadout[loadout_start:loadout_end]
assert "bot->SetHealth(bot->GetMaxHealth());" in loadout_body
assert "bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));" in loadout_body

generic_paladin = read(
    "modules/mod_playerbots/src/strategy/Classes/paladin/GenericPaladinStrategy.cpp"
)
assert 'new TriggerNode("high mana"' not in generic_paladin
assert 'new TriggerNode("medium mana"' in generic_paladin

paladin_actions = read(
    "modules/mod_playerbots/src/strategy/Classes/paladin/PaladinActions.h"
)
assert '"holy shock", 25.0f, HealingManaEfficiency::VERY_HIGH' in paladin_actions
assert '"holy light", 50.0f, HealingManaEfficiency::HIGH' in paladin_actions
assert '"flash of light", 15.0f, HealingManaEfficiency::LOW' in paladin_actions

paladin_heal = read(
    "modules/mod_playerbots/src/strategy/Classes/paladin/HealPaladinStrategy.cpp"
)
almost_full = paladin_heal[paladin_heal.index('"party member almost full health"'):]
almost_full = almost_full[:almost_full.index(")));", 0) + 4]
assert '"flash of light on party"' not in almost_full
assert '"holy shock on party"' in almost_full

paladin_noncombat = read(
    "modules/mod_playerbots/src/strategy/Classes/paladin/GenericPaladinNonCombatStrategy.cpp"
)
assert '"party member almost full health", NextAction::array(0, new NextAction("holy light on party"' in paladin_noncombat
assert '"party member medium health", NextAction::array(0, new NextAction("holy light on party"' in paladin_noncombat

shaman_actions = read(
    "modules/mod_playerbots/src/strategy/Classes/shaman/ShamanActions.h"
)
assert '"healing wave", 50.0f, HealingManaEfficiency::HIGH' in shaman_actions
assert '"chain heal", 15.0f, HealingManaEfficiency::MEDIUM' in shaman_actions

print("healer mana regression checks passed")
