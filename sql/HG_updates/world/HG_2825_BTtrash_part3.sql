-- Set 20 min respawn time for Aqueous Spawn
UPDATE creature SET spawntimesecs = 1200 WHERE id=22883;

-- Script Names
UPDATE creature_template SET ScriptName="mob_coilskar_soothsayer" WHERE entry=22876;
UPDATE creature_template SET ScriptName="mob_coilskar_seacaller" WHERE entry=22875;
UPDATE creature_template SET ScriptName="mob_coilskar_geyser" WHERE entry=23080;