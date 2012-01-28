ALTER TABLE access_requirement
    ADD COLUMN aura_id mediumint(8) unsigned NOT NULL DEFAULT '0' AFTER heroic_quest_failed_text,
    ADD COLUMN missing_aura_text text AFTER aura_id;

UPDATE access_requirement SET aura_id = 28006, missing_aura_text = 'To enter raid instance you must have completed quest: The Dread Citadel - Naxxramas' WHERE id = 28;
