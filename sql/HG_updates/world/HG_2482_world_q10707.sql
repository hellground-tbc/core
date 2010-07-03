INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES

-- Shadowlord Deathwail fly path
(2095, 1, -3244.6, 283.5, 185, 0, 0, 0, 100, 0),
(2095, 2, -3203, 307, 190, 0, 0, 0, 100, 0),
(2095, 3, -3264, 309, 180, 0, 0, 0, 100, 0),
(2095, 4, -3260, 254, 185, 0, 0, 0, 100, 0),

-- Shadowmoon Retainer left
(2096, 1, -3235, 335, 127.5, 0, 1, 0, 100, 0),
(2096, 2, -3232, 304, 137, 0, 1, 0, 100, 0),
(2096, 3, -3242, 277, 137, 0, 1, 0, 100, 0),
(2096, 4, -3228, 270, 139, 3000, 0, 0, 100, 0), 
(2096, 5, -3242, 277, 137, 0, 0, 0, 100, 0),
(2096, 6, -3256, 268, 137, 0, 0, 0, 100, 0),
(2096, 7, -3265, 279, 137, 0, 0, 0, 100, 0),
(2096, 8, -3271.8, 301.5, 137, 0, 0, 0, 100, 0),
(2096, 9, -3250.6, 301.2, 137, 0, 0, 0, 100, 0),
(2096, 10, -3229, 296.4, 137, 0, 0, 0, 100, 0),

-- Shadowmoon Retainer central
(2097, 1, -3231, 335, 127.5, 0, 1, 0, 100, 0),
(2097, 2, -3230, 302, 137, 0, 1, 0, 100, 0),
(2097, 3, -3243, 279, 137, 0, 1, 0, 100, 0),
(2097, 4, -3230.3, 269.3, 139, 3000, 0, 0, 100, 0),
(2097, 5, -3243, 279, 137, 0, 0, 0, 100, 0),
(2097, 6, -3257, 270, 137, 0, 0, 0, 100, 0),
(2097, 7, -3267, 282, 137, 0, 0, 0, 100, 0),
(2097, 8, -3270.4, 299.3, 137, 0, 0, 0, 100, 0),
(2097, 9, -3253.7, 299, 137, 0, 0, 0, 100, 0),
(2097, 10, -3233.4, 298, 137, 0, 0, 0, 100, 0),

-- Shadowmoon Retainer right
(2098, 1, -3227, 335, 127.5, 0, 1, 0, 100, 0),
(2098, 2, -3228, 300, 137, 0, 1, 0, 100, 0),
(2098, 3, -3244, 281, 137, 0, 1, 0, 100, 0),
(2098, 4, -3233, 267.5, 139, 3000, 0, 0, 100, 0),
(2098, 5, -3244, 281, 137, 0, 0, 0, 100, 0),
(2098, 6, -3258, 272, 137, 0, 0, 0, 100, 0),
(2098, 7, -3269, 285, 137, 0, 0, 0, 100, 0),
(2098, 8, -3268, 295.6, 137, 0, 0, 0, 100, 0),
(2098, 9, -3257.6, 300, 137, 0, 0, 0, 100, 0),
(2098, 10, -3238.4, 300, 137, 0, 0, 0, 100, 0);

UPDATE creature_template SET ScriptName="mob_shadowlord_deathwail" WHERE entry=22006;
UPDATE creature_template SET ScriptName="mob_shadowmoon_soulstealer" WHERE entry=22061;
UPDATE creature_template SET ScriptName="npc_shadowlord_trigger" WHERE entry=22096;

UPDATE creature SET position_x=-3248, position_y=288, position_z=138 WHERE guid=77580;

DELETE FROM spell_script_target WHERE entry=38312;

INSERT INTO spell_script_target (`entry`, `type`, `targetEntry`) VALUES (38312, 1, 22096);

UPDATE creature_template SET faction_A=1813, faction_H=1813, maxdmg = 400, minrangedmg=600, maxrangedmg=800, rangedattackpower=2600, MovementType = 2, ScriptName="mob_eventai" WHERE entry=22102;

UPDATE creature_template SET faction_H = 1813 , faction_A = 1813, type_flags=1024, flags_extra = 128, spell1 = 37999, ScriptName = "felfire_summoner" WHERE entry=22121;

INSERT INTO `eventai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES

-- Shadowmoon Retainer
('','22102','4','0','100','0','0','0','0','0','11','15547','1','0','23','1','0','0','0','0','0','0','Shadowmoon Retainer - Cast Shoot and Set Phase 1 on Aggro'),
('','22102','9','1','100','1','5','30','2500','5900','11','15547','1','0','40','2','0','0','0','0','0','0','Shadowmoon Retainer - Cast Shoot and Set Ranged Weapon Model (Phase 1)'),
('','22102','9','1','100','1','25','80','0','0','21','1','1','0','0','0','0','0','0','0','0','0','Shadowmoon Retainer - Start Combat Movement at 25 Yards (Phase 1)'),
('','22102','9','1','100','1','0','5','0','0','21','1','0','0','40','1','0','0','20','1','0','0','Shadowmoon Retainer - Start Combat Movement and Set Melee Weapon Model Below 5 Yards (Phase 1)'),
('','22102','9','1','100','1','5','15','0','0','21','0','1','0','0','0','0','0','0','0','0','0','Shadowmoon Retainer - Prevent Combat Movement at 15 Yards (Phase 1)'),
('','22102','7','0','100','0','0','0','0','0','22','0','0','0','40','1','0','0','0','0','0','0','Shadowmoon Retainer - Set Phase to 0 and Set Melee Weapon Model on Evade');


INSERT INTO `creature_equip_template` (`entry`,`equipmodel1`,`equipmodel2`,`equipmodel3`,`equipinfo1`,`equipinfo2`,`equipinfo3`,`equipslot1`,`equipslot2`,`equipslot3`) VALUES

-- Shadowmoon Retainer
('944','19552', '0', '36600', '33490690', '0', '50266626', '781', '0', '15'); 

UPDATE creature_template SET equipment_id=944 WHERE entry=22102;