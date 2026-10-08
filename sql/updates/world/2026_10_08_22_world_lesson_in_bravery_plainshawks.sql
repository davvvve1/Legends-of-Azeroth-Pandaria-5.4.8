-- A Lesson in Bravery (29918): the Great White Plainshawks were circling at
-- the edge of, or beyond, Rancher's Lariat range from the quest area.  Keep
-- them airborne but low enough to see and lasso from the ground.
UPDATE `creature`
SET `position_z` = CASE `guid`
        WHEN 512817 THEN 286.557
        WHEN 512935 THEN 286.788
        WHEN 513098 THEN 286.012
        WHEN 516816 THEN 286.912
    END,
    `wander_distance` = 5
WHERE `map` = 870
  AND `id` = 56171
  AND `guid` IN (512817, 512935, 513098, 516816);
