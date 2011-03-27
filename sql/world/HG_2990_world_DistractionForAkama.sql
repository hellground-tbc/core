UPDATE creature_template SET minlevel = 72, maxlevel = 72, minhealth = 324665, maxhealth = 324665, minmana =66180, maxmana = 66180, armor = 3596, faction_A = 1858, faction_H = 1858, mindmg = 4120, maxdmg = 7300, attackpower = 112000, baseattacktime = 1800, equipment_id = 2182, ScriptName = "npc_maiev_BT_attu" WHERE entry=22989;
UPDATE creature_template SET ScriptName = "npc_ashtongue_deathsworn" WHERE entry=21701;
UPDATE creature_template SET ScriptName = "npc_xiri", faction_A = 1858, faction_H = 1858 WHERE entry=18528;
UPDATE creature_template SET minlevel = 72, maxlevel = 72, minhealth = 147600, maxhealth = 147600, armor = 2604, faction_A = 1843, faction_H = 1843, mindmg = 2010, maxdmg = 5600, attackpower = 95000, baseattacktime = 1200,ScriptName = "mob_vagath" WHERE entry=23152;
UPDATE creature_template SET minlevel = 71, maxlevel = 71, minhealth = 6661, maxhealth = 6661, armor = 1004, faction_A = 1843, faction_H = 1843, mindmg = 260, maxdmg = 700, attackpower = 30000, baseattacktime = 1500 WHERE entry=23044;
UPDATE creature_template SET ScriptName = "mob_illidari_shadowlord" WHERE entry=22988;
UPDATE creature_template SET ScriptName = "npc_akama_shade" WHERE entry=23191;
UPDATE creature SET id=23191 WHERE id=22990;
REPLACE INTO spell_script_target (entry, `type`, targetEntry) VALUES
(39831, 1, 21701),
(39831, 1, 22990),
(39831, 1, 22989),
(39831, 1, 22861),
(39831, 1, 22862),
(39831, 1, 22863);
REPLACE INTO spell_script_target (entry, `type`, targetEntry) VALUES
(39832, 1, 21701),
(39832, 1, 22990),
(39832, 1, 22989),
(39832, 1, 22861),
(39832, 1, 22862),
(39832, 1, 22863);
REPLACE INTO spell_linked_spell (spell_trigger, spell_effect, `type`, `comment`) VALUES
(39828, 39831, 0, "Light of the Naaru");
(-39828, 39831, 0, "Light of the Naaru");
REPLACE INTO spell_linked_spell (spell_trigger, spell_effect, `type`, `comment`) VALUES
(39828, 39832, 0, "Light of the Naaru2");
(-39828, 39832, 0, "Light of the Naaru2");