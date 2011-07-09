ALTER TABLE `account` ADD COLUMN `last_local_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0' AFTER `last_ip`;
ALTER TABLE `account_login` ADD COLUMN `local_ip` varchar(30) NOT NULL AFTER `ip`;
