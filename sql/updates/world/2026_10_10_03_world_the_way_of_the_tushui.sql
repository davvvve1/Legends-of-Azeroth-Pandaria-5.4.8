-- The Way of the Tushui (29414): Aysa 54567 had a complete C++ quest and
-- meditation script, but the template still selected the partial SmartAI.
-- Attach the quest script so clicking Aysa always builds her quest menu and
-- accepting the quest starts the source-backed 59652 escort summon.

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_aysa'
WHERE `entry` = 54567;

DELETE FROM `smart_scripts`
WHERE `source_type` = 0
  AND `entryorguid` = 54567;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceEntry` = 54567;

DELETE FROM `creature_queststarter`
WHERE `id` = 54567
  AND `quest` = 29414;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(54567, 29414);

-- Keep the objective highlighted and tracked while Aysa meditates.
UPDATE `quest_objective`
SET `flags` = `flags` | 1
WHERE `questId` = 29414
  AND `id` = 252376;

-- The accepted quest must point into the Cave of Meditation, not back to its
-- obsolete start blob. The turn-in blob remains available after completion.
DELETE FROM `quest_poi_points`
WHERE `QuestID` = 29414
  AND `BlobIndex` = 0;

DELETE FROM `quest_poi`
WHERE `QuestID` = 29414
  AND `Idx1` = 0;

UPDATE `quest_poi`
SET `QuestObjectiveId` = 252376,
    `Floor` = 252376,
    `Flags` = 1
WHERE `QuestID` = 29414
  AND `Idx1` = 1;
