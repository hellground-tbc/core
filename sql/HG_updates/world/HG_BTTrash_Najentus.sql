-- Leviathan #1 Formation
INSERT INTO `creature_formations` VALUES ('14869', '14869', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14869', '12857', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14869', '12858', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14869', '12862', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14869', '12863', '0', '0', '2');


-- Turtle #1 Formation
INSERT INTO `creature_formations` VALUES ('16201', '16201', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16201', '12848', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16201', '12844', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16201', '12845', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16201', '16199', '0', '0', '2');

-- Aqueous Lord #1
UPDATE `creature` SET `position_x`='252.84', `position_y`='904.10', `position_z`='-35.58' WHERE `guid`='12869';
UPDATE `creature` SET `position_x`='262.82', `position_y`='885.78', `position_z`='-25.80' WHERE `guid`='12854';
UPDATE `creature` SET `position_x`='242.88', `position_y`='886.12', `position_z`='-26.90' WHERE `guid`='12853';

-- Aqueous Lord #1 Formation
INSERT INTO `creature_formations` VALUES ('12869', '12854', '10', '4.71', '2');
INSERT INTO `creature_formations` VALUES ('12869', '12853', '10', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('12869', '12869', '0', '0', '2');

-- Aqueous Lord #1 Waypoints
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='12869';

INSERT INTO `waypoint_data` VALUES ('12869', '1', '252.88', '886.13', '-27.00', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12869', '2', '252.73', '960.71', '-62.63', '0', '0', '0', '100', '0');

UPDATE `creature_addon` SET `path_id`='12869' WHERE `guid`='12869';

-- Aqueous Lord #2 Formation
INSERT INTO `creature_formations` VALUES ('12866', '12851', '10', '4.71', '2');
INSERT INTO `creature_formations` VALUES ('12866', '12852', '10', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('12866', '12866', '0', '0', '2');

-- Aqueous Lord #2 Waypoints
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='12866';

INSERT INTO `waypoint_data` VALUES ('12866', '1', '295.96', '813.46', '-24.52', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12866', '2', '394.70', '812.89', '13.26', '0', '0', '0', '100', '0');

UPDATE `creature_addon` SET `path_id`='12866' WHERE `guid`='12866';

-- Leviathan #2 Formation
INSERT INTO `creature_formations` VALUES ('15215', '15215', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('15215', '12865', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('15215', '12864', '0', '0', '2');

-- Aqueous Lord #3 Formation
INSERT INTO `creature_formations` VALUES ('12867', '12868', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12867', '12867', '0', '0', '2');

-- Turtle #2 Formation
INSERT INTO `creature_formations` VALUES ('16202', '16202', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16202', '12855', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16202', '12856', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16202', '12846', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16202', '12847', '0', '0', '2');

-- Leviathan #3 Formation
INSERT INTO `creature_formations` VALUES ('14856', '14856', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14856', '12860', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('14856', '12861', '0', '0', '2');

-- Najentus Position
UPDATE `creature` SET `position_x`='435.47', `position_y`='727.44', `position_z`='15.63' WHERE `guid`='40527';


-- Aqueous Spawn Respawn 20 minutes
UPDATE `creature` SET `spawntimesecs`='1200' WHERE `id`='22883';

-- Aqueous Spawn Patrol#1
UPDATE `creature_template` SET `speed`='2.5' WHERE `entry`='22883';
INSERT INTO `creature_formations` VALUES ('13405', '13405', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13405', '13409', '0', '0', '2');

UPDATE `creature_addon` SET `path_id`='12870' WHERE `guid`='13405';
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='13405';

INSERT INTO `waypoint_data` VALUES ('12870', '1', '251.44', '759.29', '-27.23', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '2', '217.36', '759.36', '-23.86', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '3', '225.43', '782.14', '-24.39', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '4', '227.67', '805.59', '-24.22', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '5', '219.13', '812.14', '-24.17', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '6', '228.22', '841.06', '-23.87', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '7', '219.55', '860.39', '-23.27', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '8', '227.97', '877.38', '-24.26', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '9', '221.87', '886.87', '-24.52', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '10', '224.47', '958.78', '-60.14', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '11', '225.26', '976.52', '-61.56', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '12', '216.84', '996.08', '-61.48', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '13', '229.36', '1021.73', '-61.49', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '14', '216.84', '1048.74', '-59.86', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '26', '217.36', '759.36', '-23.86', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '25', '225.43', '782.14', '-24.39', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '24', '227.67', '805.59', '-24.22', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '23', '219.13', '812.14', '-24.17', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '22', '228.22', '841.06', '-23.87', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '21', '219.55', '860.39', '-23.27', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '20', '227.97', '877.38', '-24.26', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '19', '221.87', '886.87', '-24.52', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '18', '224.47', '958.78', '-60.14', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '17', '225.26', '976.52', '-61.56', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '16', '216.84', '996.08', '-61.48', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12870', '15', '229.36', '1021.73', '-61.49', '0', '0', '0', '100', '0');


-- Aqueous Spawn Patrol #2
INSERT INTO `creature_formations` VALUES ('13403', '13403', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13403', '13407', '5', '0', '2');

UPDATE `creature` SET `position_x`='286.43', `position_y`='1048.05', `position_z`='-60.00' WHERE `guid`='13403';
UPDATE `creature` SET `position_x`='286.43', `position_y`='1048.05', `position_z`='-60.00' WHERE `guid`='13407';

UPDATE `creature_addon` SET `path_id`='12871' WHERE `guid`='13403';
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='13403';

INSERT INTO `waypoint_data` VALUES ('12871', '1', '286.43', '1048.05', '-60.00', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '2', '274.88', '1019.67', '-60.75', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '3', '276.16', '957.98', '-60.16', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '4', '276.64', '885.94', '-24.15', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '5', '276.59', '870.45', '-24.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '6', '284.51', '860.11', '-23.27', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '7', '275.65', '826.39', '-24.41', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '12', '274.88', '1019.67', '-60.75', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '11', '276.16', '957.98', '-60.16', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '10', '276.64', '885.94', '-24.15', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '9', '276.59', '870.45', '-24.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12871', '8', '284.51', '860.11', '-23.27', '0', '0', '0', '100', '0');


-- Aqueous Spawn Patrol #3
INSERT INTO `creature_formations` VALUES ('13410', '13410', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13410', '13411', '2', '0', '2');

UPDATE `creature` SET `position_x`='275.65', `position_y`='826.39', `position_z`='-24.41' WHERE `guid`='13410';
UPDATE `creature` SET `position_x`='275.65', `position_y`='826.39', `position_z`='-24.41' WHERE `guid`='13411';

UPDATE `creature_addon` SET `path_id`='12872' WHERE `guid`='13410';
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='13410';

INSERT INTO `waypoint_data` VALUES ('12872', '1', '275.65', '826.39', '-24.41', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '2', '291.14', '826.66', '-22.94', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '3', '332.54', '826.46', '-4.84', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '4', '341.42', '824.14', '-0.86', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '5', '349.66', '826.58', '1.78', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '6', '388.74', '826.42', '15.69', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '7', '405.79', '826.48', '14.46', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '12', '291.14', '826.66', '-22.94', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '11', '332.54', '826.46', '-4.84', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '10', '341.42', '824.14', '-0.86', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '9', '349.66', '826.58', '1.78', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12872', '8', '388.74', '826.42', '15.69', '0', '0', '0', '100', '0');

-- Aqueous Spawn Patrol #4
INSERT INTO `creature_formations` VALUES ('13412', '13412', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13412', '13413', '3', '0', '2');

UPDATE `creature_addon` SET `path_id`='12873' WHERE `guid`='13412';
UPDATE `creature` SET `MovementType`='2' WHERE `guid`='13412';

INSERT INTO `waypoint_data` VALUES ('12873', '1', '279.07', '799.00', '-24.25', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '2', '289.22', '799.62', '-24.23', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '3', '346.48', '799.92', '-0.28', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '4', '354.12', '798.57', '2.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '5', '389.46', '800.64', '14.13', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '6', '406.17', '799.02', '14.91', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '10', '289.22', '799.62', '-24.23', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '9', '346.48', '799.92', '-0.28', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '8', '354.12', '798.57', '2.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12873', '7', '389.46', '800.64', '14.13', '0', '0', '0', '100', '0');

-- Aqueous Spawn Group #1
INSERT INTO `creature_formations` VALUES ('13401', '13401', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13401', '13402', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13401', '13408', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13401', '13404', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13401', '13406', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13401', '13400', '0', '0', '2');

-- Aqueous Spawn Group #2
INSERT INTO `creature_formations` VALUES ('13399', '13399', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13399', '13393', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13399', '13395', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13399', '13396', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13399', '13268', '0', '0', '2');

-- Aqueous Spawn Group #3
INSERT INTO `creature_formations` VALUES ('13236', '13236', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13236', '13255', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13236', '13398', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13236', '13397', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('13236', '13316', '0', '0', '2');
