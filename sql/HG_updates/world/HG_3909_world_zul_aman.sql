-- Ustawiamy odpowiedni¹ frakcje dla addów hexa
UPDATE creature_template SET faction_H = 1890, faction_A = 1890 WHERE entry >= 24240 AND entry <= 24247;

UPDATE script_texts SET content_default = "I fight wit' untamed spirit..." WHERE entry = -1800487;
UPDATE script_texts SET sound = 12023 WHERE entry = -1800486;
UPDATE script_texts SET sound = 12024 WHERE entry = -1800485;

INSERT INTO script_texts VALUES
(-1800510, "Come, fools. Fill ma empty cages...",0, 0, 0, 0, 0, 0, 0, 0, 12029, 1, 0, 0, "Halazzi YELL_INTRO1"),
(-1800511, "I be waitin, strangers. Your deaths gonna make me stronger!", 0, 0, 0, 0, 0, 0, 0, 0, 12030, 1, 0, 0, "Halazzi YELL_INTRO2"),
(-1800512, "Da shadow gonna fall on you...", 0, 0, 0, 0, 0, 0, 0, 0, 12041, 1, 0, 0, "Hexlord YELL_AGGRO"),
(-1800513, "Ya don't kill me yet, ya don't get another chance!", 0, 0, 0, 0, 0, 0, 0, 0, 12042, 1, 0, 0, "Hexlord YELL_ENRAGE"),
(-1800514, "Your will belong ta me now!", 0, 0, 0, 0, 0, 0, 0, 0, 12045, 1, 0, 0, "Hexlord YELL_SIPHON_SOUL"),
(-1800515, "It not gonna make no difference.", 0, 0, 0, 0, 0, 0, 0, 0, 12048, 1, 0, 0, "Hexlord YELL_ADD_DEAD1"),
(-1800516, "You gonna die worse dan him.", 0, 0, 0, 0, 0, 0, 0, 0, 12049, 1, 0, 0, "Hexlord YELL_ADD_DEAD2"),
(-1800517, "Dat no bodda me.", 0, 0, 0, 0, 0, 0, 0, 0, 12050, 1, 0, 0, "Hexlord YELL_ADD_DEAD3");

INSERT INTO script_texts VALUES
( -1800510, 
"Wowee! It was amazing how you beat that big bad troll into the ground like that and it's too bad I can't reach the handle of this cage or I would have jumped in and blown him to bits to get back for killing all of the other poor prisoners and scaring me so badly and I feel bad for asking any more help but could you maybe come over and let me out of here?",
0, 0, 0, 0, 0, 0,0, 0,0, 1, 0,0, "Ashli - YELL_ASHLI");