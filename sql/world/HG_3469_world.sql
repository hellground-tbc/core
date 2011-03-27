UPDATE `creature_template` SET `ScriptName`='npc_melizza_brimbuzzle' WHERE `entry`='12277';

REPLACE INTO script_waypoint VALUES
   (12277,1,-1156.67,2709.78,111.132,3000,'melizza say SAY_START'),
   (12277,2,-1162.84,2712.31,111.565,0,''),
   (12277,3,-1272.48,2671.02,111.556,0,''),
   (12277,4,-1330.92,2595.85,111.557,0,''),
   (12277,5,-1346.75,2585.15,111.557,0,''),
   (12277,6,-1361.1,2561.92,111.019,0,''),
   (12277,7,-1381.24,2499.8,90.8071,0,''),
   (12277,8,-1388.19,2446.18,88.5159,0,''),
   (12277,9,-1388.64,2433.09,88.7723,0,''),
   (12277,10,-1386.79,2411.08,88.9178,3000,'melizza say SAY_COMPLETE'),
   (12277,11,-1372.82,2329.71,91.7523,0,''),
   (12277,12,-1353.83,2209.79,89.5186,0,''),
   (12277,13,-1356.72,2190.23,76.356,0,''),
   (12277,14,-1371.41,2153.2,65.7856,0,''),
   (12277,15,-1405.36,2087.79,61.8109,60000,'QUEST_POST_EVENT');

REPLACE INTO `script_texts` (`entry`,`content_default`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1000607,'Thanks $N. Now let\'s get out of here',0,0,0,0,'melizza say SAY_START'),
(-1000608,'We made it! Thanks again! I\'m going to run ahead!',0,0,0,0,'melizza say SAY_COMPLETE'),
(-1000609,'Hey Hornizz! I\'m back! And there are some people behind me who helped me out of a jam.',0,0,0,0,'melizza say SAY_POST_EVENT_1'),
(-1000610,'We\'re going to have to scratch the Maraudines off our list. Too hard to work with...',0,0,0,0,'melizza say SAY_POST_EVENT_2'),
(-1000611,'Well, I\'m off to the Gelkis. They\'re not as dumb as the Maraudines, but they\'re more reasonable.',0,0,0,0,'melizza say SAY_POST_EVENT_3');