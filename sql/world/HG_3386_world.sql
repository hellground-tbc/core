DELETE FROM `script_waypoint` WHERE `entry`=4962;
INSERT INTO `script_waypoint` VALUES
   (4962,0,-3804.438965,-828.048035,10.093068,0,''),
   (4962,1,-3803.934326,-835.772400,10.077722,0,''),
   (4962,2,-3792.629150,-835.670898, 9.655657,0,''),
   (4962,3,-3772.433838,-835.345947,10.868981,0,''),
   (4962,4,-3765.937256,-840.128601,10.885593,0,''),
   (4962,5,-3738.633789,-830.997498,11.057384,0,''),
   (4962,6,-3690.224121,-862.261597, 9.960449,0,'');

UPDATE `creature_template` SET `ScriptName` = 'npc_tapoke_slim_jahn' WHERE `entry` = '4962';
UPDATE `creature_template` SET `ScriptName` = 'npc_mikhail' WHERE `entry` = '4963';
DELETE FROM creature WHERE guid = '9555';
DELETE FROM creature_addon WHERE guid = '9555';
DELETE FROM game_event_creature WHERE guid = '9555';
DELETE FROM game_event_model_equip WHERE guid = '9555';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1249';
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = '0' WHERE `entry` = '1249';
UPDATE `quest_template` SET `ReqCreatureOrGOCount1` = '0' WHERE `entry` = '1249';