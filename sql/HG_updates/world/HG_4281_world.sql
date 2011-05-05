DELETE FROM creature_ai_scripts WHERE entryOrGUID = 25144;
UPDATE creature_ai_texts SET content_default = "Don't let that dragonhawk through! Open fire!", `type` = 1 WHERE entry = -402;
UPDATE creature_ai_texts SET content_default = "Dragonhawk incoming from the west! Shoot that $c down!", `type` = 1 WHERE entry = -403;
UPDATE creature_ai_scripts SET event_type = 10, event_chance = 15, event_param1 = 0, event_param2 = 100, event_param3 = 15000, event_param4 = 20000, action1_param2 = -402, action1_param3 = -403 WHERE id = 2513201;
REPLACE INTO creature_ai_scripts VALUES
(2513202, 25132, 10, 0, 100, 1, 0, 120, 2500, 5000, 11, 45172, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Sunblade Lookout - Shoot player when LOS OOC");
UPDATE creature_template SET AIName = "", ScriptName = "npc_shattered_sun_bombardier" WHERE entry = 25144;
UPDATE waypoint_data SET position_z = 30 WHERE id = 1776 AND `point` IN (1, 2, 3, 4);
UPDATE waypoint_data SET position_z = 80 WHERE id = 1776 AND `point` = 6;
UPDATE creature_addon SET path_id = 0 WHERE guid IN (85370, 85372, 85373, 85374);
UPDATE creature SET MovementType = 2 WHERE id = 25144;
UPDATE creature_ai_scripts SET event_type = 1, event_inverse_phase_mask = 5, event_param1 = 1000, event_param2 = 5000, event_param3= 10200, event_param4 = 10500, action1_param2 = 0 WHERE id = 2503301;
REPLACE INTO creature_ai_scripts VALUES
(2503302, 25033, 0, 5, 100, 1, 1000, 5000, 10200, 11000, 11, 45046, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Eredar Sorcerer - Cast Flames of Doom CB"),
(2503303, 25033, 11, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Eredar Sorcerer - Prevent Combat Movement"),
(2503304, 25033, 11, 0, 100, 0, 0, 0, 0, 0, 23, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Eredar Sorcerer - Set Phase 1 on Spawn"),
(2503305, 25033, 2, 0, 100, 0, 99, 99, 0, 0, 23, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Eredar Sorcerer - Change to Phase 2 on Damage");
UPDATE creature SET spawntimesecs = 10 WHERE id IN (25030, 25031, 25033);
UPDATE creature SET spawntimesecs = 25 WHERE id IN (25028, 25027);
UPDATE creature_template SET AIName = "EventAI" WHERE entry IN (25027, 25028, 25158);
REPLACE INTO creature_ai_scripts VALUES
(2502701, 25027, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Frenzied Ghoul - Prevent Combat Movement on Aggro"),
(2502801, 25028, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Skeletal Ravager - Prevent Combat Movement on Aggro"),
(2515801, 25158, 8, 0, 100, 1, 45072, -1, 5000, 6000, 1, -417, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IoQD Brutallus - Yell when hited by Arcane Charges"),
(2515802, 25158, 10, 0, 5, 0, 0, 120, 10000, 20000, 1, -418, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IoQD Brutallus - Random Yell on OCC LOS");
REPLACE INTO creature_ai_texts VALUES
(-417, "What is this pathetic magic? How about you come back with twenty-four of your best friends and try again, $c", 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 25158),
(-418, "No horror here can compare with what you'll face whe I'm through with you!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 25158);