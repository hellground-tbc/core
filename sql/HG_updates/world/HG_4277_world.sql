UPDATE creature_template SET flags_extra = 2097152 WHERE entry = 25063;
UPDATE creature_ai_scripts SET event_chance = 25 WHERE id = 2493802;
UPDATE creature_ai_scripts SET action1_param2 = 12 WHERE id = 2506301;
REPLACE INTO creature_ai_scripts VALUES
(2506303, 25063, 4, 0, 100, 0, 100, 500, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Dawnblade Hawkrider - Remove Combat on Aggro");
REPLACE INTO creature_ai_scripts VALUES
(2493803, 24938, 4, 0, 100, 0, 100, 500, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Shattered Sun Marksman - Remove Combat on Aggro");