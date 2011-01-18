UPDATE `creature` SET `spawntimesecs`=180 WHERE id IN (18445, 20812, 18369)
UPDATE `creature_template` SET `ScriptName`="npc_corki_capitive" WHERE entry IN (18445, 20812, 18369); 
UPDATE `gameobject_template` SET `data2`=1966080, `ScriptName`="go_corki_cage" WHERE entry IN(182349, 182350, 182521);
UPDATE `quest_template` SET `ReqCreatureOrGOId1`=-182349, `ReqSpellCast1`=32249 WHERE `entry`=9923;
UPDATE `quest_template` SET `ReqCreatureOrGOId1`=-182350, `ReqSpellCast1`=32255 WHERE `entry`=9924;
UPDATE `quest_template` SET `ReqCreatureOrGOId1`=-182521, `ReqSpellCast1`=32384 WHERE `entry`=9955;
INSERT INTO script_texts(`entry`, `content_default`, `type`, `language`, `comment`) VALUE
(-1230010, "Thanks, $R! I'm sure my dad will reward you greatly! Bye!", 0, 0, 'Corki free say text 1'),
(-1230011, 'This is the last time I get caught! I promise! Bye!', 0, 0, 'Corki free say text 2'),
(-1230012, 'I want my daddy!', 0, 0, 'Corki free say text 3');