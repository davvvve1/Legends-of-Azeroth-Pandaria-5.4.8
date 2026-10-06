-- Choppertunity (31777) can still be active after The Darkness Within (31779)
-- has been rewarded. Keep the Alliance airstrip/gyrocopter phase visible while
-- Choppertunity is active, then restore the Horde takeover phase afterwards.
START TRANSACTION;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` = 25
  AND `SourceGroup` = 5785
  AND `SourceEntry` IN (7, 8);

INSERT INTO `conditions`
(`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,
 `ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,
 `ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
-- Normal pre-takeover state: The Darkness Within is neither complete nor rewarded.
(25,5785,7,0,0,8,0,31779,0,0,1,0,0,'','Alliance base - The Darkness Within not rewarded'),
(25,5785,7,0,0,28,0,31779,0,0,1,0,0,'','Alliance base - The Darkness Within not complete'),
-- Recovery state: an active Choppertunity always exposes its required targets.
(25,5785,7,0,1,9,0,31777,0,0,0,0,0,'','Alliance base - Choppertunity active'),
-- The Horde takeover is visible only after The Darkness Within and while
-- Choppertunity is not active. Separate ElseGroups preserve rewarded OR complete.
(25,5785,8,0,0,8,0,31779,0,0,0,0,0,'','Horde base - The Darkness Within rewarded'),
(25,5785,8,0,0,9,0,31777,0,0,1,0,0,'','Horde base - Choppertunity not active'),
(25,5785,8,0,1,28,0,31779,0,0,0,0,0,'','Horde base - The Darkness Within complete'),
(25,5785,8,0,1,9,0,31777,0,0,1,0,0,'','Horde base - Choppertunity not active');

COMMIT;
