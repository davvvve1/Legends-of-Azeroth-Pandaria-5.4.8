-- Calamity Jade (29926): associate the Gorger POI with its real objective.
-- Kill credit itself is guarded in zone_the_jade_forest.cpp so a missed
-- generic kill reward cannot leave either objective stuck at zero.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 1,
    `QuestObjectiveId` = 255450
WHERE `QuestID` = 29926
  AND `Idx1` = 2;
