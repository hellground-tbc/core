REPLACE INTO spell_script_target VALUES (20465, 1, 12580);
UPDATE creature_template SET ScriptName = 'npc_reginald_windsor' WHERE entry = 12580;
UPDATE creature_template SET ScriptName = 'npc_squire_rowe' WHERE entry = 17804;
UPDATE creature_template SET ScriptName = 'npc_stormwind_elite_guard', minlevel = 70, maxlevel = 70, minhealth = 15000, maxhealth = 15000 WHERE entry 
= 16396;
UPDATE creature_template SET npcflag = npcflag | 268435456 WHERE entry = 16396;
REPLACE INTO creature_template_addon VALUES (12580, 0, 2410, 0, 0, 0, 0, 0, NULL);
REPLACE INTO spell_script_target VALUES (20358, 1, 1749);
