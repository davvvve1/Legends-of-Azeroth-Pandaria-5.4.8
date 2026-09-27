-- Into the Mist (11655): guarantee quest item 34814 from all existing sources.
-- Negative chance retains quest-only eligibility; counts and other loot stay intact.
UPDATE creature_loot_template SET ChanceOrQuestChance = -100
WHERE item = 34814 AND entry IN (25479,25496,32576);
