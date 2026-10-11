-- Lost Sheepie (31338): Sheepie 64385 had no interaction flag or AI, so
-- right-clicking him could never collect the required Sheepie item 86446.
-- Mirror the working interaction used by the follow-up Sheepie 64386 and
-- gate both interactions to their respective active quests.

START TRANSACTION;

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `AIName` = 'SmartAI',
    `ScriptName` = ''
WHERE `entry` = 64385;

DELETE FROM `smart_scripts`
WHERE `source_type` = 0 AND `entryorguid` = 64385;

INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,
 `event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,
 `event_param4`,`event_param5`,`action_type`,`action_param1`,`action_param2`,
 `action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,
 `target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,
 `target_y`,`target_z`,`target_o`,`comment`)
VALUES
(64385,0,0,1,64,0,100,0,0,0,0,0,0,72,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,
 'Lost Sheepie - On gossip hello - Close gossip'),
(64385,0,1,0,61,0,100,0,0,0,0,0,0,56,86446,1,0,0,0,0,7,0,0,0,0,0,0,0,0,
 'Lost Sheepie - Give Sheepie to player');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceGroup` = 1
  AND `SourceEntry` IN (64385,64386)
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` = 9
  AND `ConditionValue1` IN (31338,31339);

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,
 `ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(22,1,64385,0,0,9,0,31338,0,0,0,0,0,'',
 'Sheepie 64385 interaction requires active Lost Sheepie'),
(22,1,64386,0,0,9,0,31339,0,0,0,0,0,'',
 'Sheepie 64386 interaction requires active Lost Sheepie Again');

COMMIT;
