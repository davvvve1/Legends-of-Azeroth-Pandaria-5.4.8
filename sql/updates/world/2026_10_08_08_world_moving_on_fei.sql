-- Quest 30648: Moving On
-- Fei had no gossip option and her SmartAI listened for creature entry 59899
-- instead of gossip menu 13646. Restore the retail dialogue and complete the
-- travel objective before moving the player to the Valley of the Four Winds.

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `AIName` = 'SmartAI'
WHERE `entry` = 59899;

DELETE FROM `gossip_menu_option`
WHERE `MenuID` = 13646
  AND `OptionID` = 0;

INSERT INTO `gossip_menu_option`
(`MenuID`,`OptionID`,`OptionIcon`,`OptionText`,`OptionBroadcastTextID`,
 `OptionType`,`OptionNpcflag`,`ActionMenuID`,`ActionPoiID`,`BoxCoded`,
 `BoxMoney`,`BoxText`,`BoxBroadcastTextID`,`VerifiedBuild`)
VALUES
(13646,0,0,'I am ready to leave.',59390,
 1,1,0,0,0,
 0,'',0,18019);

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 15
  AND `SourceGroup` = 13646
  AND `SourceEntry` = 0;

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,
 `NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`)
VALUES
(15,13646,0,0,0,
 9,0,30648,0,0,
 0,0,0,'','Quest 30648 - Show Fei travel option while quest is active');

DELETE FROM `smart_scripts`
WHERE `entryorguid` = 59899
  AND `source_type` = 0;

INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,
 `event_type`,`event_phase_mask`,`event_chance`,`event_flags`,
 `event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,
 `action_type`,`action_param1`,`action_param2`,`action_param3`,
 `action_param4`,`action_param5`,`action_param6`,
 `target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,
 `target_x`,`target_y`,`target_z`,`target_o`,`comment`)
VALUES
(59899,0,0,1,
 62,0,100,0,
 13646,0,0,0,0,
 33,59692,0,0,
 0,0,0,
 7,0,0,0,0,
 0,0,0,0,
 'Quest 30648 - Fei - On ready gossip - Grant travel objective credit'),
(59899,0,1,0,
 61,0,100,0,
 0,0,0,0,0,
 62,870,0,0,
 0,0,0,
 7,0,0,0,0,
 529.507,-706.552,247.248,0.726149,
 'Quest 30648 - Fei - Teleport player to Valley of the Four Winds');
