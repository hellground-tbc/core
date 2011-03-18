UPDATE creature 
SET position_x = 18.569, position_y = 1414.512, position_z = 11.42, orientation = 0
WHERE id = 23576;

DELETE FROM creature WHERE id = 23790;

UPDATE creature_template SET faction_H = 1049, faction_A = 1049 WHERE entry IN (24175, 24180, 24549, 24225, 24159);
UPDATE creature_template SET minhealth = 17116, maxhealth = 17116, ScriptName = "npc_amanishi_warrior" WHERE entry = 24225;

UPDATE creature_template SET minhealth = 4890, maxhealth = 4890, ScriptName = "npc_amani_eagle",
 `mindmg` = 750, 
 `maxdmg` = 1500, 
 `attackpower` = ROUND((`mindmg` + `maxdmg`) / 9564), 
 `mindmg` = ROUND(`mindmg` - `attackpower` / 9564), 
 `maxdmg` = ROUND(`maxdmg` - `attackpower` / 9564) 
WHERE entry = 24159;

UPDATE creature_template SET ScriptName = "npc_amanishi_lookout", Speed = 4 WHERE entry = 24175;
UPDATE creature_template SET AIName = "EventAI" WHERE entry IN (24549, 24180);



INSERT INTO creature VALUES
(DEFAULT, 24175, 568, 1, 0, 0, 208.943, 1466.58, 25.9168, 3.83307, 1800, 0, 0, 14000, 0, 0, 0),
(DEFAULT, 24180, 568, 1, 0, 0, 224.54, 1427.66, 28.7058, 1.6481, 1800, 0, 0, 71000, 0, 0, 0),
(DEFAULT, 24180, 568, 1, 0, 0, 224.08, 1389.13, 41.9104, 1.65987, 1800, 0, 0, 71000, 0, 0, 0),
(DEFAULT, 24180, 568, 1, 0, 0, 246.103, 1368.09, 49.1573, 2.8309, 1800, 0, 0, 71000, 0, 0, 0),
(DEFAULT, 24180, 568, 1, 0, 0, 285.208, 1374.22, 49.3217, 3.40582, 1800, 0, 0, 71000, 0, 0, 0),
(DEFAULT, 24549, 568, 1, 0, 0, 302.33, 1385.29, 57.4664, 3.5178, 1800, 0, 0, 140000, 0, 0, 0),
(DEFAULT, 24179, 568, 1, 0, 2179, 282.614, 1383.56, 49.3217, 3.40079, 1800, 0, 0, 57000, 32310, 0, 0),
(DEFAULT, 24179, 568, 1, 0, 2179, 248.805, 1377.08, 49.3205, 2.85415, 1800, 0, 0, 57000, 32310, 0, 0),
(DEFAULT, 24179, 568, 1, 22262, 2179, 232.734, 1388.62, 42.659, 1.55509, 1800, 0, 0, 57000, 32310, 0, 0),
(DEFAULT, 24179, 568, 1, 0, 2179, 232.856, 1427.8, 29.0465, 1.57787, 1800, 0, 0, 57000, 32310, 0, 0);

INSERT INTO creature_ai_scripts VALUES
(2417904, 24179, 4, 0, 100, 2, 0, 0, 0, 0,
34, 24, 10, 0,
0, 0, 0, 0, 
0, 0, 0, 0,
"Amani'shi Wind Walker - Enter Combat"),

(2454901, 24549, 4, 0, 100, 2, 0, 0, 0, 0,
34, 24, 11, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Tempest - Enter Combat"),

(2418001, 24180, 4, 0, 100, 2, 0, 0, 0, 0,
34, 24, 10, 0,
0, 0, 0, 0, 
0, 0, 0, 0,
"Amani'shi Protector - Enter Combat"),

(2418002, 24180, 0, 0, 100, 3, 4000, 6000, 15000, 20000,
11, 15496, 1, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Protector - Cast Cleave"),

(2418003, 24180, 0, 0, 100, 3, 3000, 5000, 10000, 15000,
11, 43529, 1, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Protector - Cast Mortal Strike"),

(2418004, 24180, 0, 0, 100, 3, 15000, 20000, 30000, 40000,
11, 43530, 1, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Protector - Cast Piercing Howl"),

(2454902, 24549, 0, 0, 100, 3, 2000, 6000, 8000, 12000,
11, 44033, 1, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Tempest - Cast Thunder Clap"),

(2454903, 24549, 6, 0, 100, 2, 0, 0, 0, 0,
34, 24, 12, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Amani'shi Tempest - Just Died");