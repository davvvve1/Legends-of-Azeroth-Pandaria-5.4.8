-- Bilgewater Buccaneer: make both Necessary Roughness and Fourth and Goal
-- use the C++ recovery/boarding flow.  This gives the player vehicle credit,
-- restores control and keeps Coach Crosscheck as a recovery point.
UPDATE `quest_template_addon`
SET `ScriptName` = 'quest_kezan_fourth_and_goal'
WHERE `ID` IN (24502, 24503, 28414);

UPDATE `creature_template`
SET `ScriptName` = 'npc_kezan_coach_crosscheck'
WHERE `entry` = 37106;

UPDATE `creature_template`
SET `AIName` = '', `ScriptName` = 'npc_kezan_fourth_and_goal_buccaneer'
WHERE `entry` = 37213;

DELETE FROM `spell_script_names`
WHERE `spell_id` = 70052;

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(70052, 'spell_kezan_fourth_and_goal_kick');
