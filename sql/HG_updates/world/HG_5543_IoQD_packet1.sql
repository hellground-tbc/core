UPDATE creature_template SET ScriptName = "mob_mgt_kalecgos" WHERE entry = 24844;
UPDATE creature_template SET npcflag = 3, ScriptName = "mob_mgt_kalec" WHERE entry = 24848;
UPDATE creature_template SET spell1 = 44475, spell2 = 44478 WHERE entry = 24683;
UPDATE creature_template SET spell1 = 44475, spell2 = 46028 WHERE entry = 25568;
UPDATE creature_template SET spell1 = 44518, spell2 = 44519, spell3 = 44520 WHERE entry = 24686;
UPDATE creature_template SET spell1 = 46042, spell2 = 46043, spell3 = 44520 WHERE entry = 25572;
UPDATE creature_template SET spell1 = 44583, spell2 = 44599 WHERE entry = 24687;
UPDATE creature_template SET spell1 = 46045, spell2 = 46046 WHERE entry = 25570;
UPDATE creature_template SET spell1 = 44479, spell2 = 44480, spell3 = 44482 WHERE entry = 24684;
UPDATE creature_template SET spell1 = 46029, spell2 = 46030, spell3 = 46033 WHERE entry = 25565;
UPDATE creature_template SET spell1 = 35946, spell2 = 20299, spell3 = 17741, spell4 = 44639 WHERE entry = 24696;
UPDATE creature_template SET spell1 = 22907, spell2 = 46150, spell3 = 46151, spell4 = 44639 WHERE entry = 25547;
UPDATE creature_template SET spell1 = 44505, spell2 = 44533 WHERE entry IN (24688, 25577);
UPDATE creature_template SET spell1 = 44534, spell2 = 44505 WHERE entry IN (24689, 25575);
UPDATE creature_template SET spell1 = 44503, spell2 = 44504, spell3 = 44505 WHERE entry IN (24690, 25576);
REPLACE INTO creature_formations (leaderGUID, memberGUID, groupAI) VALUES
(96850, 96850, 2),
(96850, 96781, 2),
(96850, 96841, 2),
(96850, 96809, 2),
(96850, 96770, 2),
(96850, 96847, 2);
UPDATE creature_template SET ScriptName = "mob_ethereum_smuggler" WHERE entry = 24698;
UPDATE script_texts SET emote = 5 WHERE entry = -1585029;
REPLACE INTO script_texts (entry, content_default, `type`, `comment`) VALUES (-1585030, "begins to cast Pyroblast!", 3, "kaelthas MT EMOTE_HEROIC_PYROBLAST");
UPDATE creature_template SET unit_flags = 33554434, InhabitType = 4 WHERE entry = 24708;
UPDATE creature_template SET unit_flags = 0 WHERE entry = 24674;
UPDATE creature_template SET equipment_id = 2387, minrangedmg = 700, maxrangedmg = 950, rangedattackpower = 33 WHERE entry IN (24555, 25555);
UPDATE creature_template SET ScriptName = "mob_high_explosive_sheep" WHERE entry = 24715;
REPLACE INTO spell_linked_spell VALUES
(44175, 6788, 0, "NPC: Weakened Soul debuff on SW:Shield usage"),
(46193, 6788, 0, "NPC: Weakened Soul debuff on SW:Shield usage");
UPDATE creature_template SET ScriptName = "mob_fizzle" WHERE entry = 24656;
UPDATE creature_template SET ScriptName = "mob_sister_of_torment" WHERE entry = 24697;
UPDATE creature_template SET ScriptName = "mob_sunblade_sentinel" WHERE entry = 24777;
UPDATE creature_template SET ScriptName = "mob_coilskar_witch" WHERE entry = 24696;
UPDATE creature_template SET ScriptName = "mob_ethereum_smuggler" WHERE entry = 24698;
UPDATE creature_template SET flags_extra = 8388609 WHERE entry = 24744;
UPDATE creature SET spawndist = 10 WHERE id = 24761;
UPDATE creature_template SET ScriptName = "mob_brightscale_wyrm" WHERE entry = 24761;
UPDATE creature_template SET mindmg = 1800, maxdmg = 2400, attackpower = 0 WHERE entry = 24744;
UPDATE script_texts SET `comment` = "selin SAY_DRAINING" WHERE entry = -1585005;
UPDATE script_texts SET `type` = 2 WHERE entry = -1585006;
UPDATE creature_template SET AIName = "", ScriptName = "mob_sunblade_physician" WHERE entry = 24687;
UPDATE creature_template SET ScriptName = "mob_sunblade_imp" WHERE entry = 24815;
UPDATE item_template SET ScriptName = "item_specific_target" WHERE entry = 13289;
UPDATE item_template SET ScriptName = "item_specific_target" WHERE entry IN (23337, 29818);
REPLACE INTO waypoint_data VALUES
(2500, 1, 1458, 587, 58, 0, 0, 0, 100, 0),
(2500, 3, 1490, 673, 53, 0, 0, 0, 100, 0),
(2500, 2, 1479, 535, 55, 0, 0, 0, 100, 0),
(2500, 4, 1460, 623, 56, 0, 0, 0, 100, 0);
REPLACE INTO spell_linked_spell VALUES (45410, 45400, 0, "Summon Blazing Dead when summoning Vapor Trail");
REPLACE INTO creature_template_addon (entry, auras) VALUES (25268, "45415 0");
UPDATE creature_template SET mindmg = 10000, maxdmg = 40000, minmana = 3387, maxmana = 3387, attackpower = 10000, baseattacktime = 2000, InhabitType = 1 WHERE entry = 24895;
DELETE FROM spell_linked_spell WHERE spell_effect = 45665;
REPLACE INTO spell_linked_spell VALUES (45661, 44883, 1, "Encapsulate");
REPLACE INTO spell_script_target VALUES (44884, 1, 24895);
UPDATE creature_template SET flags_extra = 0 WHERE entry = 24891;
UPDATE script_texts SET `type` = 6 WHERE entry BETWEEN -1580000 AND -1580017;
UPDATE script_texts SET TYPE = 0 WHERE entry = -1580009;
REPLACE INTO script_texts (entry, content_default, `type`, `comment`)  VALUES 
(-1579990, "drives Kalecgos into a crazed rage!", 3, "sathrovarr EMOTE_KALECGOS_ENRAGE"),
(-1579991, "makes Sathrovarr go into a crazed rage!", 3, "kalecgos EMOTE_SATHROVARR_ENRAGE");
REPLACE INTO script_texts (entry, content_default, sound, `type`, `comment`)  VALUES
(-1579992, "There is no time to waste.", 12432, 0, "dragonKALECGOS_GREETING01"),
(-1579993, "Kil'Jaeden must be destroyed.", 12433, 0, "dragonKALECGOS_GREETING02"),
(-1579994, "The fate, of the world, hangs in the balance.", 12434, 0, "dragonKALECGOS_GREETING03");
UPDATE creature_template SET minmana = 169350, maxmana = 169350 WHERE entry = 24891;
UPDATE creature_template SET ScriptName = "brutallus_intro_trigger", modelid_A = 18783, modelid_H = 15435, flags_extra = 2 WHERE entry = 19871;
UPDATE creature_template SET ScriptName = "npc_captain_selana" WHERE entry = 25638;

REPLACE INTO npc_text (ID, text0_0, prob0, em0_1)  VALUES
(12588, "I'm glad you are here, $c. The military situation in the Sunwell Plateau is rather grim.", 1,396),
(12589, "The Shattered Sun Offensive has made great progress in the outer areas of the Isle of Quel'Danas. Kil'jaeden's forces are being pushed back, and in the chaos I was able to lead a small scouting force here inside the Sunwell Plateau. My troops proceeded through Parhelion Plaza, spotted a group of Sunblade blood elves, and we than engaged the enemy. That fight quickly turned into disaster.", 1,1),
(12590, "Kil'jaeden's defenders here on the Sunwell Plateau are much stronger than anything we faced outside on the Island. The Legion have even twisted our Arcane Guardian technology with fel energy to create something much more sinister. Just one of those Sunblade Protectors decimated my troops, and I ordered a swift retreat. I expected my squad to be pushed completely off the plateau, but the enemy surprised me by calling off the pursuit.", 1,1),
(12591, "I don't know, but I'm glad they did. The respite has allowed Anchorite Elbadon to save many of the men and women under my command. I've sent a few of my stealthiest scouts forward, and they've reported back something interesting. Nearly all of the Sunblade Protectors are powered down! Kil'jaeden's forces appear to be conserving their energy, and seem content to merely contain us with one active Protector. If you and your companions want to force your way inside, allow me to suggest a plan of attack.", 1,1),
(12592, "Take advantage of their deactivated Sunblade Protectors. If one of their Sunblade Scouts spots you, I'm sure the elf will try to reactivate a nearby Sunblade Protector. Kill him quickly to prevent this. With the blood elves eliminated, as long as you do not trigger the Protector's proximity sensors you should be able to move past them without a fight. That will save you both time and resources.", 1,1);
-- min max dmg to be verified
UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_arch_mage" WHERE entry = 25367;
UPDATE creature_template SET mindmg = 1000, maxdmg = 2500, mechanic_immune_mask = 133121, AIName = "", ScriptName = "mob_sunblade_cabalist" WHERE entry = 25363;
UPDATE creature_template SET mindmg = 1300, maxdmg = 3500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_dawn_priest" WHERE entry = 25371;
UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 134145, AIName = "", minhealth = 190000, maxhealth = 190000, ScriptName = "mob_sunblade_dusk_priest" WHERE entry = 25370;
UPDATE creature_template SET mindmg = 5500, maxdmg = 17000, mechanic_immune_mask = 23553, AIName = "", ScriptName = "mob_sunblade_protector" WHERE entry = 25507;
UPDATE creature_template SET mechanic_immune_mask = 537083891, AIName = "", ScriptName = "mob_sunblade_scout" WHERE entry = 25372;
UPDATE creature_template SET mindmg = 1900, maxdmg = 4500, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_slayer" WHERE entry = 25368;
UPDATE creature_template SET mindmg = 3800, maxdmg = 15000, mechanic_immune_mask = 2049, AIName = "", ScriptName = "mob_sunblade_vindicator" WHERE entry = 25369;
UPDATE creature_template SET faction_A = 16, faction_H = 16 WHERE entry IN (25867, 26101);
DELETE FROM creature_ai_scripts WHERE entryOrGUID = 25867;
REPLACE INTO creature_ai_scripts VALUES
(2610101, 26101, 0, 0, 100, 3, 2000, 3000, 9000, 10000, 11, 46551, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Fire Fiend - Cast Fire Nova"),
(2586701, 25867, 0, 0, 100, 3, 2500, 5500, 10000, 15000, 11, 47251, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Sunblade Dragonhawk - Cast Flame Breath");
UPDATE creature_template SET AIName = "EventAI" WHERE entry = 26101;
UPDATE spell_script_target SET targetEntry = 24933 WHERE entry = 44864;
DELETE FROM creature_ai_scripts WHERE entryorguid = 24918;
DELETE FROM creature_ai_texts WHERE entry BETWEEN -1006 AND -1001;
UPDATE creature_template SET ScriptName = "npc_felblood_initiate" WHERE entry IN (24918, 24955);
UPDATE creature_template SET ScriptName = "npc_archmage_nethul" WHERE entry = 25632;
-- Sunwell Radiance on SWP enemy mobs
-- should kil'jaeden be affected is to be verified!
REPLACE INTO creature_template_addon (entry, auras) VALUES
(25165, "45769 0 45769 1"),
(25166, "45769 0 45769 1"),
(25315, "45769 0 45769 1"),
(25373, "45769 0 45769 1"),
(25483, "45769 0 45769 1"),
(25486, "45769 0 45769 1"),
(25506, "45769 0 45769 1"),
(25591, "45769 0 45769 1"),
(25592, "45769 0 45769 1"),
(25593, "45769 0 45769 1"),
(25595, "45769 0 45769 1"),
(25597, "45769 0 45769 1"),
(25599, "45769 0 45769 1"),
(25608, "45769 0 45769 1");
UPDATE creature_template_addon SET auras = "45769 0 45769 1" WHERE entry IN
(24882, 25363, 25367, 25368, 25369, 25370, 25371, 25508, 25509, 25867);
UPDATE creature_template_addon SET auras = "18950 0 18950 1 45769 0 45769 1" WHERE entry IN (25372, 25507);
UPDATE creature_template_addon SET auras = "16380 0 45769 0 45769 1" WHERE entry = 25484;
REPLACE INTO spell_linked_spell VALUES (45717, 45726, 2, "Fog of Corruption link addtional auras to base spell");
REPLACE INTO waypoint_data VALUES
(2499, 1, 12165, -7066, 60, 0, 0, 0, 100, 0),
(2499, 3, 12180, -7136, 60, 0, 0, 0, 100, 0),
(2499, 2, 12244, -7160, 60, 0, 0, 0, 100, 0),
(2499, 4, 12177, -7166, 60, 0, 0, 0, 100, 0),
(2499, 5, 12240, -7015, 60, 0, 0, 0, 100, 0);
UPDATE spell_script_target SET targetEntry = 25158 WHERE entry = 45201;
UPDATE creature_template SET ScriptName = "npc_ioqd_brutallus" WHERE entry = 25158;
UPDATE creature_template SET ScriptName = "npc_ioqd_madrigosa" WHERE entry = 25160;
UPDATE creature_template SET ScriptName = "npc_wrath_enforcer" WHERE entry = 25030;
UPDATE creature_template SET ScriptName = "npc_pit_overlord" WHERE entry = 25031;
UPDATE creature_template SET ScriptName = "npc_eredar_sorcerer" WHERE entry = 25033;
UPDATE gameobject SET spawnMask = 3 WHERE guid IN (99813, 99807, 99798, 99799, 99808);
UPDATE creature_template SET flags_extra = 16777217 WHERE entry IN (24723, 25562);
UPDATE creature_template SET flags_extra = 16777216, ScriptName = "npc_wretched_devourer" WHERE entry  = 24960;
UPDATE creature_template SET flags_extra = 16777216, ScriptName = "npc_wretched_fiend" WHERE entry  = 24966;
UPDATE creature_template SET flags_extra = 33554432, ScriptName = "npc_erratic_sentry" WHERE entry  = 24972;
UPDATE creature_template SET ScriptName = "npc_archmage_nethul" WHERE entry = 25169;
REPLACE INTO spell_script_target VALUES (44883, 1, 24882);
DROP TABLE IF EXISTS `completed_cinematic_scripts`;
CREATE TABLE `completed_cinematic_scripts` (
  `entry` MEDIUMINT(8) NOT NULL,
  `ScriptName` CHAR(64) NOT NULL,
  PRIMARY KEY  (`entry`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8 COMMENT='Script library scripted completed cinematic events';
REPLACE INTO script_texts (entry, content_default, `type`, `comment`) VALUES (-1585031, "oveloads!", 3, "vexallus EMOTE_OVERLOAD");
REPLACE INTO completed_cinematic_scripts VALUES (164, "scrying_orb_cinematic");
REPLACE INTO script_texts (entry, content_default, `type`, `comment`) VALUES (-1585031, "oveloads!", 3, "vexallus EMOTE_OVERLOAD");