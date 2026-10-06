-- Regroup! (29694)
-- The original gossip actions cast tracking spells whose area restrictions can
-- silently reject the rescue credit.  Credit the four creature objectives
-- directly when their existing gossip options are selected.
UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55141,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Shademaster Kiryn - Regroup gossip - Give rescue credit directly'
WHERE `entryorguid` = 55141 AND `source_type` = 0 AND `id` = 0
  AND `event_type` = 62 AND `event_param1` = 13087
  AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55146,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Rivett Clutchpop - Regroup gossip - Give rescue credit directly'
WHERE `entryorguid` = 55146 AND `source_type` = 0 AND `id` = 0
  AND `event_type` = 62 AND `event_param1` = 13090
  AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55162,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Sergeant Gorrok - Regroup gossip - Give rescue credit directly'
WHERE `entryorguid` = 55162 AND `source_type` = 0 AND `id` = 0
  AND `event_type` = 62 AND `event_param1` = 13091
  AND `target_type` = 7;

UPDATE `smart_scripts`
SET `action_type` = 33, `action_param1` = 55170,
    `action_param2` = 0, `action_param3` = 0,
    `action_param4` = 0, `action_param5` = 0, `action_param6` = 0,
    `comment` = 'Shokia - Regroup gossip - Give rescue credit directly'
WHERE `entryorguid` = 55170 AND `source_type` = 0 AND `id` = 0
  AND `event_type` = 62 AND `event_param1` = 13098
  AND `target_type` = 7;

-- A Proper Weapon (29627)
-- All five Rattan Switch variants use the same quest-only loot template.  The
-- explicit chest quest id is also used by the core's per-player dynamic flags,
-- making the objects activate and sparkle while the quest is incomplete.
UPDATE `gameobject_template`
SET `questItem1` = 72926, `data1` = 40371, `data8` = 29627
WHERE `entry` IN (209460, 209461, 209462, 209463, 209464)
  AND `type` = 3;

INSERT INTO `gameobject_loot_template`
    (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`)
VALUES
    (40371, 72926, -100, 'REGULAR', 0, 1, 1)
ON DUPLICATE KEY UPDATE
    `ChanceOrQuestChance` = VALUES(`ChanceOrQuestChance`),
    `groupid` = VALUES(`groupid`),
    `mincountOrRef` = VALUES(`mincountOrRef`),
    `maxcount` = VALUES(`maxcount`);

-- Preserve unlimited stock for the two ingredients bought in the market.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 128
WHERE `entry` IN (54981, 54982);

INSERT INTO `npc_vendor`
    (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `type`)
VALUES
    (54981, 0, 72954, 0, 0, 0, 1),
    (54982, 0, 72979, 0, 0, 0, 1)
ON DUPLICATE KEY UPDATE
    `maxcount` = VALUES(`maxcount`),
    `incrtime` = VALUES(`incrtime`);
