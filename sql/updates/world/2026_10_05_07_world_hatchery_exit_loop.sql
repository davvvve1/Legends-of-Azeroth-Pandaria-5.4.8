-- The Hatchery exit (5939) landed inside the entrance trigger and immediately
-- teleported the player back.  Move the exit destination about 20 yards
-- forward along its existing orientation and keep it safely above the ground.

UPDATE `areatrigger_teleport`
SET `target_position_x`=4535.47,
    `target_position_y`=-2575.96,
    `target_position_z`=1126.84
WHERE `id`=5939;
