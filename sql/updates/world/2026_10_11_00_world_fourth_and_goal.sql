-- Fourth and Goal (24503/28414): summon the correct vehicle and credit a valid kick through the smokestacks.
UPDATE `quest_template_addon`
SET `ScriptName` = 'quest_kezan_fourth_and_goal'
WHERE `ID` IN (24503, 28414);

UPDATE `creature_template`
SET `AIName` = '', `ScriptName` = 'npc_kezan_fourth_and_goal_buccaneer'
WHERE `entry` = 37213;

DELETE FROM `spell_script_names`
WHERE `spell_id` = 70052;

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(70052, 'spell_kezan_fourth_and_goal_kick');
