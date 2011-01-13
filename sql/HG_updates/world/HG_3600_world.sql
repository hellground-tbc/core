REPLACE INTO script_texts VALUES
(-1568067,'Zul\'jin got a surprise for ya...',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12052,1,0,0,'zulaman SAY_INST_RELEASE'),
(-1568068,'Da spirits gonna feast today! Begin da ceremonies, sacrifice da prisoners... make room for our new guests!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12053,1,0,0,'zulaman SAY_INST_BEGIN'),
(-1568069,'Take your pick, trespassers! Any of ma priests be happy to accommodate ya.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12054,1,0,0,'zulaman SAY_INST_PROGRESS_1'),
(-1568070,'Don\'t be shy. Thousands have come before you. Ya not be alone in your service.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12055,1,0,0,'zulaman SAY_INST_PROGRESS_2'),
(-1568071,'Ya gonna fail, strangers. Many try before you, but dey only make us stronger!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12056,1,0,0,'zulaman SAY_INST_PROGRESS_3'),(-1568072,'Your efforts was in vain, trespassers. The rituals nearly be complete.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12057,1,0,0,'zulaman SAY_INST_WARN_1'),
(-1568073,'Soon da cages gonna be empty, da sacrifices be complete, and you gonna take dere places.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12058,1,0,0,'zulaman SAY_INST_WARN_2'),
(-1568074,'Time be running low, strangers. Soon you gonna join da souls of dem ya failed to save.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12059,1,0,0,'zulaman SAY_INST_WARN_3'),
(-1568075,'Make haste, ma priests! Da rituals must not be interrupted!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12060,1,0,0,'zulaman SAY_INST_WARN_4'),
(-1568076,'Ya make a good try... but now you gonna join da ones who already fall.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12061,1,0,0,'zulaman SAY_INST_SACRIF1'),
(-1568077,'Ya not do too bad. Ya efforts [...] for a small time. Come to me now. Ya prove yourself worthy offerings.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12062,1,0,0,'zulaman SAY_INST_SACRIF2'),
(-1568078,'Watch now. Every offering gonna strengthen our ties to da spirit world. Soon, we gonna be unstoppable!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,12065,1,0,0,'zulaman SAY_INST_COMPLETE'),
(-1568079,'Suit yourself. At least five of you must assist me if we\'re to get inside. Follow me.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,1,0,0,'harrison SAY_START'),
(-1568080,'According to my calculations, if enough of us bang the gong at once the seal on these doors will break and we can enter.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,1,0,0,'harrison SAY_AT_GONG'),
(-1568081,'I\'ve researched this site extensively and i won\'t allow any dim-witted treasure hunters to swoop in and steal what belongs to in a museum. I\'ll lead this charge.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,1,0,0,'harrison SAY_OPENING_ENTRANCE'),
(-1568082,'In fact, It would be the best if you just stay here. You\'d only get in my way.', NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,1,0,0,'harrison SAY_OPEN_GATE');

UPDATE creature_template SET npcflag = 1, unit_flags = 32768, equipment_id = 136, ScriptName = "npc_harrison_jones_za" WHERE entry = 24375;
UPDATE gameobject_template SET ScriptName = "go_strange_gong" WHERE entry = 187359;
UPDATE creature_template SET ScriptName = "npc_zulaman_door_trigger" WHERE entry = 25173;

REPLACE INTO `script_waypoint` VALUES
   (24375,1,121.193970,1645.619385,42.021,0, ''),
   (24375,2,132.051468,1642.176025,42.021,5000, 'SAY_AT_GONG'),
   (24375,3,120.670631,1636.346802,42.415,0, 'SAY_OPENING_ENTRANCE'),
   (24375,4,120.536003,1611.654663,43.473,10000, 'SAY_OPEN_GATE'),
   (24375,5,120.536003,1611.654663,43.473,0, '');

DELETE FROM gameobject WHERE id = 186728;
INSERT INTO gameobject VALUES ('', 186728, 568, 1, 120.6, 1605.27, 43.5046, 3.14, 0, 0, 1, 0, 25, 100, 0);