-- The Meat They'll Eat (29913): Mushan Shoulder Steak (75275) should
-- always drop from Adolescent Mushan while the player needs the quest item.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = -100
WHERE `entry` = 56239
  AND `item` = 75275
  AND `ChanceOrQuestChance` = -41
  AND `mincountOrRef` = 1
  AND `maxcount` = 1;
