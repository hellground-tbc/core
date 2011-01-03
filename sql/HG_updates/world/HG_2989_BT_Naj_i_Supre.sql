# immune dla trashu supremusa

update creature_template set mechanic_immune_mask = 8457887 where entry = 23028;
update creature_template set mechanic_immune_mask = 67603 where entry = 22963;
update creature_template set mechanic_immune_mask = 8462299, flags_extra = 256 where entry = 23030 or entry = 23330;
update creature_template set mechanic_immune_mask = 73408, flags_extra = 256 where entry = 22960;
update creature_template set mechanic_immune_mask = 583212763, flags_extra = 256 where entry = 22954;

# loot 

update creature_template set lootid = 23330 where entry = 23330;
update creature_template set lootid = 23030 where entry = 23030;
update creature_template set lootid = 23028 where entry = 23028;
update creature_template set lootid = 22963 where entry = 22963;
update creature_template set lootid = 22960 where entry = 22960;
update creature_template set lootid = 22954 where entry = 22954;
update creature_template set lootid = 22898 where entry = 22898;
update creature_template set lootid = 22887 where entry = 22887;
update creature_template set lootid = 22885 where entry = 22885;
update creature_template set lootid = 22884 where entry = 22884;
update creature_template set lootid = 22883 where entry = 22883;
update creature_template set lootid = 22878 where entry = 22878;
update creature_template set lootid = 22877 where entry = 22877;
update creature_template set lootid = 22876 where entry = 22876;
update creature_template set lootid = 22875 where entry = 22875;
update creature_template set lootid = 22874 where entry = 22874;
update creature_template set lootid = 22873 where entry = 22873;

# update wymagań na bt - wymaga wykonanego q The Distraction for Akama

update access_requirement set quest_done = 10985, quest_failed_text = 'To enter raid instance you myst have completed quest: A Distraction for Akama' where id = 47;

# dodanie uzyskania tytułu Hand of Adal po zrobieniu q The Distraction for Akama

UPDATE quest_template SET CharTitleId=64 WHERE entry=10985;

# dodanie do Xiri zaczynanie questu The Distraction for Akama

delete from creature_questrelation where quest = 10985;
insert into creature_questrelation values (18528, 10985);

# quest item z Rage Winterchill

delete from creature_loot_template where item = 32459;
insert into creature_loot_template values (17767, 32459, -100, 2, 1, 1, 0, 0, 0);


# Repa Nejentus, supremus i trash do nich

DELETE FROM creature_onkill_reputation WHERE (RewOnKillRepFaction1 = 1012 or RewOnKillRepFaction2 = 1012) and creature_id IN(22878, 22883, 22873, 22874, 22875, 22876, 22877, 22885, 22884, 23028, 22963, 23030, 23330, 22960, 22954, 22898, 22887);
INSERT INTO creature_onkill_reputation VALUES
(22873, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22874, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22875, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22876, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22877, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22878, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22883, 1012, 0, 7, 0, 3, 0, 0, 0, 0),
(22884, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22887, 1012, 0, 7, 0, 250, 0, 0, 0, 0),
(22898, 1012, 0, 7, 0, 250, 0, 0, 0, 0),
(22954, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22960, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(22963, 1012, 0, 7, 0, 3, 0, 0, 0, 0),
(23028, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(23030, 1012, 0, 7, 0, 15, 0, 0, 0, 0),
(23330, 1012, 0, 7, 0, 15, 0, 0, 0, 0);

# update czasu respawnu dla supremusa i najentusa

update creature set spawntimesecs = 6048000 where id = 22898 or id = 22887;
