-- Hop Hunting (30053) follows Chen's Resolution (30046).  The quest uses the
-- client-side auto-complete flow, and its normal accept packet is unreliable.
-- Auto-accept it server-side when the player selects it from Chen's quest list.
UPDATE `quest_template_addon`
SET `PrevQuestID` = 30046,
    `SpecialFlags` = `SpecialFlags` | 4
WHERE `ID` = 30053
  AND `PrevQuestID` = 0
  AND (`SpecialFlags` & 4) = 0;
