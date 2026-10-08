-- Shelled Salvation (25593)
-- Force Spiralung (76337) creates the shell on its unit target. The old
-- spell-click flags made the player cast it on the Spiralung NPC, so no item
-- could be created and the client reported that the spell could not be cast.
UPDATE `npc_spellclick_spells`
SET `cast_flags` = 3
WHERE `npc_entry` = 39745 AND `spell_id` = 76337;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 18
  AND `SourceGroup` = 39745
  AND `SourceEntry` = 76337;

INSERT INTO `conditions`
    (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
     `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
     `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
     `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
    (18, 39745, 76337, 0, 0, 9, 0, 25593, 0, 0, 0, 0, 0, '',
     'Spiralung - Spellclick requires Shelled Salvation active');

UPDATE `creature_template`
SET `AIName` = 'SmartAI'
WHERE `entry` IN (39729, 39745);

DELETE FROM `smart_scripts`
WHERE `entryorguid` IN (39729, 39745) AND `source_type` = 0;

INSERT INTO `smart_scripts`
    (`entryorguid`, `source_type`, `id`, `link`,
     `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
     `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
     `action_type`, `action_param1`, `action_param2`, `action_param3`,
     `action_param4`, `action_param5`, `action_param6`,
     `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`,
     `target_x`, `target_y`, `target_z`, `target_o`, `comment`)
VALUES
    (39745, 0, 0, 0,
     73, 0, 100, 0,
     0, 0, 0, 0, 0,
     41, 500, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Spiralung - On spellclick - Despawn after shell is collected'),

    (39729, 0, 0, 1,
     8, 0, 100, 0,
     76350, 0, 0, 0, 0,
     33, 39729, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Nespirah Survivor - On Spiralung spell hit - Give rescue credit'),

    (39729, 0, 1, 0,
     61, 0, 100, 0,
     0, 0, 0, 0, 0,
     41, 1000, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Nespirah Survivor - After rescue - Despawn');
