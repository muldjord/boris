title=Trapdoor
category=Fun

weight=5
bounce=1

noShadow
oneShot

define init:var lemmings = [4 + @15]

#Frames
0;1200;0;0;lemmings_letsgo.wav;
1;80;0;0;;
2;80;0;0;;
3;80;0;0;;
4;80;0;0;;
5;80;0;0;;
6;80;0;0;;
7;80;0;0;;
8;80;0;0;;
9;500;0;0;;
9;300;0;0;;label spawnlemming,var lemmings -= 1,spawn lemming 0 -8
9;300;0;0;;if lemmings >= 0 goto spawnlemming
9;60000;0;0;;break
