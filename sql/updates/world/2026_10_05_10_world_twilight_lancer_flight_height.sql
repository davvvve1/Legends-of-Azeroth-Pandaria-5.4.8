-- Wave Two (25544) / Vigilance on Wings (29177)
-- Twilight Lancers ride Twilight Firebirds (40650).  Their flat flight loops
-- intersect the Hatchery slopes, leaving several riders inside terrain.
-- Raise both the spawn points and complete waypoint loops by 20 yards.

CREATE TABLE IF NOT EXISTS `_backup_creature_twilight_firebird_height_20261005`
LIKE `creature`;

INSERT IGNORE INTO `_backup_creature_twilight_firebird_height_20261005`
SELECT `c`.*
FROM `creature` AS `c`
WHERE `c`.`id`=40650;

CREATE TABLE IF NOT EXISTS `_backup_waypoint_twilight_firebird_height_20261005`
LIKE `waypoint_data`;

INSERT IGNORE INTO `_backup_waypoint_twilight_firebird_height_20261005`
SELECT `w`.*
FROM `waypoint_data` AS `w`
JOIN `creature_addon` AS `a` ON `a`.`path_id`=`w`.`id`
JOIN `creature` AS `c` ON `c`.`guid`=`a`.`guid`
WHERE `c`.`id`=40650;

UPDATE `creature` AS `c`
JOIN `_backup_creature_twilight_firebird_height_20261005` AS `b`
  ON `b`.`guid`=`c`.`guid`
SET `c`.`position_z`=`b`.`position_z`+20.0
WHERE `c`.`id`=40650;

UPDATE `waypoint_data` AS `w`
JOIN `_backup_waypoint_twilight_firebird_height_20261005` AS `b`
  ON `b`.`id`=`w`.`id` AND `b`.`point`=`w`.`point`
SET `w`.`position_z`=`b`.`position_z`+20.0;
