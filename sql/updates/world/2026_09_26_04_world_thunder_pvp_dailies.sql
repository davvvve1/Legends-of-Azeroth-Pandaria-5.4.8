-- Captive Audience / Tactical Mana Bombs, both factions.
-- Requires matching go_thunder_daily_objective script and a worldserver restart.
-- Uses existing objects/captives. No player data or spawns are removed.
UPDATE `gameobject_template`
SET `ScriptName`='go_thunder_daily_objective'
WHERE `entry` IN (216315,216987,218731,218732,218733,218734,218735,218736)
  AND `ScriptName` IN ('','go_thunder_daily_objective') AND `AIName`='';
-- Gate interaction/sparkles to the correct faction's quest, using goober.questId.
UPDATE `gameobject_template` SET `data1`=32268 WHERE `entry` IN (218731,218732,218733);
UPDATE `gameobject_template` SET `data1`=32628 WHERE `entry` IN (218734,218735,218736);
-- Explicitly authorized exception: remove only these four obsolete NYI blockers.
DELETE FROM `disables` WHERE `sourceType`=1 AND `entry` IN (32262,32636,32268,32628) AND `comment`='NYI';
