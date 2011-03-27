DELETE FROM `areatrigger_scripts` WHERE `entry`=4017;
INSERT INTO `areatrigger_scripts` (`entry`,`ScriptName`) VALUES (4017,'at_twilight_grove');
UPDATE `creature_template` SET `ScriptName`='boss_twilight_corrupter' WHERE `entry`=15625;