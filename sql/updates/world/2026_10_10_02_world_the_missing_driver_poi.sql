-- The Missing Driver (29419): the accepted quest exposed both its start POI
-- and its objective POI. The 5.4.8 client could select the start blob and
-- point back to Merchant Lorvo instead of the trapped Min Dimwind.

-- Keep the real objective and the turn-in POI, but remove the accepted
-- quest's obsolete start blob so the incomplete quest can only point to Min.
DELETE FROM `quest_poi_points`
WHERE `QuestID` = 29419
  AND `BlobIndex` = 0;

DELETE FROM `quest_poi`
WHERE `QuestID` = 29419
  AND `Idx1` = 0;

-- This sniffed 5.4.8 field carries the objective identifier for this POI.
-- Retain it together with QuestObjectiveId; zeroing it makes the objective
-- blob lose its association in the client.
UPDATE `quest_poi`
SET `QuestObjectiveId` = 252090,
    `Floor` = 252090,
    `Flags` = 2
WHERE `QuestID` = 29419
  AND `Idx1` = 1;

UPDATE `quest_poi_points`
SET `X` = 1414,
    `Y` = 3534
WHERE `QuestID` = 29419
  AND `BlobIndex` = 1;
