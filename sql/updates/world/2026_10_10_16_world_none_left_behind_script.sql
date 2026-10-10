-- Quest 29794 "None Left Behind"
-- Replace the racy SmartAI cross-cast with direct C++ vehicle boarding. The
-- player receives the carry vehicle aura and the Injured Sailor is placed in
-- seat 0 by npc_injured_sailor_none_left_behind.

CREATE TABLE IF NOT EXISTS `_backup_smart_scripts_none_left_behind_20261010`
LIKE `smart_scripts`;

INSERT IGNORE INTO `_backup_smart_scripts_none_left_behind_20261010`
SELECT *
FROM `smart_scripts`
WHERE `source_type` = 0
  AND `entryorguid` = 55999;

DELETE FROM `smart_scripts`
WHERE `source_type` = 0
  AND `entryorguid` = 55999;

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_injured_sailor_none_left_behind',
    `npcflag` = `npcflag` | 16777216
WHERE `entry` = 55999;

UPDATE `npc_spellclick_spells`
SET `cast_flags` = 3
WHERE `npc_entry` = 55999
  AND `spell_id` = 129340;
