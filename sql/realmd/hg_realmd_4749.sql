ALTER TABLE realmlist
  DROP COLUMN color;
ALTER TABLE realmlist
  ADD COLUMN realmflags tinyint(3) unsigned NOT NULL default '2' COMMENT 'Supported masks: 0x1 (invalid, not show in realm list), 0x2 (offline, set by core), 0x4 (show version and build), 0x20 (recommended), 0x40 (new players)' AFTER icon,
  ADD COLUMN realmbuilds varchar(64) NOT NULL default '' AFTER population;
