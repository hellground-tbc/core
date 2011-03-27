DELETE FROM trinity_string WHERE entry IN (11007, 11008);
INSERT INTO trinity_string (entry, content_default) VALUES 
(11007, 'You can\'t change instance\'s difficulty while one or more players from your group are inside'),
(11008, 'You can\'t change instance\'s difficulty while one or more players from your group are offline');
