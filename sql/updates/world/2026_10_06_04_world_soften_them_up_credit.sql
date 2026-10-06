-- Soften Them Up (28852) uses two hidden objective-credit creatures rather
-- than the entries of the actual Grim Batol trash. Restore those mappings so
-- both normal kills and Battered Red Drake kills advance the quest.
UPDATE `creature_template`
SET `KillCredit1` = 51182
WHERE `entry` = 39450;

UPDATE `creature_template`
SET `KillCredit1` = 51184
WHERE `entry` IN
(
    39381, 39405, 39414, 39415, 39626, 39854, 39870, 39873,
    39890, 39909, 39954, 39956, 39962, 40166, 40167, 40268,
    40270, 40272, 40273, 40290, 40291, 40306, 40448, 41073
);
