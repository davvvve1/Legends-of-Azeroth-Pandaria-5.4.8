-- I Have No Jade And I Must Scream (29928): each Puckish Sprite carries four
-- chunks. One is always lootable from the corpse; two or three are scattered
-- on death depending on whether the sprite successfully threw one in combat.
UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_jade_forest_puckish_sprite'
WHERE `entry` = 56349;

DELETE FROM `smart_scripts`
WHERE `source_type` = 0
  AND `entryorguid` = 56349;

-- The corpse always contains exactly one quest chunk.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = -100,
    `mincountOrRef` = 1,
    `maxcount` = 1
WHERE `entry` = 56349
  AND `item` = 76209;

-- Each static or scattered Chunk of Jade object represents one chunk.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 1,
    `maxcount` = 1
WHERE `entry` = 40514
  AND `item` = 76209;

DELETE FROM `spell_script_names`
WHERE `spell_id` IN (105912, 105918);

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`)
VALUES (105912, 'spell_jade_forest_drop_jade');
