-- Per-character state for the private Sunsong Ranch crop plots.
CREATE TABLE IF NOT EXISTS `character_sunsong_farm` (
    `guid` INT UNSIGNED NOT NULL,
    `plot` TINYINT UNSIGNED NOT NULL,
    `crop` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `state` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `ready_time` INT UNSIGNED NOT NULL DEFAULT 0,
    `encounter` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `progress` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`guid`, `plot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
