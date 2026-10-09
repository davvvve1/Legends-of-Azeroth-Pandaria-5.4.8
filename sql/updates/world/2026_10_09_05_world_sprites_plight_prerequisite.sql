-- The Sprites' Plight (29745) is the live first quest of this side chain.
-- Some "Pupil of Nature" (29744) was a removed beta breadcrumb and has no
-- quest starter, so retaining it as a prerequisite makes 29745 unavailable.
UPDATE `quest_template_addon`
SET `PrevQuestID` = 0
WHERE `ID` = 29745
  AND `PrevQuestID` = 29744;
