-- Ashyo's Vision (29577): restore Clever Ashyo's quest gossip and ritual at
-- the Pools of Purity. The same template also starts the quest in New Cifera,
-- so the script limits the ritual option to the objective spawn's area.

UPDATE `creature_template`
SET `npcflag` = `npcflag` | 1,
    `ScriptName` = 'npc_vfw_clever_ashyo'
WHERE `entry` = 56113;

DELETE FROM `npc_text` WHERE `ID` = 29577;
INSERT INTO `npc_text`
(`ID`,`Text0_0`,`Text0_1`,`BroadcastTextID0`,`lang0`,`Probability0`,`VerifiedBuild`) VALUES
(29577,'This seems like a good spot to consult the water!','',55965,0,1,18414);

DELETE FROM `gossip_menu` WHERE `MenuID` = 29577;
INSERT INTO `gossip_menu` (`MenuID`,`TextID`,`VerifiedBuild`) VALUES
(29577,29577,18414);

DELETE FROM `gossip_menu_option` WHERE `MenuID` = 29577;
INSERT INTO `gossip_menu_option`
(`MenuID`,`OptionID`,`OptionIcon`,`OptionText`,`OptionBroadcastTextID`,
 `OptionType`,`OptionNpcflag`,`ActionMenuID`,`ActionPoiID`,`BoxCoded`,
 `BoxMoney`,`BoxText`,`BoxBroadcastTextID`,`VerifiedBuild`) VALUES
(29577,0,0,'Go ahead and speak with the water, Ashyo.',55764,
 1,1,0,0,0,0,NULL,0,18414);

DELETE FROM `creature_text`
WHERE `CreatureID` = 56113 AND `GroupID` IN (0,1);
INSERT INTO `creature_text`
(`CreatureID`,`GroupID`,`ID`,`Text`,`Type`,`Language`,`Probability`,`Emote`,
 `Duration`,`Sound`,`SoundType`,`BroadcastTextId`,`TextRange`,`comment`) VALUES
(56113,0,0,'I\'ve only done this once before... Hopefully this works.',12,0,100,0,
 0,0,0,56031,0,'Clever Ashyo - Ashyo\'s Vision begins'),
(56113,1,0,'What... what is this place? It is beautiful... But... something is not right.',12,0,100,0,
 0,0,0,56037,0,'Clever Ashyo - Ashyo\'s Vision seen');
