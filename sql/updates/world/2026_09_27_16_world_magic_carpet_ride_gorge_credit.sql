-- Magic Carpet Ride (11636): report arrival by speaking to Gorge.
-- SpecialFlags=2 requires an exploration/event completion despite no objectives.
START TRANSACTION;
UPDATE creature_template SET npcflag = npcflag | 3, AIName = 'SmartAI'
WHERE entry = 25329 AND ScriptName = '';
INSERT INTO smart_scripts
(entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,
 event_param1,event_param2,event_param3,event_param4,event_param5,
 action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,
 target_type,target_param1,target_param2,target_param3,target_param4,target_x,target_y,target_z,target_o,comment)
VALUES
(25329,0,0,0,64,0,100,0,0,0,0,0,0,15,11636,0,0,0,0,0,7,0,0,0,0,0,0,0,0,
 'Gorge - Gossip hello - Magic Carpet Ride arrival event')
ON DUPLICATE KEY UPDATE event_type=64,action_type=15,action_param1=11636,target_type=7;
DELETE FROM conditions WHERE SourceTypeOrReferenceId=22 AND SourceGroup=1 AND SourceEntry=25329 AND SourceId=0;
INSERT INTO conditions
(SourceTypeOrReferenceId,SourceGroup,SourceEntry,SourceId,ElseGroup,ConditionTypeOrReference,
 ConditionTarget,ConditionValue1,ConditionValue2,ConditionValue3,NegativeCondition,ErrorType,ErrorTextId,ScriptName,Comment)
VALUES (22,1,25329,0,0,9,0,11636,0,0,0,0,0,'','Gorge arrival event only while Magic Carpet Ride is active');
COMMIT;
