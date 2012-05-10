/*
SQLyog Trial v9.63 
MySQL - 5.5.23 : Database - characters
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`characters` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `characters`;

/*Table structure for table `arena_team` */

DROP TABLE IF EXISTS `arena_team`;

CREATE TABLE `arena_team` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) NOT NULL,
  `captainguid` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BackgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderColor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `arena_team_member` */

DROP TABLE IF EXISTS `arena_team_member`;

CREATE TABLE `arena_team_member` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `played_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_week` int(10) unsigned NOT NULL DEFAULT '0',
  `played_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_season` int(10) unsigned NOT NULL DEFAULT '0',
  `personal_rating` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `arenateamid_2` (`arenateamid`),
  KEY `arenateamid` (`arenateamid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `arena_team_stats` */

DROP TABLE IF EXISTS `arena_team_stats`;

CREATE TABLE `arena_team_stats` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` int(10) unsigned NOT NULL DEFAULT '0',
  `games` int(10) unsigned NOT NULL DEFAULT '0',
  `wins` int(10) unsigned NOT NULL DEFAULT '0',
  `played` int(10) unsigned NOT NULL DEFAULT '0',
  `wins2` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `armory_character_stats` */

DROP TABLE IF EXISTS `armory_character_stats`;

CREATE TABLE `armory_character_stats` (
  `guid` int(11) NOT NULL,
  `data` longtext NOT NULL,
  `save_date` int(11) DEFAULT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory table';

/*Table structure for table `at` */

DROP TABLE IF EXISTS `at`;

CREATE TABLE `at` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) NOT NULL,
  `captainguid` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BackgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderColor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `at2` */

DROP TABLE IF EXISTS `at2`;

CREATE TABLE `at2` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) NOT NULL,
  `captainguid` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BackgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderColor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `atbck` */

DROP TABLE IF EXISTS `atbck`;

CREATE TABLE `atbck` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` char(255) NOT NULL,
  `captainguid` int(10) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BackgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `EmblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `BorderColor` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `atm` */

DROP TABLE IF EXISTS `atm`;

CREATE TABLE `atm` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `played_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_week` int(10) unsigned NOT NULL DEFAULT '0',
  `played_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_season` int(10) unsigned NOT NULL DEFAULT '0',
  `personal_rating` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `arenateamid_2` (`arenateamid`),
  KEY `arenateamid` (`arenateamid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `atm2` */

DROP TABLE IF EXISTS `atm2`;

CREATE TABLE `atm2` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `played_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_week` int(10) unsigned NOT NULL DEFAULT '0',
  `played_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_season` int(10) unsigned NOT NULL DEFAULT '0',
  `personal_rating` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `arenateamid_2` (`arenateamid`),
  KEY `arenateamid` (`arenateamid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `atmbck` */

DROP TABLE IF EXISTS `atmbck`;

CREATE TABLE `atmbck` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `played_week` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_week` int(10) unsigned NOT NULL DEFAULT '0',
  `played_season` int(10) unsigned NOT NULL DEFAULT '0',
  `wons_season` int(10) unsigned NOT NULL DEFAULT '0',
  `personal_rating` int(10) unsigned NOT NULL DEFAULT '0',
  KEY `arenateamid_2` (`arenateamid`),
  KEY `arenateamid` (`arenateamid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `ats` */

DROP TABLE IF EXISTS `ats`;

CREATE TABLE `ats` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` int(10) unsigned NOT NULL DEFAULT '0',
  `games` int(10) unsigned NOT NULL DEFAULT '0',
  `wins` int(10) unsigned NOT NULL DEFAULT '0',
  `played` int(10) unsigned NOT NULL DEFAULT '0',
  `wins2` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `ats2` */

DROP TABLE IF EXISTS `ats2`;

CREATE TABLE `ats2` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` int(10) unsigned NOT NULL DEFAULT '0',
  `games` int(10) unsigned NOT NULL DEFAULT '0',
  `wins` int(10) unsigned NOT NULL DEFAULT '0',
  `played` int(10) unsigned NOT NULL DEFAULT '0',
  `wins2` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `atsbck` */

DROP TABLE IF EXISTS `atsbck`;

CREATE TABLE `atsbck` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  `rating` int(10) unsigned NOT NULL DEFAULT '0',
  `games` int(10) unsigned NOT NULL DEFAULT '0',
  `wins` int(10) unsigned NOT NULL DEFAULT '0',
  `played` int(10) unsigned NOT NULL DEFAULT '0',
  `wins2` int(10) unsigned NOT NULL DEFAULT '0',
  `rank` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `auctionhouse` */

DROP TABLE IF EXISTS `auctionhouse`;

CREATE TABLE `auctionhouse` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `houseid` int(11) unsigned NOT NULL DEFAULT '0',
  `itemguid` int(11) unsigned NOT NULL DEFAULT '0',
  `item_template` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  `item_count` int(11) unsigned NOT NULL DEFAULT '0',
  `item_randompropertyid` int(11) NOT NULL DEFAULT '0',
  `itemowner` int(11) unsigned NOT NULL DEFAULT '0',
  `buyoutprice` int(11) NOT NULL DEFAULT '0',
  `time` bigint(40) NOT NULL DEFAULT '0',
  `buyguid` int(11) unsigned NOT NULL DEFAULT '0',
  `lastbid` int(11) NOT NULL DEFAULT '0',
  `startbid` int(11) NOT NULL DEFAULT '0',
  `deposit` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `auctionhousebot` */

DROP TABLE IF EXISTS `auctionhousebot`;

CREATE TABLE `auctionhousebot` (
  `auctionhouse` int(11) NOT NULL DEFAULT '0' COMMENT 'mapID of the auctionhouse.',
  `name` char(25) DEFAULT NULL COMMENT 'Text name of the auctionhouse.',
  `minitems` int(11) DEFAULT '0' COMMENT 'This is the minimum number of items you want to keep in the auction house. a 0 here will make it the same as the maximum.',
  `maxitems` int(11) DEFAULT '0' COMMENT 'This is the number of items you want to keep in the auction house.',
  `mintime` int(11) DEFAULT '8' COMMENT 'Sets the minimum number of hours for an auction.',
  `maxtime` int(11) DEFAULT '24' COMMENT 'Sets the maximum number of hours for an auction.',
  `percentgreytradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Grey Trade Goods auction items',
  `percentwhitetradegoods` int(11) DEFAULT '27' COMMENT 'Sets the percentage of the White Trade Goods auction items',
  `percentgreentradegoods` int(11) DEFAULT '12' COMMENT 'Sets the percentage of the Green Trade Goods auction items',
  `percentbluetradegoods` int(11) DEFAULT '10' COMMENT 'Sets the percentage of the Blue Trade Goods auction items',
  `percentpurpletradegoods` int(11) DEFAULT '1' COMMENT 'Sets the percentage of the Purple Trade Goods auction items',
  `percentorangetradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Orange Trade Goods auction items',
  `percentyellowtradegoods` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the Yellow Trade Goods auction items',
  `percentgreyitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Grey auction items',
  `percentwhiteitems` int(11) DEFAULT '10' COMMENT 'Sets the percentage of the non trade White auction items',
  `percentgreenitems` int(11) DEFAULT '30' COMMENT 'Sets the percentage of the non trade Green auction items',
  `percentblueitems` int(11) DEFAULT '8' COMMENT 'Sets the percentage of the non trade Blue auction items',
  `percentpurpleitems` int(11) DEFAULT '2' COMMENT 'Sets the percentage of the non trade Purple auction items',
  `percentorangeitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Orange auction items',
  `percentyellowitems` int(11) DEFAULT '0' COMMENT 'Sets the percentage of the non trade Yellow auction items',
  `minpricegrey` int(11) DEFAULT '100' COMMENT 'Minimum price of Grey items (percentage).',
  `maxpricegrey` int(11) DEFAULT '150' COMMENT 'Maximum price of Grey items (percentage).',
  `minpricewhite` int(11) DEFAULT '150' COMMENT 'Minimum price of White items (percentage).',
  `maxpricewhite` int(11) DEFAULT '250' COMMENT 'Maximum price of White items (percentage).',
  `minpricegreen` int(11) DEFAULT '800' COMMENT 'Minimum price of Green items (percentage).',
  `maxpricegreen` int(11) DEFAULT '1400' COMMENT 'Maximum price of Green items (percentage).',
  `minpriceblue` int(11) DEFAULT '1250' COMMENT 'Minimum price of Blue items (percentage).',
  `maxpriceblue` int(11) DEFAULT '1750' COMMENT 'Maximum price of Blue items (percentage).',
  `minpricepurple` int(11) DEFAULT '2250' COMMENT 'Minimum price of Purple items (percentage).',
  `maxpricepurple` int(11) DEFAULT '4550' COMMENT 'Maximum price of Purple items (percentage).',
  `minpriceorange` int(11) DEFAULT '3250' COMMENT 'Minimum price of Orange items (percentage).',
  `maxpriceorange` int(11) DEFAULT '5550' COMMENT 'Maximum price of Orange items (percentage).',
  `minpriceyellow` int(11) DEFAULT '5250' COMMENT 'Minimum price of Yellow items (percentage).',
  `maxpriceyellow` int(11) DEFAULT '6550' COMMENT 'Maximum price of Yellow items (percentage).',
  `minbidpricegrey` int(11) DEFAULT '70' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 70',
  `maxbidpricegrey` int(11) DEFAULT '100' COMMENT 'Starting bid price of Grey items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpricewhite` int(11) DEFAULT '70' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `maxbidpricewhite` int(11) DEFAULT '100' COMMENT 'Starting bid price of White items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `minbidpricegreen` int(11) DEFAULT '80' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `maxbidpricegreen` int(11) DEFAULT '100' COMMENT 'Starting bid price of Green items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `minbidpriceblue` int(11) DEFAULT '75' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `maxbidpriceblue` int(11) DEFAULT '100' COMMENT 'Starting bid price of Blue items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `minbidpricepurple` int(11) DEFAULT '80' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `maxbidpricepurple` int(11) DEFAULT '100' COMMENT 'Starting bid price of Purple items as a percentage of the randomly chosen buyout price. Default: 100 (Bid and buyout price the same)',
  `minbidpriceorange` int(11) DEFAULT '80' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceorange` int(11) DEFAULT '100' COMMENT 'Starting bid price of Orange items as a percentage of the randomly chosen buyout price. Default: 100',
  `minbidpriceyellow` int(11) DEFAULT '80' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 80',
  `maxbidpriceyellow` int(11) DEFAULT '100' COMMENT 'Starting bid price of Yellow items as a percentage of the randomly chosen buyout price. Default: 100',
  `maxstackgrey` int(11) DEFAULT '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackwhite` int(11) DEFAULT '0' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackgreen` int(11) DEFAULT '3' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackblue` int(11) DEFAULT '2' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackpurple` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackorange` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `maxstackyellow` int(11) DEFAULT '1' COMMENT 'Stack size limits for item qualities - a value of 0 will disable a maximum stack size for that quality, which will allow the bot to create items in stack as large as the item allows.',
  `buyerpricegrey` int(11) DEFAULT '1' COMMENT 'Multiplier to vendorprice when buying grey items from auctionhouse',
  `buyerpricewhite` int(11) DEFAULT '1' COMMENT 'Multiplier to vendorprice when buying white items from auctionhouse',
  `buyerpricegreen` int(11) DEFAULT '5' COMMENT 'Multiplier to vendorprice when buying green items from auctionhouse',
  `buyerpriceblue` int(11) DEFAULT '12' COMMENT 'Multiplier to vendorprice when buying blue items from auctionhouse',
  `buyerpricepurple` int(11) DEFAULT '15' COMMENT 'Multiplier to vendorprice when buying purple items from auctionhouse',
  `buyerpriceorange` int(11) DEFAULT '20' COMMENT 'Multiplier to vendorprice when buying orange items from auctionhouse',
  `buyerpriceyellow` int(11) DEFAULT '22' COMMENT 'Multiplier to vendorprice when buying yellow items from auctionhouse',
  `buyerbiddinginterval` int(11) DEFAULT '1' COMMENT 'Interval how frequently AHB bids on each AH. Time in minutes',
  `buyerbidsperinterval` int(11) DEFAULT '1' COMMENT 'number of bids to put in per bidding interval',
  PRIMARY KEY (`auctionhouse`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `bugreport` */

DROP TABLE IF EXISTS `bugreport`;

CREATE TABLE `bugreport` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `type` varchar(255) NOT NULL DEFAULT '',
  `content` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Debug System';

/*Table structure for table `character_action` */

DROP TABLE IF EXISTS `character_action`;

CREATE TABLE `character_action` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `button` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `action` smallint(5) unsigned NOT NULL DEFAULT '0',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `misc` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`button`),
  KEY `guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_aura` */

DROP TABLE IF EXISTS `character_aura`;

CREATE TABLE `character_aura` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Full Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0',
  `effect_index` int(11) unsigned NOT NULL DEFAULT '0',
  `stackcount` int(11) NOT NULL DEFAULT '1',
  `amount` int(11) NOT NULL DEFAULT '0',
  `maxduration` int(11) NOT NULL DEFAULT '0',
  `remaintime` int(11) NOT NULL DEFAULT '0',
  `remaincharges` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`,`effect_index`),
  KEY `guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_bgcoord` */

DROP TABLE IF EXISTS `character_bgcoord`;

CREATE TABLE `character_bgcoord` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `bgid` int(10) unsigned NOT NULL DEFAULT '0',
  `bgteam` int(10) unsigned NOT NULL DEFAULT '0',
  `bgmap` int(10) unsigned NOT NULL DEFAULT '0',
  `bgx` float NOT NULL DEFAULT '0',
  `bgy` float NOT NULL DEFAULT '0',
  `bgz` float NOT NULL DEFAULT '0',
  `bgo` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Saving char in bg System';

/*Table structure for table `character_declinedname` */

DROP TABLE IF EXISTS `character_declinedname`;

CREATE TABLE `character_declinedname` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `genitive` varchar(15) NOT NULL DEFAULT '',
  `dative` varchar(15) NOT NULL DEFAULT '',
  `accusative` varchar(15) NOT NULL DEFAULT '',
  `instrumental` varchar(15) NOT NULL DEFAULT '',
  `prepositional` varchar(15) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `character_gifts` */

DROP TABLE IF EXISTS `character_gifts`;

CREATE TABLE `character_gifts` (
  `guid` int(20) unsigned NOT NULL DEFAULT '0',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `entry` int(20) unsigned NOT NULL DEFAULT '0',
  `flags` int(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`item_guid`),
  KEY `idx_guid` (`guid`),
  KEY `item_guid` (`item_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `character_homebind` */

DROP TABLE IF EXISTS `character_homebind`;

CREATE TABLE `character_homebind` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `zone` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Zone Identifier',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_instance` */

DROP TABLE IF EXISTS `character_instance`;

CREATE TABLE `character_instance` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`instance`),
  KEY `instance` (`instance`),
  KEY `guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `character_inventory` */

DROP TABLE IF EXISTS `character_inventory`;

CREATE TABLE `character_inventory` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `bag` int(11) unsigned NOT NULL DEFAULT '0',
  `slot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `item` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Global Unique Identifier',
  `item_template` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  PRIMARY KEY (`item`),
  KEY `idx_guid` (`guid`),
  KEY `item` (`item`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_pet` */

DROP TABLE IF EXISTS `character_pet`;

CREATE TABLE `character_pet` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `entry` int(11) unsigned NOT NULL DEFAULT '0',
  `owner` int(11) unsigned NOT NULL DEFAULT '0',
  `modelid` int(11) unsigned DEFAULT '0',
  `CreatedBySpell` int(11) unsigned NOT NULL DEFAULT '0',
  `PetType` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `level` int(11) unsigned NOT NULL DEFAULT '1',
  `exp` int(11) unsigned NOT NULL DEFAULT '0',
  `Reactstate` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `loyaltypoints` int(11) NOT NULL DEFAULT '0',
  `loyalty` int(11) unsigned NOT NULL DEFAULT '0',
  `trainpoint` int(11) NOT NULL DEFAULT '0',
  `name` varchar(100) DEFAULT 'Pet',
  `renamed` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `slot` int(11) unsigned NOT NULL DEFAULT '0',
  `curhealth` int(11) unsigned NOT NULL DEFAULT '1',
  `curmana` int(11) unsigned NOT NULL DEFAULT '0',
  `curhappiness` int(11) unsigned NOT NULL DEFAULT '0',
  `savetime` bigint(20) unsigned NOT NULL DEFAULT '0',
  `resettalents_cost` int(11) unsigned NOT NULL DEFAULT '0',
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `abdata` longtext,
  `teachspelldata` longtext,
  PRIMARY KEY (`id`),
  KEY `owner` (`owner`),
  KEY `owner_2` (`owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

/*Table structure for table `character_pet_declinedname` */

DROP TABLE IF EXISTS `character_pet_declinedname`;

CREATE TABLE `character_pet_declinedname` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `owner` int(11) unsigned NOT NULL DEFAULT '0',
  `genitive` varchar(12) NOT NULL DEFAULT '',
  `dative` varchar(12) NOT NULL DEFAULT '',
  `accusative` varchar(12) NOT NULL DEFAULT '',
  `instrumental` varchar(12) NOT NULL DEFAULT '',
  `prepositional` varchar(12) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `owner_key` (`owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `character_queststatus` */

DROP TABLE IF EXISTS `character_queststatus`;

CREATE TABLE `character_queststatus` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  `status` int(11) unsigned NOT NULL DEFAULT '0',
  `rewarded` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `explored` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `timer` bigint(20) unsigned NOT NULL DEFAULT '0',
  `mobcount1` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount2` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount3` int(11) unsigned NOT NULL DEFAULT '0',
  `mobcount4` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount1` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount2` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount3` int(11) unsigned NOT NULL DEFAULT '0',
  `itemcount4` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`quest`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_queststatus_daily` */

DROP TABLE IF EXISTS `character_queststatus_daily`;

CREATE TABLE `character_queststatus_daily` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `quest` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`quest`),
  KEY `idx_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_reputation` */

DROP TABLE IF EXISTS `character_reputation`;

CREATE TABLE `character_reputation` (
  `guid` bigint(10) DEFAULT NULL,
  `faction` bigint(10) DEFAULT NULL,
  `standing` bigint(10) DEFAULT NULL,
  `flags` bigint(10) DEFAULT NULL,
  UNIQUE KEY `NewIndex1` (`guid`,`faction`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `character_social` */

DROP TABLE IF EXISTS `character_social`;

CREATE TABLE `character_social` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `friend` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Friend Global Unique Identifier',
  `flags` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'Friend Flags',
  `note` varchar(48) NOT NULL DEFAULT '' COMMENT 'Friend Note',
  PRIMARY KEY (`guid`,`friend`,`flags`),
  KEY `guid` (`guid`),
  KEY `friend` (`friend`),
  KEY `guid_flags` (`guid`,`flags`),
  KEY `friend_flags` (`friend`,`flags`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_spell` */

DROP TABLE IF EXISTS `character_spell`;

CREATE TABLE `character_spell` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `slot` int(11) unsigned NOT NULL DEFAULT '0',
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `disabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `character_spell_cooldown` */

DROP TABLE IF EXISTS `character_spell_cooldown`;

CREATE TABLE `character_spell_cooldown` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `item` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Item Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `character_ticket` */

DROP TABLE IF EXISTS `character_ticket`;

CREATE TABLE `character_ticket` (
  `sad` tinyint(1) DEFAULT NULL,
  `guid` bit(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

/*Table structure for table `character_tutorial` */

DROP TABLE IF EXISTS `character_tutorial`;

CREATE TABLE `character_tutorial` (
  `account` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Account Identifier',
  `realmid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Realm Identifier',
  `tut0` int(11) unsigned NOT NULL DEFAULT '0',
  `tut1` int(11) unsigned NOT NULL DEFAULT '0',
  `tut2` int(11) unsigned NOT NULL DEFAULT '0',
  `tut3` int(11) unsigned NOT NULL DEFAULT '0',
  `tut4` int(11) unsigned NOT NULL DEFAULT '0',
  `tut5` int(11) unsigned NOT NULL DEFAULT '0',
  `tut6` int(11) unsigned NOT NULL DEFAULT '0',
  `tut7` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`,`realmid`),
  KEY `acc_key` (`account`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `characters` */

DROP TABLE IF EXISTS `characters`;

CREATE TABLE `characters` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `account` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Account Identifier',
  `data` longtext,
  `name` varchar(12) NOT NULL DEFAULT '',
  `race` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `level` tinyint(3) unsigned DEFAULT '1',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `instance_id` int(11) unsigned NOT NULL DEFAULT '0',
  `dungeon_difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `taximask` longtext,
  `online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `cinematic` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `totaltime` int(11) unsigned NOT NULL DEFAULT '0',
  `leveltime` int(11) unsigned NOT NULL DEFAULT '0',
  `logout_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `is_logout_resting` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `rest_bonus` float NOT NULL DEFAULT '0',
  `resettalents_cost` int(11) unsigned NOT NULL DEFAULT '0',
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `trans_x` float NOT NULL DEFAULT '0',
  `trans_y` float NOT NULL DEFAULT '0',
  `trans_z` float NOT NULL DEFAULT '0',
  `trans_o` float NOT NULL DEFAULT '0',
  `transguid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `extra_flags` int(11) unsigned NOT NULL DEFAULT '0',
  `stable_slots` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `at_login` int(11) unsigned NOT NULL DEFAULT '0',
  `zone` int(11) unsigned NOT NULL DEFAULT '0',
  `death_expire_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `taxi_path` text,
  `arena_pending_points` int(10) unsigned NOT NULL DEFAULT '0',
  `latency` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `idx_account` (`account`),
  KEY `idx_online` (`online`),
  KEY `idx_name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `cheaters` */

DROP TABLE IF EXISTS `cheaters`;

CREATE TABLE `cheaters` (
  `entry` bigint(20) NOT NULL AUTO_INCREMENT,
  `player` varchar(255) NOT NULL,
  `acctid` int(11) NOT NULL,
  `reason` varchar(255) NOT NULL DEFAULT 'unknown',
  `speed` float NOT NULL DEFAULT '0',
  `Val1` float NOT NULL DEFAULT '0',
  `Val2` int(10) unsigned NOT NULL DEFAULT '0',
  `count` int(11) NOT NULL DEFAULT '0',
  `Map` smallint(5) NOT NULL DEFAULT '-1',
  `Pos` varchar(255) NOT NULL DEFAULT '0',
  `Level` mediumint(9) NOT NULL DEFAULT '0',
  `first_date` datetime NOT NULL,
  `last_date` datetime NOT NULL,
  `Op` varchar(255) NOT NULL DEFAULT 'unknown',
  PRIMARY KEY (`entry`),
  KEY `idx_Count` (`count`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `corpse` */

DROP TABLE IF EXISTS `corpse`;

CREATE TABLE `corpse` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `player` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `zone` int(11) unsigned NOT NULL DEFAULT '38' COMMENT 'Zone Identifier',
  `map` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Map Identifier',
  `data` longtext,
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `corpse_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  KEY `idx_type` (`corpse_type`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Death System';

/*Table structure for table `deleted_chars` */

DROP TABLE IF EXISTS `deleted_chars`;

CREATE TABLE `deleted_chars` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `char_guid` int(10) unsigned NOT NULL,
  `oldname` varchar(15) NOT NULL,
  `acc` int(11) NOT NULL,
  `date` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `game_event_condition_save` */

DROP TABLE IF EXISTS `game_event_condition_save`;

CREATE TABLE `game_event_condition_save` (
  `event_id` mediumint(8) unsigned NOT NULL,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `done` float DEFAULT '0',
  PRIMARY KEY (`event_id`,`condition_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `game_event_save` */

DROP TABLE IF EXISTS `game_event_save`;

CREATE TABLE `game_event_save` (
  `event_id` mediumint(8) unsigned NOT NULL,
  `state` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `next_start` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `gm_tickets` */

DROP TABLE IF EXISTS `gm_tickets`;

CREATE TABLE `gm_tickets` (
  `guid` int(10) NOT NULL AUTO_INCREMENT,
  `playerGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `name` varchar(15) NOT NULL,
  `message` text NOT NULL,
  `createtime` int(10) NOT NULL,
  `map` int(11) NOT NULL DEFAULT '0',
  `posX` float NOT NULL DEFAULT '0',
  `posY` float NOT NULL DEFAULT '0',
  `posZ` float NOT NULL DEFAULT '0',
  `timestamp` int(10) NOT NULL DEFAULT '0',
  `assignedto` int(10) NOT NULL DEFAULT '0',
  `closed` int(10) NOT NULL DEFAULT '0',
  `comment` text NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Player System';

/*Table structure for table `group_instance` */

DROP TABLE IF EXISTS `group_instance`;

CREATE TABLE `group_instance` (
  `leaderGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `instance` int(11) unsigned NOT NULL DEFAULT '0',
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`leaderGuid`,`instance`),
  KEY `instance` (`instance`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `group_member` */

DROP TABLE IF EXISTS `group_member`;

CREATE TABLE `group_member` (
  `leaderGuid` int(11) unsigned NOT NULL,
  `memberGuid` int(11) unsigned NOT NULL,
  `assistant` tinyint(1) unsigned NOT NULL,
  `subgroup` smallint(6) unsigned NOT NULL,
  PRIMARY KEY (`leaderGuid`,`memberGuid`),
  KEY `Idx_memberGuid` (`memberGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=FIXED COMMENT='Groups';

/*Table structure for table `group_saved_loot` */

DROP TABLE IF EXISTS `group_saved_loot`;

CREATE TABLE `group_saved_loot` (
  `creatureId` int(11) NOT NULL DEFAULT '0',
  `instanceId` int(11) NOT NULL DEFAULT '0',
  `itemId` int(11) NOT NULL DEFAULT '0',
  `itemCount` int(11) DEFAULT NULL,
  `summoned` tinyint(1) DEFAULT '0',
  `position_x` float DEFAULT NULL,
  `position_y` float DEFAULT NULL,
  `position_z` float DEFAULT NULL,
  `playerGuids` varchar(1024) DEFAULT NULL,
  PRIMARY KEY (`creatureId`,`instanceId`,`itemId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `groups` */

DROP TABLE IF EXISTS `groups`;

CREATE TABLE `groups` (
  `leaderGuid` int(11) unsigned NOT NULL,
  `mainTank` int(11) unsigned NOT NULL,
  `mainAssistant` int(11) unsigned NOT NULL,
  `lootMethod` tinyint(4) unsigned NOT NULL,
  `looterGuid` int(11) unsigned NOT NULL,
  `lootThreshold` tinyint(4) unsigned NOT NULL,
  `icon1` int(11) unsigned NOT NULL,
  `icon2` int(11) unsigned NOT NULL,
  `icon3` int(11) unsigned NOT NULL,
  `icon4` int(11) unsigned NOT NULL,
  `icon5` int(11) unsigned NOT NULL,
  `icon6` int(11) unsigned NOT NULL,
  `icon7` int(11) unsigned NOT NULL,
  `icon8` int(11) unsigned NOT NULL,
  `isRaid` tinyint(1) unsigned NOT NULL,
  `difficulty` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`leaderGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=FIXED COMMENT='Groups';

/*Table structure for table `guild` */

DROP TABLE IF EXISTS `guild`;

CREATE TABLE `guild` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `leaderguid` int(6) unsigned NOT NULL DEFAULT '0',
  `EmblemStyle` int(5) NOT NULL DEFAULT '0',
  `EmblemColor` int(5) NOT NULL DEFAULT '0',
  `BorderStyle` int(5) NOT NULL DEFAULT '0',
  `BorderColor` int(5) NOT NULL DEFAULT '0',
  `BackgroundColor` int(5) NOT NULL DEFAULT '0',
  `info` text NOT NULL,
  `motd` varchar(255) NOT NULL DEFAULT '',
  `createdate` datetime DEFAULT NULL,
  `BankMoney` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`),
  KEY `guildid` (`guildid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

/*Table structure for table `guild_bank_eventlog` */

DROP TABLE IF EXISTS `guild_bank_eventlog`;

CREATE TABLE `guild_bank_eventlog` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `LogGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `LogEntry` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `PlayerGuid` int(11) unsigned NOT NULL DEFAULT '0',
  `ItemOrMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `ItemStackCount` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `DestTabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `TimeStamp` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`LogGuid`),
  KEY `Idx_PlayerGuid` (`PlayerGuid`),
  KEY `Idx_LogGuid` (`LogGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `guild_bank_item` */

DROP TABLE IF EXISTS `guild_bank_item`;

CREATE TABLE `guild_bank_item` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `SlotId` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `item_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `item_entry` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`TabId`,`SlotId`),
  KEY `guildid_key` (`guildid`),
  KEY `Idx_item_guid` (`item_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `guild_bank_right` */

DROP TABLE IF EXISTS `guild_bank_right`;

CREATE TABLE `guild_bank_right` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `rid` int(11) unsigned NOT NULL DEFAULT '0',
  `gbright` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `SlotPerDay` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`TabId`,`rid`),
  KEY `guildid_key` (`guildid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `guild_bank_tab` */

DROP TABLE IF EXISTS `guild_bank_tab`;

CREATE TABLE `guild_bank_tab` (
  `guildid` int(11) unsigned NOT NULL DEFAULT '0',
  `TabId` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `TabName` varchar(100) NOT NULL DEFAULT '',
  `TabIcon` varchar(100) NOT NULL DEFAULT '',
  `TabText` text,
  PRIMARY KEY (`guildid`,`TabId`),
  KEY `guildid_key` (`guildid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `guild_eventlog` */

DROP TABLE IF EXISTS `guild_eventlog`;

CREATE TABLE `guild_eventlog` (
  `guildid` int(11) NOT NULL COMMENT 'Guild Identificator',
  `LogGuid` int(11) NOT NULL COMMENT 'Log entry identificator',
  `EventType` tinyint(1) NOT NULL COMMENT 'Event type',
  `PlayerGuid1` int(11) NOT NULL COMMENT 'Player 1',
  `PlayerGuid2` int(11) NOT NULL COMMENT 'Player 2',
  `NewRank` tinyint(2) NOT NULL COMMENT 'New rank(in case promotion/demotion)',
  `TimeStamp` bigint(20) NOT NULL COMMENT 'Event UNIX time',
  PRIMARY KEY (`guildid`,`LogGuid`),
  KEY `Idx_PlayerGuid1` (`PlayerGuid1`),
  KEY `Idx_PlayerGuid2` (`PlayerGuid2`),
  KEY `Idx_LogGuid` (`LogGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Guild Eventlog';

/*Table structure for table `guild_member` */

DROP TABLE IF EXISTS `guild_member`;

CREATE TABLE `guild_member` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `rank` tinyint(2) unsigned NOT NULL DEFAULT '0',
  `pnote` varchar(255) NOT NULL DEFAULT '',
  `offnote` varchar(255) NOT NULL DEFAULT '',
  `BankResetTimeMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemMoney` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab0` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab0` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab1` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab1` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab2` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab2` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab3` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab3` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab4` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab4` int(11) unsigned NOT NULL DEFAULT '0',
  `BankResetTimeTab5` int(11) unsigned NOT NULL DEFAULT '0',
  `BankRemSlotsTab5` int(11) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `guid_key` (`guid`),
  KEY `guildid_key` (`guildid`),
  KEY `guildid_rank_key` (`guildid`,`rank`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=FIXED COMMENT='Guild System';

/*Table structure for table `guild_rank` */

DROP TABLE IF EXISTS `guild_rank`;

CREATE TABLE `guild_rank` (
  `guildid` int(6) unsigned NOT NULL DEFAULT '0',
  `rid` int(11) unsigned NOT NULL,
  `rname` varchar(255) NOT NULL DEFAULT '',
  `rights` int(3) unsigned NOT NULL DEFAULT '0',
  `BankMoneyPerDay` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guildid`,`rid`),
  KEY `Idx_rid` (`rid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

/*Table structure for table `hid_rat` */

DROP TABLE IF EXISTS `hid_rat`;

CREATE TABLE `hid_rat` (
  `guid` int(11) unsigned NOT NULL,
  `rating2` int(10) unsigned NOT NULL DEFAULT '1500',
  `rating3` int(10) unsigned NOT NULL DEFAULT '1500',
  `rating5` int(10) unsigned NOT NULL DEFAULT '1500',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `hidden_rating` */

DROP TABLE IF EXISTS `hidden_rating`;

CREATE TABLE `hidden_rating` (
  `guid` int(11) unsigned NOT NULL,
  `rating2` int(10) unsigned NOT NULL DEFAULT '1500',
  `rating3` int(10) unsigned NOT NULL DEFAULT '1500',
  `rating5` int(10) unsigned NOT NULL DEFAULT '1500',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `instance` */

DROP TABLE IF EXISTS `instance`;

CREATE TABLE `instance` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `map` int(11) unsigned NOT NULL DEFAULT '0',
  `resettime` bigint(40) NOT NULL DEFAULT '0',
  `difficulty` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `data` longtext,
  PRIMARY KEY (`id`),
  KEY `map` (`map`),
  KEY `resettime` (`resettime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `instance_reset` */

DROP TABLE IF EXISTS `instance_reset`;

CREATE TABLE `instance_reset` (
  `mapid` int(11) unsigned NOT NULL DEFAULT '0',
  `resettime` bigint(40) NOT NULL DEFAULT '0',
  PRIMARY KEY (`mapid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `item_instance` */

DROP TABLE IF EXISTS `item_instance`;

CREATE TABLE `item_instance` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0',
  `owner_guid` int(11) unsigned NOT NULL DEFAULT '0',
  `data` longtext,
  PRIMARY KEY (`guid`),
  KEY `idx_owner_guid` (`owner_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Item System';

/*Table structure for table `item_text` */

DROP TABLE IF EXISTS `item_text`;

CREATE TABLE `item_text` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `text` longtext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=FIXED COMMENT='Item System';

/*Table structure for table `mail` */

DROP TABLE IF EXISTS `mail`;

CREATE TABLE `mail` (
  `id` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Identifier',
  `messageType` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `stationery` tinyint(3) NOT NULL DEFAULT '41',
  `mailTemplateId` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `sender` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `receiver` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  `subject` longtext,
  `itemTextId` int(11) unsigned NOT NULL DEFAULT '0',
  `has_items` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `expire_time` bigint(40) NOT NULL DEFAULT '0',
  `deliver_time` bigint(40) NOT NULL DEFAULT '0',
  `money` int(11) unsigned NOT NULL DEFAULT '0',
  `cod` int(11) unsigned NOT NULL DEFAULT '0',
  `checked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `idx_receiver` (`receiver`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Mail System';

/*Table structure for table `mail_items` */

DROP TABLE IF EXISTS `mail_items`;

CREATE TABLE `mail_items` (
  `mail_id` int(11) NOT NULL DEFAULT '0',
  `item_guid` int(11) NOT NULL DEFAULT '0',
  `item_template` int(11) NOT NULL DEFAULT '0',
  `receiver` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Character Global Unique Identifier',
  PRIMARY KEY (`mail_id`,`item_guid`),
  KEY `idx_receiver` (`receiver`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `pet_aura` */

DROP TABLE IF EXISTS `pet_aura`;

CREATE TABLE `pet_aura` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'Full Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0',
  `effect_index` int(11) unsigned NOT NULL DEFAULT '0',
  `stackcount` int(11) NOT NULL DEFAULT '1',
  `amount` int(11) NOT NULL DEFAULT '0',
  `maxduration` int(11) NOT NULL DEFAULT '0',
  `remaintime` int(11) NOT NULL DEFAULT '0',
  `remaincharges` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`,`effect_index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

/*Table structure for table `pet_spell` */

DROP TABLE IF EXISTS `pet_spell`;

CREATE TABLE `pet_spell` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `slot` int(11) unsigned NOT NULL DEFAULT '0',
  `active` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Pet System';

/*Table structure for table `pet_spell_cooldown` */

DROP TABLE IF EXISTS `pet_spell_cooldown`;

CREATE TABLE `pet_spell_cooldown` (
  `guid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `spell` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'Spell Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC;

/*Table structure for table `petition` */

DROP TABLE IF EXISTS `petition`;

CREATE TABLE `petition` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(10) unsigned DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ownerguid`,`type`),
  UNIQUE KEY `index_ownerguid_petitionguid` (`ownerguid`,`petitionguid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

/*Table structure for table `petition_sign` */

DROP TABLE IF EXISTS `petition_sign`;

CREATE TABLE `petition_sign` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(11) unsigned NOT NULL DEFAULT '0',
  `playerguid` int(11) unsigned NOT NULL DEFAULT '0',
  `player_account` int(11) unsigned NOT NULL DEFAULT '0',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`petitionguid`,`playerguid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='Guild System';

/*Table structure for table `saved_variables` */

DROP TABLE IF EXISTS `saved_variables`;

CREATE TABLE `saved_variables` (
  `NextArenaPointDistributionTime` bigint(40) unsigned NOT NULL DEFAULT '0',
  `HeroicQuest` int(5) unsigned NOT NULL DEFAULT '0',
  `NormalQuest` int(5) unsigned NOT NULL DEFAULT '0',
  `CookingQuest` int(5) unsigned NOT NULL DEFAULT '0',
  `FishingQuest` int(5) unsigned NOT NULL DEFAULT '0',
  `PVPAlliance` int(5) unsigned NOT NULL DEFAULT '0',
  `PVPHorde` int(5) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Variable Saves';

/*Table structure for table `tempat` */

DROP TABLE IF EXISTS `tempat`;

CREATE TABLE `tempat` (
  `arenateamid` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`arenateamid`),
  KEY `arenateamid` (`arenateamid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `topki` */

DROP TABLE IF EXISTS `topki`;

CREATE TABLE `topki` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `url` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Table structure for table `tp_history` */

DROP TABLE IF EXISTS `tp_history`;

CREATE TABLE `tp_history` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `tid` int(100) NOT NULL,
  `login` varchar(32) NOT NULL,
  `time` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/* Trigger structure for table `characters` */

DELIMITER $$

/*!50003 DROP TRIGGER*//*!50032 IF EXISTS */ /*!50003 `accchange_upd_check` */$$

/*!50003 CREATE */ /*!50017 DEFINER = 'other'@'83.12.219.58' */ /*!50003 TRIGGER `accchange_upd_check` BEFORE UPDATE ON `characters` FOR EACH ROW BEGIN
	IF NEW.account != OLD.account THEN
		INSERT INTO realmdt.accchange_log VALUES ('XXX', OLD.account, NEW.account, NOW(), NEW.guid);
	END IF;
  END */$$


DELIMITER ;

/* Procedure structure for procedure `PreventCharDelete` */

/*!50003 DROP PROCEDURE IF EXISTS  `PreventCharDelete` */;

DELIMITER $$

/*!50003 CREATE DEFINER=`root`@`localhost` PROCEDURE `PreventCharDelete`(IN charguid INT UNSIGNED)
BEGIN
    INSERT INTO deleted_chars VALUES ('XXX', charguid, (SELECT name FROM characters WHERE guid = charguid), (SELECT account FROM characters WHERE guid = charguid), CAST(NOW() AS DATETIME));
    UPDATE characters SET account = 1 WHERE guid = charguid;
    UPDATE characters SET name = CONCAT('DEL', name, 'DEL') WHERE guid = charguid;
    DELETE FROM character_social WHERE guid = charguid OR friend = charguid;
    DELETE FROM mail WHERE receiver = charguid;
    DELETE FROM mail_items WHERE receiver = charguid;
END */$$
DELIMITER ;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
