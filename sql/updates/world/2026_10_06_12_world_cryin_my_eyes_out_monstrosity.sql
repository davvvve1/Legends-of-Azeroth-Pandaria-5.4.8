-- Monstrosity (29743)
-- The four statue goobers used source-backed credit spells.  In particular the
-- shoulder spell can be rejected without advancing objective 55388.  Preserve
-- the original GO-state trigger and award each creature objective directly.
UPDATE `gameobject_template`
SET `AIName` = 'SmartGameObjectAI', `data1` = 29743
WHERE `entry` IN (212182, 212183, 212184, 212186)
  AND `type` = 10;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55379,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Ancient Statue Torso - Quest 29743 - Give sketch credit directly'
WHERE `entryorguid` = 212182 AND `source_type` = 1 AND `id` = 0
  AND `event_type` = 70 AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55383,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Ancient Statue Arm - Quest 29743 - Give sketch credit directly'
WHERE `entryorguid` = 212183 AND `source_type` = 1 AND `id` = 0
  AND `event_type` = 70 AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55388,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Ancient Statue Shoulder - Quest 29743 - Give sketch credit directly'
WHERE `entryorguid` = 212184 AND `source_type` = 1 AND `id` = 0
  AND `event_type` = 70 AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55392,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Ancient Statue Head - Quest 29743 - Give sketch credit directly'
WHERE `entryorguid` = 212186 AND `source_type` = 1 AND `id` = 0
  AND `event_type` = 70 AND `target_type` = 7;

-- Cryin' My Eyes Out (29765)
-- All four unique belongings are quest-only drops from Hozen Groundpounders.
-- Use a practical 25% rate on both local Groundpounder variants so the quest
-- cannot turn into an excessive RNG grind while still requiring combat/loot.
INSERT INTO `creature_loot_template`
    (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`)
VALUES
    (55470, 74160, -25, '', 0, 1, 1),
    (55470, 74161, -25, '', 0, 1, 1),
    (55470, 74162, -25, '', 0, 1, 1),
    (55470, 74163, -25, '', 0, 1, 1),
    (66917, 74160, -25, '', 0, 1, 1),
    (66917, 74161, -25, '', 0, 1, 1),
    (66917, 74162, -25, '', 0, 1, 1),
    (66917, 74163, -25, '', 0, 1, 1)
ON DUPLICATE KEY UPDATE
    `ChanceOrQuestChance` = VALUES(`ChanceOrQuestChance`),
    `groupid` = VALUES(`groupid`),
    `mincountOrRef` = VALUES(`mincountOrRef`),
    `maxcount` = VALUES(`maxcount`);
