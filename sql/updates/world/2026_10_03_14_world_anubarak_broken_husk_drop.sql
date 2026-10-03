-- Death to the Traitor King (29807/13167): the quest-conditioned -100 rows
-- could be filtered out by loot eligibility. Make Anub'arak's Broken Husk an
-- unconditional guaranteed drop in both normal and heroic Azjol-Nerub.

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=100
WHERE `entry`=29120 AND `item`=43411;
