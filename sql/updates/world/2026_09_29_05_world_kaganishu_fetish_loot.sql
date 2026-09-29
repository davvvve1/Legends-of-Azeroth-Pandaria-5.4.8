-- Kaganishu (11637): guarantee Kaganishu's Fetish (34781) while the quest is
-- in progress and its item objective (263811) is still incomplete.

CREATE TABLE IF NOT EXISTS `_backup_creature_loot_kaganishu_fetish_20260929`
LIKE `creature_loot_template`;

INSERT INTO `_backup_creature_loot_kaganishu_fetish_20260929`
SELECT `clt`.*
FROM `creature_loot_template` AS `clt`
WHERE `clt`.`entry` = 25427
  AND `clt`.`item` = 34781
  AND NOT EXISTS
  (
      SELECT 1
      FROM `_backup_creature_loot_kaganishu_fetish_20260929` AS `b`
      WHERE `b`.`entry` = `clt`.`entry`
        AND `b`.`item` = `clt`.`item`
        AND `b`.`lootmode` = `clt`.`lootmode`
  );

CREATE TABLE IF NOT EXISTS `_backup_conditions_kaganishu_fetish_20260929`
LIKE `conditions`;

INSERT INTO `_backup_conditions_kaganishu_fetish_20260929`
SELECT `c`.*
FROM `conditions` AS `c`
WHERE `c`.`SourceTypeOrReferenceId` = 1
  AND `c`.`SourceGroup` = 25427
  AND `c`.`SourceEntry` = 34781
  AND `c`.`SourceId` = 0
  AND NOT EXISTS
  (
      SELECT 1 FROM `_backup_conditions_kaganishu_fetish_20260929`
  );

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100,
    `lootmode` = 'REGULAR',
    `groupid` = 0,
    `mincountOrRef` = 1,
    `maxcount` = 1
WHERE `entry` = 25427
  AND `item` = 34781;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 1
  AND `SourceGroup` = 25427
  AND `SourceEntry` = 34781
  AND `SourceId` = 0;

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,
 `ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(1,25427,34781,0,0,47,0,11637,8,0,0,0,0,'',
 'Kaganishu - Kaganishu''s Fetish requires quest 11637 in progress'),
(1,25427,34781,0,0,48,0,263811,0,0,1,0,0,'',
 'Kaganishu - Kaganishu''s Fetish objective must be incomplete');
