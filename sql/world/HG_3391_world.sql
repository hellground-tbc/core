UPDATE `creature_template` SET `ScriptName` = 'npc_private_hendel' WHERE `entry` = '4966';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1324';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600417' AND '-1600413';
INSERT INTO `script_texts` VALUES ('-1600413', 'Why don\'t we deal with you now, Hendel? Lady Proudmoore will speak for you back in the tower.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'hendel SAY_PROGRESS_1_TER');
INSERT INTO `script_texts` VALUES ('-1600414', 'Please... please... Miss Proudmore. I didn\'t mean to...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'hendel SAY_PROGRESS_2_HEN');
INSERT INTO `script_texts` VALUES ('-1600415', 'I apologize for taking so long to get here. I wanted Lady Proudmoore to be present also.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'hendel SAY_PROGRESS_3_TER');
INSERT INTO `script_texts` VALUES ('-1600416', 'We can only stay a few moments before returning to the tower. If you wish to speak to us more you may find us there.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'hendel SAY_PROGRESS_4_TER');
INSERT INTO `script_texts` VALUES ('-1600417', 'Private Hendel, too injured, gives up the chase.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '2', '0', '0', 'hendel EMOTE_SURRENDER');
UPDATE `creature_template` SET `ScriptName` = 'npc_ogron' WHERE `entry` = '4983';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1273';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1800471' AND '-1800452';
INSERT INTO `script_texts` VALUES ('-1800452', 'I noticed some fire on that island over there. A human, too. Let\'s go check it out, $N.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_START');
INSERT INTO `script_texts` VALUES ('-1800453', 'That\'s Reethe alright. Let\'s go see what he has to say, yeah?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_SPOT');
INSERT INTO `script_texts` VALUES ('-1800454', 'W-what do you want? Just leave me alone...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_WHAT');
INSERT INTO `script_texts` VALUES ('-1800455', 'I swear. I didn\'t steal anything from you! Here, take some of my supplies, just go away!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_SWEAR');
INSERT INTO `script_texts` VALUES ('-1800456', 'Just tell us what you know about the Shady Rest Inn, and I won\'t bash your skull in.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_REPLY_RET');
INSERT INTO `script_texts` VALUES ('-1800457', 'I... Well, I may of taken a little thing or two from the inn... but what would an ogre care about that?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_TAKEN');
INSERT INTO `script_texts` VALUES ('-1800458', 'Look here, if you don\'t tell me about the fire...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_TELL_FIRE');
INSERT INTO `script_texts` VALUES ('-1800459', 'Not one step closer, ogre!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_NOCLOSER');
INSERT INTO `script_texts` VALUES ('-1800460', 'And I don\'t know anything about this fire of yours...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_NOFIRE');
INSERT INTO `script_texts` VALUES ('-1800461', 'What was that? Did you hear something?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_HEAR');
INSERT INTO `script_texts` VALUES ('-1800462', 'Paval Reethe! Found you at last. And consorting with ogres now? No fear, even deserters and traitors are afforded some mercy.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_CAL_FOUND');
INSERT INTO `script_texts` VALUES ('-1800463', 'Private, show Lieutenant Reethe some mercy.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_CAL_MERCY');
INSERT INTO `script_texts` VALUES ('-1800464', 'Gladly, sir.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_HALL_GLAD');
INSERT INTO `script_texts` VALUES ('-1800465', '%s staggers backwards as the arrow lodges itself deeply in his chest.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'EMOTE_OGR_RET_ARROW');
INSERT INTO `script_texts` VALUES ('-1800466', 'Ugh... Hallan, didn\'t think you had it in you...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_ARROW');
INSERT INTO `script_texts` VALUES ('-1800467', 'Now, let\'s clean up the rest of the trash, men!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_CAL_CLEANUP');
INSERT INTO `script_texts` VALUES ('-1800468', 'Damn it! You\'d better not die on me, human!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_NODIE');
INSERT INTO `script_texts` VALUES ('-1800469', 'Still with us, Reethe?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_SURVIVE');
INSERT INTO `script_texts` VALUES ('-1800470', 'Must be your lucky day. Alright, I\'ll talk. Just leave me alone. Look, you\'re not going to believe me, but it wa... oh, Light, looks like the girl could shoot...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_RET_LUCKY');
INSERT INTO `script_texts` VALUES ('-1800471', 'By the way, thanks for watching my back.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'SAY_OGR_THANKS');
UPDATE `creature_template` SET `ScriptName` = 'npc_morokk' WHERE `entry` = '4500';
UPDATE `quest_template` SET `SpecialFlags` = '2' WHERE `entry` = '1173';
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = '0' WHERE `entry` = '1173';
UPDATE `quest_template` SET `ReqCreatureOrGOCount1` = '0' WHERE `entry` = '1173';
DELETE FROM `script_texts` WHERE `entry` BETWEEN '-1600500' AND '-1600499';
INSERT INTO `script_texts` VALUES ('-1600499', 'Puny $r wanna fight with Overlord Mok\'Morokk? Me beat you! Me boss here!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '0', 'morokk SAY_MOR_CHALLENGE');
INSERT INTO `script_texts` VALUES ('-1600500', 'Me scared! Me run now!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '1', '0', '0', 'morokk SAY_MOR_SCARED');

DELETE FROM script_waypoint WHERE entry=4500;
INSERT INTO script_waypoint VALUES
   (4500,0,-3125.597168,-2885.673828,34.731,2500,''),
   (4500,1,-3120.257080,-2877.830322,34.917,0,''),
   (4500,2,-3116.487305,-2850.670410,34.869,0,''),
   (4500,3,-3093.474854,-2819.189697,34.432,0,''),
   (4500,4,-3104.726318,-2802.020996,33.954,0,''),
   (4500,5,-3105.906006,-2780.234375,34.469,0,''),
   (4500,6,-3116.080811,-2757.902588,34.734,0,''),
   (4500,7,-3125.234375,-2733.960205,33.189,0,'');

DELETE FROM script_waypoint WHERE entry=4983;
INSERT INTO script_waypoint VALUES
   (4983,0,-3322.649414,-3124.631836,33.842,0,''),
   (4983,1,-3326.336670,-3126.833496,34.426,0,''),
   (4983,2,-3336.984131,-3129.611816,30.692,0,''),
   (4983,3,-3342.598389,-3132.146729,30.422,0,''),
   (4983,4,-3355.827881,-3140.947998,29.534,0,''),
   (4983,5,-3365.828125,-3144.284180,35.176,0,''),
   (4983,6,-3368.904541,-3147.265381,36.091,0,''),
   (4983,7,-3369.355957,-3169.828857,36.325,0,''),
   (4983,8,-3371.443359,-3183.905029,33.454,0,''),
   (4983,9,-3373.824951,-3190.861084,34.717,5000,'SAY_OGR_SPOT'),
   (4983,10,-3368.529785,-3198.210205,34.926,0,'SAY_OGR_RET_WHAT'),
   (4983,11,-3366.265625,-3210.867676,33.733,5000,'pause'),
   (4983,12,-3368.529785,-3198.210205,34.926,0,''),
   (4983,13,-3373.824951,-3190.861084,34.717,0,''),
   (4983,14,-3371.443359,-3183.905029,33.454,0,''),
   (4983,15,-3369.355957,-3169.828857,36.325,0,''),
   (4983,16,-3368.904541,-3147.265381,36.091,0,''),
   (4983,17,-3365.828125,-3144.284180,35.176,0,''),
   (4983,18,-3355.827881,-3140.947998,29.534,0,''),
   (4983,19,-3342.598389,-3132.146729,30.422,0,''),
   (4983,20,-3336.984131,-3129.611816,30.692,0,''),
   (4983,21,-3326.336670,-3126.833496,34.426,0,''),
   (4983,22,-3322.649414,-3124.631836,33.842,0,'');