-- The Lesson of Stifled Pride (29409): Huojin Trainees should be suitable
-- sparring opponents for newly created characters on the Wandering Isle.
-- Both entries are the same trainee with different models/spawn groups.
UPDATE `creature_template`
SET `mindmg` = 4,
    `maxdmg` = 8
WHERE `entry` IN (54586, 65470);
