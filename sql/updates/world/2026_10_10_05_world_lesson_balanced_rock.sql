-- The Lesson of the Balanced Rock (29663): use the dedicated scripted AI for
-- the sparring monks, track the objective, and restore the quest glow.

UPDATE `creature_template`
SET `AIName` = ''
WHERE `entry` = 55019;

DELETE FROM `smart_scripts`
WHERE (`entryorguid` = 55019 AND `source_type` = 0)
   OR (`entryorguid` = 5501900 AND `source_type` = 9);

UPDATE `quest_objective`
SET `flags` = `flags` | 1
WHERE `questId` = 29663
  AND `id` = 254382;

DELETE FROM `quest_objective_effects`
WHERE `objectiveId` = 254382;
INSERT INTO `quest_objective_effects` (`objectiveId`, `visualEffect`) VALUES
(254382, 569);
