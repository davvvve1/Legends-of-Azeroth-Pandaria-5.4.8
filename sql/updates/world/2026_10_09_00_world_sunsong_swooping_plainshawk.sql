-- Swooping Plainshawk (60072) is a personal crop-harvest encounter summoned
-- by spell 115594.  The static ranch spawn attacked players without a harvest.
DELETE FROM `creature`
WHERE `guid` = 516120
  AND `id` = 60072;
