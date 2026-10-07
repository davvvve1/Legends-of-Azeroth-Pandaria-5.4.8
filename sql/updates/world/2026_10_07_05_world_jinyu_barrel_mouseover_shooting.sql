-- Like Jinyu in a Barrel uses the retail mouseover/right-click sniper control.
-- The actor marks only its private scene targets as spell-clickable at runtime.
UPDATE `creature_template`
SET `AIName`='', `ScriptName`='npc_jade_forest_report_attacker'
WHERE `entry` IN (55709,55710,55711,55784);

-- Retail presents no vehicle action button during the rifle sequence.
UPDATE `creature_template` SET `spell1`=0 WHERE `entry`=55702;
