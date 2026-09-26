-- Jade Forest quest fixes

-- Quest 29725: SI:7 Report: Fire From the Sky
-- Attach Sully "The Pickle" McLeary to the quest script.
UPDATE `creature_template`
SET `ScriptName`='npc_sully_fire_from_the_sky'
WHERE `entry`=55282;

-- Quest 29727: SI:7 Report: Take No Prisoners
-- Restore missing objective credit when speaking with Amber Kearnen.
UPDATE `creature_template`
SET `AIName`='SmartAI'
WHERE `entry`=55283;

DELETE FROM `smart_scripts`
WHERE `entryorguid`=55283
  AND `source_type`=0
  AND `event_type`=62
  AND `event_param1`=55283
  AND `action_type`=33
  AND `action_param1`=55408;

INSERT INTO `smart_scripts`
(`entryorguid`,`source_type`,`id`,`link`,
 `event_type`,`event_phase_mask`,`event_chance`,`event_flags`,
 `event_param1`,`event_param2`,`event_param3`,`event_param4`,
 `action_type`,`action_param1`,`action_param2`,`action_param3`,
 `action_param4`,`action_param5`,`action_param6`,
 `target_type`,`target_param1`,`target_param2`,`target_param3`,
 `target_x`,`target_y`,`target_z`,`target_o`,`comment`)
VALUES
(55283,0,1,0,
 62,0,100,0,
 55283,0,0,0,
 33,55408,0,0,
 0,0,0,
 7,0,0,0,
 0,0,0,0,
 'Quest 29727 - On Gossip - Kill Credit 55408');
