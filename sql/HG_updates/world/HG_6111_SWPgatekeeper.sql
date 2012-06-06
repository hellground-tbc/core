DELETE FROM creature WHERE guid = 63716;
UPDATE creature_template SET type_flags = 5242886, ScriptName = "npc_SWP_gatekeeper" WHERE entry = 25848;

DELETE FROM script_texts WHERE entry = -1811007;
INSERT INTO script_texts (entry, content_default, `type`, `comment`) VALUES
(-1811007, "You have been tagged for extermination by Dark Forces of the Legion!", 1, "SWP gatekeeper, EMOTE_WARNING_YELL");