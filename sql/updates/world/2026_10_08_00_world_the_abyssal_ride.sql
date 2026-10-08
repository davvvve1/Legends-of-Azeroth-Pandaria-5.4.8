-- The Abyssal Ride (25371)
-- Restore the missing lure, personal seahorse vehicle, riding prompts and
-- completion credit. The quest already rewards item 54465.

START TRANSACTION;

UPDATE `gameobject_template`
SET `AIName`='', `ScriptName`='go_the_abyssal_ride_braided_rope'
WHERE `entry`=202766;

DELETE FROM `smart_scripts`
WHERE `entryorguid`=202766 AND `source_type`=1;

UPDATE `creature_template`
SET `faction`=35, `npcflag`=16777216, `AIName`='',
    `ScriptName`='npc_the_abyssal_ride_seahorse'
WHERE `entry`=39996;

DELETE FROM `creature_template_movement`
WHERE `CreatureId`=39996;
INSERT INTO `creature_template_movement`
(`CreatureId`,`Ground`,`Swim`,`Flight`,`Rooted`) VALUES
(39996,0,0,1,0);

-- Spell 75207 is the learned Vashj'ir Seahorse mount, not the quest vehicle
-- boarding spell. The C++ AI validates ownership and boards the rider.
DELETE FROM `npc_spellclick_spells`
WHERE `npc_entry`=39996;

DELETE FROM `creature_text`
WHERE `CreatureID`=39996 AND `GroupID` BETWEEN 0 AND 4;
INSERT INTO `creature_text`
(`CreatureID`,`GroupID`,`ID`,`Text`,`Type`,`Language`,`Probability`,`Emote`,
 `Duration`,`Sound`,`BroadcastTextId`,`TextRange`,`comment`) VALUES
(39996,0,0,'Lean LEFT! (Press 1)',42,0,100,0,0,0,0,0,'Abyssal Ride - left prompt'),
(39996,1,0,'Hold On Tight! (Press 2 repeatedly)',42,0,100,0,0,0,0,0,'Abyssal Ride - hold prompt'),
(39996,2,0,'Lean RIGHT! (Press 3)',42,0,100,0,0,0,0,0,'Abyssal Ride - right prompt'),
(39996,3,0,'Good! Keep going!',42,0,100,0,0,0,0,0,'Abyssal Ride - correct response'),
(39996,4,0,'The Abyssal Seahorse is subdued!',42,0,100,0,0,0,0,0,'Abyssal Ride - complete');

COMMIT;
