REPLACE INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `comment`) VALUES
(2498101, 24981, 1, 0, 100, 0, 1000, 2000, 0, 0, 1, -998, -999, -1000, "Converted Sentry - Random say on spawn"),
(2498102, 24981, 11, 0, 100, 0, 0, 0, 0, 0, 99, 20, 0, 0, "Converted Sentry - Random move on spawn"),
(2498103, 24981, 11, 0, 100, 0, 0, 0, 0, 0, 41, 5000, 0, 0, "Converted Sentry - Despawn after 5sec"),
(2498104, 24981, 11, 0, 100, 0, 0, 0, 0, 0, 11, 45009, 0, 0, "Converted Sentry - Cast credit spell on spawn");
REPLACE INTO `creature_ai_texts` (entry, content_default, `comment`) VALUES 
(-998, "Deployment successful. Trespassers will be neutralized", "Converted Sentry Say1"),
(-999, "Commence location defense", "Converted Sentry Say2"),
(-1000, "Objective acquired. Initiating security routines", "Converted Sentry Say3");
UPDATE creature_template SET ScriptName = "", AIName = "EventAI" WHERE entry = 24981;