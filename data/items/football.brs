title=Football
category=Fun

reaction=kickball_w,kickballw,-22,6;kickball_e,kickballe,22,6

weight=5
bounce=7
range=16

doNotDisturb

#Frames
0;1000;0;0;;stop

0;3650;0;0;;label kickballw
0;50;0;0;kick.wav;setvel [(@2 + 5 + (energy / 10)) * -1] [(@2 + 1 + (energy / 15)) * -1],stop

0;3650;0;0;;label kickballe
0;50;0;0;kick.wav;setvel [@2 + 5 + (energy / 10)] [(@2 + 1 + (energy / 15)) * -1],stop
