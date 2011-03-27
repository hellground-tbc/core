delete from command where name IN ('ban email', 'baninfo email', 'banlist email', 'unban email');
insert into command values
('ban email', 3, 'Skladnia: .ban email login powod'),
('baninfo email', 3, ''),
('banlist email', 3, ''),
('unban email', 3, ''),
('revivegroup', 3, 'Tpa i resa cala grupe w ktorej jest gm (o ile jest na tej samej mapie)');

delete from trinity_string where entry in (11003, 11004, 11005, 11006);
insert into trinity_string (entry, content_default) values
(11003, 'There is no such email in banlist.'),
(11004, 'Email: %s
Ban Date: %s
Reason: %s
Set by: %s'),
(11005, 'There is no matching email ban.'),
(11006, 'The following emails match your pattern:');
