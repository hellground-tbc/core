alter table `account` drop column `customRates`,change `opcodesDisabled` `opcodesDisabled` int(11) default '0' NOT NULL, change `speciallogs` `account_flags` double default '0' NULL ;
