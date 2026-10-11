#!/usr/bin/env python3
"""Source regression checks for Lost Sheepie (31338)."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
sql = (root / "sql/updates/world/2026_10_11_07_world_lost_sheepie.sql").read_text(
    encoding="utf-8"
)

assert "WHERE `entry` = 64385" in sql
assert "`npcflag` = `npcflag` | 1" in sql
assert "`AIName` = 'SmartAI'" in sql
assert "(64385,0,0,1,64" in sql
assert "(64385,0,1,0,61" in sql
assert "56,86446,1" in sql
assert "(22,1,64385,0,0,9,0,31338" in sql
assert "(22,1,64386,0,0,9,0,31339" in sql

print("Lost Sheepie: right-click collection and active-quest gates passed")
