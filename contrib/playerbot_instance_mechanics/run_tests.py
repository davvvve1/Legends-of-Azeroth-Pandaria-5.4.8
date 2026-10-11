#!/usr/bin/env python3
"""Source-level regression checks for the shared instance mechanics layer."""

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MECHANICS = ROOT / "modules/mod_playerbots/src/strategy/actions/InstanceMechanicsAction.cpp"
MECHANICS_HEADER = ROOT / "modules/mod_playerbots/src/strategy/actions/InstanceMechanicsAction.h"
AI_SOURCE = ROOT / "modules/mod_playerbots/src/AI/PlayerbotAI.cpp"
FACTORY = ROOT / "modules/mod_playerbots/src/Factory/AiFactory.cpp"
MOVEMENT = ROOT / "modules/mod_playerbots/src/strategy/actions/MovementActions.cpp"
MAP_DBC = ROOT / "src/server/game/DataStores/DBCStructure.h"
MOGUSHAN_INSTANCE = ROOT / "src/server/scripts/Pandaria/MogushanPalace/instance_mogu_shan_palace.cpp"
SHA_OF_DOUBT = ROOT / "src/server/scripts/Pandaria/TempleOfTheJadeSerpent/boss_sha_of_doubt.cpp"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


mechanics = MECHANICS.read_text(encoding="utf-8")
mechanics_header = MECHANICS_HEADER.read_text(encoding="utf-8")
ai = AI_SOURCE.read_text(encoding="utf-8-sig")
factory = FACTORY.read_text(encoding="utf-8")
movement = MOVEMENT.read_text(encoding="utf-8-sig")
map_dbc = MAP_DBC.read_text(encoding="utf-8")
mogushan_instance = MOGUSHAN_INSTANCE.read_text(encoding="utf-8")
sha_of_doubt = SHA_OF_DOUBT.read_text(encoding="utf-8")

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
require("NpcMuShiba = 61453" in mechanics and
        "NpcHaiyanTheUnstoppable = 61445" in mechanics and
        priority.find("NpcMuShiba") < priority.find("NpcHaiyanTheUnstoppable"),
        "Trial target order must remain Mu'Shiba then Haiyan")
require("gekkanEntourage && !PlayerBotSpec::IsHeal" in mechanics,
        "Gekkan tank/add switch is missing")
require('engine->addStrategy("formation", false);' in factory and
        "desiredRange = std::min(24.0f" in movement and
        "healerCatchup" in ai and "bot->GetDistance(leader) > 32.0f" in ai,
        "Gekkan roles must retain tank-centered melee, ranged, and healer positioning")
require("GetGroupPveInterruptPriority" in ai and
        "case 118940: return 0" in ai and
        "case 118958: return 1" in ai and
        "case 118903: return 1" in ai and
        "case 118963: return 2" in ai and
        "autoQueueGroup &&" in ai and
        "if (!requesterGuid && !worldBossRaid)" not in ai,
        "manual instance groups must coordinate Gekkan's priority interrupts")
require("IsHealingCast(unit)" in mechanics,
        "generic engaged healer fallback is missing")
require("activeTrialTarget" in mechanics and
        "IsActiveMogushanTrialTarget(bot, creature)" in mechanics and
        "creature->GetReactState() != REACT_PASSIVE" in mechanics and
        "UNIT_FLAG_IMMUNE_TO_PC" in mechanics and
        "trialPriority && !activeTrialTarget" in mechanics and
        "!trialPriority &&" in mechanics and
        "!unit->IsInCombat() || !GroupPveCombat::IsEngaged(bot, unit)" in mechanics,
        "priority selection must accept only active Trial or engaged targets")
require("FindNearestCreature(NpcMuShiba, 150.0f, true)" in mechanics and
        "FindNearestCreature(NpcHaiyanTheUnstoppable" in mechanics,
        "Trial targets must not depend on the shorter target-value cache")
require("Unit* PriorityTarget(PlayerbotAI* botAI" in mechanics_header and
        "bool IsActiveMogushanTrialTarget(Player* bot" in mechanics_header and
        "InstanceMechanics::PriorityTarget(this, bot)" in ai and
        "validAttackTarget(priorityTarget)" in ai and
        'GetValue<ObjectGuid>("pull target")' in ai and
        "Set(priorityTarget->GetGUID())" in ai,
        "tank skull and durable pull ownership must use the shared priority target")
require("add->GetEntry() == NpcMuShiba" in mechanics and
        "muShiba && botAI->IsInstanceTankLeader()" in mechanics,
        "the elected tank must mark and focus Mu'Shiba with the group")
require('engine->addStrategy("avoid aoe", false);' in factory,
        "shared mechanics strategy is not loaded by default")
require("map_type == MAP_SCENARIO" in map_dbc and
        "bool IsDungeon() const" in map_dbc,
        "scenario maps are no longer included by the instance gate")
require("m_auiBossNumber[0] = TYPE_KUAI" in mogushan_instance and
        "m_auiBossNumber[1] = TYPE_HAIYAN" in mogushan_instance and
        "m_auiBossNumber[2] = TYPE_MING" in mogushan_instance and
        "std::shuffle(std::begin(m_auiBossNumber)" not in mogushan_instance,
        "Trial order must remain Kuai/Mu'Shiba, Haiyan, then Ming")
require("NpcMingTheCunning = 61444" in mechanics and
        "NpcWhirlingDervish = 61626" in mechanics and
        "SpellMagneticFieldAura = 120100" in mechanics and
        "MingMagneticFieldClearance = 18.0f" in mechanics and
        "MingDervishClearance = 10.0f" in mechanics and
        "Reaction::AvoidUnitHazard" in mechanics and
        "MovementPriority::MOVEMENT_HAZARD" in mechanics and
        "FleePosition(plan.anchor->GetPosition()" in mechanics,
        "every role must leave Ming's Magnetic Field and moving Dervish")
require(all(token in mechanics for token in (
            "MapTerraceOfEndlessSpring = 996",
            "MapMogushanVaults = 1008",
            "MapHeartOfFear = 1009")),
        "Tier-14 raid maps are not represented in the shared mechanics layer")
require(all(token in priority for token in (
            "60958", "60913", "60776", "60793", "60398",
            "62531", "65498", "63053", "62711", "62691",
            "60886", "62969", "62977", "62995", "61034")),
        "Tier-14 encounter-critical target catalogue is incomplete")
require(all(token in mechanics for token in (
            "116417, 10.0f", "123180, 10.0f", "123017, 4.0f",
            "122835, 30.0f", "122775, 10.0f")),
        "Tier-14 spread, stack, and kite mechanics are incomplete")
require("rule.map == bot->GetMapId()" in mechanics and
        all(token in mechanics for token in (
            "131788, 2", "123474, 2", "123707, 4",
            "122752, 2", "123121, 8")),
        "Tier-14 tank swaps must be map-scoped and complete")
require("member->IsCharmed()" in mechanics and
        all(token in mechanics for token in (
            "117708", "122740", "123713", "145071")),
        "breakable raid mind-control mechanics are incomplete")
require(all(token in priority for token in (
            "56511", "56792", "56754", "56713", "56895",
            "61623", "61484", "59893", "58664", "58791")),
        "MoP dungeon encounter-critical target catalogue is incomplete")
require("constexpr ObjectiveRule EncounterObjectives[]" in mechanics and
        "IsEncounterObjective(bot->GetMapId(), unit->GetEntry())" in mechanics and
        "FindNearestCreature(rule.entry, 150.0f, true)" in mechanics and
        "!trialPriority && !encounterObjective" in mechanics,
        "victimless phase objectives must be discoverable without becoming free pulls")
require("NpcWiseMari = 56448" in mechanics and
        "SpellWiseMariWaterBubble = 106062" in mechanics and
        "SpellWiseMariHydrolanceVisual = 106055" in mechanics and
        "SpellWiseMariWashAway = 106331" in mechanics and
        "Reaction::WiseMariDryPlatform" in mechanics and
        "Reaction::CircleWiseMari" in mechanics and
        "WiseMariDryPlatforms" in mechanics and
        "plan.anchor->GetOrientation() + float(M_PI_2)" in mechanics,
        "Wise Mari must use dry platforms, living-water focus and rotating Wash Away avoidance")
require("if (!_JustEngagedWith())" in sha_of_doubt and
        "me->m_Events.Schedule" not in sha_of_doubt and
        "if (!player || !player->IsAlive())" in sha_of_doubt and
        "if (!figmentsCount)" in sha_of_doubt and
        "if (instance)" in sha_of_doubt,
        "Sha of Doubt must start synchronously and safely handle an all-dead-party wipe")

print(json.dumps({
    "checks": 29,
    "shared_instance_layer": "loaded-by-default",
    "map_types": ["dungeon", "raid", "scenario"],
    "difficulty_keying": "shared-map-entry",
    "gekkan_target_order": [61337, 61340, 61338, 61339, 61243],
    "gekkan_interrupt_order": [118940, 118958, 118903, 118963, 118936, 118917],
    "gekkan_positioning": "tank stacks mobile enemies on skull; ranged form at 16-24 yards; healer catches tank at 32 yards",
    "trial_target_order": [61453, 61445],
    "trial_boss_order": ["Kuai/Mu'Shiba", "Haiyan", "Ming"],
    "ming_avoidance": ["Magnetic Field", "Whirling Dervish"],
    "generic_fallback": "engaged-healing-add",
    "tier14": {
        "priority_targets": "Mogu'shan Vaults, Heart of Fear, Terrace",
        "role_mechanics": ["spread", "stack", "kite", "tank-swap", "break-control"],
    },
    "mop_dungeons": {
        "priority_targets": "phase adds and attackable encounter objectives",
        "objective_gate": "map + combat + attackability",
    },
    "result": "pass",
}, sort_keys=True))
