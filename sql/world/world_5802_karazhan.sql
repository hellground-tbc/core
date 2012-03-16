INSERT INTO npc_text(id, text0_0) VALUES
(25035, "Greetings, travelers. I have been instructed to treat any bearer of a key to Karazhan as an honoured guest of the Master. Feel free to ask any questions you wish of me. How may I assist you?"),
(25036, "This is the tower of Karazhan and residence of my master, Medivh. As a humble doorman, there is a little else I can explain to you."),
(25037, "All inquiries regarding Medivh should be directed to Moroes, the Tower Steward. I myself know very little about the Master's business. I believe several nobles are currently having an audience with Moroes in the Banquet Hall, if you wish to inquire further."),
(25038, "From here in the Gatehouse, you may travel in one of three directions. The stairs behind me lead to the Grand Ball Room, where guests are currently being entertained. I'm sure you can smell the nearby Stables and the entrance to Servant's Quarters lies next to the Gatehouse door."),
(25039, "Damn all these vermin! I don't suppose you could help me with this situation?"),
(25040, "The damn spiders, the endlessly screeching bats, and those blasted demon beasts have all taken residence in the Servant's Quarters. It's not safe to walk around anymore, especially when the big ones come out of hiding."),
(25041, "Oh yeah. I've never seen them personally, but sometimes I hear them in the distance. Keep a watchful eye out for them if you venture inside."),
(25042, "Ah, guests of the Master. I hope you've been watching your step in here."),
(25043, "Midnight is Attumen the Huntsman's personal mount. I've never seen a faster, stronger, or more mean spirited horse. And her rider is even more temperamental! Everyone keeps their distance from her, lest they get a hoof upside the head from Midnight or a beating from Attumen."),
(25044, "Aran has been defeated? Yes... I can sense his grip has loosened - if only just a little. But just enough to allow me to do my job as the doorman! Would you like me to transport you to the Guardian's Library, $n?"),
(25045, "I don't have time for idle chitchat. I have men and women to train, and a tower to watch."),
(25046, "This party is pointless. Those foolish nobles will never get to see Medivh. Can't they see this? I should have just stayed in Darkshire with my family instead of wasting my time here.");

INSERT INTO npc_gossip VALUES
(57036, 25045),      -- Bennett
(57852, 25046);      -- Ebonlocke

UPDATE creature_template SET scriptName = "npc_hastings" WHERE entry = 16169;
UPDATE creature_template SET scriptName = "npc_calliard" WHERE entry = 16159;


INSERT INTO creature_addon VALUES
-- emote work (mobki w stajniach)
(85377, 0, 0, 0, 0, 0, 28, 0, NULL),
(312876, 0, 0, 0, 0, 0, 28, 0, NULL),
-- emote attack (mobki w livery stables)
(368275, 0, 0, 0, 0, 0, 36, 0, NULL),
(361773, 0, 0, 0, 0, 0, 36, 0, NULL),
(362303, 0, 0, 0, 0, 0, 36, 0, NULL),
(368393, 0, 0, 0, 0, 0, 36, 0, NULL),
(362544, 0, 0, 0, 0, 0, 36, 0, NULL),
(368129, 0, 0, 0, 0, 0, 36, 0, NULL),
(362730, 0, 0, 0, 0, 0, 36, 0, NULL),
(367972, 0, 0, 0, 0, 0, 36, 0, NULL),
(363280, 0, 0, 0, 0, 0, 36, 0, NULL),
(367568, 0, 0, 0, 0, 0, 36, 0, NULL),
(367717, 0, 0, 0, 0, 0, 36, 0, NULL),
(363149, 0, 0, 0, 0, 0, 36, 0, NULL),
-- dancing guests in ballroom
(85134, 0, 0, 0, 0, 0, 10, 0, NULL),
(85126, 0, 0, 0, 0, 0, 10, 0, NULL),
(85128, 0, 0, 0, 0, 0, 10, 0, NULL),
(85129, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85130, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85131, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85132, 0, 0, 0, 0, 0, 10, 0, NULL),
(85116, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85120, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85117, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85110, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85119, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85112, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85113, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85114, 0, 0, 0, 0, 0, 10, 0, NULL),
(85104, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85103, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85105, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85101, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85107, 0, 0, 0, 0, 0, 10, 0, NULL), 
(85100 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85122 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85108, 0, 0, 0, 0, 0, 10, 0, NULL),
(85176 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85175 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85097 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85096 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85003 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85009 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85005 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85042, 0, 0, 0, 0, 0, 10, 0, NULL),
(85123 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85093 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85095 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85091 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85090 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85089 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85177 , 0, 0, 0, 0, 0, 10, 0, NULL),
(85099, 0, 0, 0, 0, 0, 10, 0, NULL);