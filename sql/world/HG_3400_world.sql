DELETE FROM `script_waypoint` WHERE `entry`=5391;
INSERT INTO `script_waypoint` VALUES
   (5391,1,-9901.05,-3727.01,22.1162,5000,''),
   (5391,2,-9907.75,-3726.14,23.2665,0,''),
   (5391,3,-9930.76,-3706.67,23.5959,0,''),
   (5391,4,-9939.84,-3712.56,22.1328,0,''),
   (5391,5,-9947.29,-3712.15,21.928,0,''),
   (5391,6,-9954.73,-3731.75,23.0535,0,''),
   (5391,7,-9982.74,-3724.2,22.9399,0,''),
   (5391,8,-9998.27,-3717.17,22.259,0,''),
   (5391,9,-10015.7,-3685.44,20.0259,0,''),
   (5391,10,-10026.8,-3637.83,21.3316,0,''),
   (5391,11,-10026.8,-3564.12,21.9134,0,''),
   (5391,12,-10029.8,-3541.82,22.5275,0,''),
   (5391,13,-10061.3,-3462.99,21.6763,0,''),
   (5391,14,-10074,-3439.15,21.0513,0,''),
   (5391,15,-10074,-3439.15,21.0513,0,''),
   (5391,16,-10075.1,-3430.99,20.8188,15000,''),
   (5391,17,-10111,-3409.88,22.4119,0,'');

UPDATE `creature_template` SET `ScriptName` = 'npc_galen_goodward' WHERE `entry` = '5391';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1393';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1780137' AND '-1780131';
INSERT INTO `script_texts` VALUES ('-1780131', 'Help ! Please, you must help me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'GILAN_SAY_START_1');
INSERT INTO `script_texts` VALUES ('-1780132', 'Let us leave this place', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'GILAN_SAY_START_2');
INSERT INTO `script_texts` VALUES ('-1780133', 'Help! The beast is on me!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'GILAN_SAY_UNDER_ATTACK_1');
INSERT INTO `script_texts` VALUES ('-1780134', 'Help! i\'m under attack!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'GILAN_SAY_UNDER_ATTACK_2');
INSERT INTO `script_texts` VALUES ('-1780135', 'Thank you, $N. I will remeber you always. You can find my strongbox in my camp, north of Stonard.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'GILAN_SAY_END');
INSERT INTO `script_texts` VALUES ('-1780136', '%s whispers to $N the secret to opening his strongbox.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '2', '0', '0', 'GILAN_EMOTE_END_1');
INSERT INTO `script_texts` VALUES ('-1780137', '%s disappears into the swamp.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '2', '0', '0', 'GILAN_EMOTE_END_2');