weight=1
bounce=1

hidden

define init:var air = 15,
var dir = [@8 - 1],
sound deflate.wav,
if dir = 0 goto north,
if dir = 1 goto northeast,
if dir = 2 goto east,
if dir = 3 goto southeast,
if dir = 4 goto south,
if dir = 5 goto southwest,
if dir = 6 goto west,
if dir = 7 goto northwest

define checkair:var air -= 1,
if air <= 0 { spawn balloon 0 0,break }

#Frames
0;30;0;-5;;label north
1;30;0;-5;;
0;30;0;-5;;
1;30;0;-5;;call checkair,if @2 = 1 goto northwest else goto northeast

2;30;7;-3;;label northeast
3;30;7;-3;;
2;30;7;-3;;
3;30;7;-3;;call checkair,if @2 = 1 goto north else goto east

4;30;10;0;;label east
5;30;10;0;;
4;30;10;0;;
5;30;10;0;;call checkair,if @2 = 1 goto northeast else goto southeast

6;30;7;3;;label southeast
7;30;7;3;;
6;30;7;3;;
7;30;7;3;;call checkair,if @2 = 1 goto east else goto south

8;30;0;10;;label south
9;30;0;10;;
8;30;0;10;;
9;30;0;10;;call checkair,if @2 = 1 goto southeast else goto southwest

10;30;-7;3;;label southwest
11;30;-7;3;;
10;30;-7;3;;
11;30;-7;3;;call checkair,if @2 = 1 goto south else goto west

12;30;-10;0;;label west
13;30;-10;0;;
12;30;-10;0;;
13;30;-10;0;;call checkair,if @2 = 1 goto southwest else goto northwest

14;30;-7;-3;;label northwest
15;30;-7;-3;;
14;30;-7;-3;;
15;30;-7;-3;;call checkair,if @2 = 1 goto west else goto north