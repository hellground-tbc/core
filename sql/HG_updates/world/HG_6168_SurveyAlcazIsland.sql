REPLACE INTO npc_text (ID, text0_0) VALUES (3159, "Pleased to meet you, $c");
REPLACE INTO npc_gossip VALUES
(7895, 3157),
(90442,  3159);
UPDATE creature_template SET ScriptName = "npc_dustwallow_lady_jaina_proudmoore" WHERE entry = 4968;
UPDATE creature_template SET ScriptName = "npc_highlord_bolvar_fordragon" WHERE entry = 1748;
UPDATE spell_target_position SET target_position_x = -8442.3, target_position_y = 334.3, target_position_z = 122.6, target_orientation = 5.42 WHERE id = 42710;