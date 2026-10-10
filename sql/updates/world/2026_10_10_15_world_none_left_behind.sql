-- Quest 29794 "None Left Behind"
-- Spellclick spell 129340 turns its target into the rescue vehicle.  The old
-- cast flags made the player cast it on the Injured Sailor, reversing the
-- carrier/passenger relationship.  The sailor's SmartAI then tried to board
-- the player, but the player had no vehicle kit and the rescue never started.

CREATE TABLE IF NOT EXISTS `_backup_npc_spellclick_none_left_behind_20261010`
LIKE `npc_spellclick_spells`;

INSERT IGNORE INTO `_backup_npc_spellclick_none_left_behind_20261010`
SELECT *
FROM `npc_spellclick_spells`
WHERE `npc_entry` = 55999 AND `spell_id` = 129340;

UPDATE `npc_spellclick_spells`
SET `cast_flags` = 3
WHERE `npc_entry` = 55999
  AND `spell_id` = 129340;

-- Keep the sailors clickable for the rescue spell.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 16777216
WHERE `entry` = 55999;
