UPDATE `creature_template` SET `ScriptName`="npc_exarch_admetius" WHERE `entry`=17658;
UPDATE `quest_template` SET `SuggestedPlayers`="1" WHERE `entry`=9756;
UPDATE `quest_template` SET `SrcSpell`="0" WHERE `entry`=9756;
UPDATE `quest_template` SET `CompleteScript`="9756" WHERE `entry`=9756;
INSERT quest_end_scripts(id,delay,command,datalong,datalong2,dataint,x,y,z,o) VALUES (9756,0,14,31609,0,0,0,0,0,0)