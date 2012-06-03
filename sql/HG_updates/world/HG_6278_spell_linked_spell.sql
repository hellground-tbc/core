-- remove spell_linked_spells related to Illidan Encounter since those are handled by core and applying them with DB would cause double effect
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 41126;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -41914;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = -41917;