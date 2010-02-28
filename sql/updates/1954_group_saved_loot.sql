/*
MySQL Data Transfer
Source Host: localhost
Source Database: test_
Target Host: localhost
Target Database: test_
Date: 2010-01-29 18:53:40
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for group_saved_loot
-- ----------------------------
CREATE TABLE `group_saved_loot` (
  `creatureId` int(11) DEFAULT NULL,
  `instanceId` int(11) DEFAULT NULL,
  `itemId` int(11) DEFAULT NULL,
  `itemCount` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
