UPDATE `creature_template` SET `ScriptName`="npc_letoll" WHERE `entry`=22458;
DELETE FROM script_texts WHERE entry BETWEEN -1000510 AND -1000530;
INSERT INTO `script_texts` (`entry`,`content_default`,`sound`,`type`,`language`,`emote`,`comment`) VALUES
(-1000510,'Aright, listen up! Form a circle around me and move out!',0,0,0,0,'letoll SAY_LE_START'),
(-1000511,'Aright, $r, just keep us safe from harm while we work. We\'ll pay you when we return.',0,0,0,0,'letoll SAY_LE_KEEP_SAFE'),
(-1000512,'The dig site is just north of here.',0,0,0,0,'letoll SAY_LE_NORTH'),
(-1000513,'We\'re here! Start diggin\'!',0,0,0,0,'letoll SAY_LE_ARRIVE'),
(-1000514,'I think there\'s somethin\' buried here, beneath the sand!',0,0,0,0,'letoll SAY_LE_BURIED'),
(-1000515,'Almost got it!',0,0,0,0,'letoll SAY_LE_ALMOST'),
(-1000516,'By brann\'s brittle bananas! What is it!? It... It looks like a drum.',0,0,0,0,'letoll SAY_LE_DRUM'),
(-1000517,'Wow... a drum.',0,0,0,0,'letoll SAY_LE_DRUM_REPLY'),
(-1000518,'This discovery will surely rock the foundation of modern archaeology.',0,0,0,0,'letoll SAY_LE_DISCOVERY'),
(-1000519,'Yea, great. Can we leave now? This desert is giving me hives.',0,0,0,0,'letoll SAY_LE_DISCOVERY_REPLY'),
(-1000520,'Have ye gone mad? You expect me to leave behind a drum without first beatin\' on it? Not this son of Ironforge! No sir!',0,0,0,0,'letoll SAY_LE_NO_LEAVE'),
(-1000521,'This reminds me of that one time where you made us search Silithus for evidence of sand gnomes.',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY1'),
(-1000522,'Or that time when you told us that you\'d discovered the cure for the plague of the 20th century. What is that even? 20th century?',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY2'),
(-1000523,'I don\'t think it can top the one time where he told us that he\'d heard that Artha\'s "cousin\'s" skeleton was frozen beneath a glacier in Winterspring. I\'ll never forgive you for that one, Letoll. I mean honestly... Artha\'s cousin?',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY3'),
(-1000524,'I dunno. It can\'t possibly beat the time he tried to convince us that we\'re all actually a figment of some being\'s imagination and that they only use us for their own personal amusement. That went over well during dinner with the family.',0,0,0,0,'letoll SAY_LE_NO_LEAVE_REPLY4'),
(-1000525,'Shut yer yaps! I\'m gonna bang on this drum and that\'s that!',0,0,0,0,'letoll SAY_LE_SHUT'),
(-1000526,'Say, do you guys hear that?',0,0,0,0,'letoll SAY_LE_REPLY_HEAR'),
(-1000527,'IN YOUR FACE! I told you there was somethin\' here!',0,0,0,0,'letoll SAY_LE_IN_YOUR_FACE'),
(-1000528,'Don\'t just stand there! Help him out!',0,0,0,0,'letoll SAY_LE_HELP_HIM'),
(-1000529,'%s picks up the drum.',0,2,0,0,'letoll EMOTE_LE_PICK_UP'),
(-1000530,'You\'ve been a tremendous help, $r! Let\'s get out of here before more of those things show up! I\'ll let Dwarfowitz know you did the job asked of ya\' admirably.',0,0,0,0,'letoll SAY_LE_THANKS');
