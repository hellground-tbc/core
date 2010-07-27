UPDATE `creature_template` SET `ScriptName` = 'mob_boom_bot' WHERE `entry` = 19692;
UPDATE `creature_template` SET `ScriptName` = 'mob_dr_boom', `baseattacktime` = 3000 WHERE `entry` = 20284;

DELETE FROM `creature` where `id` = 19692;