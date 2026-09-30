-- Spirit of Harmony (76061) should stack to 100.
-- Mote of Harmony (89112) already stacks to 200 and is intentionally unchanged.

UPDATE `item_template`
SET `Stackable` = 100
WHERE `entry` = 76061;
