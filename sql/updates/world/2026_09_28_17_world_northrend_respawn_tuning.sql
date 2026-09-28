-- Northrend outdoor respawn tuning for a low-population realm.
-- Map 571 is the Northrend continent; dungeon and raid maps are unaffected.
-- Cap ordinary/weak creatures at two minutes and ordinary elites at three.
-- Preserve rare-elites (rank 2), world bosses (rank 3), and rares (rank 4).

UPDATE `creature` AS `c`
INNER JOIN `creature_template` AS `ct` ON `ct`.`entry` = `c`.`id`
SET `c`.`spawntimesecs` = CASE
    WHEN `ct`.`rank` = 1 THEN 180
    ELSE 120
END
WHERE `c`.`map` = 571
  AND (
      (`ct`.`rank` IN (0, 5) AND `c`.`spawntimesecs` > 120)
      OR (`ct`.`rank` = 1 AND `c`.`spawntimesecs` > 180)
  );
