DELETE FROM world.creature where id=17612;
UPDATE world.creature_template SET ScriptName='mob_siltfin_murloc' where entry=17190;
UPDATE world.creature_template SET ScriptName='mob_eventai', mechanic_immune_mask=8454160  where entry=17612;
INSERT INTO world.eventai_texts SET entry=-500, content_default='Who dares wield magic in my presence', type=1, comment='17612 say on summon';
INSERT INTO world.eventai_scripts SET creature_id=17612, event_type=11, event_chance=100, action1_type=1, action1_param1=-500, comment='17612 say on summon';
