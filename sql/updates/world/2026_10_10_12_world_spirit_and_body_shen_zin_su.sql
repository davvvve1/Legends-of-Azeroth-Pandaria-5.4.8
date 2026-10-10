-- The Spirit and Body of Shen-zin Su (29775)
-- Let the player own the summoned Dai-Lo cart/yak, run Cart Cap so the
-- player boards the vehicle, and add an objective POI at the stationary cart.

CREATE TABLE IF NOT EXISTS `_backup_npc_spellclick_spirit_body_20261010`
LIKE `npc_spellclick_spells`;
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_spirit_body_20261010`
LIKE `quest_poi`;
CREATE TABLE IF NOT EXISTS `_backup_quest_poi_points_spirit_body_20261010`
LIKE `quest_poi_points`;

INSERT IGNORE INTO `_backup_npc_spellclick_spirit_body_20261010`
SELECT * FROM `npc_spellclick_spells` WHERE `npc_entry` = 59497;
INSERT IGNORE INTO `_backup_quest_poi_spirit_body_20261010`
SELECT * FROM `quest_poi` WHERE `QuestID` = 29775;
INSERT IGNORE INTO `_backup_quest_poi_points_spirit_body_20261010`
SELECT * FROM `quest_poi_points` WHERE `QuestID` = 29775;

START TRANSACTION;

INSERT INTO `npc_spellclick_spells`
(`npc_entry`,`spell_id`,`cast_flags`,`user_type`) VALUES
(59497,114453,3,0),
(59497,115904,1,0)
ON DUPLICATE KEY UPDATE
`cast_flags`=VALUES(`cast_flags`),`user_type`=VALUES(`user_type`);

INSERT INTO `quest_poi`
(`QuestID`,`Idx1`,`ObjectiveIndex`,`QuestObjectiveId`,`MapID`,`WorldMapAreaId`,`Floor`,`Priority`,`Flags`,`VerifiedBuild`) VALUES
(29775,1,0,276326,860,808,0,0,1,0)
ON DUPLICATE KEY UPDATE
`ObjectiveIndex`=VALUES(`ObjectiveIndex`),`QuestObjectiveId`=VALUES(`QuestObjectiveId`),
`MapID`=VALUES(`MapID`),`WorldMapAreaId`=VALUES(`WorldMapAreaId`),`Floor`=VALUES(`Floor`),
`Priority`=VALUES(`Priority`),`Flags`=VALUES(`Flags`);

INSERT INTO `quest_poi_points`
(`QuestID`,`BlobIndex`,`Idx1`,`Idx2`,`X`,`Y`,`VerifiedBuild`) VALUES
(29775,1,1,0,589,3166,0)
ON DUPLICATE KEY UPDATE `X`=VALUES(`X`),`Y`=VALUES(`Y`);

COMMIT;
