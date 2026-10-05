-- A Gap in Their Armor (25758)
-- The Twilight Armor Plate chests deactivated without delivering item 55809.
-- Handle the quest pickup atomically in the GameObject script.

UPDATE `gameobject_template`
SET `ScriptName`='go_twilight_armor_plate'
WHERE `entry` IN (203197,203198);
