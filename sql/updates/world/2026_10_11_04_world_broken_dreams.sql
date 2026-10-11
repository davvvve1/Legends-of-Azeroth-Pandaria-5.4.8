-- Broken Dreams (29952)
-- Chen Stormstout (56133) had no gossip option or event for the quest, and
-- the original interactive-memory actors are absent from this world data.
-- Provide a reliable narrated fallback and award the real objective credit
-- only after Chen finishes the story.
START TRANSACTION;

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `gossip_menu_id` = 56133
WHERE `entry` = 56133;

DELETE FROM `npc_text`
WHERE `ID` = 990052;

INSERT INTO `npc_text`
    (`ID`, `text0_0`, `BroadcastTextID0`, `lang0`, `Probability0`,
     `VerifiedBuild`)
VALUES
    (990052,
     '<Chen acknowledges you with a nod but looks uncharacteristically sullen.>',
     0, 0, 1, 18414);

DELETE FROM `gossip_menu`
WHERE `MenuID` = 56133;

INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`)
VALUES (56133, 990052, 18414);

-- Preserve Cleaning House (30078), but replace its corrupted imported text
-- with English while adding the missing Broken Dreams choice.
DELETE FROM `gossip_menu_option`
WHERE `MenuID` = 56133 AND `OptionID` IN (0, 1);

INSERT INTO `gossip_menu_option`
    (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`,
     `OptionBroadcastTextID`, `OptionType`, `OptionNpcflag`, `ActionMenuID`,
     `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`,
     `BoxBroadcastTextID`, `VerifiedBuild`)
VALUES
    (56133, 0, 0, 'I am ready. Let us clean out the brewery.',
     0, 1, 1, 0, 0, 0, 0, NULL, 0, 18414),
    (56133, 1, 0, 'What happened, Chen?',
     0, 1, 1, 0, 0, 0, 0, NULL, 0, 18414);

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 15
  AND `SourceGroup` = 56133
  AND `SourceEntry` IN (0, 1);

INSERT INTO `conditions`
    (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
     `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
     `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
     `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
    (15, 56133, 0, 0, 0, 9, 0, 30078, 0, 0, 0, 0, 0, '',
     'Chen Stormstout - Show Cleaning House option while quest 30078 is active'),
    (15, 56133, 1, 0, 0, 9, 0, 29952, 0, 0, 0, 0, 0, '',
     'Chen Stormstout - Show Broken Dreams option while quest 29952 is active');

DELETE FROM `creature_text`
WHERE `CreatureID` = 56133 AND `GroupID` BETWEEN 5 AND 8;

INSERT INTO `creature_text`
    (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`,
     `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`,
     `BroadcastTextId`, `TextRange`, `comment`)
VALUES
    (56133, 5, 0,
     'I found the brewery and went inside, hoping to meet the rest of the Stormstout family.',
     12, 0, 100, 1, 0, 0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams story 1'),
    (56133, 6, 0,
     'Uncle Gao refused to welcome me. Strange beer elementals and a drunken hozen attacked while I tried to reason with him.',
     12, 0, 100, 1, 0, 0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams story 2'),
    (56133, 7, 0,
     'I still offered to sit, talk, and share a beer, but he wanted nothing to do with me.',
     12, 0, 100, 1, 0, 0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams story 3'),
    (56133, 8, 0,
     'At last he told me that I was not welcome there. Shunned by my own family, I turned and left.',
     12, 0, 100, 1, 0, 0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams story 4');

DELETE FROM `smart_scripts`
WHERE (`entryorguid` = 56133 AND `source_type` = 0 AND `id` IN (3, 4))
   OR (`entryorguid` = 5613301 AND `source_type` = 9);

INSERT INTO `smart_scripts`
    (`entryorguid`, `source_type`, `id`, `link`,
     `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
     `event_param1`, `event_param2`, `event_param3`, `event_param4`,
     `event_param5`, `action_type`, `action_param1`, `action_param2`,
     `action_param3`, `action_param4`, `action_param5`, `action_param6`,
     `target_type`, `target_param1`, `target_param2`, `target_param3`,
     `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`)
VALUES
    (56133, 0, 3, 4,
     62, 0, 100, 0,
     56133, 1, 0, 0, 0,
     80, 5613301, 2, 0, 0, 0, 0,
     1, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams selected - Start narrated story'),
    (56133, 0, 4, 0,
     61, 0, 100, 0,
     0, 0, 0, 0, 0,
     72, 0, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams selected - Close gossip'),

    (5613301, 9, 0, 0,
     0, 0, 100, 0,
     0, 0, 0, 0, 0,
     1, 5, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams - Story 1'),
    (5613301, 9, 1, 0,
     0, 0, 100, 0,
     5000, 5000, 0, 0, 0,
     1, 6, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams - Story 2'),
    (5613301, 9, 2, 0,
     0, 0, 100, 0,
     6000, 6000, 0, 0, 0,
     1, 7, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams - Story 3'),
    (5613301, 9, 3, 0,
     0, 0, 100, 0,
     6000, 6000, 0, 0, 0,
     1, 8, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams - Story 4'),
    (5613301, 9, 4, 0,
     0, 0, 100, 0,
     6000, 6000, 0, 0, 0,
     33, 56680, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Chen Stormstout - Broken Dreams - Credit Listen to Chens story');

COMMIT;
