-- Vigilance on Wings (29177)
-- The Guardian launchers require Twilight Firelance Equipped (74180), but the
-- quest did not provide the lance and depended entirely on nearby weapon racks.
-- Give one as the quest source item and remove it again when the quest is turned in.

UPDATE `quest_template`
SET `StartItem`=52716
WHERE `Id`=29177;

UPDATE `quest_template_addon`
SET `ProvidedItemCount`=1
WHERE `ID`=29177;
