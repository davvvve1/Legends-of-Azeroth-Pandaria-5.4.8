-- The Passion of Shen-zin Su (29423): track the delivery objective and keep
-- the accepted quest focused on the Temple of Five Dawns.

UPDATE `quest_objective`
SET `flags` = `flags` | 1
WHERE `questId` = 29423
  AND `id` = 252009;

DELETE FROM `quest_objective_effects`
WHERE `objectiveId` = 252009;
INSERT INTO `quest_objective_effects` (`objectiveId`, `visualEffect`) VALUES
(252009, 569);

-- The obsolete start blob competes with the delivery and turn-in blobs in
-- the 5.4.8 client. Keep only the active objective and completed turn-in POIs.
DELETE FROM `quest_poi_points`
WHERE `QuestID` = 29423
  AND `BlobIndex` = 0;

DELETE FROM `quest_poi`
WHERE `QuestID` = 29423
  AND `Idx1` = 0;

UPDATE `quest_poi`
SET `QuestObjectiveId` = 252009,
    `Floor` = 252009,
    `Flags` = 2
WHERE `QuestID` = 29423
  AND `Idx1` = 1;

UPDATE `quest_poi_points`
SET `X` = 971,
    `Y` = 3603
WHERE `QuestID` = 29423
  AND `BlobIndex` = 1;

DELETE FROM `areatrigger_scripts`
WHERE `entry` = 7835;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES
(7835, 'AreaTrigger_at_temple_entrance');
