-- Lay of the Land (29822)
-- Restore the missing Shrine of the Sun proximity-credit bunny and keep both
-- shrine triggers stationary at their intended landmarks.
DELETE FROM `creature` WHERE `guid` = 90029822 AND `id` = 63058;

INSERT INTO `creature`
    (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`,
     `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`,
     `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`,
     `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`,
     `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`)
VALUES
    (90029822, 63058, 870, 5785, 5785, 1, 1, 0, 0,
     17612, 0, 2353.57, -782.584, 414.573, 0,
     60, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '', 0, 0);

UPDATE `creature`
SET `wander_distance` = 0, `MovementType` = 0
WHERE `id` IN (63058, 63059) AND `map` = 870 AND `zoneId` = 5785;

-- Source-backed tracking spells could silently fail.  Check once per second
-- for a player near each shrine and award its creature objective directly.
UPDATE `smart_scripts`
SET `event_param1` = 1000, `event_param2` = 1000,
    `event_param3` = 1000, `event_param4` = 1000,
    `action_type` = 33, `action_param1` = 63058,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `target_type` = 21, `target_param1` = 25,
    `comment` = 'Shrine of the Sun - Lay of the Land - Give visit credit directly'
WHERE `entryorguid` = 63058 AND `source_type` = 0 AND `id` = 1
  AND `event_type` = 1;

UPDATE `smart_scripts`
SET `event_param1` = 1000, `event_param2` = 1000,
    `event_param3` = 1000, `event_param4` = 1000,
    `action_type` = 33, `action_param1` = 63059,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `target_type` = 21, `target_param1` = 25,
    `comment` = 'Shrine of the Moon - Lay of the Land - Give visit credit directly'
WHERE `entryorguid` = 63059 AND `source_type` = 0 AND `id` = 1
  AND `event_type` = 1;

-- Repair the quest map/arrow objective mappings.  The point rows already hold
-- the correct Sun, Moon and Dook Ookem coordinates, but all three POIs were
-- incorrectly assigned to objective zero.
UPDATE `quest_poi`
SET `ObjectiveIndex` = 0, `QuestObjectiveId` = 265716,
    `MapID` = 870, `WorldMapAreaId` = 806, `Flags` = 1
WHERE `QuestID` = 29822 AND `Idx1` = 1;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 1, `QuestObjectiveId` = 268123,
    `MapID` = 870, `WorldMapAreaId` = 806, `Flags` = 1
WHERE `QuestID` = 29822 AND `Idx1` = 2;

UPDATE `quest_poi`
SET `ObjectiveIndex` = 2, `QuestObjectiveId` = 268124,
    `MapID` = 870, `WorldMapAreaId` = 806, `Flags` = 1
WHERE `QuestID` = 29822 AND `Idx1` = 3;
