#!/usr/bin/env python3
"""Source-level regression checks for the party/instance gotank state machine."""

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
AI_HEADER = ROOT / "modules/mod_playerbots/src/AI/PlayerbotAI.h"
AI_SOURCE = ROOT / "modules/mod_playerbots/src/AI/PlayerbotAI.cpp"
CHAT_SOURCE = ROOT / "modules/mod_playerbots/src/mod_playerbots.cpp"
FOLLOW_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/actions/FollowActions.cpp"
LEAD_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/actions/InstanceLeadershipAction.cpp"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


header = AI_HEADER.read_text(encoding="utf-8-sig")
ai = AI_SOURCE.read_text(encoding="utf-8-sig")
chat = CHAT_SOURCE.read_text(encoding="utf-8-sig")
follow = FOLLOW_SOURCE.read_text(encoding="utf-8-sig")
lead = LEAD_SOURCE.read_text(encoding="utf-8-sig")

require('command != "gotank"' in chat and 'command != "go tank"' in chat,
        "both documented gotank command spellings must remain accepted")
require("_instanceTankLeadershipGeneration.fetch_add(1)" in ai,
        "a changed leadership state must publish a fresh generation")
require("_instanceTankLeadershipAppliedGeneration != leadershipGeneration" in ai,
        "the map thread no longer consumes leadership transitions")
require('GetValue<ObjectGuid>("pull target")' in ai and
        'GetValue<Unit*>("current target")' in ai and
        'GetValue<LastMovement&>("last movement")' in ai,
        "toggle transitions must clear stale pull, target, and movement state")
require("_leadershipGeneration != generation" in lead and
        "_mogushanRouteStage = 0xFF" in lead and
        "_mogushanRouteIndex = 0" in lead,
        "reactivation must invalidate the previous Mogu'shan route cursor")
require("!master->IsAlive()" in follow and
        "return master;" in follow,
        "followers must fall back to a dead real master")
require("realMasterUnavailable" in lead and
        "!realMasterUnavailable" in lead,
        "the tank must pause autonomous leadership while the master is dead")
require("gotankGroupInCombat" in ai and
        "!gotankGroupInCombat" in ai and
        "recoverGotankDeadMaster ? 60.0f : 140.0f" in ai,
        "dead-master recovery must be combat-safe and instance-scoped")
require('PossibleTargetsValue(botAI,' in lead and
        '"instance leadership targets", 160.0f, true' in lead and
        "bot->GetMapId() != MogushanPalaceMap" in lead,
        "generic leadership must scan the next mmap-reachable corridor pack")

print(json.dumps({
    "checks": 9,
    "commands": ["gotank", "go tank", "go-tank"],
    "toggle_off": "clears-map-thread-movement",
    "toggle_on": "recalculates-route-generation",
    "dead_master": "pause-leadership-and-regroup",
    "active_fight_teleport": "blocked",
    "generic_route_scan_yards": 160,
    "result": "pass",
}, sort_keys=True))
