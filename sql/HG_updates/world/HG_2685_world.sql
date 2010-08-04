-- go_blood_filled_orb
UPDATE gameobject_template SET flags = 0, ScriptName="go_blood_filled_orb" WHERE entry=182024;
-- delete Zelemar the Wrathful, only summonable
DELETE FROM creature WHERE id=17830;

-- go_shrine_of_the_birds
UPDATE gameobject_template SET ScriptName="go_matrix_punchograph" WHERE entry IN (142345, 142475, 142476, 142696);