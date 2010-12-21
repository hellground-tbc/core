UPDATE creature_template SET ScriptName = "mob_ashtongue_mystic" WHERE entry = 22845;
UPDATE creature_template SET ScriptName = "totem_ashtongue_mystic" WHERE entry IN (22894, 22896, 22897);
UPDATE creature_template SET ScriptName = "mob_ashtongue_stalker" WHERE entry = 23374;
UPDATE creature_template SET minlevel = 70, maxlevel = 71, minhealth = 4800, maxhealth = 4900, faction_A = 1813, faction_H = 1813, speed = 0  WHERE entry=22897;
UPDATE creature_template SET minlevel = 71, maxlevel = 71, minhealth = 4100, maxhealth = 4200, faction_A = 1813, faction_H = 1813, speed = 0  WHERE entry=22894;
UPDATE creature_template SET minlevel = 71, maxlevel = 71, minhealth = 1800, maxhealth = 1900, faction_A = 1813, faction_H = 1813, speed = 0  WHERE entry=22896;

