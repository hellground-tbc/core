REPLACE INTO waypoint_data VALUES
(2499, 1, 12165, -7066, 72, 0, 0, 0, 100, 0),
(2499, 3, 12180, -7136, 72, 0, 0, 0, 100, 0),
(2499, 2, 12244, -7160, 72, 0, 0, 0, 100, 0),
(2499, 4, 12177, -7166, 72, 0, 0, 0, 100, 0),
(2499, 5, 12240, -7015, 72, 0, 0, 0, 100, 0);
UPDATE spell_script_target SET targetEntry = 25158 WHERE entry = 45201;
UPDATE creature_template SET ScriptName = "npc_ioqd_brutallus" WHERE entry = 25158;
UPDATE creature_template SET ScriptName = "npc_ioqd_madrigosa" WHERE entry = 25160;