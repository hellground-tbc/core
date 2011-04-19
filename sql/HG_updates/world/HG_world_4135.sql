REPLACE INTO spell_script_target VALUES (44877, 1, 24916);
UPDATE creature_template SET ScriptName = "npc_living_flare" WHERE entry IN (24916, 24958);
UPDATE creature_template SET scale = 5 WHERE entry = 24958;
UPDATE creature_template SET AIName = "EventAI" WHERE entry = 22323;
DELETE FROM gameobject WHERE id = 187084 AND map = 530;
-- Unstable Living Flare explode emote
REPLACE INTO script_texts VALUES
(-1811010, "releases its energy, engulfing its surroundings in flames!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, "UNSTABLE_LIVING_FLARE_EXPLODE_EMOTE");

-- AI for Incandescent Fel Spark
REPLACE INTO creature_ai_scripts VALUES
(2232301, 22323, 1, 0, 100, 1, 1000, 1000, 540000, 540000, 11, 38901, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Incandescent Fel Spark - Fire Shield buff each 9 min"),
(2232302, 22323, 0, 0, 100, 1, 1000, 3000, 9000, 18000, 11, 36247, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Incandescent Fel Spark - Fel Fireball"),
(2232303, 22323, 2, 0, 30, 0, 35, 25, 0, 0, 11, 39055, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Incandescent Fel Spark - Flames of Chaos"),
(2232304, 22323, 6, 0, 100, 0, 0, 0, 0, 0, 11, 44877, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Incandescent Fel Spark - Living Flare Master cast on Death for quest");