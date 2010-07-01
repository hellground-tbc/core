INSERT INTO eventai_scripts (`id`, creature_id, event_type, event_inverse_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, action1_type, action1_param1, action1_param2, action1_param3, action2_type, action2_param1, action2_param2, action2_param3, action3_type, action3_param1, action3_param2, action3_param3, `comment`) VALUES
(1879431, 18794, 6, 0, 100, 6, 0, 0, 0, 0, 34, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Cabal Ritualist - Set Inst Data on Death");

DELETE FROM eventai_scripts WHERE id=1879605;

INSERT INTO `waypoint_data` (`id`, `point`, position_x, position_y, position_z, delay, move_flag, `action`, action_chance, wpguid) VALUES
-- PATH_PATROL
(2100, 1, -156.8, 20.7, 8.07, 0, 0, 0, 100, 0),
(2100, 2, -172.2, -64, 8.07, 0, 0, 0, 100, 0),
(2100, 3, -155, -79.2, 8.07, 0, 0, 0, 100, 0),
(2100, 4, -144, -64, 8.07, 0, 0, 0, 100, 0),
(2100, 5, -144, -16, 8.07, 0, 0, 0, 100, 0),
(2100, 6, -156.764008, 20.686899, 8.073110, 0, 0, 0, 100, 0),
-- PATH_FINAL
(2101, 1, -153.78, 39.1, 7.05, 0, 0, 0, 100, 0),
(2101, 2, -144.65, 52.61, 5.65, 0, 0, 0, 100, 0),
(2101, 3, -131.32, 60.59, 4.34, 0, 0, 0, 100, 0),
(2101, 4, -110, 62.7, 3.45, 0, 0, 0, 100, 0),
(2101, 5, -89.6, 61.3, 3.11, 5000, 0, 0, 100, 0),
(2101, 6, -110, 62.7, 3.45, 0, 0, 0, 100, 0),
(2101, 7, -131.32, 60.59, 4.34, 0, 0, 0, 100, 0),
(2101, 8, -144.65, 52.61, 5.65, 0, 0, 0, 100, 0),
(2101, 9, -153.78, 39.1, 7.05, 0, 0, 0, 100, 0);
(2101, 10, -156.764008, 20.686899, 8.073110, 10000, 0, 0, 100, 0);
