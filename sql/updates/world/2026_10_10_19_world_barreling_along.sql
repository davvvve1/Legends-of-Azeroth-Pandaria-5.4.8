-- Barreling Along (30172)
--
-- Mudmug's quest-specific actor (58785) was missing from the Gilded Fan,
-- while the escort/credit actor (58341) had no behavior.  Restore the actor
-- beside the giant banana and connect both Mudmug variants to the C++ escort.

START TRANSACTION;

CREATE TABLE IF NOT EXISTS `_backup_creature_template_barreling_along_20261010`
LIKE `creature_template`;

INSERT IGNORE INTO `_backup_creature_template_barreling_along_20261010`
SELECT *
FROM `creature_template`
WHERE `entry` IN (56474, 58341, 58785);

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_mudmug_barreling_along_questgiver'
WHERE `entry` IN (56474, 58785);

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_mudmug_barreling_along_escort'
WHERE `entry` = 58341;

-- Idempotency for development databases where this migration is replayed.
DELETE FROM `creature`
WHERE `id` = 58785 AND `map` = 870
  AND `VerifiedBuild` = -2026101019;

-- Retail position beside the giant banana: world coordinates corresponding
-- to 54.3, 38.7 in the Gilded Fan.
INSERT INTO `creature`
    (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`,
     `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`,
     `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`,
     `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`,
     `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`)
VALUES
    (58785, 870, 5805, 5984, 1, 1, 0, 0, 0, 0,
     80.38063, 548.002, 153.3922, 3.54, 120, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, '', 0, -2026101019);

-- This Chen actor already occupies the retail Halfhill turn-in position.
-- Make it a questgiver for this objective rather than creating a duplicate
-- Chen on top of it.
UPDATE `creature`
SET `npcflag` = `npcflag` | 2
WHERE `guid` = 514067 AND `id` = 64946;

DELETE FROM `creature_questender`
WHERE `id` = 64946 AND `quest` = 30172;

INSERT INTO `creature_questender` (`id`, `quest`)
VALUES (64946, 30172);

COMMIT;
