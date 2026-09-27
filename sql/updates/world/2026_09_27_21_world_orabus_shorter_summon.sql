-- Horn of the Ancient Mariner (45703) sends event 16889.
-- Preserve four opening waves, but reduce the 247-second boss wait to 30 seconds.
UPDATE event_scripts
SET delay = CASE delay WHEN 7 THEN 2 WHEN 67 THEN 8 WHEN 127 THEN 14
    WHEN 187 THEN 20 WHEN 247 THEN 30 ELSE delay END
WHERE id = 16889 AND command = 10
 AND ((datalong = 32577 AND delay IN (7,67,127,187))
   OR (datalong = 32576 AND delay = 247));
