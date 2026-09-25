-- A Radical Notion (29686): one quest-only Volatile Essence from Vexallus.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=-100,
    `lootmode`='DUNGEON_NORMAL,DUNGEON_HEROIC'
WHERE `entry`=24744 AND `item`=73084
  AND `lootmode`='DUNGEON_NORMAL' AND `ChanceOrQuestChance`=-5
  AND `groupid`=0 AND `mincountOrRef`=1 AND `maxcount`=1;
