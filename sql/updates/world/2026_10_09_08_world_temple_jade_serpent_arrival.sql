-- The Temple of the Jade Serpent (29932): Wind-Yi should offer the message
-- gossip, play the East Temple Arrival scene, grant credit 57290, and move
-- the player into the temple grounds near Elder Sage Rain-Zhu.
UPDATE `creature_template`
SET `ScriptName` = 'npc_elder_sage_wind_yi'
WHERE `entry` = 57242;

DELETE FROM `spell_target_position`
WHERE `id` = 108018
  AND `effIndex` = 0;

INSERT INTO `spell_target_position`
    (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`)
VALUES
    (108018, 0, 870, 922.895, -2607.858, 185.123, 5.34876);
