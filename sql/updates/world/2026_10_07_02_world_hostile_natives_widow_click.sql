-- Scouting Report: Hostile Natives (29730): the public Widow Greenpaw (55368)
-- stands only a few yards from the private report Widow (55381).  Route both
-- through the report gossip handler; it falls back to Widow's existing
-- SmartAI whenever the player is not actively controlling Sergeant Gorrok.
UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `ScriptName` = 'npc_jade_forest_report_investigation'
WHERE `entry` = 55368;
