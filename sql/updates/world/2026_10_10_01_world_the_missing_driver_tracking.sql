-- The Missing Driver (29419): restore the retail rescue area and make Min
-- Dimwind a visible tracked quest objective on the map and in the world.

DELETE FROM `areatrigger_scripts`
WHERE `entry` = 6958;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES
(6958, 'at_min_dimwind_captured');

-- QUEST_OBJECTIVE_FLAG_TRACKED_ON_MINIMAP (0x1) supplies the objective marker.
UPDATE `quest_objective`
SET `flags` = `flags` | 1
WHERE `questId` = 29419
  AND `id` = 252090;

-- Objective 252090 uses visual effect 569 for Min Dimwind's quest glow.
DELETE FROM `quest_objective_effects`
WHERE `objectiveId` = 252090;
INSERT INTO `quest_objective_effects` (`objectiveId`, `visualEffect`) VALUES
(252090, 569);

-- The objective POI is on the outdoor Wandering Isle map, not a dungeon floor.
UPDATE `quest_poi`
SET `Floor` = 0
WHERE `QuestID` = 29419;

-- Keep the objective marker pinned to the trapped driver's actual spawn.
UPDATE `quest_poi_points`
SET `X` = 1414,
    `Y` = 3534
WHERE `QuestID` = 29419
  AND `BlobIndex` = 1;
