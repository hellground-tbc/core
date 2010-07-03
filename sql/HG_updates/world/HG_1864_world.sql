UPDATE creature_template SET `modelid_A` = 21101, `modelid_H` = 21101, `equipment_id` = 0 WHERE `entry` = 22820;

UPDATE creature_template SET minhealth=4212, maxhealth=4212, minmana=3155, maxmana=3155, ScriptName="npc_Akama" WHERE entry=21700;

INSERT INTO script_texts (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
/**************
 * Akama
 *************/

(-1563001, "Olum ... old friend.  It is good to see that you are safe.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11600, 0, 0, 1, "BT_prelude_Akama01"),
(-1563002, "Illidan will become suspicious ... that much is for certain.  We will deal with it as we always do.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11601, 0, 0, 0, "BT_prelude_Akama02"),
(-1563003, "You cannot ... be serious, Olum.  Your sacrifice has already been too great.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11602, 0, 0, 1, "BT_prelude_Akama03"),
(-1563004, "It appears your mind is set then.  We will guide you to the spirit world with as much kindness as we can.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11603, 0, 0, 1, "BT_prelude_Akama04"),
(-1563005, "Farewell, dear friend.  We shall meet you in the next world when our duties in this one are fulfilled.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11604, 0, 0, 0, "BT_prelude_Akama05"),
(-1563006, "Master!  We've found the traitor who escaped Vashj!  His body lies in front of me ... lifeless!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11605, 0, 0, 16, "BT_prelude_Akama06"),
(-1563007, "But, sire!  You have misjudged my actions ... I do have knowledge of someone close to you who plans to betray you. It is that dog, Kael'thas!  He has allied with Kil'jaeden and intends to replace you as Lord of Outland!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11606, 0, 0, 16, "BT_prelude_Akama07"),
(-1563008, "It will be done, my lord!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11607, 0, 0, 66, "BT_prelude_Akama08"),

/**************
 * Illidan
 *************/

(-1563009, "You disappoint me, Akama.  I wanted to question the treacherous worm myself!  I'm beginning to question your allegiance, Broken.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11608, 0, 0, 1, "BT_prelude_Illidan01"),
(-1563010, "I do not sense lies in your voice, Akama.  That Kael'thas would betray me does not come as a big surprise - I'm not as oblivious as some would think.  That one of your own was involved with him puts your loyalties into question.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11609, 0, 0, 1, "BT_prelude_Illidan02"),
(-1563011, "Send your Ashtongue into Tempest Keep and slay his most prized possession, the phoenix known as Al'ar.  I must know that you're not on his side.  Do not think of betraying me, Broken.  We both know who owns your soul!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11610, 0, 0, 1, "BT_prelude_Illidan03"),

/**************
 * Olum
 *************/

(-1563012, "Akama, my friend! Have you been informed? I was found out by Vashj's followers. Now our secret is in peril!","" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11611, 0, 0, 1, "BT_prelude_Olum01"),
(-1563013, "Illidan is looking for me. I'm not safe here... none of us are.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11612, 0, 0, 1, "BT_prelude_Olum02"),
(-1563014, "Illidan will find me. And when he does... he will pry the ashtongue secret from me - such is the power of the Betrayer! No... there is only one way. My usefulness to the cause has come to an end. I must venture into the spirit world!", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11613, 0, 0, 1, "BT_prelude_Olum03"),
(-1563015, "My decision is final. I will not be dissuaded. I have devoted my life to my plan. To jeopardize it now would betray all who have died for the cause long before me.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11614, 0, 0, 1, "BT_prelude_Olum04"),
(-1563016, "I thank you, Akama. You honor me by allowing me this moment, surrounded by my brothers. Farewell, friend. Guide our people well.", "" ,"" ,"" ,"" ,"" ,"" ,"" ,"" , 11615, 0, 0, 2, "BT_prelude_Olum05");