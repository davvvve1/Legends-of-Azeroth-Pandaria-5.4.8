-- The Great Bank Heist (14122): recover Personal Riches when the vault's
-- delayed SmartAI item action is interrupted after completion credit.
UPDATE `quest_template_addon`
SET `ScriptName` = 'quest_kezan_great_bank_heist'
WHERE `ID` = 14122;
