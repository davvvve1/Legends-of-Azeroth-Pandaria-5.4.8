-- Patching Up (11894): guarantee an Uncured Caribou Hide for eligible players.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=-100
WHERE `entry` IN (25680,25750) AND `item`=35288;
