-- Honor and Privilege (25972 Horde)
-- Restore the surface receiver, make the survival-kit chest quest-active,
-- award both objectives, and keep the rescue fleet visible from the balloon.
START TRANSACTION;

-- Survival Kit Remnants contains the Rescue Flare (56188). Marking the chest
-- for the quest lets the client activate it while Honor and Privilege is open.
UPDATE `gameobject_template`
SET `data8` = 25972
WHERE `entry` = 203410;

-- Bloodguard Toldrek waits beside the rescue balloon and receives the quest.
DELETE FROM `creature` WHERE `guid` = 4000150;
INSERT INTO `creature`
(`guid`,`id`,`map`,`zoneId`,`areaId`,`spawnMask`,`phaseMask`,`phaseId`,
 `phaseGroup`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,
 `orientation`,`spawntimesecs`,`spawntimesecs_max`,`wander_distance`,
 `currentwaypoint`,`curhealth`,`curmana`,`MovementType`,`npcflag`,`npcflag2`,
 `unit_flags`,`unit_flags2`,`dynamicflags`,`ScriptName`,`walk_mode`,`VerifiedBuild`)
VALUES
(4000150,40921,0,5144,4966,1,1,0,
 0,32882,0,-7309.80,4240.10,-1.00,
 3.45575,60,0,0,
 0,446790,0,0,0,0,
 0,0,0,'',0,0);

-- Reaching Toldrek at the surface completes "Swim up to the rescue balloon".
UPDATE `creature_template`
SET `AIName` = 'SmartAI'
WHERE `entry` = 40921;

DELETE FROM `smart_scripts`
WHERE `entryorguid` = -4000150 AND `source_type` = 0;
INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,
 `event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,
 `action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,
 `target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,
 `target_x`,`target_y`,`target_z`,`target_o`,`comment`)
VALUES
(-4000150,0,0,0,10,0,100,0,
 1,30,1000,1000,0,
 33,40921,0,0,0,0,0,
 7,0,0,0,0,
 0,0,0,0,'Bloodguard Toldrek - Player in line of sight - Credit surface arrival');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceGroup` = 1
  AND `SourceEntry` = -4000150
  AND `SourceId` = 0;
INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,
 `NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(22,1,-4000150,0,0,
 9,0,25972,0,0,
 0,0,0,'','Only award surface arrival while Horde Honor and Privilege is active');

-- Rescue Flare is a DBC dummy effect and needs a server-side credit handler.
DELETE FROM `spell_script_names`
WHERE `spell_id` = 77741
  AND `ScriptName` = 'spell_honor_and_privilege_rescue_flare';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(77741,'spell_honor_and_privilege_rescue_flare');

-- The six fleet hulls sit 140-425 yards from the balloon. Their default
-- continent visibility (90 yards) made the surface appear split into tiny
-- bubbles. These per-template distances expose the complete rescue scene.
DELETE FROM `object_visibility`
WHERE `type` = 'GameObject'
  AND `entry` IN (203397,203398,203399,203400,203401,203402);
INSERT INTO `object_visibility`
(`type`,`entry`,`distance`,`active`,`importance`,`comment`)
VALUES
('GameObject',203397,600,1,'DistantScenery','Vashjir - Honor and Privilege - Horde Ship 000'),
('GameObject',203398,600,1,'DistantScenery','Vashjir - Honor and Privilege - Horde Ship 001'),
('GameObject',203399,600,1,'DistantScenery','Vashjir - Honor and Privilege - Horde Ship 002'),
('GameObject',203400,600,1,'DistantScenery','Vashjir - Honor and Privilege - Alliance Ship 000'),
('GameObject',203401,600,1,'DistantScenery','Vashjir - Honor and Privilege - Alliance Ship 001'),
('GameObject',203402,600,1,'DistantScenery','Vashjir - Honor and Privilege - Alliance Ship 002');

COMMIT;
