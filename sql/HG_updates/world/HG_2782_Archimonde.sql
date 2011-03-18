#Elf Village NPCs nerf
UPDATE creature_template SET mindmg = 350, maxdmg = 750, attackpower = 0, flags_extra = 2 where entry IN (17944, 3794, 3795, 17943, 18487, 18486, 18485, 17945, 17944, 18502, 17948);

#Remove Doomfire
INSERT INTO spell_linked_spell VALUES (31224, -31944, 0, "Cloak of Shadows - Doomfire");

#Archi loot:
UPDATE creature_template SET lootid = 17968 WHERE entry = 17968;

#Archi immune:
UPDATE creature_template SET mechanic_immune_mask = 617299963 WHERE entry = 17968;
