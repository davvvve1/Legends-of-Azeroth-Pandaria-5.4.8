-- A Bird in Hand (25731): the signal fire depended on summon spell 77041.
-- When that summon failed, Marion never engaged and neither objective could
-- progress. Bind the deterministic C++ interaction instead.
UPDATE `gameobject_template`
SET `AIName` = '',
    `ScriptName` = 'go_harpy_signal_fire'
WHERE `entry` = 203187;

DELETE FROM `smart_scripts`
WHERE `entryorguid` = 203187
  AND `source_type` = 1;
