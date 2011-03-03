DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN ('-30421','-30422','-30423','38637','38638','38639');
INSERT INTO `spell_linked_spell`(`spell_trigger`,`spell_effect`,`type`,`comment`) VALUES
('-30421','38637','0','Netherspite\'s Perseverence'),
('-30422','38638','0','Netherspite\'s Serenity'),
('-30423','38639','0','Netherspite\'s Dominance');