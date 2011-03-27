--Hive in the Tower
UPDATE quest_template SET ReqSourceId1=17345, ReqSourceCount1=1 WHERE entry=1126;
--Finding the Survivors
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182486;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182487;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182488;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182489;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182490;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182491;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182492;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182493;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182494;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182495;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182496;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182497;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182498;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182499;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182500;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182501;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182502;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182503;
UPDATE gameobject_template SET ScriptName="go_warmaul_prison" WHERE entry=182504;

INSERT INTO script_texts (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1600000, "Many Thanks, hero!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Warmaul_prisoner1"),
(-1600001, "Death to the ogres!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Warmaul_prisoner2"),
(-1600002, "Spirits watch for you, $C.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Warmaul_prisoner3"),
(-1600003, "Thank you, $C! Your service to the Mag'har will not be forgotten.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 0, 0, 0, 1, "Warmaul_prisoner4");