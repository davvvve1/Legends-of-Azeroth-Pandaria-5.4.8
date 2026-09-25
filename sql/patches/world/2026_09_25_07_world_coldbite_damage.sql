-- The Root of the Problem (31026): local solo-quest balance adjustment.
-- Small Coldbite Spawn and Hatchlings had full level-90 weapon damage.
-- Reduce their weapon damage (including critical hits) to 25 percent.
-- Matriarch and fixed spell damage are unchanged; not a verified retail value.
UPDATE `creature_template` SET `dmg_multiplier`=0.25
WHERE `entry` IN (62228,63928) AND ROUND(`dmg_multiplier`,6) IN (1,0.2,0.5);
