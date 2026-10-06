-- Elemental Bonds: Doubt (29327)
-- Essence of Doubt must be hostile and attackable so its 20 kills can satisfy
-- objective 257042. Preserve unrelated movement/state flags.
UPDATE `creature_template`
SET `faction` = 14,
    `unit_flags` = `unit_flags` & ~(2 | 128 | 256 | 65536),
    `dynamicflags` = `dynamicflags` & ~8
WHERE `entry` = 53516;

-- Aggra stands inside the hostile Essence pack. Protect the quest giver from
-- both players and NPC combat, matching Thrall beside her, while preserving
-- gossip and quest interaction.
UPDATE `creature_template`
SET `unit_flags` = `unit_flags` | 256 | 512
WHERE `entry` = 53519;
