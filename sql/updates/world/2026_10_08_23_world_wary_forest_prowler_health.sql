-- Wary Forest Prowler (67071) uses the level 84 Pandaria base health of
-- 135552.  Compensate for the core's 0.7 normal-hostile health adjustment so
-- the final in-world maximum remains exactly 135552 instead of ~2.145M.
UPDATE `creature_template`
SET `Health_mod` = 1.428571
WHERE `entry` = 67071
  AND `minlevel` = 84
  AND `maxlevel` = 84;
