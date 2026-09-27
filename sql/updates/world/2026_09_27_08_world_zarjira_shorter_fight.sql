-- An Ancient Enemy (24814): reduce Zar'jira's scripted NPC fight health by 90%.
-- Keep her level, damage, event phases and quest credit unchanged.
START TRANSACTION;
UPDATE `creature_template` SET `Health_mod`=10 WHERE `entry`=38306;
-- Let the spawn initialize at its newly calculated maximum health.
UPDATE `creature` SET `curhealth`=0 WHERE `id`=38306;
COMMIT;
