-- The Nexus (map 576) has overly punishing physical damage for its intended
-- level range. Reduce the instance's Wrath combat creatures to 50% of their
-- imported normal and heroic damage multipliers. Spell base damage and every
-- other dungeon remain unchanged.

CREATE TEMPORARY TABLE `nexus_damage_target` (
    `entry` INT UNSIGNED NOT NULL PRIMARY KEY,
    `normal_damage` FLOAT NOT NULL,
    `heroic_damage` FLOAT NULL
);

INSERT INTO `nexus_damage_target`
(`entry`, `normal_damage`, `heroic_damage`)
VALUES
(26716, 4.10, 14.25), -- Azure Warder
(26722, 3.60, 14.25), -- Azure Magus
(26723, 5.20, 18.70), -- Keristrasza
(26727, 2.75,  6.95), -- Mage Hunter Ascendant
(26728, 4.05,  6.70), -- Mage Hunter Initiate
(26729, 3.85,  8.35), -- Steward
(26730, 4.00, 12.30), -- Mage Slayer
(26731, 1.35, 15.00), -- Grand Magus Telestra
(26734, 3.85, 10.00), -- Azure Enforcer
(26735, 3.85, 14.90), -- Azure Scale-Binder
(26736, 1.35,  1.35), -- Azure Skyrazor
(26737, 3.75,  7.05), -- Crazed Mana-Surge
(26761, 1.35,  1.35), -- Crazed Mana-Wyrm
(26763, 7.75, 14.55), -- Anomalus
(26782, 3.90,  7.40), -- Crystalline Keeper
(26792, 6.00, 16.45), -- Crystalline Protector
(26794, 6.90, 18.50), -- Ormorok the Tree-Shaper
(26796,12.15, 12.15), -- Commander Stoutbeard
(26800, 2.40, 12.70), -- Alliance Berserker
(26802, 2.55,  9.35), -- Alliance Ranger
(26805, 2.70,  8.85), -- Alliance Cleric
(26918, 1.35,  1.35), -- Chaotic Rift
(27048, 1.35,  NULL), -- Breath Caster
(27949, 6.00,  NULL), -- Alliance Commander
(28231, 4.10, 10.45); -- Crystalline Tender

UPDATE `creature_template` AS `ct`
INNER JOIN `nexus_damage_target` AS `n` ON `n`.`entry` = `ct`.`entry`
SET `ct`.`dmg_multiplier` = LEAST(`ct`.`dmg_multiplier`, `n`.`normal_damage`);

UPDATE `creature_difficulty` AS `cd`
INNER JOIN `nexus_damage_target` AS `n` ON `n`.`entry` = `cd`.`id`
SET `cd`.`damage_mod` = LEAST(`cd`.`damage_mod`, `n`.`heroic_damage`)
WHERE `cd`.`difficulty` = 'DUNGEON_HEROIC'
  AND `n`.`heroic_damage` IS NOT NULL;

DROP TEMPORARY TABLE `nexus_damage_target`;
