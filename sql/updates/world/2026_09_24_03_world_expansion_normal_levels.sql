-- Correct verified normal-mode level errors in TBC, Wrath and Pandaria.
-- Audit and per-entry sources: contrib/dungeon_normal_levels/audit.md
-- Existing heroic/challenge records are not modified. TBC/Wrath entries below
-- already have heroic overrides. Pandaria uses normal-only overrides because
-- several summoned creatures have no heroic record and fall back to the base.
-- This is a level correction, not a retuning of existing combat multipliers.
-- For normal overrides mirror Creature::SelectLevel: use class/level base damage
-- when nonzero, otherwise retain template damage/AP. All configured creature
-- damage rank rates were checked to be 1 (the default) before applying this patch.
-- Restart worldserver after applying; no binary rebuild is needed.
START TRANSACTION;

-- Grand Warlock Nethekurse; https://mop-shoot.tauri.hu/?npc=16807
UPDATE `creature_template` SET `minlevel` = 71, `maxlevel` = 71 WHERE `entry` = 16807 AND `minlevel` = 71 AND `maxlevel` = 72;

-- Tainted Earthgrab Totem; https://mop-shoot.tauri.hu/?npc=18176
UPDATE `creature_template` SET `minlevel` = 64, `maxlevel` = 64 WHERE `entry` = 18176 AND `minlevel` = 71 AND `maxlevel` = 72;

-- Tainted Stoneskin Totem; https://mop-shoot.tauri.hu/?npc=18177
UPDATE `creature_template` SET `minlevel` = 64, `maxlevel` = 64 WHERE `entry` = 18177 AND `minlevel` = 71 AND `maxlevel` = 72;

-- Wastewalker Captive; https://mop-shoot.tauri.hu/?npc=18206
UPDATE `creature_template` SET `minlevel` = 62, `maxlevel` = 63 WHERE `entry` = 18206 AND `minlevel` = 70 AND `maxlevel` = 70;

-- Wrath-Scryer's Felfire; https://mop-shoot.tauri.hu/?npc=20978
UPDATE `creature_template` SET `minlevel` = 69, `maxlevel` = 69 WHERE `entry` = 20978 AND `minlevel` = 70 AND `maxlevel` = 70;

-- Void Traveler; https://mop-shoot.tauri.hu/?npc=19226
UPDATE `creature_template` SET `minlevel` = 69, `maxlevel` = 69 WHERE `entry` = 19226 AND `minlevel` = 69 AND `maxlevel` = 70;

-- Crystalline Shardling; https://mop-shoot.tauri.hu/?npc=27973
UPDATE `creature_template` SET `minlevel` = 77, `maxlevel` = 77 WHERE `entry` = 27973 AND `minlevel` = 80 AND `maxlevel` = 80;

-- Blistering Steamrager; https://mop-shoot.tauri.hu/?npc=28583
UPDATE `creature_template` SET `minlevel` = 79, `maxlevel` = 80 WHERE `entry` = 28583 AND `minlevel` = 81 AND `maxlevel` = 81;

CREATE TEMPORARY TABLE `normal_level_restoration` (
    `entry` INT UNSIGNED NOT NULL PRIMARY KEY,
    `normal_level` TINYINT UNSIGNED NOT NULL,
    `old_min` TINYINT UNSIGNED NOT NULL,
    `old_max` TINYINT UNSIGNED NOT NULL
);
INSERT INTO `normal_level_restoration` VALUES
    (59051, 87, 90, 90), -- Strife
    (59726, 87, 90, 90), -- Peril
    (59479, 87, 90, 90), -- Yan-Zhu the Uncasked
    (59494, 85, 90, 90), -- Yeasty Brew Alemental
    (59518, 86, 90, 90), -- Bloated Brew Alemental
    (59522, 86, 90, 90), -- Sudsy Brew Alemental
    (58998, 31, 90, 90), -- Scarlet Defender
    (59893, 32, 90, 90), -- Empowering Spirit
    (61634, 91, 92, 92), -- Commander Vo'jak
    (56631, 85, 90, 90), -- Hopling
    (59458, 85, 90, 90), -- Hopling
    (59459, 85, 90, 90), -- Hopling
    (59461, 85, 90, 90), -- Hopling
    (60276, 85, 90, 90), -- Hozen Clinger
    (65522, 87, 90, 90), -- Bubble Shield
    (59706, 32, 90, 90), -- Fuel Tank
    (59980, 41, 90, 90); -- Meat Graft

INSERT INTO `creature_difficulty`
    (`id`, `difficulty`, `health_mod`, `damage_mod`, `level_min`, `level_max`,
     `attackpower`, `mindmg`, `maxdmg`, `rangedattackpower`, `minrangedmg`, `maxrangedmg`)
SELECT n.`entry`, 'DUNGEON_NORMAL', ct.`Health_mod`, ct.`dmg_multiplier`,
    n.`normal_level`, n.`normal_level`,
    IF(s.`damage_exp4` <> 0, s.`attackpower`, ct.`attackpower`),
    IF(s.`damage_exp4` <> 0, s.`damage_exp4`, ct.`mindmg`),
    IF(s.`damage_exp4` <> 0, s.`damage_exp4` * 1.5, ct.`maxdmg`),
    IF(s.`damage_exp4` <> 0, s.`rangedattackpower`, ct.`rangedattackpower`),
    IF(s.`damage_exp4` <> 0, s.`damage_exp4`, ct.`minrangedmg`),
    IF(s.`damage_exp4` <> 0, s.`damage_exp4` * 1.5, ct.`maxrangedmg`)
FROM `normal_level_restoration` n
JOIN `creature_template` ct ON ct.`entry` = n.`entry` AND ct.`exp` = 4
JOIN `creature_classlevelstats` s ON s.`level` = n.`normal_level` AND s.`class` = ct.`unit_class`
LEFT JOIN `creature_difficulty` existing ON existing.`id` = n.`entry` AND existing.`difficulty` = 'DUNGEON_NORMAL'
WHERE existing.`id` IS NULL AND ct.`minlevel` = n.`old_min` AND ct.`maxlevel` = n.`old_max`;

DROP TEMPORARY TABLE `normal_level_restoration`;
COMMIT;
