

UPDATE gameobject SET orientation = 3.4581, rotation2 = 0.9875, rotation3 = -0.1576 WHERE id = 186858; -- akilzon door
UPDATE gameobject SET rotation2 = 1, rotation3 = 0 WHERE id = 186858; -- halazzi exit door
UPDATE gameobject SET position_z = 62 WHERE id = 186728; -- Zul'Aman gate
UPDATE gameobject SET orientation = 1.57, rotation2 = 0.7, rotation3 = 0.7 WHERE id IN (186304, 186859); -- Lynx, zul'jin entrance door

-- UPDATE creature_template SET unit_flags = 536870912 WHERE entry IN (24441, 24442, 24443, 24444);
UPDATE creature_template SET speed = 2.75 WHERE entry = 24001;
UPDATE creature_template SET minlevel = 70, maxlevel=70 WHERE entry = 24159;


UPDATE `creature_template` SET 
 `mindmg` = 7500, 
 `maxdmg` = 9000, 
 `attackpower` = ROUND((`mindmg` + `maxdmg`) / 9564), 
 `mindmg` = ROUND(`mindmg` - `attackpower` / 9564), 
 `maxdmg` = ROUND(`maxdmg` - `attackpower` / 9564) 
WHERE entry IN(23576, 23577);

UPDATE `creature_template` SET 
 `mindmg` = 7000, 
 `maxdmg` = 8500, 
 `attackpower` = ROUND((`mindmg` + `maxdmg`) / 9564), 
 `mindmg` = ROUND(`mindmg` - `attackpower` / 9564), 
 `maxdmg` = ROUND(`maxdmg` - `attackpower` / 9564) 
WHERE entry IN(23574);

UPDATE `creature_template` SET 
 `mindmg` = 5000, 
 `maxdmg` = 6500, 
 `attackpower` = ROUND((`mindmg` + `maxdmg`) / 9564), 
 `mindmg` = ROUND(`mindmg` - `attackpower` / 9564), 
 `maxdmg` = ROUND(`maxdmg` - `attackpower` / 9564) 
WHERE entry IN(23578, 24239);


DELETE FROM creature WHERE id = 24325;

UPDATE creature_template SET faction_A = 1890, faction_H = 1890, minlevel = 70, maxlevel = 70, minhealth = 81, maxhealth = 81, mindmg = 60, maxdmg = 90, AIName = "EventAI" WHERE entry = 24338;

INSERT INTO creature_ai_scripts VALUES
(2433801, 24338, 4, 0, 100, 2, 0, 0, 0, 0,
38, 0, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani Snake - Enter Combat"),

(2433802, 24338, 0, 0, 100, 3, 0, 2000, 4000, 8000,
11, 25809, 4, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani Snake - Cast poison 1"),

(2433803, 24338, 0, 0, 100, 3, 0, 2000, 4000, 8000,
11, 25810, 4, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani Snake - Cast poison 2");


INSERT INTO script_waypoint VALUES
 (24375, 6, 120.53, 1578, 43.5, 60000, ''),
 (24375, 7, 120.53, 1578, 43.5, 60000, '');
 
 
 
 --------------------------------------------
 --- Zul'Aman trash
 --------------------------------------------
 
-- patrol przed mostkiem
DELETE FROM creature WHERE guid = '89310';
DELETE FROM creature_addon WHERE guid = '89310';
DELETE FROM game_event_creature WHERE guid = '89310';
DELETE FROM game_event_model_equip WHERE guid = '89310';
UPDATE creature SET position_x = '82.378418', position_y = '1420.176758', position_z = '0.768334', orientation = '3.282977' WHERE guid = '86209';
DELETE FROM creature WHERE guid = '86209';
INSERT INTO creature VALUES (86209,23581,568,1,22276,2170,82.3784,1420.18,0.768334,3.28298,7200,5,0,66000,31550,0,0);
DELETE FROM creature WHERE guid = '16777193';
INSERT INTO creature VALUES (16777193,24179,568,1,0,2179,82.4869,1415.99,0.768301,3.10577,300,0,0,57000,32310,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86209','86209','4.184146', '1.632550', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86209','16777193','0.000000', -3.105771+2*3.1415, '2');
UPDATE creature_addon SET path_id = 16000 WHERE guid = 86209;
UPDATE creature SET MovementType = 2 WHERE guid = 86209;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','1','94.051567', '1420.176880', '0.768301');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','2','105.467941', '1420.016602', '-1.506449');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','3','137.521805', '1419.375000', '-0.302712');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','4','149.002625', '1418.527588', '3.303191');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16000','5','154.109451', '1418.434448', '3.303191', 2000);
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','6','148.595886', '1418.591553', '3.303191');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','7','138.758972', '1418.321777', '-0.325193');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','8','103.891937', '1419.734619', '-1.550349');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16000','9','92.399139', '1420.258057', '0.768636');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16000','10','82.916054', '1420.101074', '0.768636', 2000);

-- akilzon gauntlet
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777021','16777021','10.672875', '3.357843', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777021','16777019','14.364074', '2.617106', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777022','16777022','4.299301', '2.869372', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777022','16777018','11.143018', -2.448455+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777023','16777023','3.229244', '2.487017', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777023','16777017','6.877883', '4.460752', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777024','16777024','12.430120', '2.973121', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('16777024','16777016','13.370307', '3.625662', '2');

-- nalorak trash
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86696','86696','1.860722', -1.503504+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86696','86697','18.118717', '1.966163', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86610','86610','9.865085', '3.064649', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86610','86611','13.621231', '2.771558', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86695','86695','11.071856', -2.294121+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86695','86612','17.539936', -3.124924+2*3.14, '2');
DELETE FROM creature WHERE guid = '89267';
DELETE FROM creature_addon WHERE guid = '89267';
DELETE FROM game_event_creature WHERE guid = '89267';
DELETE FROM game_event_model_equip WHERE guid = '89267';
DELETE FROM creature WHERE guid = '89266';
DELETE FROM creature_addon WHERE guid = '89266';
DELETE FROM game_event_creature WHERE guid = '89266';
DELETE FROM game_event_model_equip WHERE guid = '89266';
DELETE FROM creature WHERE guid = '89272';
DELETE FROM creature_addon WHERE guid = '89272';
DELETE FROM game_event_creature WHERE guid = '89272';
DELETE FROM game_event_model_equip WHERE guid = '89272';
DELETE FROM creature WHERE guid = '89275';
DELETE FROM creature_addon WHERE guid = '89275';
DELETE FROM game_event_creature WHERE guid = '89275';
DELETE FROM game_event_model_equip WHERE guid = '89275';


-- janalai trash
-- 1. usuwamy nadmiernych scoutow i reinforcements (beda one summowane przez scoutow
DELETE FROM creature WHERE guid = '89297';
DELETE FROM creature_addon WHERE guid = '89297';
DELETE FROM game_event_creature WHERE guid = '89297';
DELETE FROM game_event_model_equip WHERE guid = '89297';
DELETE FROM creature WHERE guid = '89274';
DELETE FROM creature_addon WHERE guid = '89274';
DELETE FROM game_event_creature WHERE guid = '89274';
DELETE FROM game_event_model_equip WHERE guid = '89274';
DELETE FROM creature WHERE guid = '89281';
DELETE FROM creature_addon WHERE guid = '89281';
DELETE FROM game_event_creature WHERE guid = '89281';
DELETE FROM game_event_model_equip WHERE guid = '89281';
DELETE FROM creature WHERE guid = '89280';
DELETE FROM creature_addon WHERE guid = '89280';
DELETE FROM game_event_creature WHERE guid = '89280';
DELETE FROM game_event_model_equip WHERE guid = '89280';
DELETE FROM creature WHERE guid = '89303';
DELETE FROM creature_addon WHERE guid = '89303';
DELETE FROM game_event_creature WHERE guid = '89303';
DELETE FROM game_event_model_equip WHERE guid = '89303';
DELETE FROM creature WHERE guid = '89305';
DELETE FROM creature_addon WHERE guid = '89305';
DELETE FROM game_event_creature WHERE guid = '89305';
DELETE FROM game_event_model_equip WHERE guid = '89305';
DELETE FROM creature WHERE guid = '89306';
DELETE FROM creature_addon WHERE guid = '89306';
DELETE FROM game_event_creature WHERE guid = '89306';
DELETE FROM game_event_model_equip WHERE guid = '89306';
DELETE FROM creature WHERE guid = '89283';
DELETE FROM creature_addon WHERE guid = '89283';
DELETE FROM game_event_creature WHERE guid = '89283';
DELETE FROM game_event_model_equip WHERE guid = '89283';
DELETE FROM creature WHERE guid = '89293';
DELETE FROM creature_addon WHERE guid = '89293';
DELETE FROM game_event_creature WHERE guid = '89293';
DELETE FROM game_event_model_equip WHERE guid = '89293';
DELETE FROM creature WHERE guid = '89309';
DELETE FROM creature_addon WHERE guid = '89309';
DELETE FROM game_event_creature WHERE guid = '89309';
DELETE FROM game_event_model_equip WHERE guid = '89309';
DELETE FROM creature WHERE guid = '89313';
DELETE FROM creature_addon WHERE guid = '89313';
DELETE FROM game_event_creature WHERE guid = '89313';
DELETE FROM game_event_model_equip WHERE guid = '89313';
DELETE FROM creature WHERE guid = '89307';
DELETE FROM creature_addon WHERE guid = '89307';
DELETE FROM game_event_creature WHERE guid = '89307';
DELETE FROM game_event_model_equip WHERE guid = '89307';
DELETE FROM creature WHERE guid = '89312';
DELETE FROM creature_addon WHERE guid = '89312';
DELETE FROM game_event_creature WHERE guid = '89312';
DELETE FROM game_event_model_equip WHERE guid = '89312';
DELETE FROM creature WHERE guid = '89290';
DELETE FROM creature_addon WHERE guid = '89290';
DELETE FROM game_event_creature WHERE guid = '89290';
DELETE FROM game_event_model_equip WHERE guid = '89290';
DELETE FROM creature WHERE guid = '89291';
DELETE FROM creature_addon WHERE guid = '89291';
DELETE FROM game_event_creature WHERE guid = '89291';
DELETE FROM game_event_model_equip WHERE guid = '89291';
DELETE FROM creature WHERE guid = '89329';
DELETE FROM creature_addon WHERE guid = '89329';
DELETE FROM game_event_creature WHERE guid = '89329';
DELETE FROM game_event_model_equip WHERE guid = '89329';
DELETE FROM creature WHERE guid = '89289';
DELETE FROM creature_addon WHERE guid = '89289';
DELETE FROM game_event_creature WHERE guid = '89289';
DELETE FROM game_event_model_equip WHERE guid = '89289';

-- 2. ustawiamy scoutow, dodajemy formacje i jeden patrol
DELETE FROM creature WHERE guid = '89278';
INSERT INTO creature VALUES (89278,23586,568,1,0,2172,-207.987,1376.99,0.006258,1.59629,7200,5,0,12000,0,0,0);
UPDATE creature SET position_x = '-215.393524', position_y = '1372.007935', position_z = '-0.116710', orientation = '1.658569' WHERE guid = '89278';
DELETE FROM creature WHERE guid = '89287';
INSERT INTO creature VALUES (89287,23586,568,1,0,2172,-200.669,1253.68,1.28333,1.86412,7200,5,0,12000,0,0,0);
UPDATE creature SET position_x = '-196.991638', position_y = '1226.943726', position_z = '0.808015', orientation = '1.828990' WHERE guid = '89287';
UPDATE creature SET position_x = '-153.003403', position_y = '1220.985596', position_z = '3.259003', orientation = '2.585319' WHERE guid = '89288';
DELETE FROM creature WHERE guid = '89284';
INSERT INTO creature VALUES (89284,23586,568,1,0,2172,-134.575,1189.24,0.125,1.69314,7200,5,0,12000,0,0,0);
UPDATE creature SET position_x = '-139.536514', position_y = '1176.938477', position_z = '0.006297', orientation = '1.627140' WHERE guid = '89284';
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89190','89190','4.885793', -2.568675+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89190','86859','16.108612', -2.349141+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89190','86701','19.073454', -2.687883+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89190','86750','14.515718', -3.340748+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89190','86702','24.581921', -3.167427+2*3.14, '2');
DELETE FROM creature WHERE guid = '89288';
INSERT INTO creature VALUES (89288,23586,568,1,0,2172,-153.003,1220.99,3.259,2.58532,7200,5,0,12000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89255','89255','11.766906', '-1.638686'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89255','89286','8.131423', '-2.694988'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89255','89167','15.882013', '-2.122589'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89255','89251','15.512838', '-2.800208'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89255','89241','18.695910', '-2.183559'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89122','89122','5.500157', '3.374148', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89122','89123','10.375655', '3.038918', '2');
UPDATE creature_addon SET path_id = 16001 WHERE guid = 89122;
UPDATE creature SET MovementType = 2 WHERE guid = 89122;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','1','-101.468040', '1106.685669', '5.316484');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','2','-114.172958', '1096.332275', '0.000088');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','3','-180.005493', '1125.432373', '0.062200');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','4','-175.022034', '1117.034790', '0.026807');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','5','-109.621445', '1092.614014', '0.000081');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','6','-97.252785', '1103.822876', '5.351527');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16001','7','-92.552071', '1110.934937', '5.594002');


-- patrol za mostkiem
DELETE FROM creature WHERE guid = '89327';
DELETE FROM creature_addon WHERE guid = '89327';
DELETE FROM game_event_creature WHERE guid = '89327';
DELETE FROM game_event_model_equip WHERE guid = '89327';
DELETE FROM creature WHERE guid = '89326';
DELETE FROM creature_addon WHERE guid = '89326';
DELETE FROM game_event_creature WHERE guid = '89326';
DELETE FROM game_event_model_equip WHERE guid = '89326';
DELETE FROM creature WHERE guid = '86212';
INSERT INTO creature VALUES (86212,23596,568,1,22308,2174,94.8123,1158.76,-3.60433,0.568136,7200,5,0,69000,6462,0,0);
DELETE FROM creature WHERE guid = '86211';
INSERT INTO creature VALUES (86211,24059,568,1,0,1272,95.7619,1155.89,-3.16286,0.533555,7200,5,0,86000,0,0,0);
UPDATE creature SET position_x = '96.750381', position_y = '1143.979004', position_z = '2.741203', orientation = '3.037609' WHERE guid = '86211';
UPDATE creature SET position_x = '96.373978', position_y = '1148.420166', position_z = '1.361593', orientation = '3.223748' WHERE guid = '86212';
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86212','86212','4.411926', '3.044618', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86212','86211','5.790401', '-2.373229'+2*3.14, '2');
UPDATE creature_addon SET path_id = 16002 WHERE guid = 86212;
UPDATE creature SET MovementType = 2 WHERE guid = 86212;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16002','1','119.805244', '1154.732666', '-0.756783');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16002','2','120.989281', '1173.883301', '-5.670388', 2000);
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16002','3','108.234924', '1157.043335', '-1.856301');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16002','4','96.420837', '1148.466553', '1.331952', 2000);

-- trash halazziego
-- 1. trash przed swiatynia


INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','89140','21.747049', '-3.655880'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','86200','14.102197', '-3.683288'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','86203','7.527732', '-3.114853'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','87042','14.395482', '-1.676248'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','86201','16.516825', '-2.093451'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89140','86202','24.942682', '-2.387093'+2*3.14, '2');
DELETE FROM creature WHERE guid = '89140';
INSERT INTO creature VALUES (89140,24530,568,1,0,0,148.932,1113.78,1.30726,0.122173,7200,0,0,55000,0,0,1);
DELETE FROM creature WHERE guid = '86200';
INSERT INTO creature VALUES (86200,24064,568,1,0,0,148.186,1125.2,2.13474,3.19395,7200,0,0,29000,0,0,1);
DELETE FROM creature WHERE guid = '86203';
INSERT INTO creature VALUES (86203,24064,568,1,0,0,155.335,1130.25,1.17472,0.907571,7200,0,0,29000,0,0,1);
DELETE FROM creature WHERE guid = '86201';
INSERT INTO creature VALUES (86201,24064,568,1,0,0,169.462,1130.48,1.09778,5.51524,7200,0,0,29000,0,0,1);
DELETE FROM creature WHERE guid = '86202';
INSERT INTO creature VALUES (86202,24064,568,1,0,0,172.895,1120.76,0.478469,6.23082,7200,0,0,29000,0,0,1);
DELETE FROM creature WHERE guid = '86923';
INSERT INTO creature VALUES (86923,24065,568,1,0,2178,179.738,1145.42,0.125,4.69055,7200,5,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '86927';
INSERT INTO creature VALUES (86927,24065,568,1,0,2178,183.726,1144.8,0.125,4.69055,7200,5,0,86000,0,0,0);
UPDATE creature SET position_x = '149.168900', position_y = '1149.457764', position_z = '0.376655', orientation = '3.240619' WHERE guid = '86923';
UPDATE creature SET position_x = '149.062698', position_y = '1146.417725', position_z = '0.499654', orientation = '3.281450' WHERE guid = '86927';
DELETE FROM creature WHERE guid = '169';
INSERT INTO creature VALUES (169,24530,568,1,0,0,149.231,1144.16,0.518477,3.14124,300,0,0,55000,0,0,0);
DELETE FROM creature WHERE guid = '171';
INSERT INTO creature VALUES (171,24530,568,1,0,0,149.234,1152.1,0.216968,3.14124,300,0,0,55000,0,0,0);
DELETE FROM creature WHERE guid = '89207';
DELETE FROM creature_addon WHERE guid = '89207';
DELETE FROM game_event_creature WHERE guid = '89207';
DELETE FROM game_event_model_equip WHERE guid = '89207';
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','89199','9.749593', '0.726225', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','89176','8.244889', '1.586418', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','89200','12.820809', '2.833142', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','87043','18.734085', '0.945213', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','86198','24.247807', '0.398813', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89199','89141','14.444684', '0.080577', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86306','86306','15.931991', '-3.178312'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86306','86239','10.931558', '-2.406988'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86306','86280','10.859262', '-3.962711'+2*3.14, '2');
DELETE FROM creature WHERE guid = '213';
INSERT INTO creature VALUES (213,24065,568,1,0,2178,372.578,929.409,5.54469e-005,1.80115,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '215';
INSERT INTO creature VALUES (215,24138,568,1,0,0,369.709,925.152,-0.023556,1.80115,300,0,0,10000,0,0,0);
DELETE FROM creature WHERE guid = '217';
INSERT INTO creature VALUES (217,24138,568,1,0,0,376.831,926.822,0.127245,1.80115,300,0,0,10000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('213','213','4.978244', '3.935606', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('213','217','0.000000', '-1.801145'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('213','215','7.315000', '-1.570801'+2*3.14, '2');
INSERT INTO creature_addon VALUES (213, 16003, 0, 0, 0, 0, 0, 0, NULL);
UPDATE creature SET MovementType = 2 WHERE guid = 213;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16003','1','380.684143', '882.053101', '0.011345', 2000);
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16003','2','372.558624', '929.268982', '0.000108', 2000);
DELETE FROM creature WHERE guid = '219';
INSERT INTO creature VALUES (219,24043,568,1,0,0,435.936,887.613,0.0130187,5.15148,300,0,0,55500,0,0,0);
DELETE FROM creature WHERE guid = '221';
INSERT INTO creature VALUES (221,24043,568,1,0,0,432.412,906.888,0.00131052,1.80961,300,0,0,53000,0,0,0);
DELETE FROM creature WHERE guid = '223';
INSERT INTO creature VALUES (223,24043,568,1,0,0,442.453,901.16,0.00131052,0.382536,300,0,0,55500,0,0,0);
DELETE FROM creature WHERE guid = '225';
INSERT INTO creature VALUES (225,24043,568,1,0,0,448.853,887.467,0.694335,5.69968,300,0,0,55500,0,0,0);
DELETE FROM creature WHERE guid = '227';
INSERT INTO creature VALUES (227,24043,568,1,0,0,442.574,918.779,0.000400507,1.80018,300,0,0,53000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('221','221','10.589442', '0.126653', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('221','223','13.054399', '-0.875352'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('221','219','10.568308', '-2.241375'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('221','225','20.688822', '-1.691584'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('221','227','25.441502', '-0.173461'+2*3.14, '2');
DELETE FROM creature WHERE guid = '221';
INSERT INTO creature VALUES (221,24043,568,1,0,0,432.412,906.888,0.00131052,1.80961,300,5,0,53000,0,0,1);
DELETE FROM creature WHERE guid = '219';
INSERT INTO creature VALUES (219,24043,568,1,0,0,435.936,887.613,0.0130187,5.15148,300,5,0,53000,0,0,1);
DELETE FROM creature WHERE guid = '223';
INSERT INTO creature VALUES (223,24043,568,1,0,0,442.453,901.16,0.00131052,0.382536,300,5,0,58000,0,0,1);
DELETE FROM creature WHERE guid = '225';
INSERT INTO creature VALUES (225,24043,568,1,0,0,448.853,887.467,0.694335,5.69968,300,5,0,58000,0,0,1);
DELETE FROM creature WHERE guid = '227';
INSERT INTO creature VALUES (227,24043,568,1,0,0,442.574,918.779,0.000400507,1.80018,300,5,0,58000,0,0,1);
DELETE FROM creature WHERE guid = '229';
INSERT INTO creature VALUES (229,24043,568,1,0,0,453.57,974.411,1.26376,4.53335,300,0,0,58000,0,0,0);
DELETE FROM creature WHERE guid = '231';
INSERT INTO creature VALUES (231,24043,568,1,0,0,442.101,987.079,3.86685e-005,2.40885,300,0,0,58000,0,0,0);
DELETE FROM creature WHERE guid = '233';
INSERT INTO creature VALUES (233,24043,568,1,0,0,452.966,991.321,3.86685e-005,0.201091,300,0,0,58000,0,0,0);
DELETE FROM creature WHERE guid = '235';
INSERT INTO creature VALUES (235,24043,568,1,0,0,445.517,960.555,0.000640544,4.6543,300,0,0,58000,0,0,0);
DELETE FROM creature WHERE guid = '237';
INSERT INTO creature VALUES (237,24043,568,1,0,0,429.798,969.131,0.844944,2.70494,300,0,0,58000,0,0,0);
DELETE FROM creature WHERE guid = '235';
INSERT INTO creature VALUES (235,24043,568,1,0,0,445.517,960.555,0.000640544,4.6543,300,5,0,58000,0,0,1);
DELETE FROM creature WHERE guid = '237';
INSERT INTO creature VALUES (237,24043,568,1,0,0,429.798,969.131,0.844944,2.70494,300,5,0,55500,0,0,1);
DELETE FROM creature WHERE guid = '229';
INSERT INTO creature VALUES (229,24043,568,1,0,0,453.57,974.411,1.26376,4.53335,300,5,0,53000,0,0,1);
DELETE FROM creature WHERE guid = '231';
INSERT INTO creature VALUES (231,24043,568,1,0,0,442.101,987.079,3.86685e-005,2.40885,300,5,0,55500,0,0,1);
DELETE FROM creature WHERE guid = '233';
INSERT INTO creature VALUES (233,24043,568,1,0,0,452.966,991.321,3.86685e-005,0.201091,300,5,0,53000,0,0,1);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('235','235','23.537498', '-3.651751'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('235','237','16.335646', '-4.514335'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('235','229','17.407461', '-2.902844'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('235','231','5.488055', '-1.549016'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('235','233','16.750031', '-1.866957'+2*3.14, '2');
DELETE FROM creature WHERE guid = '239';
INSERT INTO creature VALUES (239,24065,568,1,0,2178,372.215,1037.09,9.9418,4.68885,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '241';
INSERT INTO creature VALUES (241,24065,568,1,0,2178,450.562,954.728,0.000960274,4.67706,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '243';
INSERT INTO creature VALUES (243,23597,568,1,0,2175,444.354,954.257,0.000960274,4.63387,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '245';
INSERT INTO creature VALUES (245,23597,568,1,0,2175,368.109,1037.12,9.98475,4.79881,300,0,0,86000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('243','243','5.019747', '-3.610866'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('243','241','5.221953', '-2.304354'+2*3.14, '2');
INSERT INTO creature_addon VALUES (243, 16004, 0, 0, 0, 0, 0, 0, NULL);
UPDATE creature SET MovementType = 2 WHERE guid = 243;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16004','1','443.901337', '933.770325', '0.001015');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16004','2','432.810181', '908.425598', '0.028836', 2000);
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16004','3','446.254456', '930.220764', '0.024159');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16004','4','447.980743', '954.145020', '0.000323', 2000);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('245','245','4.834088', '4.282935', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('245','239','1.320878', '3.414849', '2');
UPDATE creature_addon SET path_id = 16005 WHERE guid = 245;
UPDATE creature SET MovementType = 2 WHERE guid = 245;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','1','368.842926', '1018.199890', '0.708307');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','2','398.662964', '982.746948', '0.000293');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','3','394.097687', '993.979126', '0.237518');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','4','374.838257', '1016.849792', '0.000090');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','5','372.341644', '1035.769043', '9.745299');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','6','369.684753', '1040.518188', '9.533621');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16005','7','367.809479', '1037.086182', '9.959715');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89138','89138','5.824355', '2.943835', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89138','89137','12.297103', '3.211983', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89138','89136','8.408832', '2.339962', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89138','89139','15.104975', '2.430417', '2');


-- 2. swiatynia halazziego


DELETE FROM creature WHERE guid = '247';
INSERT INTO creature VALUES (247,24065,568,1,0,2178,361.57,1118.12,6.42611,4.74972,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '249';
INSERT INTO creature VALUES (249,24065,568,1,22303,2178,379.845,1118.81,6.18594,4.74972,300,0,0,86000,0,0,0);
DELETE FROM creature WHERE guid = '251';
INSERT INTO creature VALUES (251,23596,568,1,22308,2174,384.281,1112.34,6.43841,2.26393,300,0,0,69000,6462,0,0);
DELETE FROM creature WHERE guid = '253';
INSERT INTO creature VALUES (253,23597,568,1,22309,2175,376.77,1112.54,5.99131,0.824294,300,0,0,86000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89143','89143','8.901989', '-1.448463'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89143','89144','4.206829', '-0.945859'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('89143','247','5.823922', '4.027127', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('253','253','10.382740', '2.167502', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('253','249','13.272203', '2.715378', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('253','251','17.709196', '2.290555', '2');
DELETE FROM creature WHERE guid = '255';
INSERT INTO creature VALUES (255,23597,568,1,0,2175,401.014,1089.93,6.48821,3.15378,300,0,0,86000,0,0,0);
INSERT INTO creature_addon VALUES (255, 16006, 0, 0, 0, 0, 0, 0, NULL);
UPDATE creature SET MovementType = 2 WHERE guid = 255;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','1','338.330078', '1089.505981', '6.351917');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','2','334.329803', '1145.711792', '6.328657');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','3','372.386414', '1144.455566', '6.368301');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','4','372.284210', '1142.820801', '6.387153');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','5','397.107483', '1142.770630', '6.268479');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16006','6','400.597046', '1089.345825', '6.421328');

-- trash przed hexem

DELETE FROM creature WHERE guid = '257';
INSERT INTO creature VALUES (257,24374,568,1,0,0,127.864,1066.65,24.3914,1.61126,300,0,0,140000,0,0,0);
DELETE FROM creature WHERE guid = '259';
INSERT INTO creature VALUES (259,24374,568,1,0,0,110.448,1065.95,24.7482,1.61126,300,0,0,140000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('257','257','9.757382', '-0.964596'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('257','259','11.854074', '0.904920', '2');
DELETE FROM creature WHERE guid = '261';
INSERT INTO creature VALUES (261,24374,568,1,0,0,127.235,965.889,30.704,1.58852,300,0,0,140000,0,0,0);
DELETE FROM creature WHERE guid = '263';
INSERT INTO creature VALUES (263,24374,568,1,0,0,108.534,966.342,29.2224,1.58852,300,0,0,140000,0,0,0);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('263','263','0.000000', '-1.588518'+2*3.14, '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('263','261','5.710073', '1.433069', '2');

-- patrol miedzy swiatynia halazziego a hexa
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86927','86927','16.663418', '2.676193', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86927','169','17.488068', '2.552459', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86927','86923','16.331146', '2.859736', '2');
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES ('86927','171','16.471535', '3.020696', '2');
UPDATE creature_addon SET path_id = 16007 WHERE guid = 86927;
UPDATE creature SET MovementType = 2 WHERE guid = 86927;
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','1','148.658035', '1149.302856', '0.391181');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','2','174.919556', '1149.276001', '0.000032');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','3','194.477707', '1119.231689', '0.002425');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16007','4','201.732712', '1118.994995', '0.011500', 2000);
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','5','201.823151', '1113.796143', '0.000592');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','6','184.216064', '1114.864624', '0.000592');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('16007','7','165.347504', '1146.374756', '0.000385');
INSERT INTO waypoint_data (id, point, position_x, position_y, position_z, delay) VALUES ('16007','8','149.283188', '1146.522461', '0.492154', 2000);
