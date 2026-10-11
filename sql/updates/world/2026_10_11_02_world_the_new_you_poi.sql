-- The New You (14109/14110): convert the three legacy item objective slots
-- to the modern quest_objective indexes and IDs used by the 5.4.8 client.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 0, `QuestObjectiveId` = 264824
WHERE `QuestID` = 14109 AND `Idx1` = 1;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 264825
WHERE `QuestID` = 14109 AND `Idx1` = 2;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 2, `QuestObjectiveId` = 264826
WHERE `QuestID` = 14109 AND `Idx1` = 3;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 0, `QuestObjectiveId` = 264967
WHERE `QuestID` = 14110 AND `Idx1` = 1;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 264968
WHERE `QuestID` = 14110 AND `Idx1` = 2;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 2, `QuestObjectiveId` = 264969
WHERE `QuestID` = 14110 AND `Idx1` = 3;
