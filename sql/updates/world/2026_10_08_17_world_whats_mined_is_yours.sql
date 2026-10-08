-- What's Mined Is Yours (29930): restore Hao's repeatable cart ride and the
-- scripted Jade Cart that grants delivery credit at Emperor's Omen.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `ScriptName` = 'npc_jade_forest_hao_mann_cart'
WHERE `entry` = 56467;

UPDATE `creature_template`
SET `ScriptName` = 'npc_jade_forest_jade_cart'
WHERE `entry` = 56527;
