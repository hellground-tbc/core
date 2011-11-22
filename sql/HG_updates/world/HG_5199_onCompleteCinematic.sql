DROP TABLE IF EXISTS `completed_cinematic_scripts`;
CREATE TABLE `completed_cinematic_scripts` (
  `entry` MEDIUMINT(8) NOT NULL,
  `ScriptName` CHAR(64) NOT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8 COMMENT='Script library scripted completed cinematic events';