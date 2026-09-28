-- Down Kitty! (29717): restore Jade Guardians to 300,000 health.
-- Level 85 expansion-4 base health is 158,079; ordinary hostile Pandaria
-- creatures receive the core's 0.7 outdoor-health multiplier.
UPDATE creature_template
SET Health_mod = 2.71112
WHERE entry = 55236;
