-- The Lesson of the Balanced Rock (29663) uses two creature entries as
-- passengers on alternating poles. Entry 65468 retained 42,299 attack power
-- and its old SmartAI, allowing it to one-shot level-5 trainees. Route both
-- entries through the same non-lethal scripted sparring behavior.

UPDATE `creature_template`
SET `mindmg` = 1,
    `maxdmg` = 2,
    `attackpower` = 0,
    `dmg_multiplier` = 1,
    `AIName` = '',
    `ScriptName` = 'npc_tushui_monk'
WHERE `entry` IN (55019, 65468);

DELETE FROM `smart_scripts`
WHERE (`entryorguid` IN (55019, 65468) AND `source_type` = 0)
   OR (`entryorguid` IN (5501900, 6546800) AND `source_type` = 9);
