-- Repair every quest_poi row whose QuestObjectiveId no longer exists for the
-- referenced quest.  Keep valid coordinates as a general POI when the old
-- objective was removed; map active objectives to their current IDs.

CREATE TABLE IF NOT EXISTS `_backup_quest_poi_objective_integrity_20261010`
LIKE `quest_poi`;

INSERT IGNORE INTO `_backup_quest_poi_objective_integrity_20261010`
SELECT poi.*
FROM `quest_poi` poi
WHERE poi.`QuestObjectiveId` <> 0
  AND NOT EXISTS
      (SELECT 1
       FROM `quest_objective` objective
       WHERE objective.`questId` = poi.`QuestID`
         AND objective.`id` = poi.`QuestObjectiveId`);

START TRANSACTION;

-- The ordinary case: ObjectiveIndex still identifies the current objective.
UPDATE `quest_poi` poi
LEFT JOIN `quest_objective` objective
  ON objective.`questId` = poi.`QuestID`
 AND objective.`index` = poi.`ObjectiveIndex`
SET poi.`QuestObjectiveId` = COALESCE(objective.`id`, 0)
WHERE poi.`QuestObjectiveId` <> 0
  AND NOT EXISTS
      (SELECT 1
       FROM `quest_objective` validObjective
       WHERE validObjective.`questId` = poi.`QuestID`
         AND validObjective.`id` = poi.`QuestObjectiveId`);

-- To the Skies! retained POIs from a removed objective zero.  Both map blobs
-- describe the surviving assault objective at index one.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 270171
WHERE `QuestID` = 32277 AND `Idx1` IN (1,2);

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 270215
WHERE `QuestID` = 32652 AND `Idx1` IN (1,2);

-- The Best Around: the Stormwind point is Agent Townsley; the two Brawlpub
-- points are Bizmo's Brawlpub.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 0, `QuestObjectiveId` = 269736
WHERE `QuestID` = 32380 AND `Idx1` = 2;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 269860
WHERE `QuestID` = 32380 AND `Idx1` IN (1,3);

COMMIT;
