-- Rolling with my Homies (14071)
--
-- The three permanent spawns below use the quest's passenger/kill-credit
-- entries.  They overlap the real friends (34890, 34892 and 34954), but have
-- neither Holy Zone Visual (70571) nor the SmartAI that awards pickup credit.
-- Keep the real friends and let spells 66597/66599/66600 summon the passenger
-- entries after each pickup.

CREATE TABLE IF NOT EXISTS `_backup_creature_rolling_with_my_homies_20261010`
LIKE `creature`;

INSERT IGNORE INTO `_backup_creature_rolling_with_my_homies_20261010`
SELECT *
FROM `creature`
WHERE (`guid` = 372270 AND `id` = 34959)
   OR (`guid` = 372271 AND `id` = 34958)
   OR (`guid` = 372272 AND `id` = 34957);

DELETE FROM `creature`
WHERE (`guid` = 372270 AND `id` = 34959)
   OR (`guid` = 372271 AND `id` = 34958)
   OR (`guid` = 372272 AND `id` = 34957);

-- Each friend is in a different sub-area and uses a different generic quest
-- invisibility type.  Apply the matching detection aura only while 14071 is
-- incomplete (quest status mask 8), as observed in the reference fix.
DELETE FROM `spell_area`
WHERE `spell` IN (49416, 49417, 60922)
  AND `quest_start` = 14071;

INSERT INTO `spell_area`
    (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`,
     `gender`, `autocast`, `quest_start_status`, `quest_end_status`)
VALUES
    (60922, 4765, 14071, 0, 0, 0, 2, 1, 8, 0), -- Izzy: Generic Quest Invisibility Detection 3
    (49417, 4737, 14071, 0, 0, 0, 2, 1, 8, 0), -- Gobber: Generic Quest Invisibility Detection 2
    (49416, 4767, 14071, 0, 0, 0, 2, 1, 8, 0); -- Ace: Generic Quest Invisibility Detection 1
