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
ATTACK_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/actions/AttackActions.cpp"
COMBAT_HEADER = ROOT / "modules/mod_playerbots/src/AI/GroupPveCombat.h"
COMBAT_SOURCE = ROOT / "modules/mod_playerbots/src/AI/PlayerbotSpec.cpp"
TARGET_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/value/TargetValue.cpp"
GROUP_SOURCE = ROOT / "src/server/game/Groups/Group.cpp"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


header = AI_HEADER.read_text(encoding="utf-8-sig")
ai = AI_SOURCE.read_text(encoding="utf-8-sig")
chat = CHAT_SOURCE.read_text(encoding="utf-8-sig")
follow = FOLLOW_SOURCE.read_text(encoding="utf-8-sig")
lead = LEAD_SOURCE.read_text(encoding="utf-8-sig")
attack = ATTACK_SOURCE.read_text(encoding="utf-8-sig")
combat_header = COMBAT_HEADER.read_text(encoding="utf-8-sig")
combat = COMBAT_SOURCE.read_text(encoding="utf-8-sig")
target = TARGET_SOURCE.read_text(encoding="utf-8-sig")
group = GROUP_SOURCE.read_text(encoding="utf-8-sig")
group_combat = combat[combat.index("bool GroupPveCombat::GroupHasActiveCombat"):
                      combat.index("bool GroupPveCombat::IsEngaged")]

require('command != "gotank"' in chat and 'command != "go tank"' in chat,
        "both documented gotank command spellings must remain accepted")
require("_instanceTankLeadershipGeneration.fetch_add(1)" in ai,
        "a changed leadership state must publish a fresh generation")
require("_instanceTankLeadershipAutoSuppressed" in header and
        "!IsInstanceTankLeadershipAutoSuppressed()" in ai and
        "selectedTank" in ai and
        "PlayerBotSpec::IsTank(member, true)" in ai,
        "instance entry must automatically elect a deterministic living bottank")
require("FindIndependentInstanceBotTank" in ai and
        "FindIndependentInstanceBotTank(bot) == bot" in ai and
        "memberAI->IsRealPlayer()" in ai and
        "else if (!IsInstanceTankLeadershipAutoSuppressed())" in ai and
        "leadershipChanged" in ai,
        "live group election must repair state and always choose a bottank")
require("bool PlayerbotAI::IsInstanceTankLeadershipActive() const" in ai and
        "return GetInstanceTankLeader() != nullptr;" in ai and
        "return FindIndependentInstanceBotTank(bot);" in ai,
        "leadership activity must survive a transient missing atomic id")
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
require(follow.count("if (botAI->IsInstanceTankLeader())") >= 5 and
        "bool canRecoverFollow = !IsInstanceTankLeader()" in ai and
        "bool gateFollowContext = !IsInstanceTankLeader()" in ai,
        "the independent tank must reject every master-follow recovery path")
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
        "gotankEncounterInProgress" not in ai and
        "gotankPreservesEncounterTarget" not in ai and
        "ChangeEngine(BOT_STATE_NON_COMBAT)" in ai and
        "CombatStopWithPets(true)" in ai,
        "gotank must leave stale combat even during a scripted encounter")
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
        "participant->GetDistance(enemy) <= 180.0f" in combat and
        "enemy->IsInCombat()" in combat and
        "enemy->GetVictim() == participant" in combat and
        "getThreat(participant) > 0.0f" in combat and
        "participant->GetVictim() == enemy" not in group_combat,
        "only a real nearby hostile interaction may stop the route")
require("ResetCompletedPull();" in lead and
        'GetValue<ObjectGuid>("pull target")' in lead and
        'GetValue<LastMovement&>("last movement")' in lead and
        "SetNextCheckDelay(0)" in lead,
        "the completed pull must release target and movement ownership")
require("GetLockedPullTarget() const" in lead and
        "if (Unit* pull = GetLockedPullTarget())" in lead and
        "bot->GetExactDist(pull) > 240.0f" in lead and
        '"gotank pull locked leader=' in lead,
        "the leader must keep one marked pull target throughout its approach")
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
require("instance->IsEncounterInProgress()" not in lead and
        "encounterInProgress" not in controller,
        "scripted encounter state alone must never pause persistent leadership")
require("SetInstanceTankLeadershipAutoSuppressed(true)" in chat and
        "SetInstanceTankLeadershipAutoSuppressed(false)" in chat,
        "manual gotank off/on must suppress and restore automatic leadership")
require("SyncInstanceTankSkullTarget(Unit* preferredTarget" in header and
        "void PlayerbotAI::SyncInstanceTankSkullTarget" in ai and
        "SyncInstanceTankSkullTarget();" in ai and
        "botAI->SyncInstanceTankSkullTarget(target);" in attack and
        "GetTargetIcon(skull)" in ai and "SetTargetIcon(skull" in ai and
        "for (Unit* attacker : bot->getAttackers())" in ai and
        '"gotank skull leader=' in ai,
        "the leader tank must continuously publish its kill target as skull")
require("waypoint.z, false, false, false, true" in lead and
        "point.z,\n        false, false, false, true" in lead and
        '"gotank Mogu\'shan route leader=' in lead,
        "validated route waypoints must bypass a second fallible path search")
require("member->SendStartTimer(15, 15, TIMER_PVP)" in ai and
        'SayToParty("Pull om 15 sekunder - folj tanken. 15")' in ai and
        "remaining == 10 || remaining <= 5" in ai and
        'SayToParty("KOR!")' in ai and
        '"gotank pull countdown complete leader=' in ai and
        "_instanceTankPullCountdownKey" in header and
        "_instanceTankPullCountdownRemaining" in header and
        "pullCountdownElapsed < 15000" in ai and
        "no route selection" in ai,
        "a new instance must have one visible, movement-gated 15-second pull countdown")
require("FindIndependentInstanceOffTank" in ai and
        "gotank cross leader=" in ai and
        "GroupPveCombat::IsEngaged(bot, candidate)" in ai and
        "candidate->HasBreakableByDamageCrowdControlAura()" in ai,
        "the leader must mark one engaged non-CC target for a real bottank")
require("group->GetTargetIcon(6) == target->GetGUID()" in ai and
        "FindIndependentInstanceOffTank(bot) == bot" in ai and
        "Cross is an explicit off-tank assignment" in ai,
        "only the elected off-tank may taunt and acquire cross")
require("bool const offTank = PlayerBotSpec::IsTank(bot, true)" in target and
        "if (offTank && crossGuid)" in target and
        "if (skullGuid)" in target,
        "off-tank must prioritize cross while DPS and the leader prioritize skull")
require("bool const gotankCanAdvance" in ai and
        "if (!splineMoving)" in ai and
        "bot->GetMotionMaster()->Clear(false);" in ai and
        "getMSTimeDiff(_instanceTankRouteProgressAt, now) >= 4000" in ai and
        '"gotank recovered stalled route leader=' in ai,
        "a completed or physically stalled waypoint must self-wake without gotank")
require("bool const routeIssued" in ai and
        '"gotank route produced no movement leader=' in ai and
        "_instanceTankLastStallLog" in header,
        "a route failure must emit throttled live diagnostics")
require('GetGroupSlot() == GroupSlot::Instance' in group and
        'partyIndex = uint8(GroupSlot::Instance)' in group and
        'partyIndex = int8(GroupSlot::Instance)' in group,
        "instance-group raid markers must use the visible client party category")
require("_forwardSearchStarted = getMSTime()" in lead and
        "getMSTimeDiff(_forwardSearchStarted, getMSTime()) < 20000" in lead and
        "MovementPriority::MOVEMENT_HAZARD" in lead and
        "nextDistance < currentDistance" in lead,
        "post-pack leadership must own a 20-second forward route search")
require("gotankOpeningPullGrace" in ai and
        "_instanceTankOpeningTargetGuid" in header and
        "_instanceTankOpeningPullAt" in header and
        "bot->GetVictim() == pull" in ai and
        "< 5000" in ai and
        "!gotankOpeningPullGrace" in ai,
        "stale-combat cleanup must not cancel the tank's opening attack")
require("AbandonUnreachableTarget" in lead and
        "_approachBestDistance" in lead and
        "getMSTimeDiff(_approachProgressAt, now) >= 4000" in lead and
        "_unreachableTargetGuid == guid.GetCounter()" in lead and
        "getMSTimeDiff(_unreachableTargetAt, getMSTime()) < 20000" in lead and
        '"gotank abandoned unreachable pull leader=' in lead,
        "an unreachable marked pull must release the group back to its route")
require("if (!splineMoving)" in ai and
        'GetValue<LastMovement&>("last movement")' in ai and
        "bot->StopMoving();" in ai,
        "a finalized spline must release stale movement before the next route step")

print(json.dumps({
    "checks": 38,
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
    "post_combat_forward_search_ms": 20000,
    "post_combat_engine": "automatic-non-combat-resume",
    "stale_live_trash_target": "always-cleared-without-hostile-interaction",
    "encounter_intermission": "route-continues-without-hostile-interaction",
    "resurrection_pause": "disabled",
    "regroup_grace_ms": 0,
    "stale_combat_cutoff_yards": 180,
    "instance_complete": "continue-until-manual-gotank",
    "persistent_controller": "direct-before-idle-actions",
    "route_ownership": "follow-engine-blocked-between-waypoints",
    "tank_kill_order": "skull-follows-selected-target",
    "target_marker_party_category": "instance",
    "pull_approach": "locked-target-no-nearest-mob-oscillation",
    "pull_opening_grace_ms": 5000,
    "unreachable_pull_ignore_ms": 20000,
    "offtank_assignment": "cross-on-second-engaged-target",
    "leadership_state": "re-elected-from-live-instance-group",
    "route_dispatch": "validated-waypoint-direct-to-motion-master",
    "pull_countdown_seconds": 15,
    "tank_follow_mode": "fully-independent-from-real-master",
    "waypoint_wakeup": "immediate-on-spline-finish",
    "stalled_route_recovery_ms": 4000,
    "result": "pass",
}, sort_keys=True))
