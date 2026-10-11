-- The Great Bank Heist (14122): Personal Riches used to be created roughly
-- 32 seconds after the vault credit. An interrupted timed action list then
-- left the player permanently at vault 1/1 and Personal Riches 0/1. Give the
-- item first; delayed credit is safe because the vault sequence can be retried.
UPDATE `smart_scripts`
SET `action_type` = 56,
    `action_param1` = 46858,
    `action_param2` = 1,
    `action_param3` = 0,
    `action_param4` = 0,
    `action_param5` = 0,
    `action_param6` = 0,
    `comment` = 'First Bank of Kezan Vault - On Script - Create Item Personal Riches'
WHERE `entryorguid` = 3548600 AND `source_type` = 9 AND `id` = 0;

UPDATE `smart_scripts`
SET `action_type` = 33,
    `action_param1` = 35486,
    `action_param2` = 0,
    `action_param3` = 0,
    `action_param4` = 0,
    `action_param5` = 0,
    `action_param6` = 0,
    `comment` = 'First Bank of Kezan Vault - On Script - Give Kill Credit'
WHERE `entryorguid` = 3548600 AND `source_type` = 9 AND `id` = 5;
