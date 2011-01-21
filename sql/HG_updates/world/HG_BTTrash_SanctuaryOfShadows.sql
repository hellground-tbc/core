-- Illidari Centurion Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='12753';
INSERT INTO `creature_formations` VALUES ('12753', '12753', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12753', '12752', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12753', '12841', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12753', '12842', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12753', '12825', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12753', '12759', '0', '0', '2');

-- Illidari Centurion Formation #2
DELETE FROM `creature_formations` WHERE `leaderguid`='12750';
INSERT INTO `creature_formations` VALUES ('12750', '12750', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12750', '12751', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12750', '12758', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12750', '12757', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12750', '12823', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12750', '12839', '0', '0', '2');

-- Illidari Centurion Formation #3
DELETE FROM `creature_formations` WHERE `leaderguid`='12746';
INSERT INTO `creature_formations` VALUES ('12746', '12746', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12746', '12747', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12746', '12833', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12746', '12834', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12746', '12820', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12746', '12755', '0', '0', '2');

-- Ashtongue Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='12809';
INSERT INTO `creature_formations` VALUES ('12809', '12809', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12811', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12801', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12816', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12786', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12785', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12809', '12795', '0', '0', '2');

-- Ashtongue Formation #2
DELETE FROM `creature_formations` WHERE `leaderguid`='12797';
INSERT INTO `creature_formations` VALUES ('12797', '12797', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12788', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12812', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12789', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12803', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12817', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12797', '12810', '0', '0', '2');

-- Illidari Centurion Formation #3
DELETE FROM `creature_formations` WHERE `leaderguid`='12745';
INSERT INTO `creature_formations` VALUES ('12745', '12745', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12745', '12832', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12745', '12754', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12745', '12744', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12745', '12818', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12745', '12831', '0', '0', '2');


-- Illidari Centurion Formation #4
DELETE FROM `creature_formations` WHERE `leaderguid`='12748';
INSERT INTO `creature_formations` VALUES ('12748', '12838', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12748', '12748', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12748', '12749', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12748', '12756', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12748', '12822', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12748', '12837', '0', '0', '2');

-- Illidari Nightlord Formation #1
UPDATE `creature` SET `position_x`='789.219', `position_y`='377.665', `position_z`='112.751' WHERE `guid`='12827';
UPDATE `creature` SET `position_x`='785.658', `position_y`='373.303', `position_z`='112.754' WHERE `guid`='12840';
UPDATE `creature` SET `position_x`='793.351', `position_y`='374.696', `position_z`='112.751' WHERE `guid`='12824';

UPDATE `creature_addon` SET `path_id`='12827' WHERE `guid`='12827';
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12827';

DELETE FROM `waypoint_data` WHERE `id`='12827';
INSERT INTO `waypoint_data` VALUES ('12827', '1', '789.22', '377.67', '112.75', '2000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12827', '2', '791.43', '369.13', '112.75', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12827', '3', '794.68', '238.93', '112.75', '2000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12827', '4', '795.25', '249.71', '112.74', '0', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='12827';
INSERT INTO `creature_formations` VALUES ('12827', '12827', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12827', '12840', '5', '0.79', '2');
INSERT INTO `creature_formations` VALUES ('12827', '12824', '5', '5.50', '2');

-- Illidari Nightlord Formation #2
UPDATE `creature_addon` SET `path_id`='12826' WHERE `guid`='12826';
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12826';

DELETE FROM `waypoint_data` WHERE `id`='12826';
INSERT INTO `waypoint_data` VALUES ('12826', '1', '616.98', '385.83', '112.71', '2000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12826', '2', '617.03', '377.12', '112.71', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12826', '3', '608.63', '260.37', '112.72', '2000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12826', '4', '608.74', '268.61', '112.73', '0', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='12826';
INSERT INTO `creature_formations` VALUES ('12826', '12826', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12826', '12835', '5', '0.79', '2');
INSERT INTO `creature_formations` VALUES ('12826', '12821', '5', '5.50', '2');

-- Ashtongue Patrol #1
UPDATE `creature` SET `position_x`='707.433', `position_y`='362.699', `position_z`='125.132' WHERE `guid`='12796';
UPDATE `creature` SET `position_x`='706.646', `position_y`='359.918', `position_z`='125.144' WHERE `guid`='12802';
UPDATE `creature` SET `position_x`='705.921', `position_y`='365.587', `position_z`='125.116' WHERE `guid`='12787';

DELETE FROM `creature_formations` WHERE `leaderguid`='12796';
INSERT INTO `creature_formations` VALUES ('12796', '12796', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12796', '12802', '5', '0.79', '2');
INSERT INTO `creature_formations` VALUES ('12796', '12787', '5', '5.50', '2');

UPDATE `creature_addon` SET `path_id`='12796' WHERE `guid`='12796';
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12796';

DELETE FROM `waypoint_data` WHERE `id`='12796';
INSERT INTO `waypoint_data` VALUES ('12796', '1', '704.168', '360.445', '125.14', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '2', '723.814', '359.922', '125.143', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '3', '725.9', '356.229', '125.149', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '4', '753.716', '322.517', '125.174', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '5', '753.62', '319.39', '125.172', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '6', '750.64', '287.66', '125.142', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '7', '748.388', '284.961', '125.142', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '8', '723.082', '263.289', '125.158', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '9', '719.945', '263.568', '125.177', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '10', '681.224', '266.071', '125.149', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '11', '675.512', '270.854', '125.131', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '12', '655.124', '294.591', '125.117', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '13', '655.017', '300.198', '125.126', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '14', '658.337', '327.576', '125.159', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '15', '665.057', '333.561', '125.156', '700', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '16', '688.727', '360.796', '125.139', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12796', '17', '692.899', '360.548', '125.139', '700', '0', '0', '100', '0');

-- Ashtongue Patrol #2
UPDATE `creature` SET `position_x`='681.224', `position_y`='266.07', `position_z`='125.145' WHERE `guid`='12794';
UPDATE `creature` SET `position_x`='681.224', `position_y`='266.07', `position_z`='125.145' WHERE `guid`='12808';
UPDATE `creature` SET `position_x`='681.224', `position_y`='266.07', `position_z`='125.145' WHERE `guid`='12784';

DELETE FROM `creature_formations` WHERE `leaderguid`='12794';
INSERT INTO `creature_formations` VALUES ('12794', '12794', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12794', '12808', '5', '0.79', '2');
INSERT INTO `creature_formations` VALUES ('12794', '12784', '5', '5.50', '2');

UPDATE `creature_addon` SET `path_id`='12796' WHERE `guid`='12794';
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12794';
UPDATE `creature` SET `currentwaypoint`='10' WHERE `guid`='12794';

-- The Refectory - Shade of Akama room
DELETE FROM `creature_formations` WHERE `leaderguid`='12782';
INSERT INTO `creature_formations` VALUES ('12782', '12782', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12782', '12815', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12782', '12793', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12782', '12807', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='12781';
INSERT INTO `creature_formations` VALUES ('12781', '12781', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12781', '12800', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12781', '12799', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12781', '12792', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='12798';
INSERT INTO `creature_formations` VALUES ('12798', '12798', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12798', '12779', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12798', '12778', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12798', '12790', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='12780';
INSERT INTO `creature_formations` VALUES ('12780', '12780', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12780', '12814', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12780', '12791', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12780', '12804', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='12805';
INSERT INTO `creature_formations` VALUES ('12805', '12805', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12805', '12806', '0', '0', '2');
