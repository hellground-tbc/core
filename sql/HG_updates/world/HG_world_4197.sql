REPLACE INTO waypoint_data VALUES
(2112, 1, 12639.1, -6915.3, 4.6, 6000, 0, 0, 100, 0),
(2112, 2, 12589.1, -6918.8, 4.6, 3000, 0, 0, 100, 0);
REPLACE INTO creature_template_addon VALUES
(25003, 2112, 0, 0, 0, 0, 0, 0, NULL);
UPDATE creature_template SET MovementType = 2 WHERE entry = 25003;
UPDATE creature SET MovementType = 2 WHERE id = 25003;