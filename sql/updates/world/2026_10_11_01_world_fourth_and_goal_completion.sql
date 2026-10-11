-- Fourth and Goal (24503/28414): the C++ vehicle script replaces the old
-- summon-time SmartAI, which otherwise conflicts with the vehicle setup and
-- awards objective credit before the player kicks the footbomb.
DELETE FROM `smart_scripts`
WHERE (`entryorguid` = 37213 AND `source_type` = 0)
   OR (`entryorguid` = 3721300 AND `source_type` = 9);
