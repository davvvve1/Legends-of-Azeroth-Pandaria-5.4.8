-- A Better Vantage (25582, 25634, 25955)
-- The three client AreaTriggers are present at the intended scouting sites,
-- but had no server-side scripts to award their creature objectives.
DELETE FROM `areatrigger_scripts`
WHERE `entry` IN (5882, 5883, 5884);

INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`)
VALUES
    (5882, 'SmartTrigger'),
    (5883, 'SmartTrigger'),
    (5884, 'SmartTrigger');

DELETE FROM `smart_scripts`
WHERE `source_type` = 2 AND `entryorguid` IN (5882, 5883, 5884);

INSERT INTO `smart_scripts`
    (`entryorguid`, `source_type`, `id`, `link`,
     `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
     `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
     `action_type`, `action_param1`, `action_param2`, `action_param3`,
     `action_param4`, `action_param5`, `action_param6`,
     `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`,
     `target_x`, `target_y`, `target_z`, `target_o`, `comment`)
VALUES
    (5882, 2, 0, 0,
     46, 0, 100, 0,
     5882, 0, 0, 0, 0,
     33, 40963, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'A Better Vantage - Northern Quel Dormir Gardens - Give scouting credit'),

    (5883, 2, 0, 0,
     46, 0, 100, 0,
     5883, 0, 0, 0, 0,
     33, 40964, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'A Better Vantage - Tunnel west of Quel Dormir Gardens - Give scouting credit'),

    (5884, 2, 0, 0,
     46, 0, 100, 0,
     5884, 0, 0, 0, 0,
     33, 40965, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'A Better Vantage - Southern structures - Give scouting credit');
