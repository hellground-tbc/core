UPDATE creature_template SET AIName = 'EventAI' WHERE entry=22911;
UPDATE creature_template set ScriptName = 'npc_vim_trigger' where entry = 23081;
INSERT INTO `creature_ai_scripts`(`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES ( '2291101','22911','6','0','100','0','0','0','0','0','11','39862','0','16','0','0','0','0','0','0','0','0','Summon Grimoire book at death');

