-- Demoralizing Roar all 6 ranks - wasnt in database
REPLACE INTO `spell_threat` VALUES (26998, 42);
REPLACE INTO `spell_threat` VALUES (9898, 42);
REPLACE INTO `spell_threat` VALUES (9747, 42);
REPLACE INTO `spell_threat` VALUES (9490, 42);
REPLACE INTO `spell_threat` VALUES (1735, 42);
REPLACE INTO `spell_threat` VALUES (99, 42);
-- Shield Bash rank 2 and rank 1 - wasnt in database
REPLACE INTO `spell_threat` VALUES (1671, 140);
REPLACE INTO `spell_threat` VALUES (72, 110);
-- Holy shield shoudn't have flat threat additions
DELETE FROM `spell_threat` WHERE `entry` = 27179;
DELETE FROM `spell_threat` WHERE `entry` = 20928;
DELETE FROM `spell_threat` WHERE `entry` = 20927;
DELETE FROM `spell_threat` WHERE `entry` = 20925;
-- Maul - druids
UPDATE `spell_threat` SET `Threat`=320 WHERE `entry` = 26996;
REPLACE INTO `spell_threat` VALUES (6808, 55);
REPLACE INTO `spell_threat` VALUES (6807, 25);
-- Counterspell. Author in addon said it was 300 there too, but was increased by him to 500, maybe should level-scale
UPDATE `spell_threat` SET `Threat`=500 WHERE `entry` = 2139;
-- Banish
REPLACE INTO `spell_threat` VALUES (18647, 95);
REPLACE INTO `spell_threat` VALUES (710, 55);
-- Mind Control - priest
REPLACE INTO `spell_threat` VALUES (10912, 5500);
REPLACE INTO `spell_threat` VALUES (10911, 3000);
REPLACE INTO `spell_threat` VALUES (605, 1500);
-- Faerie Fire
UPDATE `spell_threat` SET `Threat`=85 WHERE `entry` = 17391;
UPDATE `spell_threat` SET `Threat`=60 WHERE `entry` = 17390;
UPDATE `spell_threat` SET `Threat`=35 WHERE `entry` = 16857;
UPDATE `spell_threat` SET `Threat`=85 WHERE `entry` = 9749;
UPDATE `spell_threat` SET `Threat`=60 WHERE `entry` = 778;
UPDATE `spell_threat` SET `Threat`=35 WHERE `entry` = 770;
-- Fear
REPLACE INTO `spell_threat` VALUES (6215, 55);
REPLACE INTO `spell_threat` VALUES (6213, 32);
REPLACE INTO `spell_threat` VALUES (5782, 8);
-- Cyclone
REPLACE INTO `spell_threat` VALUES (33786, 180);
-- Distracting shot - shoudnt even be in spell_threat
DELETE FROM `spell_threat` WHERE `entry` = 20736;
DELETE FROM `spell_threat` WHERE `entry` = 15632;
DELETE FROM `spell_threat` WHERE `entry` = 15631;
DELETE FROM `spell_threat` WHERE `entry` = 15630;
DELETE FROM `spell_threat` WHERE `entry` = 15629;
DELETE FROM `spell_threat` WHERE `entry` = 14274;
-- Hammer of Justice
REPLACE INTO `spell_threat` VALUES (853, 15);
REPLACE INTO `spell_threat` VALUES (5588, 50);
REPLACE INTO `spell_threat` VALUES (5589, 80);
-- My guess. Author of an addon forgot about rank 4
REPLACE INTO `spell_threat` VALUES (10308, 100);
-- Power word: shield
REPLACE INTO `spell_threat` VALUES (25218, 660);
REPLACE INTO `spell_threat` VALUES (25217, 575);
REPLACE INTO `spell_threat` VALUES (10901, 470);
REPLACE INTO `spell_threat` VALUES (10900, 380);
REPLACE INTO `spell_threat` VALUES (10899, 300);
REPLACE INTO `spell_threat` VALUES (10898, 240);
REPLACE INTO `spell_threat` VALUES (6066, 190);
REPLACE INTO `spell_threat` VALUES (6065, 150);
REPLACE INTO `spell_threat` VALUES (3747, 120);
REPLACE INTO `spell_threat` VALUES (600, 80);
REPLACE INTO `spell_threat` VALUES (592, 45);
REPLACE INTO `spell_threat` VALUES (17, 20);
-- Curse of the elements
REPLACE INTO `spell_threat` VALUES (27228, 70);
REPLACE INTO `spell_threat` VALUES (11722, 60);
REPLACE INTO `spell_threat` VALUES (11721, 45);
REPLACE INTO `spell_threat` VALUES (1490, 30);
-- Curse of weakness
REPLACE INTO `spell_threat` VALUES (30909, 70);
REPLACE INTO `spell_threat` VALUES (27224, 60);
REPLACE INTO `spell_threat` VALUES (11708, 50);
REPLACE INTO `spell_threat` VALUES (11707, 40);
REPLACE INTO `spell_threat` VALUES (7646, 32);
REPLACE INTO `spell_threat` VALUES (6205, 22);
REPLACE INTO `spell_threat` VALUES (1108, 12);
REPLACE INTO `spell_threat` VALUES (702, 4);
-- Curse of Recklesness
REPLACE INTO `spell_threat` VALUES (27226, 70);
REPLACE INTO `spell_threat` VALUES (11717, 55);
REPLACE INTO `spell_threat` VALUES (7659, 40);
REPLACE INTO `spell_threat` VALUES (7658, 30);
REPLACE INTO `spell_threat` VALUES (704, 15);
-- Curse of Tongues
REPLACE INTO `spell_threat` VALUES (11719, 50);
REPLACE INTO `spell_threat` VALUES (1714, 25);
-- Cleave ranks 1-4
REPLACE INTO `spell_threat` VALUES (845, 20);
REPLACE INTO `spell_threat` VALUES (7369, 40);
REPLACE INTO `spell_threat` VALUES (11608, 60);
REPLACE INTO `spell_threat` VALUES (11609, 80);
-- Disarm
REPLACE INTO `spell_threat` VALUES (676, 105);
-- Revenge - in spell_threat there's much higher numbers of threat. Addon says it's much lower
REPLACE INTO `spell_threat` VALUES (30357, 200);
REPLACE INTO `spell_threat` VALUES (25269, 160);
REPLACE INTO `spell_threat` VALUES (25288, 130);
REPLACE INTO `spell_threat` VALUES (11601, 110);
REPLACE INTO `spell_threat` VALUES (11600, 90);
REPLACE INTO `spell_threat` VALUES (7379, 70);
REPLACE INTO `spell_threat` VALUES (6574, 50);
REPLACE INTO `spell_threat` VALUES (6572, 30);
-- Hamstring
REPLACE INTO `spell_threat` VALUES (25212, 180);
REPLACE INTO `spell_threat` VALUES (7373, 130);
REPLACE INTO `spell_threat` VALUES (7372, 85);
REPLACE INTO `spell_threat` VALUES (1715, 45);
-- Mocking blow
REPLACE INTO `spell_threat` VALUES (25266, 290);
REPLACE INTO `spell_threat` VALUES (20560, 230);
REPLACE INTO `spell_threat` VALUES (20559, 180);
REPLACE INTO `spell_threat` VALUES (7402, 130);
REPLACE INTO `spell_threat` VALUES (7400, 90);
REPLACE INTO `spell_threat` VALUES (694, 60);
-- Demoralizing Shout
REPLACE INTO `spell_threat` VALUES (1160, 55);
REPLACE INTO `spell_threat` VALUES (6190, 55);
REPLACE INTO `spell_threat` VALUES (11554, 55);
REPLACE INTO `spell_threat` VALUES (11555, 55);
REPLACE INTO `spell_threat` VALUES (11556, 55);
REPLACE INTO `spell_threat` VALUES (25202, 55);
REPLACE INTO `spell_threat` VALUES (25203, 55);			
-- Devastate
UPDATE `spell_threat` SET `Threat`=120 WHERE `entry` = 30022;
UPDATE `spell_threat` SET `Threat`=120 WHERE `entry` = 30016;
UPDATE `spell_threat` SET `Threat`=120 WHERE `entry` = 20243;


