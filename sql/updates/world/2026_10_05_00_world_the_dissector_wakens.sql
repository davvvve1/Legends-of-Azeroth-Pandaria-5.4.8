-- The Dissector Wakens (31606): accepting the quest must start Rik'kal's
-- timed defense event. Without this binding, he never awards wake-up credit.
UPDATE `creature_template`
SET `ScriptName` = 'npc_rikkal_dissector_quest'
WHERE `entry` = 65253;
