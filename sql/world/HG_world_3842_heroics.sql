-- capacitus instance in combat
UPDATE creature_ai_scripts
SET action2_type = 34, action2_param1 = 5, action2_param2 = 1
WHERE id = 1921901;

UPDATE creature_ai_scripts
SET action2_type = 34, action2_param1 = 5, action2_param2 = 3
WHERE id = 1921903;

INSERT INTO creature_ai_scripts VALUES
(1921910, 19219, 7, 0, 100, 6, 0, 0, 0, 0, 34, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Capacitus - Evade')


-- addy black stalkera i tak wystêpuja tylko ne heroiku
UPDATE creature_template SET heroic_entry = 0 WHERE entry = 22299;
DELETE FROM creature_template WHERE entry = 22300;

-- cache of the legion skrypt (zeby bojki byly)

UPDATE gameobject_template SET ScriptName = 'go_cache_of_the_legion' WHERE entry = 184465;
