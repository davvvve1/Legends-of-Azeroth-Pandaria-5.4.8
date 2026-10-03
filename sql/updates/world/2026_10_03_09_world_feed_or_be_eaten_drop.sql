-- Feed or Be Eaten (31092): make Succulent Turtle Filet (86489) a guaranteed
-- quest drop from Brineshell Snapper (63981).

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=-100
WHERE `entry`=63981 AND `item`=86489;
