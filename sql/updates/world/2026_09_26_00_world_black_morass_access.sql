-- Allow Black Morass / Opening the Dark Portal without Return to Andormu.
-- Remove quest attunement for both factions on normal and heroic.
UPDATE `access_requirement`
SET `quest_done_A`=0, `quest_done_H`=0, `quest_failed_text`=''
WHERE `mapId`=269 AND `difficulty` IN ('DUNGEON_NORMAL','DUNGEON_HEROIC');
