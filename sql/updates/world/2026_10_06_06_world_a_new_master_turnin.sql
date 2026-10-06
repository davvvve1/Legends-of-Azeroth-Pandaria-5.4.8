-- A New Master (25411): the retail turn-in NPC is the temporarily summoned
-- Subjugated Inferno Lord (40093). It despawns after 120 seconds, leaving a
-- completed quest with no available turn-in. Tyrus Blackhorn is the stable
-- recovery NPC and already starts this chain and ends its next quest.
INSERT IGNORE INTO `creature_questender` (`id`, `quest`)
VALUES (39933, 25411);

-- Preserve the normal continuation when 25411 is recovered at Tyrus instead
-- of at the temporary Inferno Lord.
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`)
VALUES (39933, 25412);
