INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('20619', '1', '11664'),('20619', '1', '11663'),('21075', '1', '11664'),('21075', '1', '11663'),('20619', '1', '12018'),('21075', '1', '12018');

update CREATURE_TEMPLATE set speed = 3 WHERE entry in (11663, 11664);

UPDATE `gameobject_template` SET `type`='6', `flags`='4', `data2`='15', `data3`='20532', `data4`='0', `data5`='1' WHERE (`entry`='177000');

