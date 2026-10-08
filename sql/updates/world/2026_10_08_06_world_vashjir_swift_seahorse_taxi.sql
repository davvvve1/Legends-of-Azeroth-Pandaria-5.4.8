-- Vashj'ir Swift Seahorse taxi corrections.
--
-- Legion's Rest flight master was 21 yards from TaxiNodes.dbc node 525. The
-- core validates taxi starts within twice the interaction distance, causing
-- every selected route to fail with ERR_TAXI_TOO_FAR_AWAY.
--
-- The Stygian Bounty Swift Seahorse is unlocked after A Breath of Fresh Air;
-- unlike the corresponding ship NPCs, its Horde spawn had no visibility rule
-- and therefore appeared at the surface throughout the earlier quest chain.
START TRANSACTION;

UPDATE `creature`
SET `position_x` = -6805.63,
    `position_y` =  4199.85,
    `position_z` =  -480.95
WHERE `guid` = 206290
  AND `id` = 40871;

DELETE FROM `object_visibility_state`
WHERE `type` = 'Creature'
  AND `entryorguid` = -208921;

INSERT INTO `object_visibility_state`
(`type`,`entryorguid`,`visibilityQuestID`,`visibilityQuestState`)
VALUES
('Creature',-208921,26006,6);

COMMIT;
