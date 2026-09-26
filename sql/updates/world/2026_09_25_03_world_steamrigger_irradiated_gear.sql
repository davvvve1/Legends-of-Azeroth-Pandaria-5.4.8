-- Containment is Key (29614): one Irradiated Gear from Mekgineer Steamrigger.
-- Negative chance keeps the item quest-only; support both dungeon difficulties.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=-100,
    `lootmode`='DUNGEON_NORMAL,DUNGEON_HEROIC'
WHERE `entry`=17796 AND `item`=72574
  AND `lootmode`='DUNGEON_NORMAL' AND `ChanceOrQuestChance`=-7
  AND `groupid`=0 AND `mincountOrRef`=1 AND `maxcount`=1;
