-- Love's Labor (30495): restore the four jade delivery interactions.
START TRANSACTION;

-- All recipients must be interactable. Shoku's delivery and statue-top taxi
-- options are stored under menu 59392, but his template pointed at menu 0.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1
WHERE `entry` IN (59392,59395,59397,59401);

UPDATE `creature_template`
SET `gossip_menu_id` = 59392
WHERE `entry` = 59392;

UPDATE `gossip_menu_option`
SET `OptionText` = 'I have a jade delivery for you.'
WHERE `MenuID` = 59392 AND `OptionID` = 0;

-- The old condition required Love's Labor to be fully complete before the
-- delivery option appeared, making the Kitemaster objective impossible.
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 15
  AND `SourceGroup` = 59392
  AND `SourceEntry` = 0;
INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,
 `NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(15,59392,0,0,0,
 9,0,30495,0,0,
 0,0,0,'','Kitemaster Shoku jade delivery requires active Love''s Labor');

-- Credit the exact player who speaks to each ground/top recipient instead of
-- every player found in a 100-yard search around the NPC.
UPDATE `smart_scripts`
SET `action_type` = 33,
    `action_param1` = `entryorguid`,
    `action_param2` = 0,
    `action_param3` = 0,
    `target_type` = 7,
    `target_param1` = 0,
    `target_param2` = 0,
    `comment` = 'Love''s Labor - On gossip hello - Credit delivery to player'
WHERE `entryorguid` IN (59395,59397,59401)
  AND `source_type` = 0
  AND `id` = 0
  AND `event_type` = 64;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceGroup` = 1
  AND `SourceEntry` IN (59395,59397,59401)
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` = 9
  AND `ConditionValue1` = 30495;
INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,
 `NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(22,1,59395,0,0,9,0,30495,0,0,0,0,0,'','Historian Dinh delivery requires active Love''s Labor'),
(22,1,59397,0,0,9,0,30495,0,0,0,0,0,'','Taskmaster Emi delivery requires active Love''s Labor'),
(22,1,59401,0,0,9,0,30495,0,0,0,0,0,'','Surveyor Sawa delivery requires active Love''s Labor');

COMMIT;
