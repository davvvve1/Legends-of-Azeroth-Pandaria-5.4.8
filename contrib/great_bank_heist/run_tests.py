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
require(source, "OBJECTIVE_BANK_VAULT            = 266678", "vault objective constant")
require(source, "ITEM_PERSONAL_RICHES            = 46858", "item constant")
require(source, "status != QUEST_STATUS_INCOMPLETE", "incomplete-quest recovery guard")
require(source, "player->GetQuestObjectiveCounter(OBJECTIVE_BANK_VAULT) < 1", "vault-credit guard")
require(source, "player->HasItemCount(ITEM_PERSONAL_RICHES)", "duplicate-item guard")
require(source, "player->AddItem(ITEM_PERSONAL_RICHES, 1);", "item recovery")
require(source, "void OnQuestObjectiveChange", "objective progress recovery hook")
require(source, "objective->ID == Kezan::OBJECTIVE_BANK_VAULT", "vault objective hook filter")
require(source, "new quest_kezan_great_bank_heist();", "quest script registration")
require(source, "new player_kezan_great_bank_heist_recovery();", "login recovery registration")
require(sql, "SET `ScriptName` = 'quest_kezan_great_bank_heist'", "quest script binding")
require(sql, "WHERE `ID` = 14122", "quest binding target")

SMARTAI_SQL = ROOT / "sql/updates/world/2026_10_11_10_world_great_bank_heist_reward_order.sql"
smartai_sql = SMARTAI_SQL.read_text()
require(smartai_sql, "`action_type` = 56", "immediate item action")
require(smartai_sql, "`action_param1` = 46858", "Personal Riches action item")
require(smartai_sql, "`id` = 0", "immediate reward row")
require(smartai_sql, "`action_type` = 33", "delayed vault credit action")
require(smartai_sql, "`id` = 5", "delayed credit row")

print("Great Bank Heist regression checks passed")
