-- Arconiss Thirsts (30791): Clotted Rodent's Blood (81260) was configured
-- as a 40% quest-only drop from Swamp Rodents (60733), allowing severe dry
-- streaks on a quest that only needs four samples. Keep it quest-only while
-- making each eligible Swamp Rodent provide one sample.

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = -100,
    `mincountOrRef` = 1,
    `maxcount` = 1
WHERE `entry` = 60733
  AND `item` = 81260;
