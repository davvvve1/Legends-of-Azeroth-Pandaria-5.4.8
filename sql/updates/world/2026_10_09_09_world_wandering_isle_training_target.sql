-- The Lesson of the Sandy Fist (29406): these are passive training dummies.
-- Their existing ScriptedAI already disables retaliation, but the template was
-- not bound to it and retained lethal level-one melee damage.
UPDATE `creature_template`
SET `mindmg` = 0,
    `maxdmg` = 0,
    `minrangedmg` = 0,
    `maxrangedmg` = 0,
    `attackpower` = 0,
    `rangedattackpower` = 0,
    `ScriptName` = 'npc_training_target'
WHERE `entry` = 53714;
