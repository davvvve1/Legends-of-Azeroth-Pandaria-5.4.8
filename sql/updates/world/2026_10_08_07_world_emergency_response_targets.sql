-- Emergency Response (30504 Horde / 31319 Alliance)
-- Align the rescue bodies with the quest POIs and restore missing Admiral Taylor.
START TRANSACTION;

-- General Nazgrim was 73 yards from the objective POI (777, -1909), outside
-- the 30-yard fireworks radius. This is the wounded post-battle version.
UPDATE `creature`
SET `zoneId` = 5785,
    `areaId` = 5876,
    `position_x` = 777.00,
    `position_y` = -1909.00,
    `position_z` = 60.00,
    `phaseMask` = 1,
    `curhealth` = 1
WHERE `guid` = 457630
  AND `id` = 64360;

-- Alliance objective 268533 referenced Admiral Taylor, but entry 64491 had
-- no world spawn. Place his wounded version at the quest POI (764, -1879).
DELETE FROM `creature` WHERE `guid` = 4000151;
INSERT INTO `creature`
(`guid`,`id`,`map`,`zoneId`,`areaId`,`spawnMask`,`phaseMask`,`phaseId`,
 `phaseGroup`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,
 `orientation`,`spawntimesecs`,`spawntimesecs_max`,`wander_distance`,
 `currentwaypoint`,`curhealth`,`curmana`,`MovementType`,`npcflag`,`npcflag2`,
 `unit_flags`,`unit_flags2`,`dynamicflags`,`ScriptName`,`walk_mode`,`VerifiedBuild`)
VALUES
(4000151,64491,870,5785,5876,1,1,0,
 0,0,0,764.00,-1879.00,61.80,
 2.10,60,0,0,
 0,1,0,0,0,0,
 0,0,0,'',0,18414);

COMMIT;
