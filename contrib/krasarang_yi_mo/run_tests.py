#!/usr/bin/env python3
"""Regression checks for Finding/Cheer Up, Yi-Mo."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/server/scripts/Pandaria/zone_krasarang_wilds.cpp"
SQL = ROOT / "sql/updates/world/2026_10_03_05_world_cheer_up_yi_mo.sql"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


source = SOURCE.read_text(encoding="utf-8-sig")
sql = SQL.read_text(encoding="utf-8-sig")

starter = source[source.index("class npc_cheer_up_yi_mo_starter"):
                 source.index("// Rolling Yi-Mo 57310")]
hello = starter[starter.index("bool OnGossipHello"):
                starter.index("bool OnGossipSelect")]

require("QUEST_FINDING_YI_MO                  = 30080" in source,
        "Finding Yi-Mo quest ID must remain explicit")
require("NPC_YI_MO_FINDING_CREDIT             = 57745" in source,
        "Finding Yi-Mo speak-to credit must remain explicit")
require("QUEST_STATUS_INCOMPLETE" in hello and
        "KilledMonsterCredit(NPC_YI_MO_FINDING_CREDIT)" in hello,
        "talking to real Yi-Mo must complete the invisible speak-to objective")
require(hello.index("KilledMonsterCredit(NPC_YI_MO_FINDING_CREDIT)") <
        hello.index("PrepareGossipMenu(creature, 13354, true)"),
        "Finding Yi-Mo must complete before the reward menu is prepared")
require("player->PrepareGossipMenu(creature, 13354, true)" in hello,
        "Yi-Mo must preserve questgiver entries alongside custom gossip")
require("WHERE `entry` = 58376" in sql and
        "`ScriptName` = 'npc_cheer_up_yi_mo_starter'" in sql,
        "the real Yi-Mo spawn must remain bound to the combined quest script")

print("Finding Yi-Mo: speak credit precedes reward gossip; Cheer Up flow preserved")
