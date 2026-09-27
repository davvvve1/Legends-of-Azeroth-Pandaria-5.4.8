-- Server-side mount/riding minimum levels. No client files changed.
UPDATE item_template SET RequiredLevel = 1 WHERE class = 15 AND subclass = 5;
UPDATE npc_trainer SET reqlevel = 1
WHERE reqskill = 762 OR spell IN (33388,33389,33391,33392,34090,34091,90265,54197,90267,115913,130487);
