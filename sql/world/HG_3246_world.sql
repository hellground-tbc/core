UPDATE `creature` SET `spawntimesecs`=140 WHERE `id`=17375;
UPDATE `creature_template` SET `ScriptName`="npc_stillpine_capitive" WHERE `entry`=17375;
DELETE FROM `gameobject` WHERE guid IN (3691506);
INSERT INTO gameobject(guid, id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) VALUE
(42103, 181714, 530, 1, -4580.97, -11644.1, 19.5813, 4.35313, 0, 0, 1, 0, 121, 100, 1);
UPDATE `gameobject_template` SET `data2`=1966080, `ScriptName`="go_bristlelimb_cage" WHERE `entry`=181714;
UPDATE `creature` SET `spawntimesecs`=200 WHERE `id`=17682;
UPDATE `creature_template` SET `ScriptName`="npc_princess_stillpine" WHERE `entry`=17682;
UPDATE `gameobject_template` SET `data2`=1966080, `ScriptName`="go_princess_stillpine_cage" WHERE `entry`=181928;
INSERT INTO script_texts(`entry`, `content_default`, `type`, `language`, `comment`) VALUE
(-1230010, '[Fulborg] The Stillpine furbolgs will not soon forget your bravery!', 0, 0, 'Stillpine Capitive free say text 1'),
(-1230011, '[Fulborg] Thank you, $N', 0, 0, 'Stillpine Capitive free say text 2'),
(-1230012, '[Fulborg] Those remaining at Stillpine Hold will welcome you as a hero!', 0, 0, 'Stillpine Capitive free say text 3');
INSERT INTO creature_loot_template(entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2) VALUE
(22911, 32358, -100, 0, 1, 1, 0, 0, 0);
UPDATE `creature_template` SET `lootid`=22911 WHERE `entry`=22911;
