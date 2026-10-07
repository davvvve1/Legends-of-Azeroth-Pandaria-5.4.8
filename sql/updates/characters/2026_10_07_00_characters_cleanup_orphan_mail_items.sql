-- Remove stale attachment mappings left behind after both the parent mail and
-- the attached item instance have already been deleted.  These rows cannot be
-- loaded or recovered by the mail system and leave the character database in
-- an inconsistent state.

CREATE TABLE IF NOT EXISTS `_backup_mail_items_orphans_20261007`
LIKE `mail_items`;

INSERT IGNORE INTO `_backup_mail_items_orphans_20261007`
SELECT `mi`.*
FROM `mail_items` AS `mi`
LEFT JOIN `mail` AS `m`
  ON `m`.`id` = `mi`.`mail_id`
LEFT JOIN `item_instance` AS `ii`
  ON `ii`.`guid` = `mi`.`item_guid`
WHERE `m`.`id` IS NULL
  AND `ii`.`guid` IS NULL;

DELETE `mi`
FROM `mail_items` AS `mi`
LEFT JOIN `mail` AS `m`
  ON `m`.`id` = `mi`.`mail_id`
LEFT JOIN `item_instance` AS `ii`
  ON `ii`.`guid` = `mi`.`item_guid`
WHERE `m`.`id` IS NULL
  AND `ii`.`guid` IS NULL;
