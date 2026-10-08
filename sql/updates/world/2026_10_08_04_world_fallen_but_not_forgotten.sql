-- Fallen But Not Forgotten (25753 Alliance, 25964 Horde)
-- Injured Assault Volunteer (41784) had its gossip menu and npcflag, but the
-- menu option and the rescue action were missing. Both quest variants use
-- creature objective 41281 for their rescue credit.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `gossip_menu_id` = 11481,
    `AIName` = 'SmartAI'
WHERE `entry` = 41784;

DELETE FROM `gossip_menu_option`
WHERE `MenuID` = 11481 AND `OptionID` = 0;

INSERT INTO `gossip_menu_option`
    (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`,
     `OptionType`, `OptionNpcflag`, `ActionMenuID`, `ActionPoiID`,
     `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`)
VALUES
    (11481, 0, 0, 'Stay still. I\'ll get you to safety.', 51829,
     1, 1, 0, 0, 0, 0, NULL, 0, 18019);

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 15
  AND `SourceGroup` = 11481
  AND `SourceEntry` = 0;

INSERT INTO `conditions`
    (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
     `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
     `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
     `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
    (15, 11481, 0, 0, 0, 9, 0, 25753, 0, 0, 0, 0, 0, '',
     'Show rescue option while Alliance Fallen But Not Forgotten is active'),
    (15, 11481, 0, 0, 1, 9, 0, 25964, 0, 0, 0, 0, 0, '',
     'Show rescue option while Horde Fallen But Not Forgotten is active');

DELETE FROM `creature_text`
WHERE `CreatureID` = 41784 AND `GroupID` = 0;

INSERT INTO `creature_text`
    (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`,
     `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`)
VALUES
    (41784, 0, 0, 'You\'re a welcome sight.', 12, 0, 0, 0, 0, 0, 0, 41218, 0,
     'Injured Assault Volunteer - Rescue response'),
    (41784, 0, 1, 'I was told that losing my mount was a death sentence. Thanks for proving the Admiral wrong.', 12, 0, 0, 0, 0, 0, 0, 41219, 0,
     'Injured Assault Volunteer - Rescue response'),
    (41784, 0, 2, 'You\'re a beautiful, beautiful sight, friend.', 12, 0, 0, 0, 0, 0, 0, 41220, 0,
     'Injured Assault Volunteer - Rescue response'),
    (41784, 0, 3, 'Did everyone else make it? Did the attack succeed?', 12, 0, 0, 0, 0, 0, 0, 41221, 0,
     'Injured Assault Volunteer - Rescue response');

DELETE FROM `smart_scripts`
WHERE `entryorguid` = 41784 AND `source_type` = 0;

INSERT INTO `smart_scripts`
    (`entryorguid`, `source_type`, `id`, `link`,
     `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
     `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
     `action_type`, `action_param1`, `action_param2`, `action_param3`,
     `action_param4`, `action_param5`, `action_param6`,
     `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`,
     `target_x`, `target_y`, `target_z`, `target_o`, `comment`)
VALUES
    (41784, 0, 0, 1,
     62, 0, 100, 0,
     11481, 0, 0, 0, 0,
     33, 41281, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Injured Assault Volunteer - On rescue gossip - Give quest credit'),

    (41784, 0, 1, 2,
     61, 0, 100, 0,
     0, 0, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Injured Assault Volunteer - On rescue - Say response'),

    (41784, 0, 2, 3,
     61, 0, 100, 0,
     0, 0, 0, 0, 0,
     72, 0, 0, 0, 0, 0, 0,
     7, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Injured Assault Volunteer - On rescue - Close gossip'),

    (41784, 0, 3, 0,
     61, 0, 100, 0,
     0, 0, 0, 0, 0,
     41, 3000, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0,
     0, 0, 0, 0,
     'Injured Assault Volunteer - On rescue - Despawn');
