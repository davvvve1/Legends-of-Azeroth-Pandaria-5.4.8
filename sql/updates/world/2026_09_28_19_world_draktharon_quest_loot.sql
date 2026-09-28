-- Drak'Tharon Keep: make the instance's direct quest drops guaranteed.
--
-- King Dred's Tooth is required by What the Scourge Dred (29828).
-- Enduring Mojo is the reagent for Cleansing Drak'Tharon (12238/30120).
-- Scourge Curio is collected for Shipment: Shifting Sun Curio (12963).

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100
WHERE `entry` = 27483
  AND `item` = 74755
  AND `lootmode` IN ('DUNGEON_NORMAL', 'DUNGEON_HEROIC');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 1
  AND `SourceGroup` = 27483
  AND `SourceEntry` = 74755
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` IN (47, 48);

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
 `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
 `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
 `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(1, 27483, 74755, 0, 0, 47, 0, 29828, 8, 0, 0, 0, 0, '',
 'King Dred - King Dred''s Tooth requires What the Scourge Dred in progress'),
(1, 27483, 74755, 0, 0, 48, 0, 263768, 0, 0, 1, 0, 0, '',
 'King Dred - King Dred''s Tooth objective must be incomplete');

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100
WHERE `entry` IN (26620, 26639)
  AND `item` = 38303
  AND `lootmode` IN ('DUNGEON_NORMAL', 'DUNGEON_HEROIC');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 1
  AND `SourceGroup` IN (26620, 26639)
  AND `SourceEntry` = 38303
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` = 9;

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
 `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
 `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
 `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(1, 26620, 38303, 0, 0, 9, 0, 12238, 0, 0, 0, 0, 0, '',
 'Drakkari Guardian - Enduring Mojo requires legacy Cleansing Drak''Tharon'),
(1, 26620, 38303, 0, 1, 9, 0, 30120, 0, 0, 0, 0, 0, '',
 'Drakkari Guardian - Enduring Mojo requires MoP Cleansing Drak''Tharon'),
(1, 26639, 38303, 0, 0, 9, 0, 12238, 0, 0, 0, 0, 0, '',
 'Drakkari Shaman - Enduring Mojo requires legacy Cleansing Drak''Tharon'),
(1, 26639, 38303, 0, 1, 9, 0, 30120, 0, 0, 0, 0, 0, '',
 'Drakkari Shaman - Enduring Mojo requires MoP Cleansing Drak''Tharon');

UPDATE `creature_loot_template`
SET `ChanceOrQuestChance` = 100
WHERE `entry` IN (26621, 26623, 26624, 26635, 26636, 26637, 26638, 26830, 27871)
  AND `item` = 42108
  AND `lootmode` = 'DUNGEON_NORMAL';

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 1
  AND `SourceGroup` IN (26621, 26623, 26624, 26635, 26636, 26637, 26638, 26830, 27871)
  AND `SourceEntry` = 42108
  AND `SourceId` = 0
  AND `ConditionTypeOrReference` = 47;

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`,
 `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`,
 `ConditionValue1`, `ConditionValue2`, `ConditionValue3`,
 `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(1, 26621, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Ghoul Tormentor - Scourge Curio requires quest in progress'),
(1, 26623, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Scourge Brute - Scourge Curio requires quest in progress'),
(1, 26624, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Wretched Belcher - Scourge Curio requires quest in progress'),
(1, 26635, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Risen Drakkari Warrior - Scourge Curio requires quest in progress'),
(1, 26636, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Risen Drakkari Soulmage - Scourge Curio requires quest in progress'),
(1, 26637, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Risen Drakkari Handler - Scourge Curio requires quest in progress'),
(1, 26638, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Risen Drakkari Bat Rider - Scourge Curio requires quest in progress'),
(1, 26830, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Risen Drakkari Death Knight - Scourge Curio requires quest in progress'),
(1, 27871, 42108, 0, 0, 47, 0, 12963, 8, 0, 0, 0, 0, '', 'Flesheating Ghoul - Scourge Curio requires quest in progress');
