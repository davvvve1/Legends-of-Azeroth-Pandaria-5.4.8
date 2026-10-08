-- Trapped! (29929): the gossip event targeted the miner's combat victim.
-- These friendly miners have no victim, so award the objective to the player
-- who interacts with them instead.
UPDATE `creature_template`
SET `AIName` = 'SmartAI', `npcflag` = `npcflag` | 1
WHERE `entry` = 56464;

UPDATE `smart_scripts`
SET `action_type` = 33,
    `action_param1` = 56464,
    `action_param2` = 0,
    `action_param3` = 0,
    `action_param4` = 0,
    `action_param5` = 0,
    `action_param6` = 0,
    `target_type` = 7,
    `comment` = 'Greenstone Miner - On gossip hello - Credit player for freeing miner'
WHERE `entryorguid` = 56464
  AND `source_type` = 0
  AND `id` = 0
  AND `event_type` = 64;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceGroup` = 1
  AND `SourceEntry` = 56464
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` = 9
  AND `ConditionValue1` = 29929;

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,
 `NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(22,1,56464,0,0,
 9,0,29929,0,0,
 0,0,0,'','Greenstone Miner interaction requires active Trapped!');
