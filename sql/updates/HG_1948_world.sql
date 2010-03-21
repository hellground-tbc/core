INSERT INTO spell_proc_event VALUES
(34598, 0, 0, 0, 0, 0, 0, 0, 45),
(33953, 0, 0, 0, 0, 0, 0, 0, 45),
(33297, 0, 0, 0, 0, 0, 0, 0, 45),
(38299, 0, 0, 0, 0, 0, 0, 0, 12),
(34584, 0, 0, 0, 0, 0, 0, 0, 30);

UPDATE spell_proc_event
SET cooldown = 45
WHERE entry IN (38347, 34320, 42083, 33648, 37173)