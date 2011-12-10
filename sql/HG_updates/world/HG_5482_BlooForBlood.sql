UPDATE spell_script_target SET targetEntry = 24933 WHERE entry = 44864;
DELETE FROM creature_ai_scripts WHERE entryorguid = 24918;
DELETE FROM creature_ai_texts WHERE entry BETWEEN -1006 AND -1001;
UPDATE creature_template SET ScriptName = "npc_emaciated_felblood" WHERE entry = 24955;
UPDATE creature_template SET ScriptName = "npc_felblood_initiate" WHERE entry = 24918;