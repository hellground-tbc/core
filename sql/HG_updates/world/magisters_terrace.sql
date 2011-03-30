UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunwell_mageblood_guard' WHERE `creature_template`.`entry` = 24683;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunwell_mageblood_guard' WHERE `creature_template`.`entry` = 25568;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24683;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25568;
UPDATE `creature_template` SET 
    `mindmg` = 2500, 
    `maxdmg` = 4000, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 24683;
INSERT INTO  `world`.`creature_formations` (
`leaderGUID` ,
`memberGUID` ,
`dist` ,
`angle` ,
`groupAI`
)
VALUES (
'96764',  '96765',  '30',  '30',  '2'
);
INSERT INTO  `world`.`creature_formations` (
`leaderGUID` ,
`memberGUID` ,
`dist` ,
`angle` ,
`groupAI`
)
VALUES (
'96765',  '96764',  '30',  '30',  '2'
);
UPDATE `creature_template` SET `speed` = '1.2', `baseattacktime` = '2000', `unit_flags` = '32832', `equipment_id` = '2098' WHERE `creature_template`.`entry` = 25568;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_magister' WHERE `creature_template`.`entry` = 24685;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_magister' WHERE `creature_template`.`entry` = 25569;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24685;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25569;
UPDATE `creature_template` SET 
    `mindmg` = 3000, 
    `maxdmg` = 4500, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 25568;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_warlock' WHERE `creature_template`.`entry` = 24686;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_warlock' WHERE `creature_template`.`entry` = 25572;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24686;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25572;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_blood_knight' WHERE `creature_template`.`entry` = 24684;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_sunblade_blood_knight' WHERE `creature_template`.`entry` = 25565;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24684;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25565;
UPDATE `creature_template` SET 
    `mindmg` = 50, 
    `maxdmg` = 100, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 24690;
  UPDATE `creature_template` SET 
    `mindmg` = 50, 
    `maxdmg` = 100, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 24689;
  UPDATE `creature_template` SET 
    `mindmg` = 50, 
    `maxdmg` = 100, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 24688;
  UPDATE `creature_template` SET 
    `mindmg` = 70, 
    `maxdmg` = 120, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 25576;
  UPDATE `creature_template` SET 
    `mindmg` = 70, 
    `maxdmg` = 120, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 25575;
  UPDATE `creature_template` SET 
    `mindmg` = 70, 
    `maxdmg` = 120, 
    `attackpower` = ROUND((`mindmg` + `maxdmg`) / 4 * 7), 
    `mindmg` = ROUND(`mindmg` - `attackpower` / 7), 
    `maxdmg` = ROUND(`maxdmg` - `attackpower` / 7) 
  WHERE `entry` = 25577;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_skulker' WHERE `creature_template`.`entry` = 24688;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_skulker' WHERE `creature_template`.`entry` = 25577;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24688;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25577;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_bruiser' WHERE `creature_template`.`entry` = 24689;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_bruiser' WHERE `creature_template`.`entry` = 25575;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24689;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25575;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_husk' WHERE `creature_template`.`entry` = 24690;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_wretched_husk' WHERE `creature_template`.`entry` = 25576;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 24690;
DELETE FROM `creature_ai_scripts` WHERE `creature_ai_scripts`.`creature_id` = 25576;