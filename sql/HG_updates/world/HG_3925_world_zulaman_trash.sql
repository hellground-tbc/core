UPDATE creature_template SET faction_A = 1890, faction_H = 1890, mechanic_immune_mask = 1024 WHERE entry = 23584;
UPDATE creature_template SET minhealth = 19561, maxhealth = 19561, mechanic_immune_mask = 5712 WHERE entry = 24047;
UPDATE creature_ai_scripts SET event_param1 = 2000, event_param2 = 15000, event_param3 = 10000 WHERE id = 2383401;
INSERT INTO creature_template_addon VALUES (23834, 0, 0, 0, 0, 0, 0, 0, "18950 0 18950 1");
UPDATE creature_template SET minhealth = 7500, maxhealth = 7500, minlevel = 70, maxlevel = 70 WHERE entry = 24159;
UPDATE creature_template SET mindmg = 3500, maxdmg = 4500, attackpower = 0, mechanic_immune_mask = 7760, speed = 2, faction_A = 1890, faction_h = 1890 WHERE entry = 24530;
UPDATE creature_template SET faction_A = 1890, faction_h = 1890, mindmg = 2500, maxdmg = 3500, attackpower = 0, speed = 2 WHERE entry = 24043;
INSERT INTO creature_ai_scripts VALUES (2404302, 24043, 1, 0, 100, 3, 0, 0, 30000, 30000, 11, 42866, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani Lynx - Cast Stealth");
UPDATE creature_template_addon SET auras = "18950 0 18950 1" WHERE entry = 24043;
UPDATE creature_template SET faction_a = 1890, faction_h = 1890, speed = 2, mindmg = 1500, maxdmg = 2500, attackpower = 0 WHERE entry = 24064; 
UPDATE creature_ai_scripts SET event_chance = 0 WHERE id = 2406401;
UPDATE creature_template SET mindmg = 3000, maxdmg = 4500, attackpower = 0 WHERE entry = 23543;
UPDATE creature_ai_scripts SET action1_param2 = 0, event_param1 = 5000, event_param2 = 15000, event_param3 = 60000, event_param4 = 120000 WHERE id = 2354203;
UPDATE creature_ai_scripts SET action1_param2 = 5, event_param1 = 1000, event_param2 = 4000, event_param3 = 10000 WHERE id = 2354204;
UPDATE creature_template SET AIName = "EventAI" WHERE entry = 24059;
INSERT INTO creature_ai_scripts VALUES
( 2405901, 24059, 0, 0, 100, 3, 1000, 10000, 30000, 60000, 11, 43361, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Beast Tamer - Cast Domesticate"),
( 2405902, 24059, 0, 0, 100, 3, 5000, 10000, 10000, 15000, 11, 43359, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Beast Tamer - Cast Eye of The Beast");
UPDATE creature_template SET armor = 6000, faction_A = 1890, faction_H = 1890, mindmg = 6000, maxdmg = 8500, attackpower = 0, mingold = 100000, maxgold = 200000, mechanic_immune_mask = 612441851, AIName = "EventAI", flags_extra = 0x00010000 WHERE entry = 24374;
INSERT INTO creature_ai_scripts VALUES
(2437401, 24374, 0, 0, 100, 3, 1000, 3000, 2000, 10000, 11, 43673, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Berserker - Cast Mighty Blow"),
(2437402, 24374, 2, 0, 100, 2, 30, 0, 0, 0, 11, 28747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Berserker - Cast Mighty Blow");
UPDATE creature_ai_scripts SET event_param1 = 3000, event_param2 = 6000, event_param3 = 10000 WHERE id = 2359602;
UPDATE creature_ai_scripts SET event_param1 = 1000, event_param2 = 6000, event_param3 = 2000 WHERE id = 2359603;
INSERT INTO creature_template_addon VALUES (23596, 0, 0, 0, 0, 0, 0, 0, "18950 0 18950 1");
UPDATE creature_template SET mindmg = 6000, maxdmg = 7500, attackpower = 0, mechanic_immune_mask = 612441851 WHERE entry = 23597;
INSERT INTO creature_template_addon VALUES (23597, 0, 0, 0, 0, 0, 0, 0, "18950 0 18950 1");
UPDATE creature_template set mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, mechanic_immune_mask = 70259 WHERE entry = 24065;
INSERT INTO creature_template_addon VALUES (24065, 0, 0, 0, 0, 0, 0, 0, "18950 0 18950 1");
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5 WHERE entry = 23581;
UPDATE creature_ai_scripts SET event_type = 0, event_param1 = 10000, event_param2=15000, event_param3 = 15000, event_param4 = 30000 WHERE id = 2358102;
UPDATE creature_ai_scripts SET event_type = 14, event_param1 = 5000, event_param2 = 40, event_param3 = 5000, event_param4 = 10000, action1_param2 = 6 WHERE id = 2358101;
UPDATE creature_ai_scripts SET event_param3 = 8000 WHERE id = 235804;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, mechanic_immune_mask = 70259 WHERE entry = 24180;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5 WHERE entry = 23587;
UPDATE creature_template SET ScriptName = "npc_amanishi_scout", AIName = "", mechanic_immune_mask = 66048 WHERE entry = 23586;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, mechanic_immune_mask = 71283 WHERE entry = 24549;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, AIName = "EventAI" WHERE entry = 23774;
INSERT INTO creature_ai_scripts VALUES
(2377401, 23774, 0, 0, 100, 3, 5000, 10000, 15000, 25000, 11, 20989, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Trainer - Cast Sleep"),
(2377402 ,23774, 16, 0, 100, 3, 43292, 100, 5000, 15000, 11, 43292, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Trainer - Incite Rage");
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5 WHERE entry = 23582;
UPDATE creature_ai_scripts SET event_chance = 100, action1_param2 = 0 WHERE id = 2358201;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, mechanic_immune_mask = 71283 WHERE entry = 23580;
UPDATE creature_ai_scripts SET action2_param1 = 154 WHERE id = 2358001;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5 WHERE entry = 24225;
UPDATE creature_template SET mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, mechanic_immune_mask = 70259 WHERE entry = 24179;
UPDATE creature_ai_scripts SET event_chance = 0 WHERE id = 2417904;
INSERT INTO creature_template_addon VALUES (24179, 0, 0, 0, 0, 0, 0, 0, "18950 0 18950 1");
UPDATE creature_template SET minhealth = 40000, maxhealth = 40000, faction_a = 1890, faction_h = 1890, mindmg = 5500, maxdmg = 7000, mingold = 20000, maxgold = 40000, AIName = "EventAI" WHERE entry = 24138;
INSERT INTO creature_ai_scripts VALUES
(2413801, 24138, 0, 0, 100, 3, 2000, 6000, 12000, 20000, 11, 43353, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tamed Amani Crocolisk - Cast Infected Bite"),
(2413802, 24138, 0, 0, 100, 3, 5000, 5000, 6000, 15000, 11, 43352, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tamed Amani Crocolisk - Tail Swipe");
UPDATE creature_template SET faction_a = 1890, faction_h = 1890, mindmg = mindmg*2.5, maxdmg = maxdmg*2.5, attackpower = attackpower*2.5, AIName = "EventAI" WHERE entry = 24217;
INSERT INTO creature_ai_scripts VALUES
(2421701, 24217, 0, 0, 100, 3, 1000, 2000, 10000, 15000, 11, 42747, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani Bear Mount - Cast Crunch Armor"),
(2421702, 24217, 0, 0, 100, 3, 10000, 15000, 15000, 25000, 11, 42745, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani Bear Mount - Cast Enrage");
INSERT INTO creature_ai_scripts VALUES
(2358703, 23587, 11, 0, 100, 2, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Amani'shi Reinforcement - Do Combat Pulse on spawn");
INSERT INTO script_texts VALUES
(-1811003, "Invaders! Sound the Alarm!", 0, 0, 0, 0, 0, 0, 0, 0, 12104, 1, 0, 0, "Zulaman - YELL_SCOUT_AGGRO");
UPDATE creature_template SET speed = 3 WHERE entry = 23586;