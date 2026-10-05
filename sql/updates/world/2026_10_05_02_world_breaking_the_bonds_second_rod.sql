-- Breaking the Bonds (25514): the second Rod of Subjugation sent SetData to
-- bunny GUID 284444, but that bunny has no receiver event, so the click did
-- not grant objective 40545. Cast only the second rod's result spell; the
-- encounter summon is not required for quest progress.
UPDATE `gameobject_template`
SET `AIName` = 'SmartGameObjectAI',
    `ScriptName` = ''
WHERE `entry` = 202955;

DELETE FROM `smart_scripts`
WHERE `entryorguid` = 202955
  AND `source_type` = 1;

INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,
 `event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,
 `action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,
 `target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,
 `target_x`,`target_y`,`target_z`,`target_o`,`comment`)
VALUES
(202955,1,0,0,64,0,100,0,0,0,0,0,0,
 11,75616,2,0,0,0,0,7,0,0,0,0,0,0,0,0,
 'Second Rod of Subjugation - On Use - Cast second rod result on player');
