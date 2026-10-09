-- Jaded Heart (30502): every field spawn begins as the level-86 elite Sha
-- Remnant (59434). Celestial Jade (80074 / 114297) changes it into the
-- level-86 normal-rank weakened form (59454) with exactly half the HP pool.

UPDATE `creature_template`
SET `minlevel` = 86,
    `maxlevel` = 86,
    `exp` = 4,
    `faction` = 2400,
    `rank` = 0,
    `mindmg` = 5525,
    `maxdmg` = 9393,
    `attackpower` = 24448,
    `dmg_multiplier` = 1,
    `MovementType` = 0,
    `Health_mod` = 5,
    `AIName` = '',
    `ScriptName` = ''
WHERE `entry` = 59454;

-- Two weakened templates were incorrectly spawned directly in the world.
-- They must respawn as elite remnants and only weaken through Celestial Jade.
UPDATE `creature`
SET `id` = 59434,
    `spawntimesecs` = 300
WHERE `id` = 59454
  AND `map` = 870
  AND `zoneId` = 5785
  AND `areaId` = 5876;

DELETE FROM `spell_script_names`
WHERE `spell_id` = 114297;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(114297, 'spell_jade_forest_celestial_jade');
