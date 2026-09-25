-- Eyes of Desire (29607) requires three Cobalt Eyes. Only six Cobalt
-- Serpents spawn in Sethekk Halls; the previous 8% normal-only quest drop
-- made a single clear unreliable and excluded heroic entirely.
-- Negative chance preserves quest-only loot; one eye per eligible kill.
UPDATE `creature_loot_template`
SET `ChanceOrQuestChance`=-100,
    `lootmode`='DUNGEON_NORMAL,DUNGEON_HEROIC'
WHERE `entry`=19428 AND `item`=72480
  AND `lootmode`='DUNGEON_NORMAL' AND `ChanceOrQuestChance`=-8
  AND `groupid`=0 AND `mincountOrRef`=1 AND `maxcount`=1;
