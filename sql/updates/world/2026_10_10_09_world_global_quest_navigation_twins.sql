-- Restore navigation for exact quest twins.
--
-- These target/source pairs have the same title and the same ordered objective
-- signature.  The source quest already has a complete, loader-valid POI.  This
-- covers faction/remade versions without guessing coordinates.  Objective IDs
-- are remapped to the target quest; legacy/general POIs retain ID 0.

CREATE TABLE IF NOT EXISTS `_backup_quest_poi_navigation_twins_20261010`
LIKE `quest_poi`;
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_points_navigation_twins_20261010`
LIKE `quest_poi_points`;

DROP TEMPORARY TABLE IF EXISTS `_quest_navigation_twins`;
CREATE TEMPORARY TABLE `_quest_navigation_twins`
(
    `TargetQuestID` INT UNSIGNED NOT NULL PRIMARY KEY,
    `SourceQuestID` INT UNSIGNED NOT NULL
);

INSERT INTO `_quest_navigation_twins` (`TargetQuestID`,`SourceQuestID`) VALUES
(3523,27019),
(7668,8258),
(24724,4381),
(24725,4385),
(24726,4382),
(24727,4386),
(24728,4383),
(24729,4384),
(24738,3884),
(24739,3881),
(25331,14401),
(25634,25582),
(26178,11960),
(27255,27254),
(27289,1288),
(27309,27293),
(27634,3528),
(28892,1126),
(29025,14003),
(29120,29230),
(29459,29456),
(29460,29457),
(29461,29458),
(29532,9587),
(29533,9588),
(29692,9714),
(31554,31553),
(32676,32607),
(32677,32608),
(33007,33248);

INSERT IGNORE INTO `_backup_quest_poi_navigation_twins_20261010`
SELECT poi.*
FROM `quest_poi` poi
JOIN `_quest_navigation_twins` twins
  ON twins.`TargetQuestID` = poi.`QuestID`;

INSERT IGNORE INTO `_backup_quest_poi_points_navigation_twins_20261010`
SELECT points.*
FROM `quest_poi_points` points
JOIN `_quest_navigation_twins` twins
  ON twins.`TargetQuestID` = points.`QuestID`;

START TRANSACTION;

INSERT INTO `quest_poi`
(`QuestID`,`Idx1`,`ObjectiveIndex`,`QuestObjectiveId`,`MapID`,`WorldMapAreaId`,`Floor`,`Priority`,`Flags`,`VerifiedBuild`)
SELECT twins.`TargetQuestID`, sourcePoi.`Idx1`, sourcePoi.`ObjectiveIndex`,
       CASE
           WHEN sourcePoi.`QuestObjectiveId` = 0 THEN 0
           ELSE COALESCE(targetObjective.`id`, 0)
       END,
       sourcePoi.`MapID`, sourcePoi.`WorldMapAreaId`, sourcePoi.`Floor`,
       sourcePoi.`Priority`, sourcePoi.`Flags`, 0
FROM `_quest_navigation_twins` twins
JOIN `quest_poi` sourcePoi
  ON sourcePoi.`QuestID` = twins.`SourceQuestID`
LEFT JOIN `quest_objective` sourceObjective
  ON sourceObjective.`questId` = twins.`SourceQuestID`
 AND sourceObjective.`id` = sourcePoi.`QuestObjectiveId`
LEFT JOIN `quest_objective` targetObjective
  ON targetObjective.`questId` = twins.`TargetQuestID`
 AND targetObjective.`index` = sourceObjective.`index`
 AND targetObjective.`type` = sourceObjective.`type`
 AND targetObjective.`objectId` = sourceObjective.`objectId`
ON DUPLICATE KEY UPDATE
`ObjectiveIndex`=VALUES(`ObjectiveIndex`),
`QuestObjectiveId`=VALUES(`QuestObjectiveId`),
`MapID`=VALUES(`MapID`),
`WorldMapAreaId`=VALUES(`WorldMapAreaId`),
`Floor`=VALUES(`Floor`),
`Priority`=VALUES(`Priority`),
`Flags`=VALUES(`Flags`);

INSERT INTO `quest_poi_points`
(`QuestID`,`BlobIndex`,`Idx1`,`Idx2`,`X`,`Y`,`VerifiedBuild`)
SELECT twins.`TargetQuestID`, sourcePoints.`BlobIndex`, sourcePoints.`Idx1`,
       sourcePoints.`Idx2`, sourcePoints.`X`, sourcePoints.`Y`, 0
FROM `_quest_navigation_twins` twins
JOIN `quest_poi_points` sourcePoints
  ON sourcePoints.`QuestID` = twins.`SourceQuestID`
ON DUPLICATE KEY UPDATE
`X`=VALUES(`X`),
`Y`=VALUES(`Y`);

COMMIT;

DROP TEMPORARY TABLE `_quest_navigation_twins`;
