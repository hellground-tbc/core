UPDATE gameobject_template SET size = 1 WHERE entry=187882;

INSERT INTO gameobject VALUES
(DEFAULT, 11691, 547, 1, -116, -247, -1, 0, 0, 0, 0, 0, 25, 0, 1);

INSERT INTO game_event_gameobject VALUES (@@identity, 1);

INSERT INTO creature VALUES
(DEFAULT, 25740, 547, 2,  0, 0, -116, -247, -1, 1.2, 3600*24, 0, 0, 230000, 0, 0, 0);

INSERT INTO game_event_creature VALUES (@@identity, 1);

INSERT INTO gameobject_involvedrelation VALUES (187882, 11691);

UPDATE creature_template SET flags_extra = flags_extra | 1 WHERE entry = 26338;

UPDATE creature_loot_template SET minCountOrRef = 1
WHERE
(entry = 26338 AND item = 35723) OR
(entry = 25740 AND item = 35723);

UPDATE eventai_scripts SET event_flags = event_flags | 6
WHERE creature_id IN (25755, 25756, 25757, 25740, 25865, 26342, 26340, 26341, 26339);

UPDATE quest_template SET CompleteScript = 11691 WHERE entry = 11691;
INSERT INTO quest_end_scripts VALUES
(11691, 5000, 10, 25740, 3600, 0, -116, -247, -1, 1.2);