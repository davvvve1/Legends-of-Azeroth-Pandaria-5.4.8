-- Corporal Jackson Silver (65881), Strongarm Tactics (31776).
-- His spawn origin is embedded in the hut floor.  Start him above the local
-- collision surface so creature gravity settles his feet at ground level.
UPDATE `creature`
SET `position_z` = 350.000
WHERE `guid` = 501643 AND `id` = 65881
  AND `map` = 870 AND `zoneId` = 5785;
