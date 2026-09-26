-- Make all heirlooms free at their existing vendors.
START TRANSACTION;

-- Different currency offers for the same item collapse into one free offer.
-- Keep the lowest ExtendedCost (prefer an existing gold/free offer) to avoid
-- collisions with npc_vendor's (entry, item, ExtendedCost, type) primary key.
DELETE expensive
FROM `npc_vendor` AS expensive
JOIN `npc_vendor` AS cheaper
  ON cheaper.`entry` = expensive.`entry`
 AND cheaper.`item` = expensive.`item`
 AND cheaper.`type` = expensive.`type`
 AND cheaper.`ExtendedCost` < expensive.`ExtendedCost`
JOIN `item_template` AS item ON item.`entry` = expensive.`item`
WHERE item.`Quality` = 7 AND expensive.`type` = 1;

UPDATE `npc_vendor` AS vendor
JOIN `item_template` AS item ON item.`entry` = vendor.`item`
SET vendor.`ExtendedCost` = 0
WHERE item.`Quality` = 7 AND vendor.`type` = 1;

-- Free purchases must not generate gold through vendor resale.
UPDATE `item_template`
SET `BuyPrice` = 0, `SellPrice` = 0
WHERE `Quality` = 7;

COMMIT;
