-- Throne of the Tides: Neptulon's Cache is spawned by the instance script
-- after Ozumat is defeated.  Both database spawns incorrectly used mask 4
-- (10-player normal), so neither object was loaded in a five-player normal
-- or heroic instance and DoRespawnGameObject received an empty GUID.
UPDATE `gameobject`
SET `spawnMask` = 1
WHERE `map` = 643 AND `id` = 205216;

UPDATE `gameobject`
SET `spawnMask` = 2
WHERE `map` = 643 AND `id` = 207973;
