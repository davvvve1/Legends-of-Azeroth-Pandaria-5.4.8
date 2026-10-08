-- Bloodthirsty Prowler (62945) belongs to the level 90 Greenstone Village
-- scenario, but the same template was also reused for open-world Jade Forest
-- spawns. Give the map 870 population a level 85 copy without changing the
-- scenario creatures on map 1024.

START TRANSACTION;

DELETE FROM `creature_template_model` WHERE `CreatureID` = 300001;
DELETE FROM `creature_template_addon` WHERE `entry` = 300001;
DELETE FROM `creature_template_movement` WHERE `CreatureId` = 300001;
DELETE FROM `creature_template_locale` WHERE `entry` = 300001;
DELETE FROM `creature_template` WHERE `entry` = 300001;

DROP TEMPORARY TABLE IF EXISTS `_tmp_bloodthirsty_prowler_world_template`;
CREATE TEMPORARY TABLE `_tmp_bloodthirsty_prowler_world_template`
LIKE `creature_template`;

INSERT INTO `_tmp_bloodthirsty_prowler_world_template`
SELECT * FROM `creature_template` WHERE `entry` = 62945;

UPDATE `_tmp_bloodthirsty_prowler_world_template`
SET `entry` = 300001,
    `minlevel` = 85,
    `maxlevel` = 85,
    `VerifiedBuild` = 0;

INSERT INTO `creature_template`
SELECT * FROM `_tmp_bloodthirsty_prowler_world_template`;

INSERT INTO `creature_template_model`
(`CreatureID`,`Idx`,`CreatureDisplayID`,`DisplayScale`,`Probability`,`VerifiedBuild`)
SELECT 300001,`Idx`,`CreatureDisplayID`,`DisplayScale`,`Probability`,0
FROM `creature_template_model`
WHERE `CreatureID` = 62945;

INSERT INTO `creature_template_addon`
(`entry`,`path_id`,`mount`,`MountCreatureID`,`StandState`,`AnimTier`,`VisFlags`,
 `SheathState`,`PvPFlags`,`emote`,`aiAnimKit`,`movementAnimKit`,`meleeAnimKit`,
 `visibilityDistanceType`,`auras`)
SELECT 300001,`path_id`,`mount`,`MountCreatureID`,`StandState`,`AnimTier`,`VisFlags`,
       `SheathState`,`PvPFlags`,`emote`,`aiAnimKit`,`movementAnimKit`,`meleeAnimKit`,
       `visibilityDistanceType`,`auras`
FROM `creature_template_addon`
WHERE `entry` = 62945;

INSERT INTO `creature_template_movement`
(`CreatureId`,`Ground`,`Swim`,`Flight`,`Rooted`,`Chase`,`Random`,`InteractionPauseTimer`)
SELECT 300001,`Ground`,`Swim`,`Flight`,`Rooted`,`Chase`,`Random`,`InteractionPauseTimer`
FROM `creature_template_movement`
WHERE `CreatureId` = 62945;

INSERT INTO `creature_template_locale`
(`entry`,`locale`,`Name`,`FemaleName`,`Title`,`VerifiedBuild`)
SELECT 300001,`locale`,`Name`,`FemaleName`,`Title`,0
FROM `creature_template_locale`
WHERE `entry` = 62945;

UPDATE `creature`
SET `id` = 300001
WHERE `id` = 62945
  AND `map` = 870;

DROP TEMPORARY TABLE `_tmp_bloodthirsty_prowler_world_template`;

COMMIT;
