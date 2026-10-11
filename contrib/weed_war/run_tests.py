#!/usr/bin/env python3
"""Source and database regression checks for Weed War / Weed War II."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/server/scripts/Pandaria/zone_valley_of_the_four_winds.cpp"
SQL = ROOT / "sql/updates/world/2026_10_11_00_world_weed_war.sql"
COMBAT_SQL = ROOT / "sql/updates/world/2026_10_11_01_world_weed_war_attackable.sql"
HEALTH_SQL = ROOT / "sql/updates/world/2026_10_11_02_world_weed_war_health.sql"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


source = SOURCE.read_text(encoding="utf-8-sig")
sql = SQL.read_text(encoding="utf-8-sig")
combat_sql = COMBAT_SQL.read_text(encoding="utf-8-sig")
health_sql = HEALTH_SQL.read_text(encoding="utf-8-sig")

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
require("constexpr float GaiLanX = -257.535f" in source and
        "constexpr float GaiLanY = 1164.85f" in source and
        "constexpr float EventRadius = 350.0f" in source,
        "the event boundary must contain Gai Lan's complete farm")
require("class player_weed_war_recovery : public PlayerScript" in source and
        "WeedWar::EnsureEventAura(player)" in source and
        "new player_weed_war_recovery()" in source,
        "active Weed War players must recover a missing or expired event aura")
require("void OnSpellClick(Unit* clicker, bool& result) override" in source and
        "summon->GetSummonerGUID() != player->GetGUID()" in source and
        "KilledMonsterCredit(WeedWar::QuestCredit)" in source and
        "KilledMonsterCredit(WeedWar::DailyQuestCredit)" in source,
        "only the owning player may click a weed and receive objective credit")
require("me->SetFaction(14)" in source and
        "void JustDied(Unit* /*killer*/) override" in source and
        "AwardCredit(owner)" in source,
        "weeds must be hostile and grant their owner credit when killed")
require("constexpr uint32 WeedHealth = 1000" in source and
        "me->SetCreateHealth(WeedWar::WeedHealth)" in source and
        "me->SetMaxHealth(WeedWar::WeedHealth)" in source and
        "me->SetHealth(WeedWar::WeedHealth)" in source,
        "every spawned weed must have exactly 1000 health")
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
require("SET `faction` = 14" in combat_sql and
        "WHERE `entry` IN (57306,57308)" in combat_sql,
        "both weed templates must remain hostile after a restart")
require("SET `Health_mod` = 0.00774923" in health_sql and
        "WHERE `entry` IN (57306,57308)" in health_sql,
        "both weed templates must have a 1000-health database fallback")

print("Weed War: personal hostile weeds have exactly 1000 health")
