DELETE FROM creature WHERE guid IN (86210, 89311);    -- usuwamy wind walkerów którzy nie s¹ w akilzon gauntlecie
DELETE FROM gameobject WHERE id = 186306;             -- drzwi w labiryncie, podobno w bazie serwera ju¿ ich nie ma

UPDATE gameobject_template SET faction = 1375, flags = 32, data1 = 0 WHERE entry = 186306; -- update templatu drzwi za hexem

-- dostawiamy nowe bramy: dwie u halazziego, jedna u akilzona, jedna za hexem i jedna przed zuljinem
INSERT INTO gameobject VALUES 
(DEFAULT, 186303, 568, 1, 307.221, 1122.78, 10.0772, 0, 0, 0, 0.0474907, 0.998872, 25, 0, 1),
(DEFAULT, 186304, 568, 1, 362.855, 1060.37, 10.0493, 1.57075, 0, 0, 0.772254, 0.635314, 25, 0, 1),
(DEFAULT, 186306, 568, 1, 112.735, 912.865, 34.161, 1.57075, 0, 0, 0.722213, 0.691671, 25, 0, 1),
(DEFAULT, 186859, 568, 1, 120.362, 734.103, 45.1114, 4.71225, 0, 0, 0.676343, -0.736587, 25, 0, 1),
(DEFAULT, 186858, 568, 1, 336.752, 1395.62, 74.4815, 3.1415, 0, 0, 0.985144, -0.171729, 25, 0, 1),

-- unieruchamiamy totem
UPDATE creature_template SET unit_flags = 4 WHERE entry = 24224;

--------------
-- timed event
--------------

-- Update templatow skrzynek
UPDATE gameobject_template SET flags = 2, data0 = 57, data15 = 1, ScriptName = "go_zulaman_timed_event_chest" WHERE entry IN (186648, 187021, 186667, 186672);


-- Update templatów klatek
UPDATE gameobject_template SET castBarCaption = "Opening", faction = 0, flags = 0, data0 = 57, data14 = 21400, ScriptName = "go_zulaman_cage" WHERE entry IN (187377, 187378, 187380, 187379);

-- UPDATE skryptow dla jeñców
UPDATE creature_template SET ScriptName = "npc_tanzar" WHERE entry = 23790;
UPDATE creature_template SET ScriptName = "npc_harkor" WHERE entry = 23999;
UPDATE creature_template SET ScriptName = "npc_kraz" WHERE entry = 24024;
UPDATE creature_template SET ScriptName = "npc_ashli" WHERE entry = 24001;
-- UPDATE templatów dla jeñców i ich martwych odpowiedników
UPDATE creature_template SET minlevel = 70, maxlevel = 70 WHERE entry IN (24441, 24024, 24001);
UPDATE creature_template SET minhealth = 4890, maxhealth = 4890 WHERE entry IN (24442, 23790);
UPDATE creature_template SET minhealth = 3260, maxhealth = 3260, Speed = 4 WHERE entry IN (24001, 24441);
UPDATE creature_template SET minhealth = 5624, maxhealth = 5624, Speed = 2.5 WHERE entry IN (24444, 24024);
UPDATE creature_template SET minhealth = 4890, maxhealth = 4890 WHERE entry IN (23999, 24443);
UPDATE creature_template SET npcflag = 1, flags_extra = 2 WHERE entry IN (23999, 23790, 24001, 24024);
UPDATE creature_template SET dynamicflags = 32 WHERE entry IN (24441, 24442, 24443, 24444);

-- Update templatu Loot Boxa (harkor trzyma w nim skrzynke z lootem)
UPDATE gameobject_template SET flags = 4 WHERE entry = 186622;

-- wrzucamy wszystkie go potrzebne do timed eventu (chesty, klatki, boxa i wazy)
INSERT INTO gameobject VALUES
(DEFAULT, 187377, 568, 1, -145.762, 1335.76, 48.174, 6.17758, 0, 0, 0.0527796, -0.998606, 237000, 0, 1),
(DEFAULT, 186648, 568, 1, -146.195, 1350.36, 48.1739, 4.74132, 0, 0, 0.696806, -0.71726, 237000, 0, 1),
(DEFAULT, 187378, 568, 1, 406.663, 1504.73, 81.6148, 4.24865, 0, 0, 0.850675, -0.525691, 237000, 0, 1),
(DEFAULT, 187380, 568, 1, 400.589, 1146.05, 6.34083, 4.94645, 0, 0, 0.619704, -0.784835, 237000, 0, 1),
(DEFAULT, 187021, 568, 1, 415.742, 1499.24, 81.6368, 4.25931, 0, 0, 0.847862, -0.530217, 237000, 0, 1),
(DEFAULT, 186622, 568, 1, 415.742, 1499.24, 81.6368, 4.25931, 0, 0, 0.847862, -0.530217, 237000, 0, 1),
(DEFAULT, 187379, 568, 1, -72.8802, 1176.6, 5.26225, 1.83072, 0, 0, 0.792782, 0.609506, 237000, 0, 1),
(DEFAULT, 186667, 568, 1, -84.1658, 1177.88, 5.63583, 1.28348, 0, 0, 0.598589, 0.801056, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 423.573, 1142.57, 5.51217, 3.24004, 0, 0, 0.998789, -0.0492018, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 417.317, 1150.98, 5.26049, 4.8325, 0, 0, 0.663393, -0.748271, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 332.507, 1086.77, 6.23425, 1.27307, 0, 0, 0.594414, 0.804159, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 316.036, 1083.1, 9.77774, 0.222981, 0, 0, 0.11126, 0.993791, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 342.895, 1150.49, 6.34384, 1.66733, 0, 0, 0.740399, 0.672167, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 332.877, 1150.18, 6.34384, 1.62413, 0, 0, 0.72571, 0.688001, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 390.165, 1079.93, 6.34375, 5.42309, 0, 0, 0.416913, -0.908946, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 424.028, 1092.51, 6.35764, 0.0274122, 0, 0, 0.0137057, 0.999906, 237000, 0, 1),
(DEFAULT, 186671, 568, 1, 423.14, 1084.48, 6.57313, 0.0274122, 0, 0, 0.0137057, 0.999906, 237000, 0, 1),
(DEFAULT, 186672, 568, 1, 423.14, 1084.48, 6.57313, 0.0274122, 0, 0, 0.0137057, 0.999906, 237000, 0, 1);

-- cleanup
DELETE FROM creature WHERE id IN (23790, 23999, 24024, 24001, 24442, 24443, 24444, 24441);

-- dodajemy jeñców i invis manow bedacych dummy targetami dla ashli
INSERT INTO creature VALUES
(DEFAULT, 23790, 568, 1, 0, 0, -145.762, 1335.76, 48.174, 6.17758, 237000, 0, 0, 12000, 0, 0, 0),
(DEFAULT, 23999, 568, 1, 0, 0, 406.663, 1504.73, 81.6148, 4.24865, 237000, 0, 0, 11000, 0, 0, 0),
(DEFAULT, 24001, 568, 1, 0, 0, 400.589, 1146.05, 6.34083, 4.94645, 237000, 0, 0, 1, 0, 0, 0),
(DEFAULT, 24024, 568, 1, 0, 0, -72.8802, 1176.6, 5.26225, 1.83072, 237000, 0, 0, 1, 0, 0, 0),
(DEFAULT, 23746, 568, 1, 0, 0, 423.573, 1142.57, 5.51217, 3.24004, 300, 0, 0, 1, 0, 0, 0),
(DEFAULT, 23746, 568, 1, 0, 0, 417.317, 1150.98, 5.26049, 4.8325, 300, 0, 0, 1, 0, 0, 0),
(DEFAULT, 23746, 568, 1, 0, 0, 316.036, 1083.1, 9.77774, 0.222981, 300, 0, 0, 1, 0, 0, 0),
(DEFAULT, 23746, 568, 1, 0, 0, 332.877, 1150.18, 6.34384, 1.62413, 300, 0, 0, 1, 0, 0, 0),
(DEFAULT, 23746, 568, 1, 0, 0, 424.028, 1092.51, 6.35764, 0.0274122, 300, 0, 0, 1, 0, 0, 0);