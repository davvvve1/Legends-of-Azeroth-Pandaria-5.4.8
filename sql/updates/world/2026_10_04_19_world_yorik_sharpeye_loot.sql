-- Yorik Sharpeye (50336) must always drop exactly one of his five ilvl 440
-- cloaks. They were incorrectly configured as independent rolls, allowing an
-- entirely empty corpse (or several cloaks from one kill).
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 20,
    `groupid` = 1
WHERE `entry` = 50336
  AND `item` IN (87636, 87637, 87638, 87639, 87640);

-- Pandaria champion material bag; independent of the guaranteed cloak group.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 40,
    `groupid` = 0
WHERE `entry` = 50336
  AND `item` = 87217;
