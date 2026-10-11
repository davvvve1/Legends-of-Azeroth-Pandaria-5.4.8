#!/usr/bin/env python3
"""Source and database regression checks for Weed War / Weed War II."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/server/scripts/Pandaria/zone_valley_of_the_four_winds.cpp"
SQL = ROOT / "sql/updates/world/2026_10_11_00_world_weed_war.sql"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


source = SOURCE.read_text(encoding="utf-8-sig")
sql = SQL.read_text(encoding="utf-8-sig")

require("constexpr uint32 Quest = 30052" in source and
        "constexpr uint32 DailyQuest = 30321" in source,
        "both Weed War quest IDs must remain supported")
require("constexpr uint32 QuestCredit = 57358" in source and
        "constexpr uint32 DailyQuestCredit = 59524" in source,
        "each quest must award its own objective credit")
require("SPELL_AURA_PERIODIC_DUMMY" in source and
        "std::min<uint32>(4, 16 - outstanding)" in source and
        "player->GetGUID()" in source,
        "the periodic aura must maintain a bounded set of personal weeds")
require("void OnSpellClick(Unit* clicker, bool& result) override" in source and
        "summon->GetSummonerGUID() != player->GetGUID()" in source and
        "KilledMonsterCredit(WeedWar::QuestCredit)" in source and
        "KilledMonsterCredit(WeedWar::DailyQuestCredit)" in source,
        "only the owning player may click a weed and receive objective credit")
require("(57385,0,3,0,61,0,100,0" in sql and
        "85,114494" in sql and "target_type" in sql,
        "Gai Lan's linked gossip action must cast Weed War on its invoker")
require("(15,13334,0,0,0,9,0,30052" in sql and
        "(15,13334,0,0,1,9,0,30321" in sql,
        "Gai Lan's event option must be visible for either active quest")
require("(114494,'spell_vfw_weed_war')" in sql and
        "WHERE `entry` IN (57306,57308)" in sql and
        "'npc_vfw_weed_war_weed'" in sql,
        "the aura and both weed templates must stay bound to their scripts")

print("Weed War: event aura, personal clickable weeds and both quest credits verified")
