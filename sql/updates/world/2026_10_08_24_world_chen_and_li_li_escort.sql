-- Chen and Li Li (29907): the summon spell creates Chen (56343), but the
-- summoned creature had no AI to create Li Li or begin the escort.
UPDATE `creature_template`
SET `AIName` = '', `ScriptName` = 'npc_chen_and_li_li_escort'
WHERE `entry` = 56343;
