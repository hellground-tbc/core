UPDATE creature_template SET ScriptName = "npc_sarthis" WHERE entry = 23093;
UPDATE creature_template SET ScriptName = "npc_sarthis_elemental" WHERE entry IN (23096, 23097, 23098, 23099);
UPDATE creature_template SET ScriptName = "npc_minion_of_sarthis" WHERE entry = 23094;

REPLACE INTO script_texts (`entry`, `content_default`, `type`, `comment`) VALUES
(-1600007, "So my blood was not a sufficient payment, eh? Fine, let us recover your arcane essence. After this, I owe Balthas nothing.", 0, "SAY_SARTHIS_INTRO"),
(-1600008, "places a fetish at the ritual pile.", 2, "EMOTE_SARTHIS_FETISH"),
(-1600009, "The process is arduous. We must first summon forth acolytes of the elements. you must then destroy these acolytes so that my minions can make preparations.", 0, "SAY_SARTHIS_START"),
(-1600010, "Well done! Let\'s continue.", 0, "SAY_SARTHIS_KILLED_ACOLYTE"),
(-1600011, "Prepare yourself! The acolyte of water is soon to come...", 0, "SAY_SARTHIS_WATER"),
(-1600012, "Come forth, acolyte of earth!", 0, "SAY_SARTHIS_EARTH"),
(-1600013, "Fire, show yourself!", 0, "SAY_SARTHIS_FIRE"),
(-1600014, "Now we call forth the the arcane acolyte.", 0, "SAY_SARTHIS_ARCANE"),
(-1600015, "I require your life essence to maintain my existence in this realm.", 0, "SAY_ELEMENTAL_1"),
(-1600016, "It is yours my lord!", 0, "SAY_SARTHIS_FINAL");

REPLACE INTO `script_waypoint` VALUES
   (23093, 0,-2519.80, 4665.8, 170.3, 4000, 'say intro'),
   (23093, 1, -2501.0, 4676.3, 160.8, 0, ''),
   (23093, 2, -2473.6, 4694.0, 155.9, 5000, 'summon fetish'),
   (23093, 3, -2469.9, 4692.8, 156.1, 0, ''),
   (23093, 4, -2460.0, 4699.0, 157.8, 0, ''),
   (23093, 5, -2449.0, 4692.3, 165.5, 0, ''),
   (23093, 6, -2438.0, 4699.8, 172.7, 0, 'summon air'),
   (23093, 7, -2450.0, 4692.2, 165.0, 0, ''),
   (23093, 8, -2444.0, 4683.5, 168.2, 0, ''),
   (23093, 9, -2442.5, 4672.7, 166.62, 0, ''),
   (23093, 10, -2438.1, 4661.84, 160.8, 0, ''),
   (23093, 11, -2436.9, 4648.7, 160.6, 0, 'summon water'),
   (23093, 12, -2411.5, 4634.7, 160.4, 0, ''),
   (23093, 13, -2407.7, 4625.4, 160.7, 0, ''),
   (23093, 14, -2416.5, 4587.6, 160.6, 0, ''),
   (23093, 15, -2397.4, 4578.4, 165.7, 0, 'summon earth'),
   (23093, 16, -2415.5, 4595.5, 160.6, 0, ''),
   (23093, 17, -2407.5, 4627.8, 160.6, 0, ''),
   (23093, 18, -2434.8, 4656.3, 160.6, 0, ''),
   (23093, 19, -2478.8, 4670.0, 159.3, 0, ''),
   (23093, 20, -2481.9, 4725.6, 152.7, 0, ''),
   (23093, 21, -2466.2, 4741.9, 152.8, 0, 'summon fire'),
   (23093, 22, -2484.4, 4720.9, 153.4, 0, ''),
   (23093, 23, -2479.9, 4697.7, 154.7, 0, 'summon arcane');