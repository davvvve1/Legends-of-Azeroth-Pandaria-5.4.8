-- Ordinary ground mount items can be learned from level 1.
-- AutomaticRiding.cpp grants ground riding on login and flying at the appropriate levels.
-- Preserve flying mount, reputation, faction and special mount requirements.
UPDATE item_template
SET RequiredLevel = 1
WHERE class = 15 AND subclass = 5 AND RequiredSpell IN (33388, 33391)
  AND RequiredLevel > 1;
