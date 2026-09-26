-- Local progression: enforce level 78 at the entrances to Wrath normal
-- dungeons. Preserve heroic requirements, quests and item requirements.
UPDATE `access_requirement` SET `level_min` = GREATEST(`level_min`, 78)
WHERE `difficulty` = 'DUNGEON_NORMAL'
  AND `mapId` IN (574, 575, 576, 578, 595, 599, 600, 601, 602,
                  604, 608, 619, 632, 650, 658, 668);
