DROP TABLE IF EXISTS `email_banned`;
CREATE TABLE `email_banned` (
  `email` varchar(50) NOT NULL,
  `bandate` int(11) NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(50) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Banned Emails';
