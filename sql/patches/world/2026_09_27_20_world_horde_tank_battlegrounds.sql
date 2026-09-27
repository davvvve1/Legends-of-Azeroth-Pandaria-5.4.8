-- Allow Horde Siege Tank passengers in Borean Tundra OR any 5.4.8 battleground.
-- Map IDs are all MAP_BATTLEGROUND entries in this server's Map.dbc.
START TRANSACTION;
DELETE FROM conditions WHERE SourceTypeOrReferenceId=16 AND SourceGroup=0
 AND SourceEntry=25334 AND SourceId=0 AND Comment='Horde Siege Tank - Allow battleground map';
INSERT INTO conditions
(SourceTypeOrReferenceId,SourceGroup,SourceEntry,SourceId,ElseGroup,ConditionTypeOrReference,
 ConditionTarget,ConditionValue1,ConditionValue2,ConditionValue3,NegativeCondition,ErrorType,ErrorTextId,ScriptName,Comment)
VALUES
(16,0,25334,0,30,22,0,30,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,489,22,0,489,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,529,22,0,529,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,566,22,0,566,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,607,22,0,607,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,628,22,0,628,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,726,22,0,726,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,727,22,0,727,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,761,22,0,761,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,968,22,0,968,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,998,22,0,998,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,1010,22,0,1010,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map'),
(16,0,25334,0,1105,22,0,1105,0,0,0,0,0,'','Horde Siege Tank - Allow battleground map');
COMMIT;
