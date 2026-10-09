-- All We Can Spare (29925): let Toya launch the quest-only Pandaren Kite
-- taxi path from Dawn's Blossom to Emperor's Omen.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `ScriptName` = 'npc_jade_forest_toya'
WHERE `entry` = 56348;
