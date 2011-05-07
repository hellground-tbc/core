insert into gameobject_template (entry, type, displayid, name, size, data4) values (178188, 6, 410, 'Molten Core Circle BARON', 1, 1);
update gameobject_template set faction = 114 where entry between 178187 and 178193;

insert into gameobject (id, map, spawnMask, position_x, position_y, position_z, orientation, rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state) values
(178189,	409,	1,	583.724,	-806.669,	-204.888,	1.58118,	0,	0,	1,	0,	0,	0, 1),
(176953,	409,	1,	583.724,	-806.669,	-204.888,	1.58118,	0,	0,	1,	0,	0,	100,	0),
(176951,	409,	1,	601.798,	-1174.65,	-196.009,	4.7408,	0,	0,	1,	0,	0,	100,	0),
(178187,	409,	1,	601.798,	-1174.65,	-196.009,	4.7408,	0,	0,	1,	0,	0,	0,	1),
(178188,	409,	1,	748.798,	-985.067,	-178.213,	4.70267,	0,	0,	1,	0,	0,	0,	1),
(176952,	409,	1,	748.798,	-985.067,	-178.213,	4.70267,	0,	0,	1,	0,	0,	100,	0),
(178190,	409,	1,	795.501,	-974.258,	-207.91,	3.11579,	0,	0,	1,	0,	0,	0,	1),
(176954,	409,	1,	795.501,	-974.258,	-207.91,	3.11579,	0,	0,	1,	0,	0,	100,	0),
(178192,	409,	1,	1132.16,	-1017.34,	-186.415,	0.00405526,	0,	0,	1,	0,	0,	0,	1),
(176956,	409,	1,	1132.16,	-1017.34,	-186.415,	0.00405526,	0,	0,	1,	0,	0,	100,	0);

update gameobject set state = 0 where guid in (43162, 43164);

