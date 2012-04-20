UPDATE creature_template SET `name` = "Opera Event Trigger" WHERE entry = 18654;

INSERT INTO scripted_spell_id VALUES
(29320, "spell_charge_29320");

UPDATE creature_template SET AIName = "", ScriptName = "mob_phantom_guest" WHERE entry = 16409;

UPDATE creature_ai_scritps
SET event_chance = 100, event_param1 = 0, event_param2 = 0, event_param3 = 2000, event_param4 = 2000
WHERE id = 1642403;

INSERT INTO creature_ai_scripts VALUES
(1642404, 16424, 4, 0, 100, 2, 0, 0, 0, 0,
29, 30, 0, 0,
0, 0, 0, 0,
0, 0, 0, 0,
"Spectral Sentry - Ranged Movement");

UPDATE creature_template_addon SET auras = "31526 0" WHERE entry = 16170;

INSERT INTO npc_text(id, text0_0) VALUES
(25047, "Greetings! Behold one of the finest musical instruments in all Lordaeron. It's a a true honor to perform for the Master and all his guests. Take a seat in the audience. I hear a show is starting soon."),
(25048, "Your presence here is interfering with very important work. Please leave at once. Whare was I, again?"),
(25049, "It's here... somewhere... I know it. Somewhere in this library is knowledge I'm looking for..."),
(25050, "This tower is complete and utter madness. This isn't a library, it's a huge haystack of books and I don't think the needle even exists! Now, what was I doing?");

INSERT INTO npc_gossip VALUES
(57874, 25047); -- sebastian