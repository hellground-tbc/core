-- fix trinket Shifting Naaru Silver - little hacky - works for all allies who is in circle.
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 45043;
INSERT INTO `spell_linked_spell` VALUES (45043, 45044, 2, 'Shifting Naaru Silver trinket');
-- Not sure in those - was in oregon
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 40214;
INSERT INTO `spell_linked_spell` VALUES (40214, 42016, 2, 'Dragonmaw Illusion');