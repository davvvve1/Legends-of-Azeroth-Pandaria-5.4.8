-- Stoneplow Thirsts (30117)
--
-- The quest correctly points to Loon Mai (56720) at Stoneplow, but her only
-- spawn was restricted to phase mask 1.  Loon Mai is the persistent quest hub
-- for several converging Stoneplow storylines and must remain available when
-- players arrive in another quest phase.

CREATE TABLE IF NOT EXISTS `_backup_creature_stoneplow_thirsts_20261010`
LIKE `creature`;

INSERT IGNORE INTO `_backup_creature_stoneplow_thirsts_20261010`
SELECT *
FROM `creature`
WHERE `guid` = 513516 AND `id` = 56720;

UPDATE `creature`
SET `phaseMask` = 4294967295,
    `npcflag` = 3,
    `unit_flags` = 32768,
    `spawntimesecs` = 30,
    `spawntimesecs_max` = 0
WHERE `guid` = 513516 AND `id` = 56720;
