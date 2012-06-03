ALTER TABLE arena_team DROP KEY `arenateamid`;

ALTER TABLE arena_team_member DROP KEY `arenateamid_2`;
ALTER TABLE arena_team_member DROP KEY `arenateamid`;
ALTER TABLE arena_team_member ADD PRIMARY KEY (`arenateamid`,`guid`);
ALTER TABLE arena_team_member ADD KEY `arenateamid` (`arenateamid`);

ALTER TABLE arena_team_stats DROP KEY `arenateamid`;
ALTER TABLE auctionhouse DROP KEY `id`;
ALTER TABLE character_bgcoord DROP KEY `guid`;

ALTER TABLE character_gifts DROP KEY `idx_guid`;
ALTER TABLE character_gifts DROP KEY `item_guid`;
ALTER TABLE character_gifts ADD KEY `guid` (`guid`);

ALTER TABLE character_homebind DROP KEY `guid`;

ALTER TABLE character_inventory DROP KEY `item`;
ALTER TABLE character_inventory DROP KEY `idx_guid`;
ALTER TABLE character_inventory ADD KEY `guid` (`guid`);

ALTER TABLE character_pet DROP KEY `owner_2`;

ALTER TABLE character_pet_declinedname DROP KEY `owner_key`;
ALTER TABLE character_pet_declinedname ADD KEY `owner` (`owner`);

ALTER TABLE character_queststatus_daily DROP KEY `idx_guid`;
ALTER TABLE character_queststatus_daily ADD KEY `guid` (`guid`);

ALTER TABLE character_reputation CONVERT TO CHARACTER SET utf8;
ALTER TABLE character_reputation DROP KEY `NewIndex1`;
ALTER TABLE character_reputation ADD PRIMARY KEY (`guid`,`faction`);

ALTER TABLE character_tutorial DROP KEY `acc_key`;
ALTER TABLE character_tutorial ADD KEY `account` (`account`);

ALTER TABLE characters DROP KEY `idx_account`;
ALTER TABLE characters DROP KEY `idx_online`;
ALTER TABLE characters DROP KEY `idx_name`;
ALTER TABLE characters ADD KEY `account` (`account`);
ALTER TABLE characters ADD KEY `online` (`online`);
ALTER TABLE characters ADD KEY `name` (`name`);

ALTER TABLE corpse DROP KEY `idx_type`;
ALTER TABLE corpse ADD KEY `corpse_type` (`corpse_type`);

ALTER TABLE group_member DROP KEY `Idx_memberGuid`;
ALTER TABLE group_member ADD KEY `memberGuid` (`memberGuid`);

ALTER TABLE group_saved_loot CONVERT TO CHARACTER SET utf8;

ALTER TABLE guild DROP KEY `guildid`;

ALTER TABLE guild_bank_eventlog DROP KEY `Idx_PlayerGuid`;
ALTER TABLE guild_bank_eventlog DROP KEY `Idx_LogGuid`;
ALTER TABLE guild_bank_eventlog ADD KEY `PlayerGuid` (`PlayerGuid`);
ALTER TABLE guild_bank_eventlog ADD KEY `LogGuid` (`LogGuid`);

ALTER TABLE guild_bank_item DROP KEY `guildid_key`;
ALTER TABLE guild_bank_item DROP KEY `Idx_item_guid`;
ALTER TABLE guild_bank_item ADD KEY `guildid` (`guildid`);
ALTER TABLE guild_bank_item ADD KEY `item_guid` (`item_guid`);

ALTER TABLE guild_bank_right DROP KEY `guildid_key`;
ALTER TABLE guild_bank_right ADD KEY `guildid` (`guildid`);

ALTER TABLE guild_bank_tab DROP KEY `guildid_key`;
ALTER TABLE guild_bank_tab ADD KEY `guildid` (`guildid`);

ALTER TABLE guild_eventlog CONVERT TO CHARACTER SET utf8;
ALTER TABLE guild_eventlog DROP KEY `Idx_PlayerGuid1`;
ALTER TABLE guild_eventlog DROP KEY `Idx_PlayerGuid2`;
ALTER TABLE guild_eventlog DROP KEY `Idx_LogGuid`;
ALTER TABLE guild_eventlog ADD KEY `PlayerGuid1` (`PlayerGuid1`);
ALTER TABLE guild_eventlog ADD KEY `PlayerGuid2` (`PlayerGuid2`);
ALTER TABLE guild_eventlog ADD KEY `LogGuid` (`LogGuid`);

ALTER TABLE guild_member DROP KEY `guid_key`;
ALTER TABLE guild_member DROP KEY `guildid_key`;
ALTER TABLE guild_member DROP KEY `guildid_rank_key`;
ALTER TABLE guild_member ADD PRIMARY KEY (`guid`);
ALTER TABLE guild_member ADD KEY `guildid` (`guildid`);
ALTER TABLE guild_member ADD KEY `guildid_rank` (`guildid`,`rank`);

ALTER TABLE guild_rank DROP KEY `Idx_rid`;
ALTER TABLE guild_rank ADD KEY `rid` (`rid`);

ALTER TABLE hidden_rating CONVERT TO CHARACTER SET utf8;

ALTER TABLE item_instance DROP KEY `idx_owner_guid`;
ALTER TABLE item_instance ADD KEY `owner_guid` (`owner_guid`);

ALTER TABLE mail DROP KEY `idx_receiver`;
ALTER TABLE mail ADD KEY `receiver` (`receiver`);

ALTER TABLE mail_items DROP KEY `idx_receiver`;
ALTER TABLE mail_items ADD KEY `receiver` (`receiver`);

ALTER TABLE petition DROP KEY `index_ownerguid_petitionguid`;
ALTER TABLE petition ADD UNIQUE KEY `ownerguid_petitionguid` (`ownerguid`,`petitionguid`);
