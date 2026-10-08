-- A Courteous Guest (29619) requires eight Ripe Oranges, while the orchard
-- only had five spawn points. Add five nearby points to double availability.
DELETE FROM `gameobject` WHERE `guid` BETWEEN 4000151 AND 4000155;
INSERT INTO `gameobject`
(`guid`,`id`,`map`,`zoneId`,`areaId`,`spawnMask`,`phaseMask`,`phaseId`,`phaseGroup`,
 `position_x`,`position_y`,`position_z`,`orientation`,
 `rotation0`,`rotation1`,`rotation2`,`rotation3`,
 `spawntimesecs`,`animprogress`,`state`,`ScriptName`,`VerifiedBuild`)
VALUES
(4000151,209436,870,5785,5785,1,1,0,0,
 2372.75,-1748.34,375.057,5.82895,
 0,0,0.225170,-0.974319,
 300,255,1,'',0),
(4000152,209436,870,5785,5785,1,1,0,0,
 2387.77,-1753.58,374.447,3.38691,
 0,0,0.992487,-0.122350,
 300,255,1,'',0),
(4000153,209436,870,5785,5785,1,1,0,0,
 2334.57,-1722.75,324.768,1.22942,
 0,0,0.576721,0.816941,
 300,255,1,'',0),
(4000154,209436,870,5785,5785,1,1,0,0,
 2360.14,-1751.25,374.876,0.508422,
 0,0,0.251482,0.967862,
 300,255,1,'',0),
(4000155,209436,870,5785,5785,1,1,0,0,
 2331.06,-1706.39,324.549,1.97319,
 0,0,0.834153,0.551533,
 300,255,1,'',0);
