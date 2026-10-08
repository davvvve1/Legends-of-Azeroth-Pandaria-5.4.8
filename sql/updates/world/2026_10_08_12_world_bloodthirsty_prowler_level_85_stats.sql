-- The open-world Bloodthirsty Prowler copy was lowered from level 90 to 85,
-- but it still inherited the scenario creature's health and damage values.
-- Match the normal level-85 Wild Prowler combat stat profile.
UPDATE `creature_template`
SET `Health_mod` = 1.0,
    `mindmg` = 4328,
    `maxdmg` = 7359,
    `attackpower` = 20722
WHERE `entry` = 300001;
