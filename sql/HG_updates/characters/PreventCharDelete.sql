DROP TABLE IF EXISTS deleted_chars;
CREATE TABLE deleted_chars
(
    id INT NOT NULL AUTO_INCREMENT,
    char_guid INT UNSIGNED NOT NULL,
    acc int NOT NULL,
    date DATETIME NOT NULL,
    PRIMARY KEY (id)
);


delimiter //
CREATE PROCEDURE PreventCharDelete(IN charguid INT UNSIGNED)
BEGIN
    INSERT INTO deleted_chars VALUES ('XXX', guid, (SELECT account FROM characters WHERE guid = charguid), CAST(NOW() AS DATETIME));
    UPDATE characters SET account = 1 WHERE guid = charguid;
    UPDATE characters SET name = CONCAT('DEL', name, 'DEL') WHERE guid = charguid;
    DELETE FROM character_social WHERE guid = charguid OR friend = charguid;
    DELETE FROM mail WHERE receiver = charguid;
    DELETE FROM mail_items WHERE receiver = charguid;
END//
delimiter ;
