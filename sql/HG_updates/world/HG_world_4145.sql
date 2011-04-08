UPDATE creature_template SET AIName = "EventAI" WHERE entry = 25882;
REPLACE INTO creature_ai_scripts VALUES
(2588201, 25882, 8, 0, 100, 0, 46281, -1, 0, 0, 23, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Multiphase Disturbance - Change phase when `readed` from quest"),
(2588202, 25882, 1, 1, 100, 0, 2500,  2500, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Multiphase Disturbance - Despawn after 2.5 sec in phase 2");