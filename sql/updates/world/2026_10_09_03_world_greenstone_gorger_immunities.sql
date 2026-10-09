-- Greenstone Gorgers are ordinary tamable beasts and must accept player
-- debuffs, including bleeds. Remove stale template and spawn overrides.
UPDATE `creature_template`
SET `mechanic_immune_mask` = 0,
    `unit_flags` = `unit_flags` & ~258
WHERE `entry` IN (56404, 56543);

UPDATE `creature`
SET `unit_flags` = `unit_flags` & ~258
WHERE `id` IN (56404, 56543);
