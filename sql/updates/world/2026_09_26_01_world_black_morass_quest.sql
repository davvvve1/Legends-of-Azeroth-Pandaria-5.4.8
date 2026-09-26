-- Allow Sa'at to offer his active dungeon quest without the old attunement chain.
-- Preserve existing quest progress and follow-up quests; deprecated 29672 is not revived.
UPDATE `quest_template_addon` SET `PrevQuestID`=0
WHERE `ID`=10297 AND `PrevQuestID`=10296;
-- The loader also builds prerequisite lists from the preceding quest's NextQuestID.
UPDATE `quest_template_addon` SET `NextQuestID`=0
WHERE `ID`=10296 AND `NextQuestID`=10297;
