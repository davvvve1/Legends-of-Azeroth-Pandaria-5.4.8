-- Quest 29780 "Do No Evil"
-- Ruk-Ruk had only two generic SmartAI spell casts.  The encounter-specific
-- Ji Firepaw assistance and shared quest credit were absent.

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'boss_ruk_ruk'
WHERE `entry` = 55634;

DELETE FROM `creature_text`
WHERE `CreatureID` = 56134
  AND `GroupID` BETWEEN 0 AND 3;

INSERT INTO `creature_text`
    (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`)
VALUES
    (56134, 0, 0, 'Let''s do this, monkeyface!',                 12, 0, 100, 5, 0, 27318, 0, 60619, 0, 'Ji Firepaw - Do No Evil - engage'),
    (56134, 1, 0, 'That all you''ve got?!',                     12, 0, 100, 5, 0, 27316, 0, 60620, 0, 'Ji Firepaw - Do No Evil - combat'),
    (56134, 2, 0, 'This monkey''s about to get slapped!',       12, 0, 100, 5, 0, 27317, 0, 60621, 0, 'Ji Firepaw - Do No Evil - combat'),
    (56134, 3, 0, 'And he''s down for the count! Good going.',  12, 0, 100, 5, 0, 27315, 0, 60622, 0, 'Ji Firepaw - Do No Evil - defeated');
