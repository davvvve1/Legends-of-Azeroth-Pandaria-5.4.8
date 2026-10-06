-- Data-driven world-drop stack sizes and optional elite scaling. Existing
-- rows retain their old one-item stack and +20% elite chance behaviour.
ALTER TABLE `world_drop_loot_template`
    ADD COLUMN IF NOT EXISTS `min_count` TINYINT UNSIGNED NOT NULL DEFAULT 1 AFTER `chance`,
    ADD COLUMN IF NOT EXISTS `max_count` TINYINT UNSIGNED NOT NULL DEFAULT 1 AFTER `min_count`,
    ADD COLUMN IF NOT EXISTS `elite_bonus` TINYINT(1) UNSIGNED NOT NULL DEFAULT 1 AFTER `max_count`;

-- Burning Crusade world drops (Outland creatures, levels 58-73).
-- Items in the same non-zero group are selected first and then use the
-- group's chance, giving one random gem rather than rolling all six gems.
DELETE FROM `world_drop_loot_template`
WHERE `expansion` = 1
  AND `item` IN (21929, 23077, 23079, 23107, 23112, 23117,
                 23436, 23437, 23438, 23439, 23440, 23441, 23427);

INSERT INTO `world_drop_loot_template`
    (`item`, `chance`, `min_count`, `max_count`, `elite_bonus`,
     `expansion`, `level_min`, `level_max`, `group`, `info`)
VALUES
    -- One random uncommon (green-quality) raw gem: 4.2% total.
    (21929, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),
    (23077, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),
    (23079, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),
    (23107, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),
    (23112, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),
    (23117, 4.2, 1, 1, 0, 1, 58, 73, 1, 'BC uncommon gem world drop'),

    -- One random rare (blue-quality) raw gem: 1.4% total.
    (23436, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),
    (23437, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),
    (23438, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),
    (23439, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),
    (23440, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),
    (23441, 1.4, 1, 1, 0, 1, 58, 73, 2, 'BC rare gem world drop'),

    -- Independent 30% roll for a stack of one to four Eternium Ore.
    (23427, 30.0, 1, 4, 0, 1, 58, 73, 0, 'BC Eternium Ore world drop');
