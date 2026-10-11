-- Chen Stormstout's brewery scenarios require these creatures to be hostile.
-- They were using friendly faction 35, which made the mandatory encounter
-- targets green and prevented players from attacking them.
UPDATE `creature_template`
SET `faction` = 14
WHERE `entry` IN (
    56684, -- Unruly Alemental (Broken Dreams)
    56691, -- Wuk-Wuk (Broken Dreams)
    58014, -- Eddy (Cleaning House)
    58017  -- Fizzy Yellow Alemental (Cleaning House)
);
