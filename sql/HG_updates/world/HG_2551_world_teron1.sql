INSERT INTO spell_script_target (entry, `type`, targetEntry) VALUES (40268, 1, 23109);
INSERT INTO spell_linked_spell (spell_trigger, spell_effect, `type`, `comment`) VALUES
(-40251, 40266, 0, "Shadow of Death");
UPDATE creature_template SET faction_A = minlevel=70, maxlevel=70, minhealth=4890, maxhealth=4890, unit_flags=256 WHERE entry=23109;