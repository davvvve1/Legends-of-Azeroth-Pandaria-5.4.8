-- The Battle Ring (30306): Shado-Pan Trainee 58992 is a training opponent,
-- but inherited full level-90 outdoor mob damage and attack power.  Keep its
-- raw swing range low enough to land for roughly 700-900 on a level-90 player.

UPDATE `creature_template`
SET `mindmg` = 1400,
    `maxdmg` = 1800,
    `attackpower` = 0
WHERE `entry` = 58992;
