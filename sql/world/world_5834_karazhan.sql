
UPDATE creature_template SET AIName = "", ScriptName = "mob_spectral_sentry" WHERE entry = 16424;
UPDATE creature_template SET AIName = "", ScriptName = "mob_arcane_protector" WHERE entry = 16504;
UPDATE creature_template SET AIName = "", ScriptName = "mob_mana_warp" WHERE entry = 16530;
UPDATE creature_template SET AIName = "", ScriptName = "mob_shadow_pillager" WHERE entry = 16540;
UPDATE creature_template SET speed = 1.125 WHERE entry = 16470;
UPDATE creature_addon SET emote = 173 WHERE guid IN(392058, 391887, 85377, 312876);  -- prawid³owy emote work no sheathe

UPDATE creature_ai_scripts SET event_type = 1, event_flags = 2, event_chance = 100, event_param1 = 0, event_param2 = 0, event_param3 = 0, event_param4 = 0, action1_param1 = 0 WHERE id = 1649101;

UPDATE creature_ai_scripts SET event_param1 = 2000, event_param2 = 10000, event_param3 = 5000, event_param4 = 15000 WHERE entryOrGUID = 16525;
UPDATE creature_ai_scripts SET event_chance = 100 WHERE entryOrGUID IN (16525, 16529);
UPDATE creature_ai_scripts SET event_type = 1, event_flags = 2,event_param1 = 0, event_param2 = 0, event_param3 = 0, event_param4 = 0 WHERE id = 1652504;

UPDATE creature_ai_scripts SET action1_param2 = 0 WHERE entryOrGUID = 16529;

INSERT INTO scripted_spell_id VALUES (30004, "spell_flame_wreath");