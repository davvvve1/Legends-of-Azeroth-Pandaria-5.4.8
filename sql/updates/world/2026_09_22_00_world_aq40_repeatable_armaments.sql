-- Imperial Qiraji Armaments and Imperial Qiraji Regalia are repeatable turn-ins.
-- Missing addon rows made both quests permanently unavailable after one reward.
-- Preserve any other addon settings when this migration is reapplied.
INSERT INTO `quest_template_addon` (`ID`, `SpecialFlags`)
VALUES (8789, 1), (8790, 1)
ON DUPLICATE KEY UPDATE `SpecialFlags` = `SpecialFlags` | VALUES(`SpecialFlags`);
