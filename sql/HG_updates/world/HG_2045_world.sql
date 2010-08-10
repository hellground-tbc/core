#Advisors not immune to bleed effects
UPDATE creature_template SET mechanic_immune_mask=619659263 WHERE entry IN (20060, 20062, 20063, 20064);

#Kael'thas not immune to bleed effects
UPDATE creature_template SET mechanic_immune_mask=617299963 WHERE entry=19622;