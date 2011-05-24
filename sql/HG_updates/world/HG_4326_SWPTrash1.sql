UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_arch_mage" WHERE entry = 25367;
UPDATE creature_template SET mindmg = 1000, maxdmg = 2500, mechanic_immune_mask = 133121, AIName = "", ScriptName = "mob_sunblade_cabalist" WHERE entry = 25363;
UPDATE creature_template SET mindmg = 1300, maxdmg = 3500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_dawn_priest" WHERE entry = 25371;
UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 134145, AIName = "", minhealth = 190000, maxhealth = 190000, ScriptName = "mob_sunblade_dusk_priest" WHERE entry = 25370;
UPDATE creature_template SET mindmg = 5500, maxdmg = 17000, mechanic_immune_mask = 23553, AIName = "", ScriptName = "mob_sunblade_protector" WHERE entry = 25507;
UPDATE creature_template SET mechanic_immune_mask = 537083899, AIName = "", ScriptName = "mob_sunblade_scout" WHERE entry = 25372;
UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_slayer" WHERE entry = 25368;
UPDATE creature_template SET mindmg = 3800, maxdmg = 15000, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_vindicator" WHERE entry = 25369;
UPDATE creature_template SET faction_A = 16, faction_H = 16 WHERE entry IN (25867, 26101);
DELETE FROM creature_ai_scripts WHERE entryOrGUID = 25867;
REPLACE INTO creature_ai_scripts VALUES
(2610101, 26101, 0, 0, 100, 3, 2000, 3000, 9000, 10000, 11, 46551, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Fire Fiend - Cast Fire Nova"),
(2586701, 25867, 0, 0, 100, 3, 2500, 5500, 10000, 15000, 11, 47251, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Sunblade Dragonhawk - Cast Flame Breath");
UPDATE creature_template SET AIName = "EventAI" WHERE entry = 26101;