UPDATE creature_template SET equipment_id = 523 WHERE entry = 15551;  -- pitchforks!
UPDATE creature_template SET equipment_id = 1044 WHERE entry = 16389;  -- hammertime
UPDATE creature_template SET equipment_id = 16411 WHERE entry = 16411;  -- tasak
UPDATE creature_template SET equipment_id = 16409 WHERE entry = 16409;  -- throwing knives

INSERT INTO creature_equip_template VALUES
(16411, 8482, 0, 0, 50267138, 0, 0, 781, 0, 0),
(16409, 0, 0, 40187, 0, 0, 33493506, 0, 0, 26);


-- emote work - kucharze
INSERT INTO creature_addon VALUES
(392058, 0, 0, 0, 0, 0, 28, 0, NULL),
(391887, 0, 0, 0, 0, 0, 28, 0, NULL);

-- mage, warlock, conjurer in library
INSERT INTO npc_gossip VALUES
(46851, 25048),
(46857, 25049),
(46858, 25050);