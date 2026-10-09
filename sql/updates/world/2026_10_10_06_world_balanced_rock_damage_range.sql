-- The Lesson of the Balanced Rock (29663): keep the pole-top sparring monks'
-- fallback melee values appropriate for a level-5 training encounter. The
-- scripted Throw Rock remains their primary attack.

UPDATE `creature_template`
SET `mindmg` = 1,
    `maxdmg` = 2,
    `attackpower` = 0,
    `dmg_multiplier` = 1
WHERE `entry` = 55019;
