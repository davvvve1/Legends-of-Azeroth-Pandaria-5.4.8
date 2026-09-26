-- Supersedes the previous level-78 entrance policy. Restore the original
-- normal entrance limits, with a level-68 floor for early Wrath dungeons.
-- Dungeon Finder retains each specific dungeon's native level bracket.
UPDATE `access_requirement`
SET `level_min` = CASE `mapId`
    WHEN 574 THEN 68 WHEN 575 THEN 77 WHEN 576 THEN 68
    WHEN 578 THEN 75 WHEN 595 THEN 75 WHEN 599 THEN 74
    WHEN 600 THEN 71 WHEN 601 THEN 69 WHEN 602 THEN 75
    WHEN 604 THEN 73 WHEN 608 THEN 72 WHEN 619 THEN 70
    WHEN 632 THEN 75 WHEN 650 THEN 75 WHEN 658 THEN 75
    WHEN 668 THEN 75 END
WHERE `difficulty` = 'DUNGEON_NORMAL' AND `level_min` = 78
  AND `mapId` IN (574, 575, 576, 578, 595, 599, 600, 601, 602,
                  604, 608, 619, 632, 650, 658, 668);
