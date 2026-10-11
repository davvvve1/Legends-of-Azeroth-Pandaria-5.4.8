#!/usr/bin/env python3
"""Source and SQL regression checks for both Bilgewater Buccaneer quests."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/server/scripts/Maelstrom/zone_kezan.cpp"
SQL = ROOT / "sql/updates/world/2026_10_11_08_world_bilgewater_buccaneer.sql"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


source = SOURCE.read_text(encoding="utf-8-sig")
sql = SQL.read_text(encoding="utf-8-sig")

require("QUEST_NECESSARY_ROUGHNESS" in source and "24502" in source,
        "Necessary Roughness must remain part of the Buccaneer quest script")
require("NPC_NECESSARY_ROUGHNESS_CREDIT = 48271" in source and
        "KilledMonsterCredit(NPC_NECESSARY_ROUGHNESS_CREDIT)" in source,
        "boarding must award the Necessary Roughness vehicle objective")
require("SetControlled(false, UNIT_STATE_ROOT)" in source,
        "the Necessary Roughness vehicle must be released from the stale SmartAI root")
require("SetSpeed(MOVE_RUN, 0.001f)" not in source,
        "Fourth and Goal must never restore the near-zero movement workaround")
require("player->VehicleSpellInitialize()" in source,
        "boarding must initialize the player's vehicle action bar")
require("GetBuccaneerRider(GetCaster(), Kezan::NPC_BILGEWATER_BUCCANEER)" in source and
        "AfterCast += SpellCastFn" in source,
        "a successful Footbomb cast must credit the actual rider")
require("WHERE `ID` IN (24502, 24503, 28414)" in sql and
        "(70052, 'spell_kezan_fourth_and_goal_kick')" in sql,
        "the live SQL migration must bind both quests and the Footbomb script")

print("Bilgewater Buccaneer: control, boarding and quest-credit checks passed")
