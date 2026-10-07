-- Razorthorn Ravager is the premier Knothide Leather farming creature:
-- skinning it always yields one to six leather instead of leather scraps.
DELETE FROM `skinning_loot_template`
WHERE `entry` = 24922
  AND `item` IN (21887, 25649);

INSERT INTO `skinning_loot_template`
    (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`)
VALUES
    (24922, 21887, 100, '', 0, 1, 6);
