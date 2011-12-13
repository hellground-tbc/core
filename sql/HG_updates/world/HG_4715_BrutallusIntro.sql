UPDATE creature_template SET mindmg = 10000, maxdmg = 40000, minmana = 3387, maxmana = 3387, attackpower = 10000, baseattacktime = 2000, InhabitType = 1 WHERE entry = 24895;
DELETE FROM spell_linked_spell WHERE spell_effect = 45665;
REPLACE INTO spell_linked_spell VALUES (45661, 44883, 1, "Encapsulate");
REPLACE INTO spell_script_target VALUES (44884, 1, 24895);