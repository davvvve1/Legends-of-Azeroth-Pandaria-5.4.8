-- Increase active mining-node limits by another 50 percent, rounded up.
-- This follows 2026_09_29_00 and uses its audited ore-pool set. Limits remain
-- capped at the number of available direct members or child location pools.

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_density_second_targets_20260929`;
CREATE TEMPORARY TABLE `_tmp_ore_density_second_targets_20260929`
(
    `pool_entry` MEDIUMINT UNSIGNED NOT NULL,
    `capacity` MEDIUMINT UNSIGNED NOT NULL,
    PRIMARY KEY (`pool_entry`)
);

INSERT INTO `_tmp_ore_density_second_targets_20260929`
(`pool_entry`,`capacity`)
SELECT `b`.`entry`,
       CASE
           WHEN COUNT(DISTINCT `pg`.`guid`) > 0
           THEN COUNT(DISTINCT `pg`.`guid`)
           ELSE COUNT(DISTINCT `pp`.`pool_id`)
       END
FROM `_backup_pool_template_ore_density_20260929` AS `b`
LEFT JOIN `pool_gameobject` AS `pg` ON `pg`.`pool_entry` = `b`.`entry`
LEFT JOIN `pool_pool` AS `pp` ON `pp`.`mother_pool` = `b`.`entry`
GROUP BY `b`.`entry`;

CREATE TABLE IF NOT EXISTS `_backup_pool_template_ore_density_second_20260929`
LIKE `pool_template`;

INSERT INTO `_backup_pool_template_ore_density_second_20260929`
SELECT `pt`.*
FROM `pool_template` AS `pt`
INNER JOIN `_tmp_ore_density_second_targets_20260929` AS `t`
    ON `t`.`pool_entry` = `pt`.`entry`
WHERE NOT EXISTS
(
    SELECT 1
    FROM `_backup_pool_template_ore_density_second_20260929` AS `b`
    WHERE `b`.`entry` = `pt`.`entry`
);

UPDATE `pool_template` AS `pt`
INNER JOIN `_tmp_ore_density_second_targets_20260929` AS `t`
    ON `t`.`pool_entry` = `pt`.`entry`
INNER JOIN `_backup_pool_template_ore_density_second_20260929` AS `b`
    ON `b`.`entry` = `pt`.`entry`
SET `pt`.`max_limit` = GREATEST(
    `b`.`max_limit`,
    LEAST(`t`.`capacity`,CEIL(`b`.`max_limit` * 1.5))
);

DROP TEMPORARY TABLE IF EXISTS `_tmp_ore_density_second_targets_20260929`;
