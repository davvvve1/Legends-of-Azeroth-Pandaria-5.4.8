-- Choppertunity (31777) requires 12 Strongarm Gyrocopters (65843).
-- The original quest-phase spawns are spread over a large part of the airstrip
-- and take two minutes to return.  Add a central flight group and shorten the
-- quest-phase respawn so several players/bot groups cannot exhaust the targets.
UPDATE `creature`
SET `spawntimesecs` = 30, `spawntimesecs_max` = 0
WHERE `id` = 65843 AND `map` = 870 AND `zoneId` = 5785
  AND `phaseMask` = 268435456;

DELETE FROM `creature`
WHERE `guid` BETWEEN 90031770 AND 90031781 AND `id` = 65843;

INSERT INTO `creature`
    (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`,
     `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`,
     `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`,
     `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`,
     `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`)
VALUES
    (90031770, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2490.0, -480.0, 388.0, 0.45, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031771, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2510.0, -500.0, 396.0, 1.10, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031772, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2530.0, -520.0, 404.0, 1.75, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031773, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2550.0, -540.0, 410.0, 2.40, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031774, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2570.0, -520.0, 402.0, 3.05, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031775, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2590.0, -500.0, 396.0, 3.70, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031776, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2500.0, -550.0, 402.0, 4.35, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031777, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2520.0, -430.0, 382.0, 5.00, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031778, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2540.0, -450.0, 390.0, 5.65, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031779, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2560.0, -470.0, 400.0, 0.20, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031780, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2580.0, -440.0, 392.0, 0.85, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0),
    (90031781, 65843, 870, 5785, 5867, 1, 268435456, 0, 0, 0, 0, 2600.0, -540.0, 406.0, 1.50, 30, 0, 8, 0, 1, 0, 1, 0, 0, 0, 0, 0, '', 0, 0);
