#-- add triggers to chess event
#--backup to jest!!
/*delete from `creature` where (`id` IN (22519, 17459, 16816)) and `map` = 532;
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11078.4','-1857.28','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11071.2','-1865.44','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11074.7','-1861.32','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11067.6','-1869.92','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11082','-1852.62','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11074.2','-1853.46','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11077.8','-1848.72','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11064','-1874.51','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11060.9','-1878.79','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11088.1','-1871.21','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11082.1','-1888.3','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11085.2','-1883.88','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11103.6','-1869.67','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11092.3','-1875.02','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11095.7','-1870.69','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11099.5','-1866.32','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11079.6','-1864.84','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11083','-1860.62','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11086.5','-1855.82','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11090.6','-1859.26','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11087','-1863.74','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11083.4','-1868.07','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11091.5','-1867.34','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11078.5','-1892.6','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11080.4','-1872.4','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11084.3','-1875.8','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11088.8','-1879.49','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11093.4','-1882.5','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11089.8','-1887.26','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11086','-1891.58','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11083','-1895.89','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11079.3','-1900.4','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11057.2','-1883.18','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11075','-1897','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11095','-1862.82','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11072.5','-1873.48','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11069.9','-1885.58','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11069.1','-1877.82','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11077.7','-1884.61','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11081.1','-1880.24','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11076.7','-1876.65','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11065.5','-1882.34','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11062.2','-1886.5','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11073.2','-1881.37','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11076.2','-1868.9','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11074.2','-1888.95','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11066.2','-1889.93','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11070.6','-1893.41','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11071.2','-1857.5','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11067.8','-1861.95','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11063.5','-1866.52','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11060.3','-1870.96','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11057','-1874.88','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11053.2','-1879.64','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11083.5','-1903.97','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11087.1','-1899.65','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11090.5','-1895.05','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11094.1','-1890.8','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11097.5','-1886.66','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11100.8','-1881.68','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11104.3','-1877.5','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11108.4','-1873.1','220.668','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11099.9','-1873.89','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','22519','532','1','0','0','-11096.6','-1878.21','220.667','0','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17459','532','1','0','0','-11108.2','-1841.56','229.625','5.39745','25','0','0','8','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','16816','532','1','0','1512','-11099.8','-1852.19','221.071','5.35504','25','0','0','18000','0','0','0');

#--ten wrzucac, chwilowo powyzszego nie wyrzucam w celu backupu

delete from `creature` where (`id` IN (22519, 17459, 16816)) and `map` = 532;
INSERT INTO `creature` VALUES
('XXX', 16816, 532, 1, 0, 1512, -11099.8, -1852.19, 221.071, 5.35504, 25, 0, 0, 18000, 0, 0, 0),
('XXX', 17459, 532, 1, 0, 0, -11108.2, -1841.56, 229.625, 5.39745, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11078.4, -1857.28, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11071.2, -1865.44, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11074.7, -1861.32, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11067.6, -1869.92, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11082, -1852.62, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11074, -1853.52, 220.667, 3.85417, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11077.6, -1848.99, 220.668, 3.77956, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11064, -1874.51, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11060.9, -1878.79, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11088.1, -1871.21, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11082.1, -1888.3, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11085.2, -1883.88, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11103.6, -1869.67, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11092.3, -1875.02, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11095.7, -1870.69, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11099.5, -1866.32, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11079.6, -1864.84, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11083, -1860.62, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11086.5, -1855.82, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11090.6, -1859.26, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11087, -1863.74, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11083.4, -1868.07, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11091.5, -1867.34, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11078.5, -1892.6, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11080.4, -1872.4, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11084.3, -1875.8, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11088.8, -1879.49, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11093.4, -1882.5, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11089.8, -1887.26, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11086, -1891.58, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11083, -1895.89, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11079.3, -1900.4, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11057.2, -1883.18, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11075, -1897, 220.668, 0, 25, 0, 0, 8, 0, 0, 0),
('XXX', 22519, 532, 1, 0, 0, -11095, -1862.82, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11072.5, -1873.48, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11069.9, -1885.58, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11069.1, -1877.82, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11077.7, -1884.61, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11081.1, -1880.24, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11076.7, -1876.65, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11065.5, -1882.34, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11062.2, -1886.5, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11073.2, -1881.37, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11076.2, -1868.9, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11074.2, -1888.95, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11066.2, -1889.93, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11070.6, -1893.41, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11070.6, -1857.63, 220.668, 3.81883, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11067.1, -1862.05, 220.668, 3.8259, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11063.5, -1866.52, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11060.3, -1870.96, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11057, -1874.88, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11053.2, -1879.64, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11083.5, -1903.97, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11087.1, -1899.65, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11090.5, -1895.05, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11094.1, -1890.8, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11097.5, -1886.66, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11100.8, -1881.68, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11104.3, -1877.5, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11108.4, -1873.1, 220.668, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11099.9, -1873.89, 220.667, 0, 25, 0, 0, 8, 0, 0, 0), 
('XXX', 22519, 532, 1, 0, 0, -11096.6, -1878.21, 220.667, 0, 25, 0, 0, 8, 0, 0, 0);

#--add chess pieces to chess board
DELETE FROM creature WHERE (id IN (21752, 21750, 21747, 21748, 21726, 17469, 21683, 21684, 21682, 21664, 21160, 17211)) AND map = 532;
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11082.9','-1895.75','220.667','0.759299','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11079.4','-1900.17','220.667','0.649344','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11103.7','-1869.63','220.667','0.661124','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11100.2','-1873.96','220.668','0.597337','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11096.8','-1878.21','220.668','0.593409','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11089.7','-1887.03','220.667','0.617927','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11086.2','-1891.53','220.667','0.617928','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17211','532','1','0','0','-11093.4','-1882.59','220.668','0.671956','604800','0','0','35000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11082.1','-1852.43','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11071.5','-1865.54','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11061','-1878.56','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11064.3','-1874.3','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11067.8','-1869.89','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11057.4','-1883.03','220.667','3.813','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11078.5','-1856.93','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','17469','532','1','0','0','-11075','-1861.13','220.667','3.80908','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21160','532','1','0','0','-11108.1','-1872.91','220.667','0.679031','604800','0','0','53000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21160','532','1','0','0','-11083.7','-1903.94','220.667','0.671177','604800','0','0','53000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21664','532','1','0','0','-11104.4','-1877.7','220.667','0.59264','604800','0','0','48000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21664','532','1','0','0','-11087.2','-1899.56','220.667','0.624056','604800','0','0','48000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21682','532','1','0','0','-11101','-1881.83','220.667','0.598288','604800','0','0','47000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21682','532','1','0','0','-11090.5','-1895.02','220.667','0.665045','604800','0','0','47000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21683','532','1','0','0','-11097.6','-1886.43','220.667','0.723955','604800','0','0','58000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21684','532','1','0','0','-11094.1','-1890.57','220.667','0.62578','604800','0','0','150000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21726','532','1','0','133','-11077.7','-1849.04','220.667','3.82912','25','0','0','58000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21726','532','1','0','0','-11053.4','-1879.64','220.667','3.89304','604800','0','0','58000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21747','532','1','0','0','-11070.8','-1857.7','220.667','3.81692','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21747','532','1','0','0','-11060.3','-1870.81','220.667','3.76195','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21748','532','1','0','0','-11074','-1853.28','220.667','3.84199','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21748','532','1','0','0','-11056.7','-1875.07','220.667','3.7698','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21750','532','1','0','0','-11067','-1861.86','220.667','3.72268','604800','0','0','50000','0','0','0');
insert into `creature` (`guid`, `id`, `map`, `spawnMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) values('XXX','21752','532','1','0','0','-11063.6','-1866.47','220.667','3.84835','604800','0','0','150000','0','0','0');*/

#--add flags to chess pieces
UPDATE creature_template SET unit_flags = unit_flags | 530432, flags_extra | 2129920
WHERE entry IN (17469,17211,21748,21664,21750,21683,21747,21682,21726,21160,21752,21684); #-- pvp, incombat, pet_in_combat flags, extra flags: no_target, charm_ai
UPDATE creature_template SET flags_extra = flags_extra | 2 WHERE entry = 22519;           #-- ignore aggro for trigger
UPDATE creature_template SET unit_flags = unit_flags | 262148 WHERE entry = 22519;        #-- disable_Move + disable_rotate for trigger
UPDATE creature_template SET faction_A = 1691, faction_H = 1691 WHERE faction_A = 1689;   #-- set proper faction (same as used in script)

#--add spell_script_target's for spells used in chess event
DELETE FROM spell_script_target WHERE `entry` IN(37146,30012,37144,37148,37151,37152,37153,30532, 30284, 37462, 
37471, 37455, 37453, 37427, 37406, 37472, 37463, 37456, 37454, 37434, 37413, 37465, 37474, 37459, 37498, 37432, 37414, 37476, 37461, 37469, 37502, 37428, 37416,
32227, 32228, 37142, 37220, 37143, 37339, 37147, 37337, 37149, 37345, 37150, 37348);
INSERT INTO spell_script_target () VALUES (37146,1,22519);#--8
INSERT INTO spell_script_target () VALUES (30012,1,22519);#--unlimited
INSERT INTO spell_script_target () VALUES (37144,1,22519);#--15
INSERT INTO spell_script_target () VALUES (37148,1,22519);#--20
INSERT INTO spell_script_target () VALUES (37151,1,22519);#--8
INSERT INTO spell_script_target () VALUES (37152,1,22519);#--8
INSERT INTO spell_script_target () VALUES (37153,1,22519);#--8
INSERT INTO spell_script_target () VALUES (30532,1,17460);
INSERT INTO spell_script_target () VALUES (30284,1,22519);#--Change Facing




#----melee spells

#--Melee Attack: Footman
INSERT INTO spell_script_target () VALUES (32227,1,21752);#--Melee Attack: Footman -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (32227,1,21750);#--Melee Attack: Footman -> Orc Warlock
INSERT INTO spell_script_target () VALUES (32227,1,21747);#--Melee Attack: Footman -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (32227,1,21748);#--Melee Attack: Footman -> Orc Wolf
INSERT INTO spell_script_target () VALUES (32227,1,21726);#--Melee Attack: Footman -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (32227,1,17469);#--Melee Attack: Footman -> Orc Grunt

#--Melee Attack: Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37142,1,21752);#--Melee Attack: Conjured Water Elemental -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37142,1,21750);#--Melee Attack: Conjured Water Elemental -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37142,1,21747);#--Melee Attack: Conjured Water Elemental -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37142,1,21748);#--Melee Attack: Conjured Water Elemental -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37142,1,21726);#--Melee Attack: Conjured Water Elemental -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37142,1,17469);#--Melee Attack: Conjured Water Elemental -> Orc Grunt

#--Melee Attack: Conjured Water Elemental2
INSERT INTO spell_script_target () VALUES (37143,1,21752);#--Melee Attack: Conjured Water Elemental2 -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37143,1,21750);#--Melee Attack: Conjured Water Elemental2 -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37143,1,21747);#--Melee Attack: Conjured Water Elemental2 -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37143,1,21748);#--Melee Attack: Conjured Water Elemental2 -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37143,1,21726);#--Melee Attack: Conjured Water Elemental2 -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37143,1,17469);#--Melee Attack: Conjured Water Elemental2 -> Orc Grunt

#--Melee Attack: Human Cleric
INSERT INTO spell_script_target () VALUES (37147,1,21752);#--Melee Attack: Human Cleric -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37147,1,21750);#--Melee Attack: Human Cleric -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37147,1,21747);#--Melee Attack: Human Cleric -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37147,1,21748);#--Melee Attack: Human Cleric -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37147,1,21726);#--Melee Attack: Human Cleric -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37147,1,17469);#--Melee Attack: Human Cleric -> Orc Grunt

#--Melee Attack: Human Conjurer
INSERT INTO spell_script_target () VALUES (37149,1,21752);#--Melee Attack: Human Conjurer -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37149,1,21750);#--Melee Attack: Human Conjurer -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37149,1,21747);#--Melee Attack: Human Conjurer -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37149,1,21748);#--Melee Attack: Human Conjurer -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37149,1,21726);#--Melee Attack: Human Conjurer -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37149,1,17469);#--Melee Attack: Human Conjurer -> Orc Grunt

#--Melee Attack: King Llane
INSERT INTO spell_script_target () VALUES (37150,1,21752);#--Melee Attack: King Llane -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37150,1,21750);#--Melee Attack: King Llane -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37150,1,21747);#--Melee Attack: King Llane -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37150,1,21748);#--Melee Attack: King Llane -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37150,1,21726);#--Melee Attack: King Llane -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37150,1,17469);#--Melee Attack: King Llane -> Orc Grunt



#--Melee Attack: Grunt
INSERT INTO spell_script_target () VALUES (32228,1,21683);#--Melee Attack: Grunt -> Human Conjurer
INSERT INTO spell_script_target () VALUES (32228,1,21684);#--Melee Attack: Grunt -> King Llane
INSERT INTO spell_script_target () VALUES (32228,1,21682);#--Melee Attack: Grunt -> Human Cleric
INSERT INTO spell_script_target () VALUES (32228,1,21664);#--Melee Attack: Grunt -> Human Charger
INSERT INTO spell_script_target () VALUES (32228,1,21160);#--Melee Attack: Grunt -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (32228,1,17211);#--Melee Attack: Grunt -> Human Footman

#--Melee Attack: Summoned Daemon
INSERT INTO spell_script_target () VALUES (37220,1,21683);#--Melee Attack: Summoned Daemon -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37220,1,21684);#--Melee Attack: Summoned Daemon -> King Llane
INSERT INTO spell_script_target () VALUES (37220,1,21682);#--Melee Attack: Summoned Daemon -> Human Cleric
INSERT INTO spell_script_target () VALUES (37220,1,21664);#--Melee Attack: Summoned Daemon -> Human Charger
INSERT INTO spell_script_target () VALUES (37220,1,21160);#--Melee Attack: Summoned Daemon -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37220,1,17211);#--Melee Attack: Summoned Daemon -> Human Footman

#--Melee Attack: Orc Wolf
INSERT INTO spell_script_target () VALUES (37339,1,21683);#--Melee Attack: Orc Wolf -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37339,1,21684);#--Melee Attack: Orc Wolf -> King Llane
INSERT INTO spell_script_target () VALUES (37339,1,21682);#--Melee Attack: Orc Wolf -> Human Cleric
INSERT INTO spell_script_target () VALUES (37339,1,21664);#--Melee Attack: Orc Wolf -> Human Charger
INSERT INTO spell_script_target () VALUES (37339,1,21160);#--Melee Attack: Orc Wolf -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37339,1,17211);#--Melee Attack: Orc Wolf -> Human Footman

#--Melee Attack: Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37337,1,21683);#--Melee Attack: Orc Necrolyte -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37337,1,21684);#--Melee Attack: Orc Necrolyte -> King Llane
INSERT INTO spell_script_target () VALUES (37337,1,21682);#--Melee Attack: Orc Necrolyte -> Human Cleric
INSERT INTO spell_script_target () VALUES (37337,1,21664);#--Melee Attack: Orc Necrolyte -> Human Charger
INSERT INTO spell_script_target () VALUES (37337,1,21160);#--Melee Attack: Orc Necrolyte -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37337,1,17211);#--Melee Attack: Orc Necrolyte -> Human Footman

#--Melee Attack: Orc Warlock
INSERT INTO spell_script_target () VALUES (37345,1,21683);#--Melee Attack: Orc Warlock -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37345,1,21684);#--Melee Attack: Orc Warlock -> King Llane
INSERT INTO spell_script_target () VALUES (37345,1,21682);#--Melee Attack: Orc Warlock -> Human Cleric
INSERT INTO spell_script_target () VALUES (37345,1,21664);#--Melee Attack: Orc Warlock -> Human Charger
INSERT INTO spell_script_target () VALUES (37345,1,21160);#--Melee Attack: Orc Warlock -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37345,1,17211);#--Melee Attack: Orc Warlock -> Human Footman

#--Melee Attack: Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37348,1,21683);#--Melee Attack: Warchief Blackhand -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37348,1,21684);#--Melee Attack: Warchief Blackhand -> King Llane
INSERT INTO spell_script_target () VALUES (37348,1,21682);#--Melee Attack: Warchief Blackhand -> Human Cleric
INSERT INTO spell_script_target () VALUES (37348,1,21664);#--Melee Attack: Warchief Blackhand -> Human Charger
INSERT INTO spell_script_target () VALUES (37348,1,21160);#--Melee Attack: Warchief Blackhand -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37348,1,17211);#--Melee Attack: Warchief Blackhand -> Human Footman


#----ability 1 alliance

#--Elemental Blast - any enemy piece
INSERT INTO spell_script_target () VALUES (37462,1,21752);#--Elemental Blast -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37462,1,21750);#--Elemental Blast -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37462,1,21747);#--Elemental Blast -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37462,1,21748);#--Elemental Blast -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37462,1,21726);#--Elemental Blast -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37462,1,17469);#--Elemental Blast -> Orc Grunt
#--Heroism - all allied not self
INSERT INTO spell_script_target () VALUES (37471,1,21683);#--Heroism -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37471,1,21682);#--Heroism -> Human Cleric
INSERT INTO spell_script_target () VALUES (37471,1,21664);#--Heroism -> Human Charger
INSERT INTO spell_script_target () VALUES (37471,1,21160);#--Heroism -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37471,1,17211);#--Heroism -> Human Footman
#--Healing - any friendly piece
INSERT INTO spell_script_target () VALUES (37455,1,21683);#--Healing -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37455,1,21684);#--Healing -> King Llane
INSERT INTO spell_script_target () VALUES (37455,1,21682);#--Healing -> Human Cleric
INSERT INTO spell_script_target () VALUES (37455,1,21664);#--Healing -> Human Charger
INSERT INTO spell_script_target () VALUES (37455,1,21160);#--Healing -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37455,1,17211);#--Healing -> Human Footman
#--Smash - any enemy piece
INSERT INTO spell_script_target () VALUES (37453,1,21752);#--Smash -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37453,1,21750);#--Smash -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37453,1,21747);#--Smash -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37453,1,21748);#--Smash -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37453,1,21726);#--Smash -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37453,1,17469);#--Smash -> Orc Grunt
#--Geyser - all enemy prieces
INSERT INTO spell_script_target () VALUES (37427,1,21752);#--Geyser -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37427,1,21750);#--Geyser -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37427,1,21747);#--Geyser -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37427,1,21748);#--Geyser -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37427,1,21726);#--Geyser -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37427,1,17469);#--Geyser -> Orc Grunt
#--Heroic Blow - any enemy piece
INSERT INTO spell_script_target () VALUES (37406,1,21752);#--Heroic Blow -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37406,1,21750);#--Heroic Blow -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37406,1,21747);#--Heroic Blow -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37406,1,21748);#--Heroic Blow -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37406,1,21726);#--Heroic Blow -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37406,1,17469);#--Heroic Blow -> Orc Grunt

#----ability 1 horde

#--Bloodlust - all allied not self
INSERT INTO spell_script_target () VALUES (37472,1,21750);#--Bloodlust -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37472,1,21747);#--Bloodlust -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37472,1,21748);#--Bloodlust -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37472,1,21726);#--Bloodlust -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37472,1,17469);#--Bloodlust -> Orc Grunt
#--Fireball - any enemy piece
INSERT INTO spell_script_target () VALUES (37463,1,21683);#--Fireball -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37463,1,21684);#--Fireball -> King Llane
INSERT INTO spell_script_target () VALUES (37463,1,21682);#--Fireball -> Human Cleric
INSERT INTO spell_script_target () VALUES (37463,1,21664);#--Fireball -> Human Charger
INSERT INTO spell_script_target () VALUES (37463,1,21160);#--Fireball -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37463,1,17211);#--Fireball -> Human Footman
#--Shadow Mend - any friendly piece
INSERT INTO spell_script_target () VALUES (37456,1,21752);#--Shadow Mend -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37456,1,21750);#--Shadow Mend -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37456,1,21747);#--Shadow Mend -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37456,1,21748);#--Shadow Mend -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37456,1,21726);#--Shadow Mend -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37456,1,17469);#--Shadow Mend -> Orc Grunt
#--Bite - any enemy piece
INSERT INTO spell_script_target () VALUES (37454,1,21683);#--Bite -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37454,1,21684);#--Bite -> King Llane
INSERT INTO spell_script_target () VALUES (37454,1,21682);#--Bite -> Human Cleric
INSERT INTO spell_script_target () VALUES (37454,1,21664);#--Bite -> Human Charger
INSERT INTO spell_script_target () VALUES (37454,1,21160);#--Bite -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37454,1,17211);#--Bite -> Human Footman
#--Fire Shield - only self
INSERT INTO spell_script_target () VALUES (37434,1,21726);#--Fire Shield -> Summoned Daemon
#--Vicious Strike - any enemy piece
INSERT INTO spell_script_target () VALUES (37413,1,21683);#--Vicious Strike -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37413,1,21684);#--Vicious Strike -> King Llane
INSERT INTO spell_script_target () VALUES (37413,1,21682);#--Vicious Strike -> Human Cleric
INSERT INTO spell_script_target () VALUES (37413,1,21664);#--Vicious Strike -> Human Charger
INSERT INTO spell_script_target () VALUES (37413,1,21160);#--Vicious Strike -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37413,1,17211);#--Vicious Strike -> Human Footman

#----ability 2 alliance

#--Rain of Fire - any enemy piece
INSERT INTO spell_script_target () VALUES (37465,1,21752);#--Rain of Fire -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37465,1,21750);#--Rain of Fire -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37465,1,21747);#--Rain of Fire -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37465,1,21748);#--Rain of Fire -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37465,1,21726);#--Rain of Fire -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37465,1,17469);#--Rain of Fire -> Orc Grunt
#--Sweep - all enemy pieces
INSERT INTO spell_script_target () VALUES (37474,1,21752);#--Sweep -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37474,1,21750);#--Sweep -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37474,1,21747);#--Sweep -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37474,1,21748);#--Sweep -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37474,1,21726);#--Sweep -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37474,1,17469);#--Sweep -> Orc Grunt
#--Holy Lance - all enemy pieces
INSERT INTO spell_script_target () VALUES (37459,1,21752);#--Holy Lance -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37459,1,21750);#--Holy Lance -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37459,1,21747);#--Holy Lance -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37459,1,21748);#--Holy Lance -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37459,1,21726);#--Holy Lance -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37459,1,17469);#--Holy Lance -> Orc Grunt
#--Stomp - all enemy pieces
INSERT INTO spell_script_target () VALUES (37498,1,21752);#--Stomp -> Warchief Blackhand
INSERT INTO spell_script_target () VALUES (37498,1,21750);#--Stomp -> Orc Warlock
INSERT INTO spell_script_target () VALUES (37498,1,21747);#--Stomp -> Orc Necrolyte
INSERT INTO spell_script_target () VALUES (37498,1,21748);#--Stomp -> Orc Wolf
INSERT INTO spell_script_target () VALUES (37498,1,21726);#--Stomp -> Summoned Daemon
INSERT INTO spell_script_target () VALUES (37498,1,17469);#--Stomp -> Orc Grunt
#--Water Shield - self only
INSERT INTO spell_script_target () VALUES (37432,1,21160);#--Water Shield -> Conjured Water Elemental
#--Shield Block - self only
INSERT INTO spell_script_target () VALUES (37414,1,17211);#--Shield Block ->Human Footman

#----ability 2 horde 

#--Cleave - all enemy pieces
INSERT INTO spell_script_target () VALUES (37476,1,21683);#--Cleave -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37476,1,21684);#--Cleave -> King Llane
INSERT INTO spell_script_target () VALUES (37476,1,21682);#--Cleave -> Human Cleric
INSERT INTO spell_script_target () VALUES (37476,1,21664);#--Cleave -> Human Charger
INSERT INTO spell_script_target () VALUES (37476,1,21160);#--Cleave -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37476,1,17211);#--Cleave -> Human Footman
#--Shadow Spear - any enemy piece
INSERT INTO spell_script_target () VALUES (37461,1,21683);#--Shadow Spear -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37461,1,21684);#--Shadow Spear -> King Llane
INSERT INTO spell_script_target () VALUES (37461,1,21682);#--Shadow Spear -> Human Cleric
INSERT INTO spell_script_target () VALUES (37461,1,21664);#--Shadow Spear -> Human Charger
INSERT INTO spell_script_target () VALUES (37461,1,21160);#--Shadow Spear -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37461,1,17211);#--Shadow Spear -> Human Footman
#--Poison Cloud - any enemy piece
INSERT INTO spell_script_target () VALUES (37469,1,21683);#--Poison Cloud -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37469,1,21684);#--Poison Cloud -> King Llane
INSERT INTO spell_script_target () VALUES (37469,1,21682);#--Poison Cloud -> Human Cleric
INSERT INTO spell_script_target () VALUES (37469,1,21664);#--Poison Cloud -> Human Charger
INSERT INTO spell_script_target () VALUES (37469,1,21160);#--Poison Cloud -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37469,1,17211);#--Poison Cloud -> Human Footman
#--Howl - all enemy pieces
INSERT INTO spell_script_target () VALUES (37502,1,21683);#--Howl -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37502,1,21684);#--Howl -> King Llane
INSERT INTO spell_script_target () VALUES (37502,1,21682);#--Howl -> Human Cleric
INSERT INTO spell_script_target () VALUES (37502,1,21664);#--Howl -> Human Charger
INSERT INTO spell_script_target () VALUES (37502,1,21160);#--Howl -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37502,1,17211);#--Howl -> Human Footman
#--Hellfire - all enemy pieces
INSERT INTO spell_script_target () VALUES (37428,1,21683);#--Hellfire -> Human Conjurer
INSERT INTO spell_script_target () VALUES (37428,1,21684);#--Hellfire -> King Llane
INSERT INTO spell_script_target () VALUES (37428,1,21682);#--Hellfire -> Human Cleric
INSERT INTO spell_script_target () VALUES (37428,1,21664);#--Hellfire -> Human Charger
INSERT INTO spell_script_target () VALUES (37428,1,21160);#--Hellfire -> Conjured Water Elemental
INSERT INTO spell_script_target () VALUES (37428,1,17211);#--Hellfire -> Human Footman
#--Weapon Deflection - self only
INSERT INTO spell_script_target () VALUES (37416,1,17469);#--Weapon Deflection -> Orc Grunt

#INSERT INTO spell_script_target () VALUES (,,);#--

#--update spells for chess pieces
UPDATE creature_template SET spell1=30284, spell2=37148, spell3=37462, spell4=37465 WHERE entry=21683;#--Human Conjurer
UPDATE creature_template SET spell1=30284, spell2=37151, spell3=37471, spell4=37474 WHERE entry=21684;#--King Llane
UPDATE creature_template SET spell1=30284, spell2=37152, spell3=37455, spell4=37459 WHERE entry=21682;#--Human Cleric
UPDATE creature_template SET spell1=30284, spell2=37144, spell3=37453, spell4=37498 WHERE entry=21664;#--Human Charger
UPDATE creature_template SET spell1=30284, spell2=37153, spell3=37427, spell4=37432 WHERE entry=21160;#--Conjured Water Elemental
UPDATE creature_template SET spell1=30284, spell2=37146, spell3=37406, spell4=37414 WHERE entry=17211;#--Human Footman
UPDATE creature_template SET spell1=30284, spell2=37151, spell3=37472, spell4=37476 WHERE entry=21752;#--Warchief Blackhand
UPDATE creature_template SET spell1=30284, spell2=37148, spell3=37463, spell4=37469 WHERE entry=21750;#--Orc Warlock
UPDATE creature_template SET spell1=30284, spell2=37152, spell3=37456, spell4=37461 WHERE entry=21747;#--Orc Necrolyte
UPDATE creature_template SET spell1=30284, spell2=37144, spell3=37454, spell4=37502 WHERE entry=21748;#--spell1=37339 Orc Wolf
UPDATE creature_template SET spell1=30284, spell2=37153, spell3=37434, spell4=37428 WHERE entry=21726;#--Summoned Daemon
UPDATE creature_template SET spell1=30284, spell2=37146, spell3=37413, spell4=37416 WHERE entry=17469;#--Orc Grunt

#--update stats for chess pieces
#----Human Footman, Orc Grunt
UPDATE creature_template 
SET minhealth = 50000, maxhealth = 50000, armor = 20, speed = 1.0, mindmg = 206, maxdmg = 206, attackpower = 4323
WHERE entry = 17469 OR entry = 17211;
#----Conjured Elemental, Summoned Demon
UPDATE creature_template 
SET minhealth = 80000, maxhealth = 80000, armor = 20, speed = 1.0, mindmg = 206, maxdmg = 206, attackpower = 4323
WHERE entry = 21160 OR entry = 21726;
#----Human Charger ,Orc Wolf
UPDATE creature_template 
SET minhealth = 65000, maxhealth = 65000, armor = 20, speed = 1.0, mindmg = 206, maxdmg = 206, attackpower = 4323
WHERE entry = 21664 OR entry = 21748;
#----Human Cleric, Orc Necrolyte
UPDATE creature_template 
SET minhealth = 60000, maxhealth = 60000, armor = 20, speed = 1.0, mindmg = 206, maxdmg = 206, attackpower = 4323
WHERE entry = 21682 OR entry = 21747;
#----Human Conjurer, Orc Warlock
UPDATE creature_template 
SET minhealth = 80000, maxhealth = 80000, armor = 20, speed = 1.0, mindmg = 206, maxdmg = 206, attackpower = 4323
WHERE entry = 21683 OR entry = 21750;
#----King Llane, Warchief Blackhand
UPDATE creature_template 
SET minhealth = 150000, maxhealth = 150000, armor = 6792, speed = 1.0, mindmg = 445, maxdmg = 909, attackpower = 14205
WHERE entry = 21684 OR entry = 21752;

#--update scale for "unused" pieces ;)
UPDATE creature_template 
SET scale = 0.5 
WHERE entry IN (16556, 16567, 16561, 16569, 16557, 16572, 16560, 16571, 16562, 16570, 16563, 16581);

##----
#UPDATE creature_template 
#SET minhealth = , maxhealth = , armor = , speed = , mindmg = , maxdmg = , attackpower = 
#WHERE entry =  OR entry = ;

#--add script_texts for chess event
DELETE FROM script_texts WHERE entry BETWEEN -1650000 and -1650021;
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650000','Very well. Let the game begin.',10338,0,'Chess Event - Event Start');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650001','Yes...all according to plan.',10349,0,'Chess Event - LOSE_KNIGHT_P');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650002','Yes...of course.',10350,0,'Chess Event - LOSE_KNIGHT_M');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650003','A transparent stratagem.',10339,0,'Chess Event - LOSE_PAWN_P_1');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650004','Let us see.',10340,0,'Chess Event - LOSE_PAWN_P_2');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650005','Ah, the wheels have begun to turn.',10341,0,'Chess Event - LOSE_PAWN_P_3');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650006','Hmm.',10342,0,'Chess Event - LOSE_PAWN_M_1');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650007','No matter.',10344,0,'Chess Event - LOSE_PAWN_M_2');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650008','Interesting.',10343,0,'Chess Event - LOSE_PAWN_M_3');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650009','Now it gets interesting.',10351,0,'Chess Event - LOSE_QUEEN_P');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650010','Ahh, I should have known..',10352,0,'Chess Event - LOSE_QUEEN_M');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650011','The slightest loss of concentration is all it takes.',10347,0,'Chess Event - LOSE_BISHOP_P');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650012','A necessary sacrifice.',10348,0,'Chess Event - LOSE_BISHOP_M');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650013','Foolish! Very foolish!',10345,0,'Chess Event - LOSE_ROOK_P');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650014','A minor concern.',10346,0,'Chess Event - LOSE_ROOK_M');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650015','And so, the end draws near.',10353,0,'Chess Event - PLAYER_CHECK');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650016','As it should be.',10354,0,'Chess Event - MEDIVH_CHECK');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650017','And so it ends.',10355,0,'Chess Event - PLAYER_WIN');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650018','Nothing less than perfection will do.',10356,0,'Chess Event - MEDIVH_WIN');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650019','Perhaps a change is in order.',10357,0,'Chess Event - MEDIVH_CHEAT_1');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650020','Time for an alternative scenario.',10358,0,'Chess Event - MEDIVH_CHEAT_2');
INSERT INTO script_texts (entry, content_default,sound,type,comment) values('-1650021','One must not become too complacent.',10359,0,'Chess Event - MEDIVH_CHEAT_3');

#--add missing gossip text's
REPLACE INTO `npc_text` (ID, text0_0) VALUES (10413, '<The water elemental is ready to surge.>');
REPLACE INTO `npc_text` (ID, text0_0) VALUES (10425, '<The grunt''s mouth waters, anticipating bloodshed.>');
REPLACE INTO `npc_text` (ID, text0_0) VALUES (10434, '<Dark magic ripples from the necrolyte.>');
REPLACE INTO `npc_text` (ID, text0_0) VALUES (10442, '<Warchief Blackhand towers over the field.>');

#--update scripts used for chess event npc's
update creature_template set ScriptName = 'npc_chesspiece', npcflag = npcflag | 1 where entry in (17469,17211,21748,21664,21750,21683,21747,21682,21726,21160,21752,21684);
update creature_template set ScriptName = 'npc_echo_of_medivh', npcflag = npcflag | 1 where entry = 16816;
update creature_template set ScriptName = 'chess_move_trigger', unit_flags = unit_flags | 33554436 where entry = 22519;

#--add game_tele to chess event in karazhan
DELETE FROM game_tele WHERE name = 'chess';
INSERT INTO game_tele VALUES ('XXX', -11096.638672, -1856.335938, 221.1, 5.408476, 532, 'chess');


INSERT INTO spell_linked_spell VALUES (-30019, 30529, 0, 'Add debuff when player stopped possessing Chesspiece');
INSERT INTO spell_linked_spell VALUES (30019, 30532, 0, 'Add InGame aura when player start possessing Chesspiece');
INSERT INTO spell_linked_spell VALUES (-30019, -30532, 0, 'Remove InGame aura when player stopped possessing Chesspiece');
#INSERT INTO spell_linked_spell VALUES (30019, 39331, 0, 'Add Game In Session buff when player start possessing Chesspiece');

