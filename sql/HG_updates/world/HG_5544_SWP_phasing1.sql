REPLACE INTO item_template (entry, class, `name`, displayid, Quality, Flags, BuyCount, ItemLevel, bonding) VALUES (34548, 15, "Cache of the Shattered Sun", 44685, 4, 4, 1, 70, 1);
REPLACE INTO item_template (entry, class, `name`, displayid, Quality, Flags, BuyCount, ItemLevel, stackable, bonding, description) VALUES (34544, 15, "Essence of the Immortals", 47899, 4, 2048, 1, 70, 100, 1, "Chaotic energy pulses through this object.");
DELETE FROM item_loot_template WHERE entry = 34548;
UPDATE quest_template SET NextQuestId = 11551 WHERE entry = 11550;
REPLACE INTO quest_template (entry, Method, ZoneOrSort, MinLevel, QuestLevel, `Type`, SuggestedPlayers, QuestFlags, SpecialFlags, PrevQuestId, Title, Details, Objectives, OfferRewardText, RequestItemsText, ReqItemId1, ReqItemCount1, RewItemId1, RewItemCount1, RewRepFaction1, RewRepValue1, RewOrReqMoney, RewMoneyMaxLevel, DetailsEmote1, OfferRewardEmote1) VALUES
(11551, 1, -368, 70, 70, 62, 25, 192, 1, 11550, "Agamath, the First Gate", 
"The first barrier lies just beyond the path of destruction, where the fiendish Brutallus stands. The gateway, Agamath, prevents passage beyond the Dead Scar. $B$B <Ne'thul points to the magical replica of the Sunwell.> $B$BBring to me the essence of immortals, found only on the most powerful beings of this world! With the essence in hand, my mages will be able to dismantle Agamath, ultimately collapsing the gateway and unlocking the way further into the sunwell.",
"Archmage Ne'thul asked you to bring the Essence of the Immortals that will be used to deactivate Agamath, The First Gate", "The destruction of Agamath commences. Soon, the barrier will crumble.", "Bring to me the essence of immortals, found only on the most powerful beings of this world! With the essence in hand, my mages will be able to dismantle Agamath, ultimately collapsing the gateway and unlocking the way further into the sunwell.", 34544, 1, 34548, 1, 1077, 250, 7500, 114000, 25, 21);
REPLACE INTO creature_involvedrelation VALUES (25169, 11551);
REPLACE INTO quest_template (entry, Method, ZoneOrSort, MinLevel, QuestLevel, `Type`, SuggestedPlayers, QuestFlags, SpecialFlags, Title, Details, Objectives, OfferRewardText, RequestItemsText, ReqItemId1, ReqItemCount1, RewItemId1, RewItemCount1, RewRepFaction1, RewRepValue1, RewOrReqMoney, RewMoneyMaxLevel, DetailsEmote1, OfferRewardEmote1) VALUES
(11552, 1, -368, 70, 70, 62, 25, 192, 1, "Rohendor, the Second Gate", 
"Rohendor, the Second Gate, will let you go no further than the footsteps beyond the lair of the eredar twins, Sacrolash and Alythess. M'uru stands guard behind it, a twisted reflection of its former self. $B$BBring to me the essence of immortals, found only on the most powerful beings of this world! Rohendor cannot withstand the combined force of our mages!",
"Archmage Ne'thul asked you to bring the Essence of the Immortals that will be used to break Rohendor, the Second Gate", "Good, our mages started to weaken Rohendor. The barrier must fall, sooner or later.", "Bring to me the essence of immortals, found only on the most powerful beings of this world! Rohendor cannot withstand the combined force of our mages!", 34544, 1, 34548, 1, 1077, 250, 7500, 114000, 25, 21);
REPLACE INTO creature_involvedrelation VALUES (25169, 11552);
REPLACE INTO quest_template (entry, Method, ZoneOrSort, MinLevel, QuestLevel, `Type`, SuggestedPlayers, QuestFlags, SpecialFlags, Title, Details, Objectives, OfferRewardText, RequestItemsText, ReqItemId1, ReqItemCount1, RewItemId1, RewItemCount1, RewRepFaction1, RewRepValue1, RewOrReqMoney, RewMoneyMaxLevel, DetailsEmote1, OfferRewardEmote1) VALUES
(11553, 1, -368, 70, 70, 62, 25, 192, 1, "Archonisus, the Final Gate", 
"The final gate, Archonisus, prevents passage to the Sunwell's inner chamber. The barrier is proving to be the most fortified of the lot! Difficult to crack, but not impossible...Alas, I would expect nothing less from Kil'jaeden! $B$BBring to me the essence of immortals — found only on the most powerful beings of this world — and Archonisus is sure to crumble! ",
"Archmage Ne'thul asked you to bring the Essence of the Immortals that will be used to eventually dismantle Archonisus, the Final Gate", "Great work. $BWith more essences in hand, we can try to break down the last obstacle on our way to The Deceiver. Even this barrier will fall when enough power is gathered.", "Bring to me the essence of immortals — found only on the most powerful beings of this world — and Archonisus is sure to crumble!", 34544, 1, 34548, 1, 1077, 250, 7500, 114000, 25, 21);
REPLACE INTO creature_involvedrelation VALUES (25169, 11553);
UPDATE gameobject SET state = 1 WHERE guid IN (50439, 50440, 50441);
REPLACE INTO game_event (entry, occurence, `length`, description, world_event) VALUES 
(50, 5184000, 0,  "Sunwell Plateau Offensive: Phase A - Breaking Agamath, the First Gate", 2),
(51, 5184000, 0, "Sunwell Plateau Offensive: Phase B - Breaking Rohendor, the Second Gate", 1),
(52, 5184000, 0, "Sunwell Plateau Offensive: Phase C - Breaking Archonisus, the Final Gate", 1),
(53, 5184000, 5184000, "Sunwell Plateau Offensive: Phase D - All barriers are down!!", 1);
REPLACE INTO game_event_condition VALUES
(50, 1, 80, 0, 3300, "breaking Agamath"),
(51, 1, 150, 0, 3310, "breaking Rohendor"),
(52, 1, 500, 0, 3320, "breaking Archonisus");
REPLACE INTO game_event_creature_quest VALUES
(25169, 11551, 50),
(25169, 11552, 51),
(25169, 11553, 52);
REPLACE INTO game_event_gameobject VALUES
(50441, 50),
(50439, 50),
(50439, 51),
(50440, 50),
(50440, 51),
(50440, 52);
REPLACE INTO game_event_prerequisite VALUES
(51, 50),
(52, 51),
(53, 52);
REPLACE INTO game_event_quest_condition VALUES
(11551, 50, 1, 1),
(11552, 51, 1, 1),
(11553, 52, 1, 1);
UPDATE creature_template SET ScriptName = "npc_archmage_nethul" WHERE entry = 25169;
REPLACE INTO npc_gossip VALUES (93947, 12309);
REPLACE INTO npc_text (ID, text0_0) VALUES 
(12400, "We are trying to bring Agamath, the First Gate down. Our mages do what they can, but we need more power.. $B$B We are now $3300w percent done with our task."),
(12401, "Agamath, the First Gate is breached and two of Kil'jaeden's lieutenants, Lady Sacrolash and Grand Warlock Alythess, must be destroyed.$B$BRohendor, the Second Gate is our next target. It is now $3310w percent weakened, keep working!"),
(12402, "We have made it! Rohendor, the Second Gate is down. M'uru, a twisted reflection of its former self that stands behind it must be destroyed! $B$BAlso it is time now to bring down Archonisus, the Final Gate - strongest one, very difficult to break. $B$BAt the moment, our forces are $3320w percent to complete this task, please continue your support and we can make it!"),
(12403, "Great work!! All three of the gates ** the Sunwell Plateau have been brought down, $N.$B$BNow you must marshal your forces and stop Kil'jaeden before all is lost!");
REPLACE INTO item_loot_template VALUES 
(34548, 22851, 70, 0, 1, 3, 0, 0, 0),
(34548, 22853, 70, 0, 1, 3, 0, 0, 0),
(34548, 32428, 50, 0, 1, 5, 0, 0, 0),
(34548, 22861, 0, 1, 1, 2, 0, 0, 0),
(34548, 22866, 0, 1, 1, 2, 0, 0, 0),
(34548, 22854, 0, 2, 1, 2, 0, 0, 0),
(34548, 22866, 0, 2, 1, 2, 0, 0, 0),
(34548, 22861, 0, 3, 1, 2, 0, 0, 0),
(34548, 22854, 0, 3, 1, 2, 0, 0, 0),
-- engi
(34548, 35186, 3, 4, 1, 1, 7, 202, 350),
(34548, 35187, 3, 4, 1, 1, 7, 202, 350),
(34548, 35189, 3, 4, 1, 1, 7, 202, 350),
(34548, 35190, 3, 4, 1, 1, 7, 202, 350),
(34548, 35191, 3, 4, 1, 1, 7, 202, 350),
(34548, 35192, 3, 4, 1, 1, 7, 202, 350),
(34548, 35193, 3, 5, 1, 1, 7, 202, 350),
(34548, 35194, 3, 5, 1, 1, 7, 202, 350),
(34548, 35195, 3, 5, 1, 1, 7, 202, 350),
(34548, 35196, 3, 5, 1, 1, 7, 202, 350),
(34548, 35197, 3, 5, 1, 1, 7, 202, 350),
-- jc
(34548, 35198, 3, 6, 1, 1, 7, 755, 350),
(34548, 35199, 3, 6, 1, 1, 7, 755, 350),
(34548, 35200, 3, 6, 1, 1, 7, 755, 350),
(34548, 35201, 3, 6, 1, 1, 7, 755, 350),
(34548, 35202, 3, 6, 1, 1, 7, 755, 350),
(34548, 35203, 3, 6, 1, 1, 7, 755, 350),
-- tailor
(34548, 35204, 2, 7, 1, 1, 7, 197, 350),
(34548, 35206, 2, 7, 1, 1, 7, 197, 350),
(34548, 35207, 2, 7, 1, 1, 7, 197, 350),
-- bs
(34548, 35208, 2, 8, 1, 1, 7, 164, 350),
(34548, 35209, 2, 8, 1, 1, 7, 164, 350),
(34548, 35210, 2, 8, 1, 1, 7, 164, 350),
(34548, 35211, 2, 8, 1, 1, 7, 164, 350),
-- leather
(34548, 35212, 3, 9, 1, 1, 7, 165, 350),
(34548, 35213, 3, 9, 1, 1, 7, 165, 350),
(34548, 35214, 3, 9, 1, 1, 7, 165, 350),
(34548, 35215, 3, 9, 1, 1, 7, 165, 350),
(34548, 35216, 3, 10, 1, 1, 7, 165, 350),
(34548, 35217, 3, 10, 1, 1, 7, 165, 350),
(34548, 35218, 3, 10, 1, 1, 7, 165, 350),
(34548, 35219, 3, 10, 1, 1, 7, 165, 350),
(34548, 32231, 0, 11, 1, 2, 0, 0, 0),
(34548, 32230, 0, 11, 1, 2, 0, 0, 0),
(34548, 32249, 0, 11, 1, 2, 0, 0, 0),
(34548, 32227, 0, 12, 1, 2, 0, 0, 0),
(34548, 32229, 0, 12, 1, 2, 0, 0, 0),
(34548, 32228, 0, 12, 1, 2, 0, 0, 0),
(34548, 22521, 50, 13, 1, 3, 0, 0, 0),
(34548, 22522, 50, 13, 1, 3, 0, 0, 0);