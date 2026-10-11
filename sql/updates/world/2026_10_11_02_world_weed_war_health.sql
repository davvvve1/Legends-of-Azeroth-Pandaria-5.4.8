-- Weed War (30052) / Weed War II (30321): level 86 weeds inherited the full
-- Pandaria creature base health.  Faction 14 applies the core's 0.7 open-world
-- hostile health factor, so this modifier yields 1000 HP from basehp4 184350.
-- The creature script also enforces exactly 1000/1000 HP at spawn time.
UPDATE `creature_template`
SET `Health_mod` = 0.00774923
WHERE `entry` IN (57306,57308);
