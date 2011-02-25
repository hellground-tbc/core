-- Do wrzucenie po dodaniu lootu
UPDATE gameobject_template SET data1 = 22699 WHERE entry = 186648;
UPDATE gameobject_loot_template SET condition_value1 = 30 WHERE entry = 22699;

UPDATE gameobject_template SET data1 = 22790 WHERE entry = 187021;
UPDATE gameobject_loot_template SET condition_value1 = 31 WHERE entry = 22790;

UPDATE gameobject_template SET data1 = 22797 WHERE entry = 186667;
UPDATE gameobject_loot_template SET condition_value1 = 32 WHERE entry = 22797;

UPDATE gameobject_template SET data1 = 22968 WHERE entry = 186672;
UPDATE gameobject_loot_template SET condition_value1 = 33 WHERE entry = 22968;

UPDATE gameobject_loot_template SET groupid = condition_value2 WHERE entry IN (22699, 22790, 22797, 22968);