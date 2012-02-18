DELETE FROM `command` WHERE `name` IN ('senditems','sendmail','sendmoney','sendmessage','send items','send mail','send money','send message');

INSERT INTO `command` VALUES
('send items',3,'Syntax: .send items #playername "#subject" "#text" itemid1[:count1] itemid2[:count2] ... itemidN[:countN]\r\n\r\nSend a mail to a player. Subject and mail 
text must be in "". If for itemid not provided related count values then expected 1, if count > max items in stack then items will be send in required amount stacks. All 
stacks amount in mail limited to 12.'),
('send mail',1,'Syntax: .send mail #playername "#subject" "#text"\r\n\r\nSend a mail to a player. Subject and mail text must be in "".'),
('send message',3,'Syntax: .send message $playername $message\r\n\r\nSend screen message to player from ADMINISTRATOR.'),
('send money','3','Syntax: .send money #playername "#subject" "#text" #money\r\n\r\nSend mail with money to a player. Subject and mail text must be in "".');

DELETE FROM trinity_string WHERE entry IN(1300,1301);
INSERT INTO trinity_string VALUES
(1300,'You try to view cinemitic %u but it doesn\'t exist.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `command` WHERE `name` IN ('debug playsound','debug play sound','debug play cinematic');

INSERT INTO `command` VALUES
('debug play cinematic',1,'Syntax: .debug play cinematic #cinematicid\r\n\r\nPlay cinematic #cinematicid for you. You stay at place while your mind fly.\r\n'),
('debug play sound',1,'Syntax: .debug play sound #soundid\r\n\r\nPlay sound with #soundid.\r\nSound will be play only for you. Other players do not hear this.\r\nWarning: 
client may have more 5000 sounds...');

