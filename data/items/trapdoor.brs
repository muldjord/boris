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
9;300;0;0;;if lemmings >= 0 goto spawnlemming,var loops = 600
9;50;0;0;;label moreloop,var loops -= 1,if mdist <= 32 { if mhvel < -10 or mhvel > 10 spawn lemming 0 -8 }
9;50;0;0;;if loops >= 0 goto moreloop,break
