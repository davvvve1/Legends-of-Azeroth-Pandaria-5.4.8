-- Bind every 5.4.8 variant of Engulfing Flames to the deterministic
-- Grim Batol one-shot damage handler.
DELETE FROM `spell_script_names`
WHERE `spell_id` IN (74039, 74040, 74041)
  AND `ScriptName` = 'spell_grim_batol_engulfing_flames';

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(74039, 'spell_grim_batol_engulfing_flames'),
(74040, 'spell_grim_batol_engulfing_flames'),
(74041, 'spell_grim_batol_engulfing_flames');
