-- Working Animation
DELETE FROM `creature_template_addon` WHERE `entry`='22963';
INSERT INTO `creature_template_addon` VALUES ('22963', '0', '0', '0', '0', '1', '233', '0', '0');

-- Bonechewer Taskmaster Formation #1
DELETE FROM `creature_formations` WHERE `leaderguid`='53815';
INSERT INTO `creature_formations` VALUES ('53815', '53815', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52912', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52913', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52914', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52915', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52916', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53815', '52917', '0', '0', '2');

-- Bonechewer Taskmaster Formation #2
UPDATE `creature` SET `position_x`='639.91', `position_y`='965.94', `position_z`='55.07' WHERE `guid`='53821';
DELETE FROM `creature_formations` WHERE `leaderguid`='53821';
INSERT INTO `creature_formations` VALUES ('53821', '53821', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '52991', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '53048', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '52940', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '52939', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '53035', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '52941', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53821', '53820', '0', '0', '2');

-- Bonechewer Taskmaster Formation #3
DELETE FROM `creature_formations` WHERE `leaderguid`='53818';
INSERT INTO `creature_formations` VALUES ('53818', '53818', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '53817', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '52907', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '52908', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '52909', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '52910', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53818', '52911', '0', '0', '2');

-- Bonechewer Taskmaster Formation #4
DELETE FROM `creature_formations` WHERE `leaderguid`='53819';
INSERT INTO `creature_formations` VALUES ('53819', '53819', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52935', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52936', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52919', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52938', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52930', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53819', '52918', '0', '0', '2');

-- Bonechewer Taskmaster Formation #5
UPDATE `creature` SET `position_x`='775.70', `position_y`='770.58', `position_z`='65.18' WHERE `guid`='53816';
DELETE FROM `creature_formations` WHERE `leaderguid`='53816';
INSERT INTO `creature_formations` VALUES ('53816', '53816', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53816', '53049', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53816', '53050', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53816', '53051', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53816', '53052', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('53816', '53053', '0', '0', '2');

-- Wyrmcaller Formation #1
DELETE FROM `creature_addon` WHERE `guid`='52890';
INSERT INTO `creature_addon` VALUES ('52890', '52890', '0', '0', '0', '0', '0', '0', '0');

DELETE FROM `creature_addon` WHERE `guid`='53893';
DELETE FROM `creature_addon` WHERE `guid`='53906';
INSERT INTO `creature_addon` VALUES ('53906', '0', '24725', '512', '0', '4097', '0', '0', '0');
INSERT INTO `creature_addon` VALUES ('53893', '0', '24725', '512', '0', '4097', '0', '0', '0');


UPDATE `creature` SET `movementType`='2' WHERE `guid`='52890';
UPDATE `creature` SET `movementType`='0' WHERE `guid`='52889';

DELETE FROM `waypoint_data` WHERE `id`='52890';
INSERT INTO `waypoint_data` VALUES ('52890', '1', '651.72', '912.17', '58.13', '1000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52890', '2', '655.74', '941.66', '54.60', '1000', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52890';
INSERT INTO `creature_formations` VALUES ('52890', '52890', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52890', '52889', '5', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('52890', '53893', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52890', '53906', '0', '0', '2');


-- Wyrmcaller Formation #2
DELETE FROM `creature_addon` WHERE `guid`='52871';
INSERT INTO `creature_addon` VALUES ('52871', '52871', '0', '0', '0', '0', '0', '0', '0');

DELETE FROM `creature_addon` WHERE `guid`='53911';
DELETE FROM `creature_addon` WHERE `guid`='53910';
INSERT INTO `creature_addon` VALUES ('53911', '0', '24725', '512', '0', '4097', '0', '0', '0');
INSERT INTO `creature_addon` VALUES ('53910', '0', '24725', '512', '0', '4097', '0', '0', '0');


UPDATE `creature` SET `movementType`='2' WHERE `guid`='52871';
UPDATE `creature` SET `movementType`='0' WHERE `guid`='52872';

DELETE FROM `waypoint_data` WHERE `id`='52871';
INSERT INTO `waypoint_data` VALUES ('52871', '1', '730.20', '918.96', '53.05', '1000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52871', '2', '727.12', '946.68', '53.05', '1000', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52871';
INSERT INTO `creature_formations` VALUES ('52871', '52871', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52871', '52872', '5', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('52871', '53911', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52871', '53910', '0', '0', '2');

--  Wyrmcaller Formation #3
UPDATE `creature` SET `position_x`='703.60', `position_y`='758.63', `position_z`='92.03' WHERE `guid`='53912';
UPDATE `creature` SET `currentwaypoint`='4' WHERE `guid`='53912';

DELETE FROM `creature_addon` WHERE `guid`='53912';
INSERT INTO `creature_addon` VALUES ('53912', '53912', '24725', '512', '0', '4097', '0', '1024', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='53912';

DELETE FROM `waypoint_data` WHERE `id`='53912';
INSERT INTO `waypoint_data` VALUES ('53912', '1', '700.25', '830.89', '88.79', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53912', '2', '663.79', '804.95', '91.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53912', '3', '665.00', '765.50', '91.58', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53912', '4', '703.60', '758.63', '92.03', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53912', '5', '724.68', '799.00', '94.83', '0', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52874';
INSERT INTO `creature_formations` VALUES ('52874', '52874', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52874', '53912', '0', '0', '2');

--  Wyrmcaller Formation #4
DELETE FROM `creature_addon` WHERE `guid`='53913';
INSERT INTO `creature_addon` VALUES ('53913', '53912', '24725', '512', '0', '4097', '0', '1024', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='53913';

DELETE FROM `creature_formations` WHERE `leaderguid`='52873';
INSERT INTO `creature_formations` VALUES ('52873', '52873', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52873', '53913', '0', '0', '2');

--  Illidari Fearbringer #1
UPDATE `creature` SET `position_x`='688.26', `position_y`='816.65', `position_z`='62.65' WHERE `guid`='52772';
DELETE FROM `creature_addon` WHERE `guid`='52772';
INSERT INTO `creature_addon` VALUES ('52772', '52772', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `position_x`='722.93', `position_y`='816.12', `position_z`='60.67' WHERE `guid`='52773';
DELETE FROM `creature_addon` WHERE `guid`='52773';
INSERT INTO `creature_addon` VALUES ('52773', '52773', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52772';
UPDATE `creature` SET `movementType`='2' WHERE `guid`='52773';

DELETE FROM `creature_formations` WHERE `leaderguid`='52772';
INSERT INTO `creature_formations` VALUES ('52772', '52772', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52772', '52773', '0', '0', '2');

DELETE FROM `waypoint_data` WHERE `id`='52772';
INSERT INTO `waypoint_data` VALUES ('52772', '1', '688.26', '816.65', '62.65', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52772', '2', '692.00', '766.79', '65.71', '0', '0', '0', '100', '0');

DELETE FROM `waypoint_data` WHERE `id`='52773';
INSERT INTO `waypoint_data` VALUES ('52773', '1', '722.93', '816.12', '60.67', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52773', '2', '725.31', '766.17', '65.16', '0', '0', '0', '100', '0');

-- Useless Spawn Delete
DELETE FROM `creature` WHERE `id`='23095';
DELETE FROM `creature` WHERE `guid`='52768';
DELETE FROM `creature` WHERE `guid`='52769';

-- Wyrmcaller Formation #5
DELETE FROM `creature_addon` WHERE `guid`='52901';
INSERT INTO `creature_addon` VALUES ('52901', '52901', '0', '0', '0', '0', '0', '0', '0');

DELETE FROM `creature_addon` WHERE `guid`='12741';
DELETE FROM `creature_addon` WHERE `guid`='12742';
INSERT INTO `creature_addon` VALUES ('12741', '0', '24725', '512', '0', '4097', '0', '0', '0');
INSERT INTO `creature_addon` VALUES ('12742', '0', '24725', '512', '0', '4097', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52901';
UPDATE `creature` SET `movementType`='0' WHERE `guid`='52902';

DELETE FROM `waypoint_data` WHERE `id`='52901';
INSERT INTO `waypoint_data` VALUES ('52901', '1', '598.96', '783.63', '70.80', '1000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52901', '2', '617.75', '787.13', '70.22', '1000', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52901';
INSERT INTO `creature_formations` VALUES ('52901', '52901', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52901', '52902', '5', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('52901', '12741', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52901', '12742', '0', '0', '2');

-- Wyrmcaller Formation #6
DELETE FROM `creature_addon` WHERE `guid`='52860';
INSERT INTO `creature_addon` VALUES ('52860', '52860', '0', '0', '0', '0', '0', '0', '0');

DELETE FROM `creature_addon` WHERE `guid`='12743';
DELETE FROM `creature_addon` WHERE `guid`='53917';
INSERT INTO `creature_addon` VALUES ('12743', '0', '24725', '512', '0', '4097', '0', '0', '0');
INSERT INTO `creature_addon` VALUES ('53917', '0', '24725', '512', '0', '4097', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52860';

DELETE FROM `waypoint_data` WHERE `id`='52860';
INSERT INTO `waypoint_data` VALUES ('52860', '1', '630.15', '841.76', '66.49', '1000', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52860', '2', '610.63', '838.17', '66.51', '1000', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52860';
INSERT INTO `creature_formations` VALUES ('52860', '52860', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52860', '12743', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52860', '53917', '0', '0', '2');

-- Wyrmcaller Formation #7
DELETE FROM `creature_addon` WHERE `guid`='12739';
INSERT INTO `creature_addon` VALUES ('12739', '12739', '24725', '512', '0', '4097', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='12739';

DELETE FROM `waypoint_data` WHERE `id`='12739';
INSERT INTO `waypoint_data` VALUES ('12739', '1', '810.26', '842.08', '78.13', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12739', '2', '766.92', '853.89', '79.80', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12739', '3', '760.46', '821.47', '80.51', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('12739', '4', '805.76', '813.83', '83.06', '0', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52861';
INSERT INTO `creature_formations` VALUES ('52861', '52861', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52861', '12739', '0', '0', '2');

-- Wyrmcaller Formation #8 TODO THIS
DELETE FROM `creature_addon` WHERE `guid`='52887';
INSERT INTO `creature_addon` VALUES ('52887', '52887', '0', '0', '0', '0', '0', '0', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='52887';
UPDATE `creature` SET `movementType`='0' WHERE `guid`='52888';

DELETE FROM `waypoint_data` WHERE `id`='52887';
INSERT INTO `waypoint_data` VALUES ('52887', '1', '788.52', '798.81', '64.29', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('52887', '2', '788.55', '766.18', '67.67', '0', '0', '0', '100', '0');

DELETE FROM `creature_addon` WHERE `guid`='53842';
DELETE FROM `creature_addon` WHERE `guid`='12737';
INSERT INTO `creature_addon` VALUES ('53842', '53842', '24725', '512', '0', '4097', '0', '1024', '0');
INSERT INTO `creature_addon` VALUES ('12737', '53842', '24725', '512', '0', '4097', '0', '1024', '0');

UPDATE `creature` SET `movementType`='2' WHERE `guid`='53842';
UPDATE `creature` SET `movementType`='2' WHERE `guid`='12737';

UPDATE `creature` SET `currentwaypoint`='4' WHERE `guid`='12737';

DELETE FROM `waypoint_data` WHERE `id`='53842';
INSERT INTO `waypoint_data` VALUES ('53842', '1', '807.67', '783.40', '89.07', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53842', '2', '787.88', '804.49', '88.75', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53842', '3', '765.90', '789.06', '102.52', '0', '0', '0', '100', '0');
INSERT INTO `waypoint_data` VALUES ('53842', '4', '786.35', '741.23', '98.13', '0', '0', '0', '100', '0');

DELETE FROM `creature_formations` WHERE `leaderguid`='52887';
INSERT INTO `creature_formations` VALUES ('52887', '52887', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52887', '52888', '5', '1.57', '2');
INSERT INTO `creature_formations` VALUES ('52887', '53842', '0', '0', '2');
INSERT INTO `creature_formations` VALUES ('52887', '12737', '0', '0', '2');

-- TODO Cosmetic Waypoints
-- 53841 53830 12738
-- 12740 53843
