title=Let it rip
category=Idle

fun=2
social=-2
hygiene=-5

allowFlip
oneShot

define init:var cycle = @3

#Frames
0;800;0;0;;label lookout
1;75;0;0;;
2;750;0;0;;
1;75;0;0;;var cycle -= 1,if cycle > 0 goto lookout
3;2000;0;0;;if bdir != -1 break,
var fart = @6,
if fart = 1 goto fart1,
if fart = 2 goto fart2,
if fart = 3 goto fart3,
if fart = 4 goto fart4,
if fart = 5 goto fart5,
if fart = 6 goto fart6

4;1200;0;0;fart1.wav;label fart1,goto redface
4;900;0;0;fart2.wav;label fart2,goto redface
4;1200;0;0;fart3.wav;label fart3,goto redface
4;2200;0;0;fart4.wav;label fart4,goto redface
4;1500;0;0;fart5.wav;label fart5,goto redface
4;900;0;0;fart6.wav;label fart6,goto redface

5;750;0;0;;label redface
6;750;0;0;;
7;3000;0;0;;
6;100;0;0;;
5;100;0;0;;
