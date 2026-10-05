-- Wave One (25525): Twilight Knight Rider (39835)
-- The old LOS event awarded credit to the colliding Guardian vehicle rather
-- than its player passenger.  Aviana's Guardian now owns collision credit and
-- cleanup for the Rider/Buzzard pair, just as it does for Twilight Lancers.

UPDATE `creature_template`
SET `AIName`=''
WHERE `entry`=39835 AND `AIName`='SmartAI';

DELETE FROM `smart_scripts`
WHERE `source_type`=0 AND `entryorguid`=39835;
