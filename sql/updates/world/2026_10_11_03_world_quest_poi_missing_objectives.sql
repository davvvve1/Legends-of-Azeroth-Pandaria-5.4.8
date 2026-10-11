-- Restore missing objective IDs for unambiguous quest POIs.  Imported legacy
-- POIs often retained their map polygons but lost QuestObjectiveId, leaving
-- the 5.4.8 client unable to advance the map marker when credit changed.
--
-- A row is safe to repair when either its ObjectiveIndex names exactly one
-- current objective or the quest itself has exactly one objective.  Ambiguous
-- multi-objective legacy slots remain untouched for targeted data fixes.
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_missing_objectives_20261011`
LIKE `quest_poi`;

INSERT IGNORE INTO `_backup_quest_poi_missing_objectives_20261011`
SELECT poi.*
FROM `quest_poi` poi
LEFT JOIN
(
    SELECT `questId`, `index`, MIN(`id`) AS `id`
    FROM `quest_objective`
    GROUP BY `questId`, `index`
    HAVING COUNT(*) = 1
) exactObjective
  ON exactObjective.`questId` = poi.`QuestID`
 AND exactObjective.`index` = poi.`ObjectiveIndex`
LEFT JOIN
(
    SELECT `questId`, MIN(`id`) AS `id`
    FROM `quest_objective`
    GROUP BY `questId`
    HAVING COUNT(*) = 1
) singleObjective
  ON singleObjective.`questId` = poi.`QuestID`
WHERE poi.`QuestObjectiveId` = 0
  AND poi.`ObjectiveIndex` >= 0
  AND COALESCE(exactObjective.`id`, singleObjective.`id`) IS NOT NULL;

UPDATE `quest_poi` poi
LEFT JOIN
(
    SELECT `questId`, `index`, MIN(`id`) AS `id`
    FROM `quest_objective`
    GROUP BY `questId`, `index`
    HAVING COUNT(*) = 1
) exactObjective
  ON exactObjective.`questId` = poi.`QuestID`
 AND exactObjective.`index` = poi.`ObjectiveIndex`
LEFT JOIN
(
    SELECT `questId`, MIN(`id`) AS `id`
    FROM `quest_objective`
    GROUP BY `questId`
    HAVING COUNT(*) = 1
) singleObjective
  ON singleObjective.`questId` = poi.`QuestID`
SET poi.`QuestObjectiveId` = COALESCE(exactObjective.`id`, singleObjective.`id`)
WHERE poi.`QuestObjectiveId` = 0
  AND poi.`ObjectiveIndex` >= 0
  AND COALESCE(exactObjective.`id`, singleObjective.`id`) IS NOT NULL;
