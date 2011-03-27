-- fix for DB errors in Shade of Akama script, this should also be verified in details on script site
INSERT INTO spell_script_target (entry, `type`, targetEntry) VALUES (40520, 1 , 22841);

-- increase melee dmg for shadowy constructs
UPDATE creature_template SET mindmg = 320, maxdmg = 450 WHERE entry = 23111;