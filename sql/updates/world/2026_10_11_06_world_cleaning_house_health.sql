-- Cleaning House (30078): keep the encounter bosses manageable while their
-- retail shield and helper mechanics are not fully scripted.
-- Level 87 base health is 214985 and ordinary hostile Pandaria creatures use
-- a 0.7 health factor, producing 500000 and 600000 maximum health respectively.
UPDATE `creature_template`
SET `Health_mod` = CASE `entry`
    WHEN 58014 THEN 3.322490938 -- Eddy: 500000 health
    WHEN 58017 THEN 3.986989125 -- Fizzy Yellow Alemental: 600000 health
END
WHERE `entry` IN (58014, 58017);
