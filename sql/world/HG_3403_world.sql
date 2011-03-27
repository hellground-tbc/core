UPDATE `creature_template` SET `ScriptName` = 'npc_death_ravager' WHERE `entry` = '17556';
UPDATE `creature_template` SET `ScriptName` = 'npc_stillpine_capitive' WHERE `entry` = '17375';
UPDATE `gameobject_template` SET `ScriptName` = 'go_ravager_cage' WHERE `entry` = '181849';
UPDATE `gameobject_template` SET `ScriptName` = 'go_bristlelimb_cage' WHERE `entry` = '181714';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600476' AND '-1600474';
INSERT INTO `script_texts` VALUES ('-1600474', '[Fulborg] The Stillpine furbolgs will not soon forget your bravery!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'Stillpine Capitive free say text 1');
INSERT INTO `script_texts` VALUES ('-1600475', '[Fulborg] Thank you, $N', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'Stillpine Capitive free say text 2');
INSERT INTO `script_texts` VALUES ('-1600476', '[Fulborg] Those remaining at Stillpine Hold will welcome you as a hero!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'Stillpine Capitive free say text 3');

UPDATE `creature_template` SET `ScriptName` = 'npc_aged_dying_ancient_kodo' WHERE `entry` = '4700';
UPDATE `creature_template` SET `ScriptName` = 'npc_aged_dying_ancient_kodo' WHERE `entry` = '4701';
UPDATE `creature_template` SET `ScriptName` = 'npc_aged_dying_ancient_kodo' WHERE `entry` = '4702';
UPDATE `creature_template` SET `ScriptName` = 'npc_aged_dying_ancient_kodo' WHERE `entry` = '11627';
DELETE FROM `npc_text` WHERE `ID`=21000;
INSERT INTO `npc_text` VALUES ('21000', 'Why is it that small creatures such as goblins are always fascinated with larger creatures?', NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0', NULL, NULL, '0', '0', '0', '0', '0', '0', '0', '0');
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600350' AND '-1600348';
INSERT INTO `script_texts` VALUES ('-1600348', 'Ah...the wondrous sound of kodos. I love the way they make the ground shake... inspect the beast for me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'kodo round SAY_SMEED_HOME_1');
INSERT INTO `script_texts` VALUES ('-1600349', 'Hey, look out with that kodo! You had better inspect that beast before I give you credit!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'kodo round SAY_SMEED_HOME_2');
INSERT INTO `script_texts` VALUES ('-1600350', 'That kodo sure is a beauty. Wait a minute, where are my bifocals? Perhaps you should inspect the beast for me.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'kodo round SAY_SMEED_HOME_3');