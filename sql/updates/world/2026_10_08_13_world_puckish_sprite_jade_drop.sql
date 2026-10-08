-- Puckish Sprite (56349) casts dummy spell 105918 on death. The old
-- spell_scripts rows never run for a dummy effect, so no Chunk of Jade
-- gameobjects are created for I Have No Jade And I Must Scream (29928).
DELETE FROM `spell_scripts`
WHERE `id` = 105918;

DELETE FROM `spell_script_names`
WHERE `spell_id` = 105918;

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`)
VALUES (105918, 'spell_jade_forest_drop_jade_cover');
