-- data for Emissary of Hate
REPLACE INTO creature VALUES (200125, 25003, 530, 1, 0, 0, 12587.1, -6916.6, 4.60115, 6.25981, 120, 10, 0, 7080, 3309, 0, 1);

-- cosmetics and better mechanic for q 'Blood for Blood'
UPDATE quest_template SET ReqSpellCast1 = 0 WHERE entry = 11515;
UPDATE item_template SET ScriptName = "item_specific_target" WHERE entry = 34257;
UPDATE creature_template SET minhealth = 16767, maxhealth = 16767, minmana = 9465, maxmana = 9465, rank = 1, mindmg = 700, maxdmg = 2000 WHERE entry = 24918;
UPDATE creature SET curhealth = 16767, curmana = 9465 WHERE id = 24918;
UPDATE creature_ai_scripts SET event_param1 = 44936 WHERE id = 2491804;

REPLACE INTO `creature_ai_texts` (entry, content_default, `type`, `comment`) VALUES 
(-1001, "Your life force is my nourishment, demon... Kil'jaeden's gift to us!", 1, "Felblood Initiate random Yell1"),
(-1002, "Unparalleled power... I... crave... more!", 1, "Felblood Initiate random Yell2"),
(-1003, "I will soon be stronger than any elf! I will serve at Kil'jaeden's side!", 1, "Felblood Initiate random Yell3"),
(-1004, "More... more... MORE!!!", 1, "Felblood Initiate random Yell4"),
(-1005, "My power... is gone!", 1, "Felblood Initiate template change Yell1"),
(-1006, "No... no... NO!!!!!", 1, "Felblood Initiate template change Yell2");

REPLACE INTO creature_ai_scripts VALUES
(2491805, 24918, 1, 0, 12, 1, 3000, 120000, 10000, 120000, 1, -1001, -1002, -1003, 0, 0, 0, 0, 0, 0, 0, 0, "Felblood Initiate - Random yell when OOC1"),
(2491806, 24918, 1, 0, 3, 1, 10000, 40000, 20000, 80000, 1, -1004, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Felblood Initiate - Random yell when OOC1"),
(2491807, 24918, 8, 0, 100, 0, 44936, -1, 0, 0, 1, -1005, -1006, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Felblood Initiate - Yell when changes to Emaciated Felblood");