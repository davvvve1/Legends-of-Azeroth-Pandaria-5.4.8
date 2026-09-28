-- A Celestial Experience: make Sha Corruption an avoidable 20,000-damage ground pool.
DELETE FROM `spell_script_names`
WHERE `spell_id` = 126625
  AND `ScriptName` = 'spell_celestial_experience_sha_corruption';

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`)
VALUES (126625, 'spell_celestial_experience_sha_corruption');
