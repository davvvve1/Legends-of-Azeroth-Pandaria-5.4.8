-- Fresh Bait (25493): the six Core Hound hunting-area POIs were attached to
-- nonexistent objective index 4 instead of the entrails objective (252372).
-- Keep the source-backed polygons and the separate turn-in POI unchanged.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 0,
    `QuestObjectiveId` = 252372
WHERE `QuestID` = 25493
  AND `Idx1` BETWEEN 0 AND 5
  AND `ObjectiveIndex` = 4
  AND `QuestObjectiveId` = 0;
