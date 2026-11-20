category=Locomotion

oneShot

define init:var steps = [8 + @24],
var straight = @6,
if fun > 60 and health > 60 and social > 60 and anxiety < 20 var mood = 1 else if fun > 40 and health > 40 and social > 40 and anxiety < 40 var mood = 2 else if fun > 20 and health > 20 and social > 20 and anxiety < 80 var mood = 3 else var mood = 4
var dir = @8 - 1,
if dir = 0 goto north,
if dir = 1 goto northeast,
if dir = 2 goto east,
if dir = 3 goto southeast,
if dir = 4 goto south,
if dir = 5 goto southwest,
if dir = 6 goto west,
if dir = 7 goto northwest

define progress:var steps -= 1,var straight -= 1,if @32 = 1 break,if queued > 0 break

define drawe1:if mood = 1 draw sprite faces 0 12 9 else if mood = 2 draw sprite faces 1 12 9 else if mood = 3 draw sprite faces 2 12 9 else draw sprite faces 3 12 9

define drawe2:if mood = 1 draw sprite faces 0 12 8 else if mood = 2 draw sprite faces 1 12 8 else if mood = 3 draw sprite faces 2 12 8 else draw sprite faces 3 12 8

define drawse1:if mood = 1 draw sprite faces 4 12 9 else if mood = 2 draw sprite faces 5 12 9 else if mood = 3 draw sprite faces 6 12 9 else draw sprite faces 7 12 9

define drawse2:if mood = 1 draw sprite faces 4 12 8 else if mood = 2 draw sprite faces 5 12 8 else if mood = 3 draw sprite faces 6 12 8 else draw sprite faces 7 12 8

define draws1:if mood = 1 draw sprite faces 8 10 8 else if mood = 2 draw sprite faces 9 10 8 else if mood = 3 draw sprite faces 10 10 8 else draw sprite faces 11 10 8

define draws2:if mood = 1 draw sprite faces 8 10 7 else if mood = 2 draw sprite faces 9 10 7 else if mood = 3 draw sprite faces 10 10 7 else draw sprite faces 11 10 7

define drawsw1:if mood = 1 draw sprite faces 12 9 9 else if mood = 2 draw sprite faces 13 9 9 else if mood = 3 draw sprite faces 14 9 9 else draw sprite faces 15 9 9

define drawsw2:if mood = 1 draw sprite faces 12 9 8 else if mood = 2 draw sprite faces 13 9 8 else if mood = 3 draw sprite faces 14 9 8 else draw sprite faces 15 9 8

define draww1:if mood = 1 draw sprite faces 16 9 9 else if mood = 2 draw sprite faces 17 9 9 else if mood = 3 draw sprite faces 18 9 9 else draw sprite faces 19 9 9

define draww2:if mood = 1 draw sprite faces 16 9 8 else if mood = 2 draw sprite faces 17 9 8 else if mood = 3 draw sprite faces 18 9 8 else draw sprite faces 19 9 8

#Frames
# Walk north
60;75;0;0;;label north
61;75;0;0;;call progress
62;75;0;-1;;if @4 = 1 stat energy -= 1
63;75;0;-1;;
64;75;0;-1;;
65;75;0;0;;
66;75;0;0;;
67;75;0;-1;;
68;75;0;-1;;
69;75;0;-1;;if steps <= 0 break,
if straight > 0 goto north,
var straight = @6,
if @2 = 1 goto northeast else goto northwest

# Walk north-east
70;75;1;0;;label northeast
71;75;1;-1;;call progress
72;75;1;0;;if @4 = 1 stat fun -= 1
73;75;1;-1;;
74;75;1;0;;
75;75;1;-1;;
76;75;1;0;;
77;75;1;-1;;
78;75;1;0;;
79;75;1;-1;;if steps <= 0 break,
if straight > 0 goto northeast,
var straight = @6,
if @2 = 1 goto east else goto north

# Walk east
0;75;1;0;;label east,call drawe1
1;75;1;0;;call drawe2,call progress
2;75;1;0;;call drawe2,if @4 = 1 stat energy -= 1
3;75;1;0;;call drawe2,if @3 = 1 { draw #c3795d line 17 9 19 9,draw #c3795d line 21 9 22 9 }
4;75;1;0;;call drawe1
5;75;1;0;;call drawe1
6;75;1;0;;call drawe2
7;75;1;0;;call drawe2
8;75;1;0;;call drawe2
9;75;1;0;;call drawe1,if steps <= 0 break,
if straight > 0 goto east,
var straight = @6,
if @2 = 1 goto southeast else goto northeast

# Walk south-east
10;75;1;0;;label southeast,call drawse1
11;75;1;1;;call drawse2,call progress
12;75;1;0;;call drawse2,if @4 = 1 stat fun -= 1
13;75;1;1;;call drawse2
14;75;1;0;;call drawse1,if @3 = 1 { draw #c3795d line 14 10 16 10,draw #c3795d line 19 10 21 10 }
15;75;1;1;;call drawse1
16;75;1;0;;call drawse2
17;75;1;1;;call drawse2
18;75;1;0;;call drawse2
19;75;1;1;;call drawse1,if steps <= 0 break,
if straight > 0 goto southeast,
var straight = @6,
if @2 = 1 goto south else goto east

# Walk south
20;75;0;0;;label south,call draws1
21;75;0;1;;call draws2,call progress
22;75;0;1;;call draws2,if @4 = 1 stat energy -= 1
23;75;0;1;;call draws2
24;75;0;0;;call draws1,if @3 = 1 { draw #c3795d line 11 9 13 9,draw #c3795d line 17 9 19 9 }
25;75;0;0;;call draws1
26;75;0;1;;call draws2
27;75;0;1;;call draws2
28;75;0;1;;call draws2
29;75;0;0;;call draws1,if steps <= 0 break,
if straight > 0 goto south,
var straight = @6,
if @2 = 1 goto southeast else goto southwest

# Walk south-west
30;75;-1;0;;label southwest,call drawsw1
31;75;-1;1;;call drawsw2,call progress
32;75;-1;0;;call drawsw2,if @4 = 1 stat fun -= 1
33;75;-1;1;;call drawsw2
34;75;-1;0;;call drawsw1,if @3 = 1 { draw #c3795d line 10 10 12 10,draw #c3795d line 15 10 17 10 }
35;75;-1;1;;call drawsw1
36;75;-1;0;;call drawsw2
37;75;-1;1;;call drawsw2
38;75;-1;0;;call drawsw2
39;75;-1;1;;call drawsw1,if steps <= 0 break,
if straight > 0 goto southwest,
var straight = @6,
if @2 = 1 goto west else goto south

# Walk west
40;75;-1;0;;label west,call draww1
41;75;-1;0;;call draww2,call progress
42;75;-1;0;;call draww2,if @4 = 1 stat energy -= 1
43;75;-1;0;;call draww2
44;75;-1;0;;call draww1
45;75;-1;0;;call draww1,if @3 = 1 { draw #c3795d line 9 10 10 10,draw #c3795d line 12 10 14 10 }
46;75;-1;0;;call draww2
47;75;-1;0;;call draww2
48;75;-1;0;;call draww2
49;75;-1;0;;call draww1,if steps <= 0 break,
if straight > 0 goto west,
var straight = @6,
if @2 = 1 goto northwest else goto southwest

# Walk north-west
50;75;-1;0;;label northwest
51;75;-1;-1;;call progress
52;75;-1;0;;if @4 = 1 stat fun -= 1
53;75;-1;-1;;
54;75;-1;0;;
55;75;-1;-1;;
56;75;-1;0;;
57;75;-1;-1;;
58;75;-1;0;;
59;75;-1;-1;;if steps <= 0 break,
if straight > 0 goto northwest,
var straight = @6,
if @2 = 1 goto north else goto west
