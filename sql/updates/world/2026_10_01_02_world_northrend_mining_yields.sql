-- Northrend mining nodes are consumed after one loot in this ruleset, but
-- their ore rows still use the old per-tap quantities.  Restore useful MoP
-- node totals while keeping crystallized elements and gems as independent
-- secondary rolls.

-- Cobalt Deposit: 2-9 Cobalt Ore.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 2,
    `maxcount` = 9
WHERE `entry` = 189978
  AND `item` = 36909;

-- Rich Cobalt Deposit: 5-12 Cobalt Ore.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 5,
    `maxcount` = 12
WHERE `entry` = 189979
  AND `item` = 36909;

-- Saronite Deposit: 5-7 Saronite Ore.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 5,
    `maxcount` = 7
WHERE `entry` = 189980
  AND `item` = 36912;

-- Rich Saronite Deposit: 8-12 Saronite Ore.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 8,
    `maxcount` = 12
WHERE `entry` = 189981
  AND `item` = 36912;

-- Titanium Vein: 4-6 Titanium Ore.
UPDATE `gameobject_loot_template`
SET `ChanceOrQuestChance` = 100,
    `mincountOrRef` = 4,
    `maxcount` = 6
WHERE `entry` = 191133
  AND `item` = 36910;
