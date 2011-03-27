DELETE FROM eventai_texts WHERE entry = -10000;
INSERT INTO eventai_texts(`entry`,`content_default`,`type`,`comment`) VALUES
(-10000,'Insolent fool! You thought to steal Zelemar\'s blood? You shall pay with your own!',1,'Zelemar the Wrathful - Aggro');

DELETE FROM eventai_scripts WHERE id = 1783001;
INSERT INTO  eventai_scripts VALUES
(1783001,17830,4,0,100,2, 0,0,0,0, 1,-10000,0,0, 0,0,0,0, 0,0,0,0, 'Zelemar the Wrathful - Aggro yell');