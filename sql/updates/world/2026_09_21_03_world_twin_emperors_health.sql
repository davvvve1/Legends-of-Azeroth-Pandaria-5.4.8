-- Both emperors previously used Health_mod = 650 on this server.
-- Keep 75% of their health. Absolute values make this update repeatable.
UPDATE `creature_template`
SET `Health_mod` = 487.5
WHERE `entry` IN (15275, 15276);
