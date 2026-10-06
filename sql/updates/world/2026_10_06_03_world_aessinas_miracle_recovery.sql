-- Aessina's Miracle (25372): permit recovery at the Grove after abandoning
-- the delivery quest. Both overlapping Hamuul versions can now re-offer it.
DELETE FROM `creature_queststarter`
WHERE `quest` = 25372
  AND `id` IN (39858, 52838);

INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(39858, 25372),
(52838, 25372);
