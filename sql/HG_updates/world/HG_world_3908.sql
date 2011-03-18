UPDATE creature_template SET ScriptName = "npc_ancestral_spirit_wolf" WHERE entry = 17077;
UPDATE creature_template SET ScriptName = "npc_earthcaller_ryga" WHERE entry = 17123;
UPDATE creature_template SET minhealth = 2442, maxhealth = 2442, minmana = 0, maxmana = 0, dynamicflags = 32 WHERE entry = 17062;
UPDATE creature_template SET dynamicflags = 32 WHERE entry = 21962;
UPDATE creature_template_addon SET emote = 65, bytes1 = 7 WHERE entry IN (17062, 21962);

REPLACE INTO script_waypoint VALUES 
(17077, 1, -18.2, 3806.4, 93.2, 0, "START"),
(17077, 2, -16.9, 3806.3, 94.3, 5000, "EMOTE_HEAD_UP"),
(17077, 3, -16.4, 3801.8, 95.3, 0, ""),
(17077, 4, -19.2, 3801.2, 94.2, 0, ""),
(17077, 5, -20.3, 3805.6, 92.6, 0, ""),
(17077, 6, -18.4, 3807.3, 93.0, 5000, "EMOTE_HOWL"),
(17077, 7, -19.6, 3826.5, 88.3, 0, ""),
(17077, 8, -17.2, 3871.84, 82.3, 0, ""),
(17077, 9, 7.8, 3905.9, 86.2, 0, ""),
(17077, 10, 38.9, 3927.3, 84.1, 0, ""),
(17077, 11, 57.0, 3933.4, 82.0, 0, ""),
(17077, 12, 96.9, 3942.6, 76.6, 0, ""),
(17077, 13, 145.4, 3953.6, 78.0, 0, ""),
(17077, 14, 158.3, 3956.6, 76.6, 0, ""),
(17077, 15, 205.14, 3962.7, 73.4, 0, ""),
(17077, 16, 240.8, 3967.3, 76.5, 0, ""),
(17077, 17, 264.6, 3979.9, 84.5, 0, ""),
(17077, 18, 285.9, 3988.6, 90.0, 0, ""),
(17077, 19, 304.84, 3996.4, 96.4, 0, ""),
(17077, 20, 327.9, 3984.0, 111.4, 0, ""),
(17077, 21, 329.2, 3963.8, 123.4, 0, ""),
(17077, 22, 331.8, 3950.9, 128.6, 0, ""),
(17077, 23, 335.4, 3933.7, 133.1, 0, ""),
(17077, 24, 338.8, 3902.6, 137.4, 0, ""),
(17077, 25, 343.3, 3858.8, 148.1, 0, ""),
(17077, 26, 285.6, 3790.2, 177.3, 0, ""),
(17077, 27, 300.4, 3772.9, 185.2, 0, ""),
(17077, 28, 306.9, 3762.9, 185.1, 0, ""),
(17077, 29, 335.1, 3719.5, 183.9, 0, ""),
(17077, 30, 370.8, 3716.1, 183.9, 0, ""),
(17077, 31, 386.3, 3739.5, 184.8, 0, ""),
(17077, 32, 393.8, 3745.4, 186.1, 0, ""),
(17077, 33, 460.7, 3775.2, 186.4, 0, ""),
(17077, 34, 494.9, 3826.5, 191.4, 0, ""),
(17077, 35, 504.7, 3857.5, 194.7, 0, ""),
(17077, 36, 519.9, 3883.2, 190.6, 60000, ""),
(17077, 37, 517.3, 3885.3, 190.4, 0, "DESPAWN");

REPLACE INTO script_waypoint VALUES 
(17123, 1, 516.86, 3881.66, 190.95, 8000, "npc_earthcaller_ryga EMOTE_RYGA"),
(17123, 2, 504.76, 3882.59, 192.00, 200, "");

REPLACE INTO script_texts VALUES
(-1811000, "lifts its head into the air, as if listening for something.", 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, "npc_ancestral_spirit_wolf EMOTE_HEAD_UP"),
(-1811001, "lets out a howl that rings across the mountains to the north and motions for you to follow.", 0, 0, 0, 0, 0, 0, 0, 0, 9036, 2, 1, 393, "npc_ancestral_spirit_wolf EMOTE_HOWL"),
(-1811002, "Welcome, kind spirit. What has brought you to us?", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 16, "npc_earthcaller_ryga EMOTE_RYGA");