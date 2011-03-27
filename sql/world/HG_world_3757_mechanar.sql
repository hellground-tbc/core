-- Mechanar

-- drzwi przy windzie
INSERT INTO gameobject VALUES 
(DEFAULT, 184632, 554, 3, 236.423, 52.5, 1.67, 3.1415, 0, 0, 1, 0, 0, 0, 1),
(DEFAULT, 184322, 554, 3, 242.887, 52.5, 1.67, 3.1415, 0, 0, 1, 0, 0, 0, 1),
(DEFAULT, 184449, 554, 3, 267.857, 52.5, 27.2293, 3.1415, 0, 0, 1, 0, 0, 0, 1);
UPDATE gameobject_template SET flags = 36 WHERE entry IN (184632, 184322, 184449);

-- gyro-kill script
UPDATE creature_template SET AIName = '', ScriptName = 'boss_gatewatcher_gyro_kill' WHERE entry = 19218;

-- gatewatchers patrol
DELETE FROM waypoint_data WHERE id IN (1697, 1698);
INSERT INTO waypoint_data VALUES
(1697, 1, 217, 55, 0,4000, 0, 0, 100, 0),
(1697, 2, 171, 73, 0,0, 0, 0, 100, 0),
(1697, 3, 119, 75, 15,4000, 0, 0, 100, 0),
(1697, 4, 171, 73, 0 ,0, 0, 0, 100, 0),
(1698, 1, 207, -59, 0,4000, 0, 0, 100, 0),
(1698, 2, 171, -81, 0,0, 0, 0, 100, 0),
(1698, 3, 120, -73, 15,4000, 0, 0, 100, 0),
(1698, 4, 171, -81, 0,0, 0, 0, 100, 0);
UPDATE creature SET
position_x = 217,
position_y = 55,
position_z = 0
WHERE guid = 83239;
UPDATE creature SET
position_x = 207,
position_y = -59,
position_z = 0
WHERE guid = 83240;