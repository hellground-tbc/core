-- fix trinket Shifting Naaru Silver - little hacky - works for all allies who is in circle.
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 45043;
INSERT INTO `spell_linked_spell` VALUES (45043, 45044, 2, 'Shifting Naaru Silver trinket');
-- Not sure in those - was in oregon
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 41126;
INSERT INTO `spell_linked_spell` VALUES (41126, 41131, 1, 'Flame Crash');
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -41914;
INSERT INTO `spell_linked_spell` VALUES (-41914, 41915, 0, 'Summon Parasitic Shadowfiend');
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -41917;
INSERT INTO `spell_linked_spell` VALUES (-41917, 45044, 0, 'Summon Parasitic Shadowfiend');
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 40214;
INSERT INTO `spell_linked_spell` VALUES (40214, 42016, 2, 'Dragonmaw Illusion');