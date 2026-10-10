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
COMBAT_HEADER = ROOT / "modules/mod_playerbots/src/AI/GroupPveCombat.h"
COMBAT_SOURCE = ROOT / "modules/mod_playerbots/src/AI/PlayerbotSpec.cpp"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


header = AI_HEADER.read_text(encoding="utf-8-sig")
ai = AI_SOURCE.read_text(encoding="utf-8-sig")
chat = CHAT_SOURCE.read_text(encoding="utf-8-sig")
follow = FOLLOW_SOURCE.read_text(encoding="utf-8-sig")
lead = LEAD_SOURCE.read_text(encoding="utf-8-sig")
combat_header = COMBAT_HEADER.read_text(encoding="utf-8-sig")
combat = COMBAT_SOURCE.read_text(encoding="utf-8-sig")

require('command != "gotank"' in chat and 'command != "go tank"' in chat,
        "both documented gotank command spellings must remain accepted")
require("_instanceTankLeadershipGeneration.fetch_add(1)" in ai,
        "a changed leadership state must publish a fresh generation")
require("_instanceTankLeadershipAutoSuppressed" in header and
        "!IsInstanceTankLeadershipAutoSuppressed()" in ai and
        "selectedTank" in ai and
        "PlayerBotSpec::IsTank(member, true)" in ai,
        "instance entry must automatically elect a deterministic living bottank")
require('DoSpecificAction("lead instance", Event(), true)' in ai and
        'DoSpecificAction("lead instance", Event(), true)' in
        ai[:ai.index("// Update internal AI")],
        "persistent leadership must run before the ordinary non-combat engine")
controller = ai[ai.index("// Instance leadership is a persistent controller"):
                ai.index("// Update internal AI")]
require('&&\n        DoSpecificAction("lead instance"' not in controller and
        'DoSpecificAction("lead instance", Event(), true);' in controller and
        'GetValue<LastMovement&>("last movement")' in controller and
        'YieldThread(GetReactDelay());\n        return;' in controller,
        "leadership must own the tick while movement is in flight and clear a stationary latch")
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
require("gotankGroupInCombat" in ai and
        "!gotankGroupInCombat" in ai and
        "!IsInstanceTankLeader()" in ai and
        "recoverGotankDeadMaster ? 60.0f : 140.0f" in ai,
        "followers must recover a dead master without stopping the leader")
require("gotankOwnsFormationMovement" in ai and
        "(!gotankOwnsFormationMovement || recoverGotankDeadMaster)" in ai,
        "active leadership must not be bounded by distance to a living master")
require("_currentState == BOT_STATE_COMBAT" in ai and
        "!gotankGroupHasActiveCombat" in ai and
        "gotankEncounterInProgress" in ai and
        "!gotankPreservesEncounterTarget" in ai and
        "ChangeEngine(BOT_STATE_NON_COMBAT)" in ai and
        "CombatStopWithPets(true)" in ai,
        "gotank must leave stale trash combat but preserve boss intermissions")
require('PossibleTargetsValue(botAI,' in lead and
        '"instance leadership targets", 160.0f, true' in lead and
        "bot->GetMapId() != MogushanPalaceMap" in lead,
        "generic leadership must scan the next mmap-reachable corridor pack")
require("GroupHasActiveCombat(Player* observer)" in combat_header and
        "GroupPveCombat::GroupHasActiveCombat(bot)" in lead and
        "GroupPveCombat::GroupHasActiveCombat(bot)" in follow and
        "!bot->IsInCombat()" not in lead,
        "gotank must use live enemies rather than a stale core combat flag")
require("enemy->IsAlive()" in combat and
        "member->GetDistance(enemy) <= 180.0f" in combat,
        "dead or remote stale hostile references must not stop the route")
require("ResetCompletedPull();" in lead and
        'GetValue<ObjectGuid>("pull target")' in lead and
        'GetValue<LastMovement&>("last movement")' in lead and
        "SetNextCheckDelay(0)" in lead,
        "the completed pull must release target and movement ownership")
require("GroupIsReady" not in lead and "_groupWaitStarted" not in lead and
        "GroupNeedsResurrection" not in lead,
        "leadership must not pause between packs for regrouping or resurrection")
require("GetDungeonEncounterList" in lead and
        "GetCompletedEncounterMask" in lead and
        "path.SetPathLengthLimit(4000.0f)" in lead and
        "walked >= 35.0f" in lead,
        "generic instances must advance toward the next encounter over mmap steps")
require("IsInstanceComplete" not in lead and
        "FinishLeadership" not in lead and
        "SetInstanceTankLeader(0)" not in lead,
        "completion must not stop or clear leadership inside the instance")
require("instance->IsEncounterInProgress()" in lead,
        "persistent route execution must preserve scripted boss intermissions")
require("SetInstanceTankLeadershipAutoSuppressed(true)" in chat and
        "SetInstanceTankLeadershipAutoSuppressed(false)" in chat,
        "manual gotank off/on must suppress and restore automatic leadership")

print(json.dumps({
    "checks": 21,
    "automatic_start": "deterministic-main-bottank-on-instance-entry",
    "commands": ["gotank", "go tank", "go-tank"],
    "toggle_off": "clears-map-thread-movement",
    "toggle_on": "recalculates-route-generation",
    "dead_master": "followers-recover-while-leader-continues",
    "active_fight_teleport": "blocked",
    "living_master_distance_limit": "disabled-during-leadership",
    "generic_route_scan_yards": 160,
    "generic_boss_route": "live-mmap-35-yard-steps",
    "post_combat": "dead-target-and-movement-cleared",
    "post_combat_engine": "automatic-non-combat-resume",
    "stale_live_trash_target": "cleared-unless-encounter-active",
    "resurrection_pause": "disabled",
    "regroup_grace_ms": 0,
    "stale_combat_cutoff_yards": 180,
    "instance_complete": "continue-until-manual-gotank",
    "persistent_controller": "direct-before-idle-actions",
    "route_ownership": "follow-engine-blocked-between-waypoints",
    "result": "pass",
}, sort_keys=True))
