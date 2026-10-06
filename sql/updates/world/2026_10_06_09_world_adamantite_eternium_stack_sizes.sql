-- Burning Crusade mining: when Eternium Ore drops from an Adamantite node,
-- use the intended stack range without changing its drop chance.
UPDATE `gameobject_loot_template`
SET `MinCountOrRef` = 1, `MaxCount` = 5
WHERE `Entry` = 181556 AND `Item` = 23427;

UPDATE `gameobject_loot_template`
SET `MinCountOrRef` = 1, `MaxCount` = 6
WHERE `Entry` = 181569 AND `Item` = 23427;
