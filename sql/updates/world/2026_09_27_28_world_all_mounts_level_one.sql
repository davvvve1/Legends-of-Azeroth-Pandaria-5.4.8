-- All mount items can be learned from level 1, including flying mounts.
-- The matching AutomaticRiding script grants all riding and flight licenses at level 1.
-- Preserve faction/reputation and special zone restrictions.
UPDATE item_template SET RequiredLevel = 1
WHERE class = 15 AND subclass = 5 AND RequiredLevel > 1;
