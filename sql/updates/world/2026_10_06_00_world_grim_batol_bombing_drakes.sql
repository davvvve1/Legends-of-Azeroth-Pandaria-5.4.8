-- Restore Grim Batol's introductory Battered Red Drake bombing run.
-- Entry 39294 is the real vehicle (VehicleId 681, Engulfing Flames 74039).
-- Entry 42571 is a separate post-Throngus shortcut that shares the model.

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 16777216,
    `ScriptName` = 'npc_battered_red_drake'
WHERE `entry` = 39294;

DELETE FROM `npc_spellclick_spells`
WHERE `npc_entry` = 39294;

INSERT INTO `npc_spellclick_spells`
    (`npc_entry`, `spell_id`, `cast_flags`, `user_type`)
VALUES
    (39294, 80343, 1, 0);
