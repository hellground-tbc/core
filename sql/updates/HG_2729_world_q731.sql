UPDATE world.creature_template set ScriptName='npc_prospector_remtravel' where entry=2917;
insert into world.script_texts VALUES (-1581001, "Ok, $n, let's go find where I left that mysterious fossil. Follow me!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel on q begin");  )
insert into world.script_texts VALUES (-1581002, "Now where did I put that mysterious fossil? Ah, maybe up there...", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581003, "Hmm, nothing up here..", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581004, "No mysterious fossil here...Ah, but my copy of Green Hills of Stranglethorn. What a good book!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581005, "I bet you I left it in the tent!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581006, "Oh wait, that's Hollee's tent...and it's empty", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581007, "Interesting... I hadn't noticed this earlier...", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581008, "Oh wait! I'm supposed to be looking for that mysterious fossil!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581009, "Nope, didn't leave the fossil back here!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581010, "Ah, I remember now! I gave the mysterious fossil to Hollee! Check with her", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,0,0,0, "Prospector Remtravel");
insert into world.script_texts VALUES (-1581011, "Help me! This creature won't get off of me!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,1,0,0, "Prospector Remtravel on attack");
insert into world.script_texts VALUES (-1581012, "Oh my! What's this? Help!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,1,0,0, "Prospector Remtravel on attack");
insert into world.script_texts VALUES (-1581013, "Something tells me this creature wants mysterious fossil too. Help!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0,1,0,0, "Prospector Remtravel on attack");
DELETE FROM world.quest_start_scripts where id=731;
UPDATE world.quest_template SET StartScript=0 where entry=731;
DELETE FROM world.db_script_string WHERE entry BETWEEN 2000000004 AND  2000000008;