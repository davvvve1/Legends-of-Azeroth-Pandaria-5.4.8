-- Wave Two (25544) / Vigilance on Wings (29177)
-- Make Guardian boarding atomic and award collision credit to the player
-- riding the Guardian.  Both quests share Twilight Lancer objective 40660.

UPDATE `creature_template`
SET `AIName`='', `ScriptName`='npc_avianas_guardian_launcher'
WHERE `entry` IN (40720,40723);

DELETE FROM `smart_scripts`
WHERE `source_type`=0 AND `entryorguid` IN (40720,40723);

UPDATE `creature_template`
SET `AIName`='', `ScriptName`='npc_avianas_guardian_vehicle'
WHERE `entry` IN (39710,40719);

DELETE FROM `smart_scripts`
WHERE `source_type`=0 AND `entryorguid` IN (39710,40719);

-- The old LOS event targeted the Guardian (the event invoker), not its player
-- passenger.  Collision handling now lives on the Guardian vehicle instead.
UPDATE `creature_template`
SET `AIName`=''
WHERE `entry`=40660 AND `AIName`='SmartAI';

DELETE FROM `smart_scripts`
WHERE `source_type`=0 AND `entryorguid`=40660;
