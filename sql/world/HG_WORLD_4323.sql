update creature_template set scriptname = 'flamewaker_healer', faction_A = 54, faction_H = 54 where entry = 11663;
update creature_template set scriptname = 'flamewaker_elite', faction_A = 54, faction_H = 54 where entry = 11664;

insert into creature (id, map, spawnmask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, deathstate, movementtype) 
values (12018,	409,	1,	0,	0,	757.103,	-1179.89,	-118.61,	2.98811,	0,	0,	0,	666200,	0,	0,	0)