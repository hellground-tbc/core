/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

/*Table structure for table `account` */

DROP TABLE IF EXISTS `account`;

CREATE TABLE `account` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL,
  `sha_pass_hash` varchar(40) NOT NULL DEFAULT '',
  `gmlevel` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `sessionkey` longtext,
  `v` longtext,
  `s` longtext,
  `email` text,
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(30) NOT NULL DEFAULT '127.0.0.1',
  `last_local_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `failed_logins` int(11) unsigned NOT NULL DEFAULT '0',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` tinyint(4) NOT NULL DEFAULT '0',
  `expansion` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `mutetime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `opcodesDisabled` int(11) NOT NULL DEFAULT '0',
  `account_flags` double DEFAULT '0',
  `operatingSystem` tinyint(4) DEFAULT NULL,
  `vote` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_username` (`username`),
  KEY `idx_gmlevel` (`gmlevel`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Account System';

/*Table structure for table `account_banned` */

DROP TABLE IF EXISTS `account_banned`;

CREATE TABLE `account_banned` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `bandate` bigint(40) NOT NULL DEFAULT '0',
  `unbandate` bigint(40) NOT NULL DEFAULT '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`bandate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Ban List';

/*Table structure for table `account_login` */

DROP TABLE IF EXISTS `account_login`;

CREATE TABLE `account_login` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `logindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ip` varchar(30) NOT NULL,
  `local_ip` varchar(30) NOT NULL,
  PRIMARY KEY (`id`,`logindate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Account login history';

/*Table structure for table `account_mute` */

DROP TABLE IF EXISTS `account_mute`;

CREATE TABLE `account_mute` (
  `id` int(11) NOT NULL DEFAULT '0' COMMENT 'Account id',
  `mutedate` bigint(40) NOT NULL DEFAULT '0',
  `unmutedate` bigint(40) NOT NULL DEFAULT '0',
  `mutedby` varchar(50) NOT NULL,
  `mutereason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`mutedate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Mute List';

/*Table structure for table `blocked_ips` */

DROP TABLE IF EXISTS `blocked_ips`;

CREATE TABLE `blocked_ips` (
  `ip` varchar(20) COLLATE utf8_bin NOT NULL,
  `recorded` bigint(20) unsigned NOT NULL,
  `rec_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `cnt` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ip`,`recorded`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

/*Table structure for table `email_banned` */

DROP TABLE IF EXISTS `email_banned`;

CREATE TABLE `email_banned` (
  `email` varchar(50) NOT NULL,
  `bandate` int(11) NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(50) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Banned Emails';

/*Table structure for table `ip_banned` */

DROP TABLE IF EXISTS `ip_banned`;

CREATE TABLE `ip_banned` (
  `ip` varchar(32) NOT NULL DEFAULT '127.0.0.1',
  `bandate` int(11) NOT NULL,
  `unbandate` int(11) NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(50) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Banned IPs';

/*Table structure for table `online` */

DROP TABLE IF EXISTS `online`;

CREATE TABLE `online` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) NOT NULL DEFAULT '0',
  `user_name` varchar(200) NOT NULL DEFAULT 'Guest',
  `user_ip` varchar(15) NOT NULL DEFAULT '0.0.0.0',
  `logged` int(10) NOT NULL DEFAULT '0',
  `currenturl` varchar(255) NOT NULL DEFAULT './',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8;

/*Table structure for table `pattern_banned` */

DROP TABLE IF EXISTS `pattern_banned`;

CREATE TABLE `pattern_banned` (
  `ip_pattern` char(20) DEFAULT NULL,
  `localip_pattern` char(20) DEFAULT NULL,
  `comment` char(20) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Table structure for table `realmcharacters` */

DROP TABLE IF EXISTS `realmcharacters`;

CREATE TABLE `realmcharacters` (
  `realmid` int(11) unsigned NOT NULL DEFAULT '0',
  `acctid` bigint(20) unsigned NOT NULL,
  `numchars` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`realmid`,`acctid`),
  KEY `idx_acctid` (`acctid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Realm Character Tracker';

/*Table structure for table `realmlist` */

DROP TABLE IF EXISTS `realmlist`;

CREATE TABLE `realmlist` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(32) NOT NULL DEFAULT '127.0.0.1',
  `port` int(11) NOT NULL DEFAULT '8085',
  `icon` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `realmflags` tinyint(3) unsigned NOT NULL DEFAULT '2' COMMENT 'Supported masks: 0x1 (invalid, not show in realm list), 0x2 (offline, set by core), 0x4 (show version and build), 0x20 (recommended), 0x40 (new players)',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `allowedSecurityLevel` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `population` float unsigned NOT NULL DEFAULT '0',
  `realmbuilds` varchar(64) NOT NULL DEFAULT '',
  `dbinfo` varchar(355) NOT NULL DEFAULT 'username;password;3306;127.0.0.1;DBWorld;DBCharacter' COMMENT 'Database info to THIS row',
  `ra_address` varchar(25) NOT NULL DEFAULT '127.0.0.1',
  `ra_port` int(5) NOT NULL DEFAULT '3443',
  `ra_user` varchar(355) NOT NULL DEFAULT 'username',
  `ra_pass` varchar(355) NOT NULL DEFAULT 'password',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Realm System';

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
