UPDATE creature_template SET AIName = "EventAI" WHERE entry = 25030;
UPDATE creature_ai_scripts SET event_chance = 12, event_param3 = 10000, event_param4 = 10000 WHERE id = 2513201;
UPDATE creature_ai_scripts SET event_chance = 20, event_flags = 1 WHERE id = 2515802;
REPLACE INTO creature_ai_scripts VALUES
(2502702, 25027, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Frenzied Ghoul - Set Active on Spawn"),
(2502802, 25028, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Skeletal Ravager - Set Active on Spawn"),
(2503001, 25030, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Wrath Enforcer - Set Active on Spawn"),
(2503306, 25033, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Eredar Sorcerer - Set Active on Spawn"),
(2503104, 25031, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Pit Overlord - Set Active on Spawn"),
(2513203, 25132, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Sunblade Lookout - Set Active on Spawn"),
(2515803, 25158, 11, 0, 100, 0, 0, 0, 0, 0, 101, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IoQD Brutallus - Set Active on Spawn");