-- The Cell (11943): guarantee Prison Casing (35668) from General Cerulean
-- (25716) while the quest is in progress and its item objective is incomplete.
--
-- The legacy negative quest chance is filtered through HasQuestForItem(),
-- which suppresses ordinary quest loot while a player is in a raid group.
-- Explicit conditions preserve the intended eligibility while allowing the
-- item to drop for bot groups that have been converted to raids.

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100
WHERE `entry` = 25716 AND `item` = 35668;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 1
  AND `SourceGroup` = 25716
  AND `SourceEntry` = 35668
  AND `SourceId` = 0
  AND `ElseGroup` = 0
  AND `ConditionTypeOrReference` IN (47, 48);

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
 `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
 `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
 `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(1, 25716, 35668, 0, 0, 47, 0, 11943, 8, 0, 0, 0, 0, '',
 'General Cerulean - Prison Casing requires The Cell in progress'),
(1, 25716, 35668, 0, 0, 48, 0, 262623, 0, 0, 1, 0, 0, '',
 'General Cerulean - Prison Casing objective must be incomplete');
