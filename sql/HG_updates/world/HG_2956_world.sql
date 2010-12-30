INSERT INTO script_texts (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1600004, "Thanks for your help. Let`s get out of here!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Skyguard_prisoner1"),
(-1600005, "Let`s keep moving. I don`t like this place.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Skyguard_prisoner2"),
(-1600006, "Thanks again. Sergant Doryn will be glad to hear he has one less scout to replace this week.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Skyguard_prisoner3");
UPDATE creature_template SET ScriptName="npc_skyguard_prisoner" WHERE entry=23383;
INSERT INTO creature_questrelation (`id`, `quest`) VALUES (23383, 11085);
INSERT INTO creature (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES (99999, 23383, 530, 1, 0, 0, -4106.61, 3029.86, 345.05, 2.38, 180, 0, 0, 7176, 0, 0, 0);
UPDATE quest_template SET SpecialFlags=3 WHERE entry=11085;