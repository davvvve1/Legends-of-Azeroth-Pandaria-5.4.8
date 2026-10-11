-- Weed War (30052) / Weed War II (30321): both weed templates used friendly
-- or neutral factions.  Keep them visibly hostile and attackable; the creature
-- script also retains the retail spell-click interaction and awards kill credit
-- to the player who owns the personal summon.
UPDATE `creature_template`
SET `faction` = 14,
    `unit_flags` = `unit_flags` & ~(2 | 256 | 512 | 33554432)
WHERE `entry` IN (57306,57308);
