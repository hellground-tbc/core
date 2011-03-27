-- Quest: To Legion Hold
UPDATE creature_template 
Set ScriptName = 'mob_deathbringer_joovan'
WHERE entry = 21633;

UPDATE quest_template
SET EndText = "Discover Legion's Plans", ObjectiveText3 = '',
ReqCreatureOrGOId3 = 0, ReqCreatureOrGoCount3 = 0, QuestFlags = 130
WHERE entry IN (10596, 10563);

DELETE FROM creature WHERE id = 21633;
DELETE FROM gameobject WHERE guid IN (25737, 25738);