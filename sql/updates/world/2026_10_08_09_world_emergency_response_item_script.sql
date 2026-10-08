-- Emergency Response (30504 Horde / 31319 Alliance)
-- Run rescue credit directly from Cho's Fireworks item-use hook. The spells
-- are self-targeted script effects and do not reliably invoke SpellScript on
-- this core.
DELETE FROM `item_script_names`
WHERE `Id` IN (86467,86511);

INSERT INTO `item_script_names` (`Id`,`ScriptName`) VALUES
(86467,'item_jade_forest_emergency_response_fireworks'),
(86511,'item_jade_forest_emergency_response_fireworks');
