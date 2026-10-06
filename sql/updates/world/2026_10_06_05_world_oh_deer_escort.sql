-- Oh, Deer! (25392): replace the incomplete click/despawn SmartAI with the
-- scripted Injured Fawn escort back to Mylune.
UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_hyjal_injured_fawn',
    `npcflag` = `npcflag` | 16777216
WHERE `entry` = 39999;

DELETE FROM `smart_scripts`
WHERE (`entryorguid` = 39999 AND `source_type` = 0)
   OR (`entryorguid` = 3999900 AND `source_type` = 9);

-- One legacy spawn sits outside the rescue area in Leyara's Sorrow
-- (Mount Hyjal map coordinates 8.25, 35.07).  The remaining spawns are in
-- The Regrowth/The Inferno and include the retail locations around
-- 14.7, 40.6 and 13.2, 44.0.
DELETE FROM `creature`
WHERE `guid` = 284581 AND `id` = 39999 AND `areaId` = 5015;
