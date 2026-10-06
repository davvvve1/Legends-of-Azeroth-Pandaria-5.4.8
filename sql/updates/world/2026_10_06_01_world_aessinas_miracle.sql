-- Aessina's Miracle (25372): two Hamuul versions overlap at the Grove of
-- Aessina.  The visible later-phase version (52838) did not end the quest,
-- while the original version (39858) did not offer the follow-up.
DELETE FROM `creature_questender`
WHERE `quest` = 25372
  AND `id` IN (39858, 52838);

INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(39858, 25372),
(52838, 25372);

DELETE FROM `creature_queststarter`
WHERE `quest` = 25843
  AND `id` IN (39858, 52838);

INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(39858, 25843),
(52838, 25843);

UPDATE `creature_template`
SET `AIName` = '',
    `ScriptName` = 'npc_arch_druid_hamuul_aessinas_miracle'
WHERE `entry` IN (39858, 52838);
