-- Increase the number of active mining nodes by 50 percent, rounded up.
--
-- Small child pools usually contain alternative ore types for one location.
-- Raising those pools would stack multiple nodes at the same coordinates, so
-- only root pools are changed:
--   * root pools containing only ore gameobjects; and
--   * root master pools whose children contain only ore gameobjects.
-- Limits are capped at the number of available members. Unpooled nodes and
-- respawn times are intentionally left unchanged.

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_entries_20260929`;
CREATE TEMPORARY TABLE `_tmp_ore_entries_20260929`
(
    `entry` MEDIUMINT UNSIGNED NOT NULL,
    PRIMARY KEY (`entry`)
);

INSERT INTO `_tmp_ore_entries_20260929` (`entry`) VALUES
(324),(1731),(1732),(1733),(1734),(1735),(2040),(2047),(2054),(2055),
(73940),(73941),(123309),(123310),(123848),(150082),(165658),(175404),
(176643),(176645),(177388),(181108),(181109),(181248),(181249),(181555),
(181556),(181557),(181569),(185877),(189978),(189979),(189980),(189981),
(191133),(202736),(202737),(202738),(202739),(202740),(202741),(209311),
(209312),(209313),(209328),(209329),(209330),(215413),(221538),(221539),
(221540),(221541);

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_pool_stats_20260929`;
CREATE TEMPORARY TABLE `_tmp_ore_pool_stats_20260929`
(
    `pool_entry` MEDIUMINT UNSIGNED NOT NULL,
    `member_count` MEDIUMINT UNSIGNED NOT NULL,
    `ore_count` MEDIUMINT UNSIGNED NOT NULL,
    PRIMARY KEY (`pool_entry`)
);

INSERT INTO `_tmp_ore_pool_stats_20260929`
(`pool_entry`,`member_count`,`ore_count`)
SELECT `pg`.`pool_entry`,COUNT(*),SUM(`oe`.`entry` IS NOT NULL)
FROM `pool_gameobject` AS `pg`
INNER JOIN `gameobject` AS `g` ON `g`.`guid` = `pg`.`guid`
LEFT JOIN `_tmp_ore_entries_20260929` AS `oe` ON `oe`.`entry` = `g`.`id`
GROUP BY `pg`.`pool_entry`;

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_density_targets_20260929`;
CREATE TEMPORARY TABLE `_tmp_ore_density_targets_20260929`
(
    `pool_entry` MEDIUMINT UNSIGNED NOT NULL,
    `capacity` MEDIUMINT UNSIGNED NOT NULL,
    `pool_kind` VARCHAR(16) NOT NULL,
    PRIMARY KEY (`pool_entry`)
);

-- Direct root pools containing ore nodes and nothing else.
INSERT INTO `_tmp_ore_density_targets_20260929`
(`pool_entry`,`capacity`,`pool_kind`)
SELECT `s`.`pool_entry`,`s`.`member_count`,'direct'
FROM `_tmp_ore_pool_stats_20260929` AS `s`
LEFT JOIN `pool_pool` AS `parent` ON `parent`.`pool_id` = `s`.`pool_entry`
WHERE `s`.`member_count` = `s`.`ore_count`
  AND `s`.`ore_count` > 0
  AND `parent`.`pool_id` IS NULL;

-- Root master pools containing only ore-only child pools. The child pools are
-- deliberately not changed because they select the ore variant for a location.
INSERT INTO `_tmp_ore_density_targets_20260929`
(`pool_entry`,`capacity`,`pool_kind`)
SELECT `pp`.`mother_pool`,COUNT(*),'master'
FROM `pool_pool` AS `pp`
LEFT JOIN `_tmp_ore_pool_stats_20260929` AS `s`
    ON `s`.`pool_entry` = `pp`.`pool_id`
LEFT JOIN `pool_pool` AS `parent`
    ON `parent`.`pool_id` = `pp`.`mother_pool`
WHERE `parent`.`pool_id` IS NULL
  AND NOT EXISTS
  (
      SELECT 1
      FROM `pool_gameobject` AS `direct_member`
      WHERE `direct_member`.`pool_entry` = `pp`.`mother_pool`
  )
GROUP BY `pp`.`mother_pool`
HAVING COUNT(*) = SUM(
    CASE
        WHEN `s`.`member_count` = `s`.`ore_count` AND `s`.`ore_count` > 0
        THEN 1 ELSE 0
    END
);

-- Preserve the original pool rows once. This also makes the update repeatable:
-- every subsequent run derives the requested value from the original limit.
CREATE TABLE IF NOT EXISTS `_backup_pool_template_ore_density_20260929`
LIKE `pool_template`;

INSERT INTO `_backup_pool_template_ore_density_20260929`
SELECT `pt`.*
FROM `pool_template` AS `pt`
INNER JOIN `_tmp_ore_density_targets_20260929` AS `t`
    ON `t`.`pool_entry` = `pt`.`entry`
WHERE NOT EXISTS
(
    SELECT 1
    FROM `_backup_pool_template_ore_density_20260929` AS `b`
    WHERE `b`.`entry` = `pt`.`entry`
);

UPDATE `pool_template` AS `pt`
INNER JOIN `_tmp_ore_density_targets_20260929` AS `t`
    ON `t`.`pool_entry` = `pt`.`entry`
INNER JOIN `_backup_pool_template_ore_density_20260929` AS `b`
    ON `b`.`entry` = `pt`.`entry`
SET `pt`.`max_limit` = GREATEST(
    `b`.`max_limit`,
    LEAST(`t`.`capacity`,CEIL(`b`.`max_limit` * 1.5))
);

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_density_targets_20260929`;
DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_pool_stats_20260929`;
DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_entries_20260929`;
