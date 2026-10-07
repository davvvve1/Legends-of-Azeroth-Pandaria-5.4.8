-- Boom Bait (29939): Rivett has the correct questender relation, but his
-- multi-service gossip can fail to present the reward page for a completed
-- quest.  The script handles only the completed Boom Bait state and lets the
-- normal quest/vendor/repair gossip process every other interaction.
UPDATE `creature_template`
SET `ScriptName` = 'npc_jade_forest_rivett_boom_bait'
WHERE `entry` = 56406;
