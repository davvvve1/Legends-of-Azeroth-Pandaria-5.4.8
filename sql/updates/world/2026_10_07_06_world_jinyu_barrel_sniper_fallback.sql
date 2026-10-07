-- Keep a visible Sniper Shot button as a fallback for clients which do not
-- emit CMSG_SPELLCLICK while the player controls Shokia's rifle vehicle.
UPDATE `creature_template` SET `spell1`=104384 WHERE `entry`=55702;
