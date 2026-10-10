-- The Deadmines (map 36), normal mode
-- Low-level Cataclysm creature_classlevelstats rows have placeholder values
-- (basehp3 = 1 and damage_exp3 = 0). The normal Deadmines templates were
-- selecting those placeholders through exp = 3, leaving bosses with only
-- their Health_mod value as hit points (for example, Glubtok had 35 HP).
-- Use the populated classic-level stat curve for normal creatures. Heroic
-- level 85+ templates retain their Cataclysm stats.

CREATE TABLE IF NOT EXISTS `_backup_creature_template_deadmines_stats_20261010`
LIKE `creature_template`;

INSERT IGNORE INTO `_backup_creature_template_deadmines_stats_20261010`
SELECT ct.*
FROM `creature_template` ct
WHERE ct.`exp` = 3
  AND ct.`minlevel` >= 10
  AND ct.`maxlevel` <= 20
  AND
  (
      ct.`entry` IN (SELECT DISTINCT c.`id` FROM `creature` c WHERE c.`map` = 36)
      OR ct.`entry` = 47296 -- Helix Gearbreaker is summoned by the encounter
  );

UPDATE `creature_template` ct
SET ct.`exp` = 0
WHERE ct.`exp` = 3
  AND ct.`minlevel` >= 10
  AND ct.`maxlevel` <= 20
  AND
  (
      ct.`entry` IN (SELECT DISTINCT c.`id` FROM `creature` c WHERE c.`map` = 36)
      OR ct.`entry` = 47296
  );
