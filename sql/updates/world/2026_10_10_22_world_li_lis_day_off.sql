-- Li Li's Day Off (29950)
--
-- Li Li (56549), summoned by Li Li's Wishing-Stone (76350 / spell 106276),
-- had no companion AI and therefore never awarded the three exploration
-- credits.  The C++ script now follows the summoner, recovers if stuck and
-- awards each credit at its destination.  Two POI rows also referenced the
-- Silken Fields objective instead of their own objectives.

CREATE TABLE IF NOT EXISTS `_backup_creature_template_li_lis_day_off_20261010`
LIKE `creature_template`;

CREATE TABLE IF NOT EXISTS `_backup_quest_poi_li_lis_day_off_20261010`
LIKE `quest_poi`;

INSERT IGNORE INTO `_backup_creature_template_li_lis_day_off_20261010`
SELECT *
FROM `creature_template`
WHERE `entry` = 56549;

INSERT IGNORE INTO `_backup_quest_poi_li_lis_day_off_20261010`
SELECT *
FROM `quest_poi`
WHERE `QuestID` = 29950;

START TRANSACTION;

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_li_li_day_off_companion'
WHERE `entry` = 56549;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1,
    `QuestObjectiveId` = 258092
WHERE `QuestID` = 29950 AND `Idx1` = 2;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 2,
    `QuestObjectiveId` = 258093
WHERE `QuestID` = 29950 AND `Idx1` = 3;

COMMIT;
