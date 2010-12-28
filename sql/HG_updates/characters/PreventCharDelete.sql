DROP PROCEDURE IF EXISTS PreventCharDelete;
CREATE PROCEDURE PreventCharDelete(IN @guid INT UNSIGNED)
BEGIN
    INSERT INTO deleted_chars VALUES ('XXX', guid, (SELECT account FROM characters WHERE guid = @guid), CAST(NOW() AS DATETIME));
    UPDATE characters SET account = 1 WHERE guid = @guid;
    UPDATE characters SET name = CONCAT('DEL', name, 'DEL') WHERE guid = @guid;
    DELETE FROM character_social WHERE guid = @guid OR friend = @guid;
    DELETE FROM mail WHERE receiver = @guid;
    DELETE FROM mail_items WHERE receiver = @guid;
END;

DROP TABLE IF EXISTS deleted_chars;
CREATE TABLE email_log
(
    id INT NOT NULL AUTO_INCREMENT,
    char_guid INT UNSIGNED NOT NULL,
    acc int NOT NULL,
    date DATETIME NOT NULL,
    PRIMARY KEY (id)
);
