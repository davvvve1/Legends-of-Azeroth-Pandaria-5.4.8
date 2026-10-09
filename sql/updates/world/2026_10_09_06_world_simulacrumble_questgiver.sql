-- Simulacrumble (29748) is offered by the Shattered Destroyer after The
-- Sprites' Plight. Break the Cycle (29747) is auto-accepted and both quests
-- are turned in through the quest UI, so neither belongs in this object's
-- involved-quest list. Keeping those extra relations made the object present
-- a conflicting multi-quest menu instead of opening Simulacrumble directly.
DELETE FROM `gameobject_queststarter`
WHERE `id` = 214871
  AND `quest` = 29747;

DELETE FROM `gameobject_questender`
WHERE `id` = 214871
  AND `quest` IN (29747, 29748);

DELETE FROM `gameobject_queststarter`
WHERE `id` = 214871
  AND `quest` = 29748;

INSERT INTO `gameobject_queststarter` (`id`, `quest`)
VALUES (214871, 29748);

-- Gossip menu 16631 is absent; this questgiver needs only its quest menu.
UPDATE `gameobject_template`
SET `Data1` = 0
WHERE `entry` = 214871
  AND `type` = 2
  AND `Data1` = 16631;
