-- Guarantee the three requested classic cloth drops wherever they already
-- exist. Moving a cloth row out of an exclusive group makes 100% literal
-- without changing which creatures or containers are eligible to drop it.
-- 2592 Wool Cloth, 4306 Silk Cloth, 4338 Mageweave Cloth.

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100,
    `groupid` = 0
WHERE `item` IN (2592, 4306, 4338);

UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `groupid` = 0
WHERE `item` IN (2592, 4306, 4338);

UPDATE `item_loot_template`
SET `ChanceOrQuestChance` = 100,
    `groupid` = 0
WHERE `item` IN (2592, 4306, 4338);

UPDATE `pickpocketing_loot_template`
SET `ChanceOrQuestChance` = 100,
    `groupid` = 0
WHERE `item` IN (2592, 4306, 4338);
