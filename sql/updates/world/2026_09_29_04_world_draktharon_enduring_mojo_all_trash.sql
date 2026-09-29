-- Cleansing Drak'Tharon (12238/30120): every ordinary hostile creature in
-- Drak'Tharon Keep drops one Enduring Mojo.  Keep this unconditional: the
-- item is a quest reagent rather than a quest objective, and attaching quest
-- conditions to the loot row can hide it during personal-loot generation.
-- Bosses, critters, triggers, summons without loot, and friendly NPCs are
-- intentionally excluded.

DROP TEMPORARY TABLE IF EXISTS `_tmp_draktharon_mojo_sources_20260929`;
CREATE TEMPORARY TABLE `_tmp_draktharon_mojo_sources_20260929`
(
    `entry` MEDIUMINT UNSIGNED NOT NULL,
    `name` VARCHAR(100) NOT NULL,
    PRIMARY KEY (`entry`)
);

INSERT INTO `_tmp_draktharon_mojo_sources_20260929` (`entry`,`name`) VALUES
(26620,'Drakkari Guardian'),
(26621,'Ghoul Tormentor'),
(26622,'Drakkari Bat'),
(26623,'Scourge Brute'),
(26624,'Wretched Belcher'),
(26625,'Darkweb Recluse'),
(26626,'Scourge Reanimator'),
(26628,'Drakkari Scytheclaw'),
(26635,'Risen Drakkari Warrior'),
(26636,'Risen Drakkari Soulmage'),
(26637,'Risen Drakkari Handler'),
(26638,'Risen Drakkari Bat Rider'),
(26639,'Drakkari Shaman'),
(26641,'Drakkari Gutripper'),
(26830,'Risen Drakkari Death Knight'),
(27431,'Drakkari Commander'),
(27871,'Flesheating Ghoul');

CREATE TABLE IF NOT EXISTS `_backup_creature_loot_draktharon_mojo_all_20260929`
LIKE `creature_loot_template`;

INSERT INTO `_backup_creature_loot_draktharon_mojo_all_20260929`
SELECT `clt`.*
FROM `creature_loot_template` AS `clt`
INNER JOIN `_tmp_draktharon_mojo_sources_20260929` AS `source`
    ON `source`.`entry` = `clt`.`entry`
WHERE `clt`.`item` = 38303
  AND NOT EXISTS
  (
      SELECT 1
      FROM `_backup_creature_loot_draktharon_mojo_all_20260929` AS `b`
      WHERE `b`.`entry` = `clt`.`entry`
        AND `b`.`item` = `clt`.`item`
        AND `b`.`lootmode` = `clt`.`lootmode`
  );

CREATE TABLE IF NOT EXISTS `_backup_conditions_draktharon_mojo_all_20260929`
LIKE `conditions`;

INSERT INTO `_backup_conditions_draktharon_mojo_all_20260929`
SELECT `c`.*
FROM `conditions` AS `c`
INNER JOIN `_tmp_draktharon_mojo_sources_20260929` AS `source`
    ON `source`.`entry` = `c`.`SourceGroup`
WHERE `c`.`SourceTypeOrReferenceId` = 1
  AND `c`.`SourceEntry` = 38303
  AND `c`.`SourceId` = 0
  AND NOT EXISTS
  (
      SELECT 1
      FROM `_backup_conditions_draktharon_mojo_all_20260929`
  );

DELETE `clt`
FROM `creature_loot_template` AS `clt`
INNER JOIN `_tmp_draktharon_mojo_sources_20260929` AS `source`
    ON `source`.`entry` = `clt`.`entry`
WHERE `clt`.`item` = 38303;

INSERT INTO `creature_loot_template`
(`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`)
SELECT `entry`,38303,100,'DUNGEON_NORMAL',0,1,1
FROM `_tmp_draktharon_mojo_sources_20260929`
UNION ALL
SELECT `entry`,38303,100,'DUNGEON_HEROIC',0,1,1
FROM `_tmp_draktharon_mojo_sources_20260929`;

DELETE `c`
FROM `conditions` AS `c`
INNER JOIN `_tmp_draktharon_mojo_sources_20260929` AS `source`
    ON `source`.`entry` = `c`.`SourceGroup`
WHERE `c`.`SourceTypeOrReferenceId` = 1
  AND `c`.`SourceEntry` = 38303
  AND `c`.`SourceId` = 0;

DROP TEMPORARY TABLE IF EXISTS `_tmp_draktharon_mojo_sources_20260929`;
