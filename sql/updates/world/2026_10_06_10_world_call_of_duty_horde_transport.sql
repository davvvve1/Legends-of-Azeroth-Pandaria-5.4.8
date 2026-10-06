-- Call of Duty (25924): bind the Horde Vashj'ir ship to its arrival handler.
UPDATE `gameobject_template`
SET `ScriptName` = 'transport_call_of_duty_horde'
WHERE `entry` = 203466;
