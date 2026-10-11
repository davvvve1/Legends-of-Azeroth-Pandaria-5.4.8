-- Weed War (30052) / Weed War II (30321): Gai Lan's gossip previously
-- awarded an unrelated self credit but never started spell 114494, and that
-- periodic dummy aura had no script to create clickable personal weeds.

-- The only gossip option was incorrectly conditioned on Hop Hunting (30053),
-- hiding the event starter after that breadcrumb was complete and from the
-- repeatable Weed War II daily.  ElseGroup makes the two active quests ORed.
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 13334
  AND `SourceEntry` = 0;
INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,
 `ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,
 `ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(15,13334,0,0,0,9,0,30052,0,0,0,0,0,'',
 'Gai Lan gossip option requires Weed War'),
(15,13334,0,0,1,9,0,30321,0,0,0,0,0,'',
 'Gai Lan gossip option requires Weed War II');

UPDATE `smart_scripts`
SET `link` = 3
WHERE `entryorguid` = 57385 AND `source_type` = 0 AND `id` = 2;

DELETE FROM `smart_scripts`
WHERE `entryorguid` = 57385 AND `source_type` = 0 AND `id` = 3;
INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,
 `event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,
 `event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,
 `action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,
 `target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,
 `comment`)
VALUES
(57385,0,3,0,61,0,100,0,0,0,0,0,85,114494,0,0,0,0,0,7,0,0,0,0,0,0,0,
 'Gai Lan - Linked - Invoker Cast Weed War');

DELETE FROM `spell_script_names`
WHERE `spell_id` = 114494 AND `ScriptName` = 'spell_vfw_weed_war';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(114494,'spell_vfw_weed_war');

UPDATE `creature_template`
SET `AIName` = '', `ScriptName` = 'npc_vfw_weed_war_weed'
WHERE `entry` IN (57306,57308);
