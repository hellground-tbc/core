
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
(-1069090, 'Die!', '', '', NULL, NULL, NULL, NULL, NULL, NULL, 6337, 1, 0, 0, 'For script npc_sethir_the_ancient');

UPDATE `creature_template` SET `ScriptName` = 'npc_sethir_the_ancient' WHERE `creature_template`.`entry` = 6909;
