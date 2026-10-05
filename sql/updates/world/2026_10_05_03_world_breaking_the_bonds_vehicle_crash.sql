-- Breaking the Bonds (25514): both rods already have their own result spell
-- (75615/75616) for quest progress. Keep their behavior consistent and avoid
-- starting the optional encounter summon from only one of the two rods.
DELETE FROM `smart_scripts`
WHERE `source_type` = 1
  AND `entryorguid` IN (202954, 202955)
  AND `action_type` = 85
  AND `action_param1` = 75625;
