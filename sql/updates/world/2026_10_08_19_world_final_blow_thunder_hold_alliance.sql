-- The Final Blow! (31769): remove the defeated Alliance force from Thunder
-- Hold for each player after the quest has actually been turned in.  The quest
-- objectives are currently completed on acceptance, so this phase must depend
-- only on rewarded status rather than QUEST_STATUS_COMPLETE.
START TRANSACTION;

SET @THUNDER_HOLD_ALLIANCE_PHASE := 33554432;

UPDATE `creature`
SET `phaseMask` = @THUNDER_HOLD_ALLIANCE_PHASE
WHERE `map` = 870
  AND (
    (`areaId` = 6524 AND `id` IN
      (66200, 66202, 66283, 66284, 66285, 66286, 66287, 66288,
       66308, 66348, 66395, 66647, 66648, 66649, 66650, 66651, 66654))
    OR `id` = 66203
  );

DELETE FROM `phase_definitions`
WHERE `zoneId` = 5785 AND `entry` = 9;
INSERT INTO `phase_definitions`
(`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `worldMapArea`, `flags`, `comment`)
VALUES
(5785, 9, @THUNDER_HOLD_ALLIANCE_PHASE, 0, 0, 0, 0,
 'Jade Forest - Thunder Hold Alliance before The Final Blow is rewarded');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 25
  AND `SourceGroup` = 5785 AND `SourceEntry` = 9;
INSERT INTO `conditions`
(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`,
 `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`,
 `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(25, 5785, 9, 0, 0, 8, 0, 31769, 0, 0, 1, 0, 0, '',
 'Thunder Hold Alliance - The Final Blow not rewarded');

COMMIT;
