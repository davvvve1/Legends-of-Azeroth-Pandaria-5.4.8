-- Unlock Xaril's daily after turning in The Poisoned Mind.
-- Remove the erroneous upper reputation limit, which blocks Revered players.
-- Preserve pool 608 and its rotation with Rampage Against the Machine.
UPDATE `quest_template_addon`
SET `PrevQuestID`=31211, `RequiredMaxRepFaction`=0, `RequiredMaxRepValue`=0
WHERE `ID`=31216;
