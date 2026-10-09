-- The Missing Driver (29419): use the quest-aware scripts for the trapped
-- Min Dimwind and the personal copy that runs back to Merchant Lorvo's cart.
-- The old SmartAI selected the nearest player instead of the triggering
-- player, cast the summon spell on Min himself, and referenced no existing
-- waypoint path, leaving the objective and scene unreliable.

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_min_dimwind'
WHERE `entry` = 54855;

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_min_dimwind_summon'
WHERE `entry` = 56503;

DELETE FROM `smart_scripts`
WHERE `source_type` = 0
  AND `entryorguid` IN (54855, 56503);

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 22
  AND `SourceEntry` IN (54855, 56503);
