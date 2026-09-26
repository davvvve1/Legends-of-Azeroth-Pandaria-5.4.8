-- Auchenai Crypts: guaranteed drop for its existing collectible quest loot.
-- Exarch Maladaar's Mysterious Grimoire starts An Intriguing Grimoire;
-- keep positive chance so it can drop before that quest is accepted.
-- The Exarch's Soul Gem (33836) is already -100 on heroic (quest-required).
-- Preserve existing difficulty masks, eligibility, counts and ordinary loot.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100
WHERE `entry` = 18373 AND `item` = 71637 AND `lootmode` = 'DUNGEON_NORMAL'
  AND `mincountOrRef` = 1 AND `groupid` = 0;
