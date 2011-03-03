USE world; REPLACE INTO eventai_scripts (`id`, creature_id, event_type, event_inverse_phase_mask, event_chance, event_flags, action1_type, action1_param1, action1_param2, action1_param3, `comment` ) 
VALUES (23080, 23080, 11, 0, 100, 2, 11, 40089, 0, 1, "Coilskar Geyser - Geyser on spawn");

USE world; UPDATE creature_template SET faction_A = 1813, faction_H = 1813, ScriptName="mob_eventai" WHERE entry=23080;
