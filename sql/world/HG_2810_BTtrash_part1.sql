-- Merge script target (still to be fixed some way)
UPDATE spell_script_target SET targetEntry=22878 WHERE targetEntry=22883;
-- Free Friend yell
REPLACE INTO script_texts (entry, content_default, sound, `type`, `language`, emote, `comment`) VALUES
(-2287300, "Return to your original form, Coilskar Soothsayer!", 0, 1, 0, 0, "Coilskar General - Free Friend emote");

-- Najentus
UPDATE creature_template SET ScriptName = "mob_aqueous_lord" WHERE entry=22878;
UPDATE creature_template SET ScriptName = "mob_aqueous_spawn" WHERE entry=22883;
UPDATE creature_template SET ScriptName = "mob_coilskar_general" WHERE entry=22873;
-- Supremus
UPDATE creature_template SET ScriptName="mob_bonechewer_taskmaster" WHERE entry=23028;
UPDATE creature_template SET ScriptName="mob_bonechewer_worker" WHERE entry=22963;
UPDATE creature_template SET ScriptName="mob_dragonmaw_skystalker" WHERE entry=23030;
UPDATE creature_template SET ScriptName="mob_dragonmaw_windreaver" WHERE entry=23330;
UPDATE creature_template SET ScriptName="mob_dragonmaw_wyrmcaller" WHERE entry=22960;
UPDATE creature_template SET ScriptName="mob_illidari_fearbringer" WHERE entry=22954;