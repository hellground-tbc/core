update creature_template set faction_A = 54, faction_H = 54, scriptname='boss_theldren' where entry = 16059;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_malgen_longspear' where entry = 16052;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_lefty' where entry = 16049;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_rotfang' where entry = 16050;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_vajashni' where entry = 16055;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_volida' where entry = 16058;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_snokh' where entry = 16051;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_korv' where entry = 16053;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0, scriptname='boss_rezznik' where entry = 16054;
update creature_template set faction_A = 54, faction_H = 54, lootid = 0 where entry = 16095;

DELETE FROM `creature_loot_template` where entry in(16052, 16049, 16050, 16055, 16058, 16051, 16053, 16054, 16095);

UPDATE `item_template` SET `ScriptName`='item_banner_of_provocation' WHERE (`entry`='21986');

INSERT INTO `gameobject_template` (entry,type,displayId,name,flags,size,data0,data1,data15) values(211085,3,10,'Arena Spoils', 64, 1, 43, 24301, 1);
INSERT INTO `gameobject_loot_template` (entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount) values (24301,22330,50,1,1,1),(24301,22317,26,1,1,1),(24301,22318,16,1,1,1),(24301,22305,8,1,1,1);

UPDATE `quest_template` SET `ReqCreatureOrGOId1`='16059' WHERE (`entry`='9015');