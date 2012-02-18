DELETE FROM `command` WHERE `name` IN ('senditems','sendmail','sendmoney','sendmessage','send items','send mail','send money','send message');

INSERT INTO `command` VALUES
('send items',3,'Syntax: .send items #playername "#subject" "#text" itemid1[:count1] itemid2[:count2] ... itemidN[:countN]\r\n\r\nSend a mail to a player. Subject and mail text must be in "". If for itemid not provided related count values then expected 1, if count > max items in stack then items will be send in required amount stacks. All stacks amount in mail limited to 12.'),
('send mail',1,'Syntax: .send mail #playername "#subject" "#text"\r\n\r\nSend a mail to a player. Subject and mail text must be in "".'),
('send message',3,'Syntax: .send message $playername $message\r\n\r\nSend screen message to player from ADMINISTRATOR.'),
('send money','3','Syntax: .send money #playername "#subject" "#text" #money\r\n\r\nSend mail with money to a player. Subject and mail text must be in "".');

DELETE FROM `command` WHERE `name` IN ('gmnotify', 'gmnameannounce', 'gmannounce', 'wp addwp', 'loadpath');

INSERT INTO `command` VALUES
('gm notify', 1, 'Syntax: .gm notify #text\r\n\r\nSend a on screen notification to GM.'),
('gm nameannounce', 1, 'Syntax: .gm nameannounce #text\r\n\r\nSend announce to GM with sender name (similar to .nameannounce command but visible only for gm).'),
('gm announce', 1, 'Syntax: .gm announce #text\r\n\r\nSend announce to GM without sender name (similar to .announce command but visible only for gm).'),
('wp add', 2, 'Syntax: .wp add [#pathid] #moveflags #delay\r\n\r\nAdds waypoint to path. All arguments are optional.'),
('wp reloadpath', 2, 'Syntax: .wp reloadpath');


INSERT INTO trinity_string VALUES
(1200,'You try to view cinemitic %u but it doesn\'t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `command` WHERE `name` IN ('debug playsound','debug play sound','debug play cinematic');

INSERT INTO `command` VALUES
('debug play cinematic',1,'Syntax: .debug play cinematic #cinematicid\r\n\r\nPlay cinematic #cinematicid for you. You stay at place while your mind fly.\r\n'),
('debug play sound',1,'Syntax: .debug play sound #soundid\r\n\r\nPlay sound with #soundid.\r\nSound will be play only for you. Other players do not hear this.\r\nWarning: client may have more 5000 sounds...');

