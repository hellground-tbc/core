-- Sunwell Radiance on SWP enemy mobs
-- should kil'jaeden be affected is to be verified!
REPLACE INTO creature_template_addon (entry, auras) VALUES
(25165, "45769 0 45769 1"),
(25166, "45769 0 45769 1"),
(25315, "45769 0 45769 1"),
(25373, "45769 0 45769 1"),
(25483, "45769 0 45769 1"),
(25486, "45769 0 45769 1"),
(25506, "45769 0 45769 1"),
(25591, "45769 0 45769 1"),
(25592, "45769 0 45769 1"),
(25593, "45769 0 45769 1"),
(25595, "45769 0 45769 1"),
(25597, "45769 0 45769 1"),
(25599, "45769 0 45769 1"),
(25608, "45769 0 45769 1");
UPDATE creature_template_addon SET auras = "45769 0 45769 1" WHERE entry IN
(24882, 25363, 25367, 25368, 25369, 25370, 25371, 25508, 25509, 25867);
UPDATE creature_template_addon SET auras = "18950 0 18950 1 45769 0 45769 1" WHERE entry IN (25372, 25507);
UPDATE creature_template_addon SET auras = "16380 0 45769 0 45769 1" WHERE entry = 25484;