UPDATE script_texts SET `type` = 6 WHERE entry BETWEEN -1580000 AND -1580017;
UPDATE script_texts SET TYPE = 0 WHERE entry = -1580009;
REPLACE INTO script_texts (entry, content_default, `type`, `comment`)  VALUES 
(-1579990, "drives Kalecgos into a crazed rage!", 3, "sathrovarr EMOTE_KALECGOS_ENRAGE"),
(-1579991, "makes Sathrovarr go into a crazed rage!", 3, "kalecgos EMOTE_SATHROVARR_ENRAGE");
REPLACE INTO script_texts (entry, content_default, sound, `type`, `comment`)  VALUES
(-1579992, "There is no time to waste.", 12432, 0, "dragonKALECGOS_GREETING01"),
(-1579993, "Kil'Jaeden must be destroyed.", 12433, 0, "dragonKALECGOS_GREETING02"),
(-1579994, "The fate, of the world, hangs in the balance.", 12434, 0, "dragonKALECGOS_GREETING03"),
(-1579995, "There is no time to waste.", 12435, 0, "humanKALECGOS_GREETING04"),
(-1579996, "Kil'Jaeden must be destroyed.", 12436, 0, "humanKALECGOS_GREETING05"),
(-1579997, "The fate, of the world, hangs in the balance.", 12437, 0, "humanKALECGOS_GREETING06");