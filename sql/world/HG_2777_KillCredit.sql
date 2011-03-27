ALTER TABLE `creature_template`ADD COLUMN `KillCredit` INT(11) NULL AFTER `heroic_entry`;

-- Burden of Souls
UPDATE `creature_template` set `KillCredit` = 22334 WHERE `entry` IN(16870, 16878, 19410, 19411, 19415, 19413, 19414);

-- Wanted: Giselda te Crone
UPDATE `creature_template` set `KillCredit` = 21276 WHERE `entry` IN(17416, 17417, 17418);

-- Disrupting the Twilight Portal
UPDATE `creature_template` set `KillCredit` = 23393 WHERE `entry` IN(22341, 22342, 22343, 22393, 22363);

-- Fel Orc Scavengers
UPDATE `creature_template` set `KillCredit` = 21161 WHERE `entry` IN(19701, 16876, 16925, 18952);

-- Infiltrating Dragonmaw Fortress
UPDATE `creature_template` set `KillCredit` = 22197 WHERE `entry` IN(21717, 21718, 21719, 21720, 22331);

-- Threat from Above
UPDATE `creature_template` set `KillCredit` = 23450 WHERE `entry` IN(23022, 22148, 22144, 22143);

-- The Battle for the Sun's Reach Armory
UPDATE `creature_template` set `KillCredit` = 25068 WHERE `entry` IN(24999, 25001, 25002);

-- Delete all eventai scripts
DELETE FROM `eventai_scripts` where `event_type` = 6 AND `action1_type` = 33;