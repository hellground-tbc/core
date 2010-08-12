UPDATE world.creature_template set ScriptName='npc_therylune' where entry=3584;
insert into world.script_texts VALUES (-1581014, "Ok, let's go!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Therylune on q begin");
insert into world.script_texts VALUES (-1581015, "I can make it the rest of the way, $n. THANKS!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Therylune on q end");
DELETE FROM world.quest_start_scripts where id=945;
UPDATE world.quest_template SET StartScript=0 where entry=945;
DELETE FROM world.db_script_string WHERE entry=2000000017 AND  entry=2000000018;