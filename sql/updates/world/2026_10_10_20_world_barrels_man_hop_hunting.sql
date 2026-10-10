-- Barrels, Man (30077) and Hop Hunting (30053)
--
-- Barreled Pandaren had a valid three-second breaking aura and completion
-- script, but entry 57662 lacked the spell-click NPC flag, so clients could
-- not start the interaction.  Gardener Fran's database option exists, but its
-- SmartAI gossip event is keyed to menu 0/option 62377 instead of her actual
-- menu 13850/option 0.  C++ now presents and handles the Hop Hunting response
-- directly while retaining Fran's SmartAI for her watering event.

CREATE TABLE IF NOT EXISTS `_backup_creature_template_barrels_hops_20261010`
LIKE `creature_template`;

CREATE TABLE IF NOT EXISTS `_backup_conditions_barrels_hops_20261010`
LIKE `conditions`;

INSERT IGNORE INTO `_backup_creature_template_barrels_hops_20261010`
SELECT *
FROM `creature_template`
WHERE `entry` IN (57662, 62377);

INSERT IGNORE INTO `_backup_conditions_barrels_hops_20261010`
SELECT *
FROM `conditions`
WHERE (`SourceTypeOrReferenceId` = 18 AND `SourceGroup` = 57662 AND `SourceEntry` = 108817)
   OR (`SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 13850 AND `SourceEntry` = 0);

START TRANSACTION;

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 16777216
WHERE `entry` = 57662;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 18
  AND `SourceGroup` = 57662
  AND `SourceEntry` = 108817;

INSERT INTO `conditions`
    (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`,
     `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`,
     `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
    (18, 57662, 108817, 0, 0, 9, 0, 30077, 0, 0, 0, 0, 0, '',
     'Barreled Pandaren - Spell click requires Barrels, Man active');

UPDATE `creature_template`
SET `ScriptName` = 'npc_gardener_fran_hop_hunting'
WHERE `entry` = 62377;

COMMIT;
