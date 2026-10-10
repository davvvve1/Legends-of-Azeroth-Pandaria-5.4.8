-- The Source of Our Livelihood (29680)
-- The stationary cart was casting the summon spell on itself and the
-- server-side Cart Cap spell that boards the player was absent. Make the
-- player own the summoned cart/yak, execute the boarding script, and expose
-- the cart objective separately from Ji's turn-in location.

CREATE TABLE IF NOT EXISTS `_backup_npc_spellclick_source_livelihood_20261010`
LIKE `npc_spellclick_spells`;
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_source_livelihood_20261010`
LIKE `quest_poi`;
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_points_source_livelihood_20261010`
LIKE `quest_poi_points`;

INSERT IGNORE INTO `_backup_npc_spellclick_source_livelihood_20261010`
SELECT * FROM `npc_spellclick_spells` WHERE `npc_entry` = 57710;
INSERT IGNORE INTO `_backup_quest_poi_source_livelihood_20261010`
SELECT * FROM `quest_poi` WHERE `QuestID` = 29680;
INSERT IGNORE INTO `_backup_quest_poi_points_source_livelihood_20261010`
SELECT * FROM `quest_poi_points` WHERE `QuestID` = 29680;

START TRANSACTION;

INSERT INTO `npc_spellclick_spells`
(`npc_entry`,`spell_id`,`cast_flags`,`user_type`) VALUES
(57710,107784,3,0),
(57710,115904,1,0)
ON DUPLICATE KEY UPDATE
`cast_flags`=VALUES(`cast_flags`),`user_type`=VALUES(`user_type`);

INSERT INTO `quest_poi`
(`QuestID`,`Idx1`,`ObjectiveIndex`,`QuestObjectiveId`,`MapID`,`WorldMapAreaId`,`Floor`,`Priority`,`Flags`,`VerifiedBuild`) VALUES
(29680,1,0,276325,860,808,0,0,1,0)
ON DUPLICATE KEY UPDATE
`ObjectiveIndex`=VALUES(`ObjectiveIndex`),`QuestObjectiveId`=VALUES(`QuestObjectiveId`),
`MapID`=VALUES(`MapID`),`WorldMapAreaId`=VALUES(`WorldMapAreaId`),`Floor`=VALUES(`Floor`),
`Priority`=VALUES(`Priority`),`Flags`=VALUES(`Flags`);

INSERT INTO `quest_poi_points`
(`QuestID`,`BlobIndex`,`Idx1`,`Idx2`,`X`,`Y`,`VerifiedBuild`) VALUES
(29680,1,1,0,979,2864,0)
ON DUPLICATE KEY UPDATE `X`=VALUES(`X`),`Y`=VALUES(`Y`);

COMMIT;
