-- Bonechewer Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='53228';
INSERT INTO `creature_formations` VALUES ('53228', '53228', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53227', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53235', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53585', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53704', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53212', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53211', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53228', '53703', '0', '0', '2');

-- Bonechewer Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='53214';
INSERT INTO `creature_formations` VALUES ('53214', '53214', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53231', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53688', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53213', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53206', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53207', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53699', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53214', '53232', '0', '0', '2');

-- Animations
DELETE FROM `creature_template_addon` WHERE `entry`='23222';
INSERT INTO `creature_template_addon` VALUES ('23222', '0', '0', '0', '0', '0', '35', '0', '0');
DELETE FROM `creature_template_addon` WHERE `entry`='23239';
INSERT INTO `creature_template_addon` VALUES ('23239', '0', '0', '0', '0', '0', '37', '0', '0');
DELETE FROM `creature_template_addon` WHERE `entry`='23223';
INSERT INTO `creature_template_addon` VALUES ('23223', '0', '0', '0', '0', '0', '5', '0', '0');

-- Animation per guid
DELETE FROM `creature_addon` WHERE `guid`='53167';
INSERT INTO `creature_addon` VALUES ('53167', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53084';
INSERT INTO `creature_addon` VALUES ('53084', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53082';
INSERT INTO `creature_addon` VALUES ('53082', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53081';
INSERT INTO `creature_addon` VALUES ('53081', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53084';
INSERT INTO `creature_addon` VALUES ('53084', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53075';
INSERT INTO `creature_addon` VALUES ('53075', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53071';
INSERT INTO `creature_addon` VALUES ('53071', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53079';
INSERT INTO `creature_addon` VALUES ('53079', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53078';
INSERT INTO `creature_addon` VALUES ('53078', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53077';
INSERT INTO `creature_addon` VALUES ('53077', '0', '0', '0', '0', '0', '22', '0', '0');
DELETE FROM `creature_addon` WHERE `guid`='53080';
INSERT INTO `creature_addon` VALUES ('53080', '0', '0', '0', '0', '0', '22', '0', '0');

-- Bonechewer Spectator #1
DELETE FROM `creature_formations` WHERE `leaderguid`='53067';
INSERT INTO `creature_formations` VALUES ('53067', '53067', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53713', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53084', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53068', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53085', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53167', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53158', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53080', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53081', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53117', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53082', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53205', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53067', '53083', '0', '0', '2');

-- Bonechewer Spectator #2
DELETE FROM `creature_formations` WHERE `leaderguid`='53066';
INSERT INTO `creature_formations` VALUES ('53066', '53066', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53712', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53079', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53074', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53075', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53076', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53077', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53069', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53078', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53070', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53071', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53072', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53066', '53073', '0', '0', '2');

-- Behemoth #1 Patrol
DELETE FROM `creature_addon` WHERE `guid`='52740';
INSERT INTO `creature_addon` VALUES ('52740', '52740', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52740';

DELETE FROM `waypoint_data` WHERE `id`='52740';
INSERT INTO `waypoint_data` VALUES ('52740', '1', '900.452', '189.279', '86.1607', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52740', '2', '926.699', '163.143', '86.1355', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52740', '3', '958.281', '192.768', '86.1599', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52740', '4', '938.038', '213.257', '85.6412', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52740', '5', '915.064', '213.227', '85.6412', '0', '0', '0', '100', '0');

-- Behemoth #2 Patrol
DELETE FROM `creature_addon` WHERE `guid`='52739';
INSERT INTO `creature_addon` VALUES ('52739', '52739', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52739';

DELETE FROM `waypoint_data` WHERE `id`='52739';
INSERT INTO `waypoint_data` VALUES ('52739', '1', '876.77', '253.563', '86.0846', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '2', '905.101', '253.406', '86.1595', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '3', '910.961', '284.762', '85.6413', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '4', '896.222', '296.013', '86.1606', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '5', '885.016', '300.671', '86.1518', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '6', '876.499', '301.063', '86.127', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '7', '871.073', '286.37', '86.0069', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '8', '873.674', '267.913', '85.6667', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52739', '9', '878.258', '254.941', '86.0983', '0', '0', '0', '100', '0');

-- Bonechewer Formation #3
UPDATE `creature` SET `position_x`='701.65', `position_y`='75.3377', `position_z`='112.749' WHERE `guid`='53711';

DELETE FROM `creature_formations` WHERE `leaderguid`='53711';
INSERT INTO `creature_formations` VALUES ('53711', '53711', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53711', '53229', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53711', '53710', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53711', '53210', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53711', '53209', '0', '0', '2');

-- Bonechewer Formation #4
DELETE FROM `creature_formations` WHERE `leaderguid`='53701';
INSERT INTO `creature_formations` VALUES ('53701', '53701', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53700', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53234', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53216', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53233', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53215', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53208', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53617', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53701', '53230', '0', '0', '2');

-- Ghosts
DELETE FROM `creature_formations` WHERE `leaderguid`='46817';
INSERT INTO `creature_formations` VALUES ('46817', '46817', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('46817', '40236', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('46817', '40234', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('46817', '40243', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('46817', '40406', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('46817', '40407', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='42920';
INSERT INTO `creature_formations` VALUES ('42920', '42920', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '52420', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '52421', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '16208', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '16210', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '16209', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '16211', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('42920', '16212', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='52419';
INSERT INTO `creature_formations` VALUES ('52419', '52419', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '52418', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '16221', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '40411', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '16220', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '40229', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '40228', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '40233', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '40446', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52419', '16222', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='52426';
INSERT INTO `creature_formations` VALUES ('52426', '52426', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52426', '40340', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52426', '40244', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52426', '52427', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52426', '40342', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='52411';
INSERT INTO `creature_formations` VALUES ('52411', '52411', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '52422', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '52423', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '16215', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '16213', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '39924', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '39925', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '39926', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '16217', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '39928', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '16214', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52411', '16216', '0', '0', '2');

DELETE FROM `creature_formations` WHERE `leaderguid`='40526';
INSERT INTO `creature_formations` VALUES ('40526', '40526', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '52425', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '52424', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '16205', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '40069', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '39929', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '16204', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('40526', '16218', '0', '0', '2');

-- 3 Behemots Patrols
DELETE FROM `creature_addon` WHERE `guid`='52743';
INSERT INTO `creature_addon` VALUES ('52743', '52743', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52743';

DELETE FROM `waypoint_data` WHERE `id`='52743';
INSERT INTO `waypoint_data` VALUES ('52743', '1', '732.067', '68.0795', '113.27', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52743', '2', '735.485', '135.019', '112.914', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52743', '3', '824.055', '132.73', '112.539', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52743', '4', '821.966', '67.0451', '112.743', '0', '0', '0', '100', '0');

DELETE FROM `creature_addon` WHERE `guid`='52741';
INSERT INTO `creature_addon` VALUES ('52741', '52743', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52741';
UPDATE `creature` SET `currentwaypoint`='3' WHERE `guid`='52741';

DELETE FROM `creature_addon` WHERE `guid`='52742';
INSERT INTO `creature_addon` VALUES ('52742', '52742', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52742';

DELETE FROM `waypoint_data` WHERE `id`='52742';
INSERT INTO `waypoint_data` VALUES ('52742', '1', '849.753', '66.2091', '112.751', '2000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52742', '2', '921.093', '68.8445', '112.753', '2000', '0', '0', '100', '0');
