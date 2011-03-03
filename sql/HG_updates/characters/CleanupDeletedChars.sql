delimiter //
CREATE PROCEDURE CleanupDeletedChars()
BEGIN
    DECLARE m_guid INT;
    DECLARE m_pet INT;
    DECLARE m_acc INT;
    DECLARE m_curs CURSOR FOR SELECT char_guid FROM deleted_chars WHERE period_diff(date_format(now(), '%Y%m'), date_format(date, '%Y%m')) >= 1;
    DECLARE m_curs2 CURSOR FOR SELECT id FROM character_pet WHERE owner = m_guid;    
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET @done = 1;

    OPEN m_curs;
    
    char_loop: LOOP
        FETCH m_curs INTO m_guid;
        IF @done THEN
            LEAVE char_loop;
        END IF;

        SELECT id INTO m_acc FROM account where guid = m_guid;

        IF m_acc = 1 THEN
            OPEN m_curs2;
            pet_loop: LOOP
                FETCH m_curs2 INTO m_pet;
                IF @done THEN
                    LEAVE pet_loop;
                END IF;

                DELETE FROM character_pet WHERE id = m_pet;
                DELETE FROM character_pet_declinedname WHERE id = m_pet;
                DELETE FROM pet_aura WHERE guid = m_pet;
                DELETE FROM pet_spell WHERE guid = m_pet;
                DELETE FROM pet_spell_cooldown WHERE guid = m_pet;
            END LOOP;

            CLOSE m_curs2;

            DELETE FROM characters WHERE guid = m_guid;
            DELETE FROM character_declinedname WHERE guid = m_guid;
            DELETE FROM character_action WHERE guid = m_guid;
            DELETE FROM character_aura WHERE guid = m_guid;
            DELETE FROM character_gifts WHERE guid = m_guid;
            DELETE FROM character_homebind WHERE guid = m_guid;
            DELETE FROM character_instance WHERE guid = m_guid;
            DELETE FROM group_instance WHERE leaderGuid = m_guid;
            DELETE FROM character_inventory WHERE guid = m_guid;
            DELETE FROM character_queststatus WHERE guid = m_guid;
            DELETE FROM character_reputation WHERE guid = m_guid;
            DELETE FROM character_spell WHERE guid = m_guid;
            DELETE FROM character_spell_cooldown WHERE guid = m_guid;
            DELETE FROM gm_tickets WHERE playerGuid = m_guid;
            DELETE FROM item_instance WHERE owner_guid = m_guid;
            DELETE FROM character_social WHERE guid = m_guid OR friend=m_guid;
            DELETE FROM mail WHERE receiver = m_guid;
            DELETE FROM mail_items WHERE receiver = m_guid;
            DELETE FROM character_pet WHERE owner = m_guid;
            DELETE FROM character_pet_declinedname WHERE owner = m_guid;
        END IF;
    END LOOP;
    CLOSE m_curs;
    DELETE FROM deleted_chars WHERE period_diff(date_format(now(), '%Y%m'), date_format(date, '%Y%m')) >= 1;
END//
delimiter ;
