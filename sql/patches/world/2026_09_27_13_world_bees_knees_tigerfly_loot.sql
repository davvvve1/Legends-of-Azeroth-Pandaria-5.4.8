-- The Bees' Knees (29933): guarantee Bug Leg drops while needed for the quest.
-- Negative chance keeps quest-only eligibility; preserve the existing 6-10 count.
INSERT INTO creature_loot_template
    (entry, item, ChanceOrQuestChance, lootmode, groupid, mincountOrRef, maxcount)
VALUES (56283, 76173, -100, 0, 0, 6, 10)
ON DUPLICATE KEY UPDATE ChanceOrQuestChance = -100, groupid = 0,
    mincountOrRef = 6, maxcount = 10;
