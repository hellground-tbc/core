UPDATE creature_template SET ScriptName="npc_henry_stern" WHERE entry=8696;
UPDATE gameobject_template SET ScriptName="go_gong" WHERE entry=148917;
UPDATE creature_template SET ScriptName="npc_tomb_creature" WHERE entry IN (7349, 7351, 7355);
UPDATE instance_template SET script="instance_razorfen_downs" WHERE map=129;