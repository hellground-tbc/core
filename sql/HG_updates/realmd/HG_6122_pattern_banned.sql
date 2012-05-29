DROP TABLE IF EXISTS `pattern_banned`;

CREATE TABLE `pattern_banned` (
  `ip_pattern` varchar(20) DEFAULT NULL,
  `localip_pattern` varchar(20) DEFAULT NULL,
  `comment` varchar(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
