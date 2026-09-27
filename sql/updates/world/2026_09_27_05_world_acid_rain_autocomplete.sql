-- Acid Rain: bypass the unreliable helicopter event with immediate completion.
-- Preserve prerequisites, quest giver, rewards and the following quest.
START TRANSACTION;
UPDATE `quest_template`
SET `Flags` = `Flags` | 65536
WHERE `ID` = 29827;
DELETE FROM `quest_objective` WHERE `questId` = 29827;
COMMIT;
