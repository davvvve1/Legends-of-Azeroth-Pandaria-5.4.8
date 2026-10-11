#!/usr/bin/env python3

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "src/server/scripts/Maelstrom/zone_kezan.cpp"
SQL = ROOT / "sql/updates/world/2026_10_11_09_world_great_bank_heist_personal_riches.sql"


def require(text: str, needle: str, description: str) -> None:
    if needle not in text:
        raise AssertionError(f"missing {description}: {needle}")


source = SOURCE.read_text()
sql = SQL.read_text()

require(source, "QUEST_GREAT_BANK_HEIST          = 14122", "quest constant")
require(source, "ITEM_PERSONAL_RICHES            = 46858", "item constant")
require(source, "player->GetQuestStatus(QUEST_GREAT_BANK_HEIST) != QUEST_STATUS_COMPLETE", "completed-quest guard")
require(source, "player->HasItemCount(ITEM_PERSONAL_RICHES)", "duplicate-item guard")
require(source, "player->AddItem(ITEM_PERSONAL_RICHES, 1);", "item recovery")
require(source, "new quest_kezan_great_bank_heist();", "quest script registration")
require(source, "new player_kezan_great_bank_heist_recovery();", "login recovery registration")
require(sql, "SET `ScriptName` = 'quest_kezan_great_bank_heist'", "quest script binding")
require(sql, "WHERE `ID` = 14122", "quest binding target")

print("Great Bank Heist regression checks passed")
