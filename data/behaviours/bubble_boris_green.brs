category=Reaction

fun=-10
social=5
energy=-5
hygiene=-20

oneShot
doNotDisturb

define init:sound flee2.wav,
var dir = @2,
var floats = [@3 + 4],
if dir = 1 goto west,
if dir = 2 goto east

#Frames
0;125;-1;-1;;label west
0;125;-1;0;;var floats -= 1
1;125;-1;1;;
1;125;-1;1;;
2;125;-1;1;;
2;125;-1;0;;
1;125;-1;-1;;
1;125;-1;-1;;if floats >= 0 goto west else goto pop

0;125;1;-1;;label east
0;125;1;0;;var floats -= 1
1;125;1;1;;
1;125;1;1;;
2;125;1;1;;
2;125;1;0;;
1;125;1;-1;;
1;125;1;-1;;if floats >= 0 goto east else goto pop

3;50;0;0;pop.wav;label pop
4;50;0;0;;setvel 0 4

# Falling
5;50;0;0;;label falling
6;50;0;0;;goto falling
