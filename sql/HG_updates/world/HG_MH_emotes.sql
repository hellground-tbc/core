/*Horde grunt*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820000', '17932', 
'4','25','0','0','0','0','0','1','-800','-801','MH Grunt aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820001', '17932', 
'6','25','0','0','0','0','0','1','-802','-803','MH Grunt die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820002', '17932', 
'12','25','0','2','0','500','1000','1','-804','-805','MH Grunt kill');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-800','Have at them! 
Lok-Tar!','0','0','0','0','MH Grunt Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-801','Destroy them 
all!','0','0','0','0','MH Grunt Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-802','Keep... 
fighting!','0','0','0','0','MH Grunt Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-803','Spill their... 
blood!','0','0','0','0','MH Grunt Die2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-804','Demon 
filth!','0','0','0','0','MH Grunt Kill1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-805','Another one 
down!','0','0','0','0','MH Grunt Kill2');

/*Horde Headhunter*/
UPDATE creature_template SET AIName='EventAI' WHERE entry=17934;
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, 
`event_param3`,`event_param4`,`action1_type`, `action1_param1`,`action1_param2`, `comment`) VALUES ('2820003', '17934', 
'4','25','0','0','0','0','0','1','-965','-966','MH Headhunter aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `comment`) VALUES ('2820004', '17934', '6','25','0','0','0','0','0','1','-967','MH Headhunter die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `comment`) VALUES ('2820005', '17934', '12','25','0','2','0','500','1000','1','-968','MH Headhunter 
kill');

INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-965','We kill \'em good, 
mon.','0','0','0','0','MH Headhunter Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-966','Take their 
heads!','0','0','0','0','MH Headhunter Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-967','Die no 
good...','0','0','0','0','MH Headhunter Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-968','Da Legion gonna 
pay!','0','0','0','0','MH Headhunter Kill1');

/*Horde Shaman and Witch Doctor*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `comment`) VALUES ('2820006', '17936', '6','25','0','0','0','0','0','1','-969','MH Shaman die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `comment`) VALUES ('2820007', '17935', '6','25','0','0','0','0','0','1','-970','MH Witch Doctor 
die');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-969','I am one... with 
all.','0','0','0','0','MH Shaman Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-970','Bad 
Mojo.','0','0','0','0','MH Witch Doctor Die1');

/*Tauren Warrior*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820008', '17933', 
'4','25','0','0','0','0','0','1','-971','-972','MH Tauren aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820009', '17933', 
'6','25','0','0','0','0','0','1','-973','-974','MH Tauren die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820010', '17933', 
'12','25','0','2','0','500','1000','1','-975','-976','MH Tauren kill');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-971','Crush 
them!','0','0','0','0','MH Tauren Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-972','For the Earth 
Mother!','0','0','0','0','MH Tauren Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-973','I return... to the 
earth.','0','0','0','0','MH Tauren Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-974','My spirit... goes 
on.','0','0','0','0','MH Tauren Die2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-975','Balance must be 
restored!','0','0','0','0','MH Tauren Kill1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-976','This world is not for 
you!','0','0','0','0','MH Tauren Kill2');

/*Alliance Knight*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, 
`event_param3`,`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820011', '17920', 
'4','25','0','0','0','0','0','1','-977','-978','MH Knight aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820012', '17920', 
'6','25','0','0','0','0','0','1','-979','-980','MH Knight die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820013', '17920', 
'12','25','0','2','0','500','1000','1','-981','-982','MH Knight kill');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-977','For Lordaeron! For 
victory!','0','0','0','0','MH Knight Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-978','Rout these 
demons!','0','0','0','0','MH Knight Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-979','Good will... 
prevail.','0','0','0','0','MH Knight Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-980','Light receive 
me!','0','0','0','0','MH Knight Die2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-981','For the 
Light!','0','0','0','0','MH Knight Kill1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-982','Justice is 
served!','0','0','0','0','MH Knight Kill2');

/*Alliance Priest*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820014', '17928', 
'6','15','0','0','0','0','0','1','-983','-984','MH Priest die');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-983','For... 
Quel\'Thalas.','0','0','0','0','MH Priest Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-984','Stay... the 
course.','0','0','0','0','MH Priest Die2');

/*Alliance Footman*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, 
`event_param3`,`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820015', '17919', 
'4','25','0','0','0','0','0','1','-985','-986','MH Footman aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820016', '17919', 
'6','25','0','0','0','0','0','1','-987','-988','MH Footman die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820017', '17919', 
'12','25','0','2','0','500','1000','1','-989','-990','MH Footman kill');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-985','Let these monsters taste 
cold steel!','0','0','0','0','MH Footman Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-986','Back, hold them back! 
','0','0','0','0','MH Footman Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
('-987','Agghh!','0','0','0','0','MH Footman Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-988','I... 
can\'t...','0','0','0','0','MH Footman Die2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-989','One less demon to contend 
with!','0','0','0','0','MH Footman Kill1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-990','Back to the void with 
you!','0','0','0','0','MH Footman Kill2');

INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820018', '17922', 
'6','25','0','0','0','0','0','1','-991','-992','MH Sorceress die');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
('-991','Res...?','0','0','0','0','MH Sorceress Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-992','Not... my best... 
day.','0','0','0','0','MH Sorceress Die2');

/*Alliance Rifleman*/
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, 
`event_param3`,`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820019', '17921', 
'4','25','0','0','0','0','0','1','-993','-994','MH Rifleman aggro');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `action1_param2`, `comment`) VALUES ('2820020', '17921', 
'6','25','0','0','0','0','0','1','-995','-996','MH Rifleman die');
INSERT INTO creature_ai_scripts (`id`, `creature_id`, `event_type`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, 
`event_param4`,`action1_type`, `action1_param1`, `comment`) VALUES ('2820021', '17921', '12','25','0','2','0','500','1000','1','-997','MH Rifleman 
kill');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-993','Blow them 
away!','0','0','0','0','MH Rifleman Aggro1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-994','I\'m itchin\' to shoot 
somethin\'!','0','0','0','0','MH Rifleman Aggro2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-995','Carry on the 
fight!','0','0','0','0','MH Rifleman Die1');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-996','For... Khaz... 
Modan!','0','0','0','0','MH Rifleman Die2');
INSERT INTO creature_ai_texts (`entry`, `content_default`, `sound`, `type`, `language`, `emote`, `comment`) VALUES ('-997','That\'s what I\'m talkin\' 
about!','0','0','0','0','MH Rifleman Kill1');

