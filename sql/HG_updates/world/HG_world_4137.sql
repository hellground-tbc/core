DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 19397;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 19398;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 19399;

UPDATE `creature_template` SET `ScriptName` = 'npc_abyssal_shelf_quest' WHERE `creature_template`.`entry` = 19397;
UPDATE `creature_template` SET `ScriptName` = 'npc_abyssal_shelf_quest' WHERE `creature_template`.`entry` = 19398;
UPDATE `creature_template` SET `ScriptName` = 'npc_abyssal_shelf_quest' WHERE `creature_template`.`entry` = 19399;

DELETE FROM `creature_template` WHERE `creature_template`.`entry`=22423;

INSERT INTO `creature_template` (`entry`, `heroic_entry`, `KillCredit`, `modelid_A`, `modelid_A2`, `modelid_H`, `modelid_H2`, `name`, `subname`, `IconName`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `baseattacktime`, `rangeattacktime`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `class`, `race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `RacialLeader`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(22423, 0, NULL, 20991, 0, 20991, 0, 'Evergrove Druid', '', NULL, 70, 70, 5000, 5000, 0, 0, 0, 35, 35, 2, 1.2, 1, 0, 1, 1, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, '', 0, 3, 0, 1, 0, 0, 2, '');

