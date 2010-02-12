UPDATE creature_template SET mindmg      = round(mindmg + attackpower / 14);
UPDATE creature_template SET maxdmg      = round(maxdmg + attackpower / 14);
UPDATE creature_template SET attackpower = round((maxdmg + mindmg) * 0.3);
UPDATE creature_template SET mindmg      = round(mindmg * 0.7), maxdmg = round(maxdmg * 0.7);