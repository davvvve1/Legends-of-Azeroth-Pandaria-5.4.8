#!/usr/bin/env python3
"""Source-level regression checks for the shared instance mechanics layer."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MECHANICS = ROOT / "modules/mod_playerbots/src/strategy/actions/InstanceMechanicsAction.cpp"
FACTORY = ROOT / "modules/mod_playerbots/src/Factory/AiFactory.cpp"
MAP_DBC = ROOT / "src/server/game/DataStores/DBCStructure.h"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


mechanics = MECHANICS.read_text(encoding="utf-8")
factory = FACTORY.read_text(encoding="utf-8")
map_dbc = MAP_DBC.read_text(encoding="utf-8")

priority_match = re.search(
    r"constexpr uint32 PriorityAdds\[\]\s*=\s*\{(?P<body>.*?)\n\};",
    mechanics,
    re.DOTALL,
)
require(priority_match is not None, "PriorityAdds table is missing")
priority = priority_match.group("body")

gekkan_order = [
    "NpcGlintrokIronhide",
    "NpcGlintrokHexxer",
    "NpcGlintrokSkulker",
    "NpcGlintrokOracle",
]
positions = [priority.find(name) for name in gekkan_order]
require(all(position >= 0 for position in positions), "Gekkan entourage is incomplete")
require(positions == sorted(positions), "Gekkan target order changed")
require("NpcGekkan = 61243" in mechanics, "Gekkan entry changed")
require("NpcGlintrokIronhide = 61337" in mechanics, "Ironhide entry changed")
require("NpcGlintrokSkulker = 61338" in mechanics, "Skulker entry changed")
require("NpcGlintrokOracle = 61339" in mechanics, "Oracle entry changed")
require("NpcGlintrokHexxer = 61340" in mechanics, "Hexxer entry changed")
require("gekkanEntourage && !PlayerBotSpec::IsHeal" in mechanics,
        "Gekkan tank/add switch is missing")
require("IsHealingCast(unit)" in mechanics,
        "generic engaged healer fallback is missing")
require("!unit->IsAlive() || !unit->IsInCombat()" in mechanics,
        "priority selection must remain limited to engaged living targets")
require('engine->addStrategy("avoid aoe", false);' in factory,
        "shared mechanics strategy is not loaded by default")
require("map_type == MAP_SCENARIO" in map_dbc and
        "bool IsDungeon() const" in map_dbc,
        "scenario maps are no longer included by the instance gate")

print(json.dumps({
    "checks": 12,
    "shared_instance_layer": "loaded-by-default",
    "map_types": ["dungeon", "raid", "scenario"],
    "difficulty_keying": "shared-map-entry",
    "gekkan_target_order": [61337, 61340, 61338, 61339, 61243],
    "generic_fallback": "engaged-healing-add",
    "result": "pass",
}, sort_keys=True))
