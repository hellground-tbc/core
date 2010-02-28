UPDATE `creature_template` SET `mechanic_immune_mask`=0x24A13FFF WHERE `entry` IN
('14762','14763', '14764','14765','11948','4772','14776','14773','14777','11946');

UPDATE `creature_template` SET `mindmg`=`mindmg`*1.15, `maxdmg`=`maxdmg`*1.15 WHERE `entry` IN
('14762','14763', '14764','14765','11948','4772','14776','14773','14777','11946');