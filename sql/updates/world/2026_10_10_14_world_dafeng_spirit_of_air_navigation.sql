-- Quest 29785 "Dafeng, the Spirit of Air"
-- The objective POI incorrectly stored objective 264042 as the map floor.
-- That invalid floor prevents the client from displaying its navigation pin.

CREATE TABLE IF NOT EXISTS `_backup_quest_poi_dafeng_20261010` LIKE `quest_poi`;
INSERT IGNORE INTO `_backup_quest_poi_dafeng_20261010`
SELECT * FROM `quest_poi` WHERE `QuestID` = 29785;

CREATE TABLE IF NOT EXISTS `_backup_quest_poi_points_dafeng_20261010` LIKE `quest_poi_points`;
INSERT IGNORE INTO `_backup_quest_poi_points_dafeng_20261010`
SELECT * FROM `quest_poi_points` WHERE `QuestID` = 29785;

INSERT INTO `quest_poi`
    (`QuestID`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveId`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `VerifiedBuild`)
VALUES
    (29785, 1, 0, 264042, 860, 808, 0, 0, 2, 0)
ON DUPLICATE KEY UPDATE
    `ObjectiveIndex` = VALUES(`ObjectiveIndex`),
    `QuestObjectiveId` = VALUES(`QuestObjectiveId`),
    `MapID` = VALUES(`MapID`),
    `WorldMapAreaId` = VALUES(`WorldMapAreaId`),
    `Floor` = VALUES(`Floor`),
    `Priority` = VALUES(`Priority`),
    `Flags` = VALUES(`Flags`);

DELETE FROM `quest_poi_points`
WHERE `QuestID` = 29785 AND `Idx1` = 1;

INSERT INTO `quest_poi_points`
    (`QuestID`, `BlobIndex`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`)
VALUES
    (29785, 1, 1, 0, 667, 4211, 0);
