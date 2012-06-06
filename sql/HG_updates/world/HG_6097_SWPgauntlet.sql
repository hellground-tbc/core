UPDATE creature_template SET ScriptName = "mob_shadowsword_commander" WHERE entry = 25837;
UPDATE creature_template SET ScriptName = "mob_shadowsword_deathbringer" WHERE entry = 25485;
UPDATE creature_template SET ScriptName = "mob_volatile_fiend" WHERE entry = 25851;

DELETE FROM waypoint_data where id = 2501;
INSERT INTO waypoint_data (`id`, `point`, `position_x`, `position_y`, `position_z`, `move_flag`) VALUES
(2501, 1, 1605.6, 598.9, 85.0, 1),
(2501, 2, 1602.9, 581.4, 85.0, 1),
(2501, 3, 1623.3, 554.6, 67.7, 1),
(2501, 4, 1655.8, 512.5, 50.6, 1),
(2501, 5, 1628.7, 519.1, 50.6, 1),
(2501, 6, 1564.1, 573.5, 50.6, 1),
(2501, 7, 1561.8, 561.6, 50.6, 1),
(2501, 8, 1580.6, 528.2, 32.3, 1);