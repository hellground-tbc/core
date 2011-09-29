ALTER TABLE auctionhouse
  ADD COLUMN houseid int(11) unsigned NOT NULL default '0' AFTER id;

UPDATE auctionhouse SET houseid = 6 WHERE auctioneerguid IN (6601, 4659, 4656); # Orgrimmar
UPDATE auctionhouse SET houseid = 4 WHERE auctioneerguid IN (32064, 32059, 32065, 32063, 32060, 32058, 32061, 32062); # Undercity
UPDATE auctionhouse SET houseid = 5 WHERE auctioneerguid IN (26769, 24708); # Thunder Bluff
UPDATE auctionhouse SET houseid = 6 WHERE auctioneerguid IN (63360, 63361, 67041, 63359, 57622, 57624, 57623); # Silvermoon City
UPDATE auctionhouse SET houseid = 1 WHERE auctioneerguid IN (79705, 79706, 79707); # Stormwind
UPDATE auctionhouse SET houseid = 2 WHERE auctioneerguid IN (127, 126, 119); # Ironforge
UPDATE auctionhouse SET houseid = 3 WHERE auctioneerguid IN (46243, 43240, 46190, 49214); # Darnassus
UPDATE auctionhouse SET houseid = 2 WHERE auctioneerguid IN (85606, 85605, 85604); # Exodar
UPDATE auctionhouse SET houseid = 7 WHERE auctioneerguid IN (23442); # Tanaris
UPDATE auctionhouse SET houseid = 7 WHERE auctioneerguid IN (642, 8, 570, 569, 568); # Booty Bay
UPDATE auctionhouse SET houseid = 7 WHERE auctioneerguid IN (42203); # Everlook

ALTER TABLE auctionhouse
  DROP COLUMN auctioneerguid;

#ALTER TABLE `auctionhouse`
#  ADD COLUMN `moneyTime` BIGINT(40) DEFAULT '0' NOT NULL AFTER `time`;

ALTER TABLE `auctionhouse`
  DROP KEY `item_guid`,
  ADD COLUMN `item_count` int(11) unsigned NOT NULL default '0' AFTER `item_template`,
  ADD COLUMN `item_randompropertyid` int(11) NOT NULL default '0' AFTER `item_count`;


UPDATE auctionhouse, item_instance
  SET auctionhouse.item_count = SUBSTRING_INDEX(SUBSTRING_INDEX(item_instance.data, ' ', 14 + 1), ' ', -1)  WHERE auctionhouse.itemguid = item_instance.guid;

UPDATE mail_items, mail
  SET mail_items.receiver = mail.receiver WHERE mail.id = mail_items.mail_id;
