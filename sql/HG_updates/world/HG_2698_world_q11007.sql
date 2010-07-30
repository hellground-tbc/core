UPDATE creature_template SET unit_flags = 0x2000002, scriptName = 'npc_kaelthas_image', InhabitType=3 WHERE entry = 23054;
UPDATE creature_template SET scriptName='npc_adal' WHERE entry = 18481;
INSERT INTO script_texts VALUES (-1645011, 'Silence descends upon Shattrath', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 2, 0, 0, 'adal emote');
INSERT INTO script_texts VALUES (-1645012, "A'dal's thoughts invade your mind", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 2, 0, 0, 'adal emote');