-- Darkwood Lurker should yield between one and seven Savage Leather when skinned.
UPDATE `skinning_loot_template`
SET `mincountOrRef` = 1, `maxcount` = 7
WHERE `Entry` = 46508 AND `Item` = 52976;
