UPDATE creature SET spawntimesecs = 20 WHERE id IN (23399, 23401, 23398);
UPDATE creature_template SET mindmg = 2555, maxdmg = 4301, ScriptName = "mob_suffering_soul_fragment" WHERE entry = 23399;
UPDATE creature_template SET ScriptName = "mob_hungering_soul_fragment" WHERE entry = 23401;
UPDATE creature_template SET ScriptName = "mob_angered_soul_fragment" WHERE entry = 23398;