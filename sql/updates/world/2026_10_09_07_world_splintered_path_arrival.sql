-- The Splintered Path (29586): Shao is the destination at Camp Gormal.
-- Grant the existing Mogu Camp Discovered credit when the player reaches him,
-- even if the separate ambush area-trigger scene was suppressed or interrupted.
UPDATE `creature_template`
SET `ScriptName` = 'npc_shao_the_defiant'
WHERE `entry` = 55009;
