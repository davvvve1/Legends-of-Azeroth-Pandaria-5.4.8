-- Blood and Thunder! (25949): restore the timed Horde defeat, troop capture,
-- player abduction, transport to Shallow's End and Earthen Ring rescue.
UPDATE `creature_template`
SET `ScriptName` = 'npc_blood_and_thunder_troop_abductor'
WHERE `entry` = 41809;

UPDATE `creature_template`
SET `ScriptName` = 'npc_blood_and_thunder_player_abductor'
WHERE `entry` = 41838;

DELETE FROM `creature_text`
WHERE `CreatureID` IN (41788, 41793, 41838);

INSERT INTO `creature_text`
    (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`,
     `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`)
VALUES
    (41793, 0, 0, 'We are... beaten. Escape if you can...', 14, 0, 100, 0, 0, 0, 41610, 2,
     'Legionnaire Nazgrim - Blood and Thunder defeat'),
    (41838, 0, 0, 'What a fine ssspecimen you are.', 12, 0, 100, 0, 0, 0, 40707, 0,
     'Zin\'jatar Abductor - player captured'),
    (41838, 1, 0, 'My Lady will be mosst grateful for my effortsss.', 12, 0, 100, 0, 0, 0, 40708, 0,
     'Zin\'jatar Abductor - transporting player'),
    (41788, 0, 0, 'We were most fortunate to escape, $n. Follow me.', 12, 0, 100, 0, 0, 0, 40710, 0,
     'Erunak Stonespeaker - Blood and Thunder rescue');
