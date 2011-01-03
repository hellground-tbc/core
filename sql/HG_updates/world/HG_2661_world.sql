-- allow to loot "Piece of Threshadon Carcass" properly
UPDATE gameobject_loot_template SET lootcondition = 9, condition_value1 = 4290 WHERE entry = 11460;
-- delete Larkorwi Mate, only summonable by areatrigger
DELETE FROM creature WHERE id=9683;