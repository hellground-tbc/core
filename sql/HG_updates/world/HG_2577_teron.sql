DELETE FROM spell_linked_spell WHERE spell_trigger=-40251;

UPDATE creature_template SET faction_A = 14, faction_H = 14, minlevel=70, maxlevel=70, minhealth=4890, maxhealth=4890, spell1=40325, spell2=40157, spell3=40175, spell4=40314, spell5=40322, unit_flags=256 WHERE entry=23109;
UPDATE creature_template SET faction_A = 1813, faction_H = 1813 WHERE entry=23111;

INSERT INTO spell_linked_spell (spell_trigger, spell_effect, `type`, `comment`) VALUES
(40266, 40270, 0, "Summon Skeleton1"),
(40266, 41948, 0, "Summon Skeleton2"),
(40266, 41949, 0, "Summon Skeleton3"),
(40266, 41950, 0, "Summon Skeleton4");