UPDATE creature_template SET ScriptName="npc_medivh_bm" WHERE entry=15608;
UPDATE creature_template SET faction_A=168, faction_H = 168 WHERE entry IN (18994, 17892, 18995);
DELETE FROM creature WHERE id IN (17881, 17880, 17879, 21104, 17839, 21136, 21818, 21137, 17838);