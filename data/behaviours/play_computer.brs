title=Play computer
category=Fun

energy=-3
hunger=1
social=-8
hygiene=-3

oneShot
pitchLock

#Frames
0;75;0;0;;
1;75;0;0;;
2;75;0;0;;
3;75;0;0;;
4;75;0;0;;
5;75;0;0;;
6;75;0;0;;
7;75;0;0;;
8;500;0;0;;
9;800;0;0;game_boot.wav;
9;200;0;0;;var game = @4,
if game = 1 behav invaders,
if game = 2 behav mazegame,
if game = 3 behav pong,
if game = 4 behav speed_race

