-- Emergency Response (30504/31319): Cho's Fireworks are self-targeted
-- script-effect spells. Credit the nearest wounded companion when used nearby.
DELETE FROM `spell_script_names`
WHERE `spell_id` IN (125700,125923)
  AND `ScriptName`='spell_jade_forest_emergency_response_fireworks';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(125700,'spell_jade_forest_emergency_response_fireworks'),
(125923,'spell_jade_forest_emergency_response_fireworks');
