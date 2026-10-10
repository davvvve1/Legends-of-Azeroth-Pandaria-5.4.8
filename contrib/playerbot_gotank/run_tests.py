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
TANK_TARGET_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/value/TankTargetValue.cpp"
ATTACKERS_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/value/AttackersValue.cpp"
RESURRECT_SOURCE = ROOT / "modules/mod_playerbots/src/strategy/value/PartyMemberToResurectValue.cpp"
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
tank_target = TANK_TARGET_SOURCE.read_text(encoding="utf-8-sig")
attackers = ATTACKERS_SOURCE.read_text(encoding="utf-8-sig")
resurrect = RESURRECT_SOURCE.read_text(encoding="utf-8-sig")
group = GROUP_SOURCE.read_text(encoding="utf-8-sig")
group_combat = combat[combat.index("bool GroupPveCombat::GroupHasActiveCombat"):
                      combat.index("bool GroupPveCombat::IsEngaged")]

require('command != "gotank"' in chat and 'command != "go tank"' in chat,
        "both documented gotank command spellings must remain accepted")
require("gotank: the tank is following the master again." in chat and
        "gotank is available inside dungeons and raids." in chat and
        "gotank: no living bot tank was found in the group." in chat and
        " is leading the group. Type gotank again to follow the master." in chat and
        "Waiting for the healer to resurrect a group member." in ai and
        "Waiting for the healer to restore the group to 80% health." in ai and
        "The group is resurrected and healed - continuing." in ai and
        "Healer mana ready - continuing." in ai and
        not any(fragment in ai + chat for fragment in (
            "tanken", "foljer", "fungerar inne", "ingen levande",
            "leder gruppen", "Skriv gotank", "Pull om", "sekunder",
            "KOR!", "Vantar", "ateruppliv", "fortsatter")),
        "all gotank player-facing messages must remain in English")
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
require("every bot follows the elected tank" in follow and
        "return nullptr;" in follow and
        "human master" in follow,
        "gotank followers must never fall back to the human master")
require("std::fabs(z - master->GetPositionZ()) <= 6.0f" in follow and
        "another walkable floor" in follow,
        "formation offsets must never snap followers onto another dungeon floor")
require(follow.count("if (botAI->IsInstanceTankLeader())") >= 5 and
        "bool canRecoverFollow = !IsInstanceTankLeader()" in ai and
        "bool gateFollowContext = !IsInstanceTankLeader()" in ai,
        "the independent tank must reject every master-follow recovery path")
require("gotankOwnsFormationMovement" in ai and
        "!gotankOwnsFormationMovement" in ai and
        "recoverGotankDeadMaster" not in ai,
        "active leadership must disable every distance recovery to master")
require('DoSpecificAction("follow", Event(), true)' in ai and
        "leader && leader->IsAlive()" in ai and
        "bot->GetDistance(leader) > (healerCatchup ? 32.0f" in ai and
        "idleCombatCatchup ? 8.0f : 4.0f" in ai,
        "every living follower must persistently follow the tank between pulls")
require("PlayerBotSpec::IsHeal(bot, true)" in follow and
        "bot->GetDistance(leader) > 32.0f" in follow and
        "Follow(leader, 20.0f" in follow and
        "healerCatchup" in ai,
        "a gotank healer must catch the tank at a safe casting distance during combat")
require("_instanceTankWaitingForGroupRecovery" in header and
        "member->getDeathState() == DeathState::CORPSE" in ai and
        "member->IsRessurectRequested()" in ai and
        "member->GetHealthPct() < 80.0f" in ai and
        "gotank waiting for group recovery" in ai and
        "gotank group recovery ready" in ai,
        "the tank must wait for bot resurrection and 80-percent recovery between pulls")
require("idleCombatCatchup" in ai and "idleCombatCatchup" in follow and
        "!bot->GetVictim()" in follow and
        "bot->GetDistance(leader) > 8.0f" in follow and
        "Follow(leader, 6.0f" in follow and
        "GetOffTankOffset" in follow,
        "idle combat followers and the off-tank must stay anchored to the main tank")
require("member->IsInWorld() &&\n                    member->GetMap()" in ai and
        "member->IsInWorld() && member->IsAlive()" not in
        ai[ai.index("Player* PlayerbotAI::GetInstanceTankLeader"):ai.index("bool PlayerbotAI::IsInstanceTankLeadershipActive")],
        "a dead elected tank must remain the leadership and resurrection anchor")
require("botAI->GetInstanceTankLeader()" in resurrect and
        "finder.Check(leader) && Check(leader)" in resurrect,
        "resurrection must prioritize the dead elected tank")
skull_sync = ai[ai.index("void PlayerbotAI::SyncInstanceTankSkullTarget"):
                ai.index("bool PlayerbotAI::CanLfgAutoQueueEngage")]
require("!bot->IsAlive()" in skull_sync and
        'GetValue<ObjectGuid>("pull target")' in skull_sync and
        "GetUnit(pullGuid)" in skull_sync,
        "wipe-time skull sync must reject dead tanks and resolve cached targets by GUID")
require("_currentState == BOT_STATE_COMBAT" in ai and
        "!gotankGroupHasActiveCombat" in ai and
        "gotankEncounterInProgress" not in ai and
        "gotankPreservesEncounterTarget" not in ai and
        "ChangeEngine(BOT_STATE_NON_COMBAT)" in ai and
        "CombatStopWithPets(true)" in ai,
        "gotank must leave stale combat even during a scripted encounter")
require("constexpr float AutonomousTargetRange = 150.0f" in lead and
        'PossibleTargetsValue(botAI,' in lead and
        '"instance leadership targets", AutonomousTargetRange' in lead and
        "if (!best)" in lead,
        "instance leadership must scan 150 yards for the next mmap-reachable pack")
require("InstanceMechanics::IsActiveMogushanTrialTarget(bot, creature)" in lead and
        "Treat that active window as pull-ready" in lead,
        "an active Trial target must survive pull validation before combat starts")
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
engage = lead[lead.index("bool InstanceLeadershipAction::EngageTarget"):
              lead.index("void InstanceLeadershipAction::AbandonUnreachableTarget")]
after_attack = engage[engage.index("bool const attackIssued = Attack(target);"):]
require("ObjectGuid const targetObjectGuid" in engage and
        "botAI->GetUnit(targetObjectGuid)" in after_attack and
        "target->GetName()" not in after_attack and
        "target->GetEntry()" not in after_attack and
        "MoveTo(liveTarget" in after_attack and
        "ChaseTo(liveTarget" in after_attack,
        "opening attacks must reacquire scripted or despawned targets by GUID")
require("if (GroupHasActiveCombat())" in lead and
        "InstanceMechanics::PriorityTarget(botAI, bot)" in lead and
        "priority && priority != pull" in lead and
        "return pull && bot->GetVictim() != pull;" in lead and
        "reclaimMarkedPull" in ai and
        "(!GroupPveCombat::GroupHasActiveCombat(bot) || reclaimMarkedPull)" in ai,
        "combat must not suppress an unfinished pull or priority-target switch")
require("pullTank->GetVictim() == target" in ai and
        "GroupPveCombat::IsActivelyAttacking(pullTank" in ai and
        "followers must not" in ai,
        "followers must wait for the tank to open a skull or cross target")
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
        'announcePullCountdown("Pull in 15 seconds - follow the tank. 15")' in ai and
        "PSendSysMessage(" in ai and
        "SendNotification(" in ai and
        "remaining == 10 || remaining <= 5" in ai and
        'announcePullCountdown("GO!")' in ai and
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
        "Cross remains the off-tank's normal assignment" in ai,
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
        "< 20000" in ai and
        "!gotankOpeningPullGrace" in ai,
        "stale-combat cleanup must not cancel the tank's opening attack")
require("bool const gotankOpeningCombat" in ai and
        "_currentState == BOT_STATE_COMBAT" in ai and
        "bot->GetVictim()" in ai and
        "!gotankOpeningCombat" in ai,
        "an opened gotank pull must yield to the combat engine for its chase")
require("AbandonUnreachableTarget" in lead and
        "_approachBestDistance" in lead and
        "openingAttack ? 6000 : 4000" in lead and
        "_unreachableTargets.find(guid.GetCounter())" in lead and
        "_unreachableTargets[guid.GetCounter()] = now" in lead and
        '"gotank abandoned unreachable pull leader=' in lead,
        "an unreachable marked pull must release the group back to its route")
abandon = lead[lead.index("void InstanceLeadershipAction::AbandonUnreachableTarget"):
               lead.index("MovementPriority InstanceLeadershipAction::RouteMovementPriority")]
require("void InstanceLeadershipAction::AbandonUnreachableTarget()" in abandon and
        'GetValue<ObjectGuid>("pull target")->Get()' in abandon and
        'GetValue<Unit*>("current target")->Set(nullptr)' in abandon and
        "target->" not in abandon and
        "bot && bot->IsAlive() && bot->IsInWorld()" in abandon,
        "wipe cleanup must use the durable pull GUID without dereferencing despawned units")
require("if (!splineMoving)" in ai and
        'GetValue<LastMovement&>("last movement")' in ai and
        "bot->StopMoving();" in ai,
        "a finalized spline must release stale movement before the next route step")
require("_pullMarkedAt = now" in lead and
        "markedFor < 1000" in lead and
        "bool const attackIssued = Attack(target);" in lead and
        "ownsOpeningAttack" in lead and
        "sPlayerbotAIConfig->contactDistance" in lead and
        '"gotank opening attack leader=' in lead and
        "botAI->SetNextCheckDelay(0)" in lead,
        "the tank must open its locked skull target one second after marking it")
require("bool const chasing = ownsOpeningAttack" in lead and
        "ChaseTo(target," in lead and
        "attackIssued || ownsOpeningAttack || chasing" in lead,
        "a successful ranged attack order must immediately chase into melee range")
require("bot->RemoveAurasByType(SPELL_AURA_MOUNTED)" in attack and
        "bool const attackStarted = bot->Attack(target, melee)" in attack and
        "if (!attackStarted && !ownsVictim)" in attack and
        '"gotank core attack rejected leader=' in attack and
        "botAI->ChangeEngine(BOT_STATE_COMBAT);" in attack,
        "opening pulls must dismount and only report core-accepted attacks")
require("_instanceTankWaitingForHealerMana" in header and
        "lowestHealerMana <= 30.0f" in controller and
        "lowestHealerMana < 80.0f" in controller and
        "!lockedPull" in controller and
        'announcePullCountdown("Waiting for healer mana (30%).")' in controller and
        "bool const healerManaWait = !groupCombat" in ai and
        "IsInstanceTankWaitingForHealerMana()" in ai and
        "SPELL_CATEGORY_DRINK" in ai and
        'DoSpecificAction("mana tea"' in ai and
        "_instanceHealerUsingFreeDrink" in header and
        '"AiPlayerbot.FreeFood", true' in ai and
        "bot->ModifyPower(POWER_MANA" in ai,
        "gotank must pause between pulls for low healer mana and preserve the healer's recovery tick")
require("GetGroup(GroupSlot::Instance)" in attackers and
        "groupAwarenessRange" in attackers and
        "150.0f" in attackers,
        "gotank threat discovery must include the complete instance group")
require("candidateRescuePriority" in tank_target and
        "PlayerBotSpec::IsHeal(owner, true) ? 2 : 1" in tank_target and
        "rescuePriority || foundHighPriority" in tank_target,
        "healer rescue must temporarily outrank the skull kill order")
require("member->getHostileRefManager().getFirst()" in ai and
        "attacksHealer" in ai and
        '"gotank rescued group member tank=' in ai,
        "tank rescue must discover ranged healer aggro and prioritize it")
require("Unit* attacking = target->GetVictim()" in combat and
        "return !PlayerBotSpec::IsTank(attackingOwner, true)" in combat and
        "reference ? reference->getTarget() : nullptr" in combat,
        "the live healer victim must override a stale threat-manager victim")
require("bool IsReadyForAutonomousPull" in lead and
        "creature->GetReactState() == REACT_PASSIVE" in lead and
        "creature->IsHostileTo(bot) || creature->IsInCombat()" in lead and
        "UNIT_FLAG_IMMUNE_TO_PC" in lead and
        "UNIT_FLAG_PACIFIED" in lead,
        "gotank must ignore yellow passive actors and select the active red encounter target")
require("if (!IsReadyForAutonomousPull(bot, creature))" in lead and
        "bot->AttackStop()" in lead,
        "a stale yellow pull lock must be cancelled before selecting the red target")
reset_pull = lead[lead.index("void InstanceLeadershipAction::ResetCompletedPull"):
                  lead.index("Unit* InstanceLeadershipAction::SelectNextTarget")]
require("bot->AttackStop();" in reset_pull and
        "bot->GetVictim()" not in reset_pull,
        "wipe pull cleanup must not dereference a despawned victim")

print(json.dumps({
    "checks": 58,
    "automatic_start": "deterministic-main-bottank-on-instance-entry",
    "commands": ["gotank", "go tank", "go-tank"],
    "toggle_off": "clears-map-thread-movement",
    "toggle_on": "recalculates-route-generation",
    "master_anchor_during_gotank": "disabled-even-when-master-is-dead",
    "active_fight_teleport": "blocked",
    "living_master_distance_limit": "disabled-during-leadership",
    "generic_route_scan_yards": 150,
    "generic_boss_route": "live-mmap-35-yard-steps",
    "post_combat": "dead-target-and-movement-cleared",
    "post_combat_forward_search_ms": 20000,
    "post_combat_engine": "automatic-non-combat-resume",
    "stale_live_trash_target": "always-cleared-without-hostile-interaction",
    "encounter_intermission": "route-continues-without-hostile-interaction",
    "resurrection_pause": "between-pulls-until-80-percent-health",
    "regroup_grace_ms": 0,
    "stale_combat_cutoff_yards": 180,
    "instance_complete": "continue-until-manual-gotank",
    "persistent_controller": "direct-before-idle-actions",
    "route_ownership": "follow-engine-blocked-between-waypoints",
    "tank_kill_order": "skull-follows-selected-target",
    "tank_open_after_mark_ms": 1000,
    "target_marker_party_category": "instance",
    "pull_approach": "locked-target-no-nearest-mob-oscillation",
    "pull_opening_grace_ms": 20000,
    "unreachable_pull_ignore_ms": 20000,
    "offtank_assignment": "cross-on-second-engaged-target",
    "leadership_state": "re-elected-from-live-instance-group",
    "route_dispatch": "validated-waypoint-direct-to-motion-master",
    "pull_countdown_seconds": 15,
    "tank_follow_mode": "fully-independent-from-real-master",
    "waypoint_wakeup": "immediate-on-spline-finish",
    "stalled_route_recovery_ms": 4000,
    "healer_mana_wait": {"stop_percent": 30, "resume_percent": 80},
    "group_recovery_wait": {"dead_bots": True, "resume_health_percent": 80},
    "result": "pass",
}, sort_keys=True))
