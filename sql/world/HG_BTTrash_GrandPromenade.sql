-- Promenade Sentinel #1
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12884';
UPDATE `creature_addon` SET `path_id`='12884' WHERE `guid`='12884';

DELETE FROM `waypoint_data` WHERE `id`='12884';
INSERT INTO `waypoint_data` VALUES ('12884', '1', '812.123', '125.816', '204.768', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '2', '799.14', '125.507', '204.768', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '3', '779.057', '126.132', '218.732', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '4', '765.125', '129.251', '218.486', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '5', '765.234', '168.396', '218.484', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '6', '779.136', '173.331', '218.711', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '7', '799.658', '172.646', '204.768', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12884', '8', '814.094', '165.973', '204.768', '0', '0', '0', '100', '0');

-- Promenade Sentinel #2
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12889';
UPDATE `creature_addon` SET `path_id`='12889' WHERE `guid`='12889';

DELETE FROM `waypoint_data` WHERE `id`='12889';
INSERT INTO `waypoint_data` VALUES ('12889', '1', '705.441', '171.091', '231.997', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '2', '690.369', '173.901', '232.076', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '3', '669.939', '174.737', '245.999', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '4', '653.311', '164.479', '245.237', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '5', '650.77', '130.322', '245.115', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '6', '669.788', '127.227', '246.022', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '7', '690.383', '127.113', '232.078', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12889', '8', '702.803', '129.194', '232.033', '0', '0', '0', '100', '0');

-- Illidari Blood Lord Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='16197';
INSERT INTO `creature_formations` VALUES ('16197', '16197', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16197', '52417', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16197', '52433', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16197', '52439', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16197', '16198', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16197', '52438', '0', '0', '2');

-- Promenade Sentinel #3
UPDATE `creature` SET `movementtype`='2' WHERE `guid`='12888';
UPDATE `creature_addon` SET `path_id`='12888' WHERE `guid`='12888';

DELETE FROM `waypoint_data` WHERE `id`='12888';
INSERT INTO `waypoint_data` VALUES ('12888', '1', '548.676', '217.212', '272.324', '3000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12888', '2', '550.305', '297.947', '271.404', '3000', '0', '0', '100', '0');

-- Illidari Blood Lord Formations
DELETE FROM `creature_formations` WHERE `leaderguid`='52437';
INSERT INTO `creature_formations` VALUES ('52437', '52437', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52437', '52432', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52437', '52414', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52437', '16195', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52437', '16196', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52437', '52431', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='16193';
INSERT INTO `creature_formations` VALUES ('16193', '16193', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16193', '52413', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16193', '52430', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16193', '16194', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16193', '52435', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16193', '52436', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='16192';
INSERT INTO `creature_formations` VALUES ('16192', '16192', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16192', '52412', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16192', '52434', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16192', '15217', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16192', '52428', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('16192', '52429', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='12885';
INSERT INTO `creature_formations` VALUES ('12885', '12885', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('12885', '12887', '0', '0', '2');

