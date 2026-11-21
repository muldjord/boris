title=Go for a Car Ride
category=Fun

fun=15

oneShot
doNotDisturb

define init:sound car_start.wav,var fuel = [100 + @30]

define nextride:if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto rideeast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridenorthwest

define usefuel:var fuel -= 1,
if fuel <= 0 goto endride

#Frames
# Idle north
0;50;0;0;car_idle.wav;label idlenorth,call usefuel
1;50;0;0;;if kdir = -1 goto idlenorth else call nextride

# Idle northeast
2;50;0;0;car_idle.wav;label idlenortheast,call usefuel
3;50;0;0;;if kdir = -1 goto idlenortheast else call nextride

# Idle east
4;50;0;0;car_idle.wav;label idleeast,call usefuel
5;50;0;0;;if kdir = -1 goto idleeast else call nextride

# Idle southeast
6;50;0;0;car_idle.wav;label idlesoutheast,call usefuel
7;50;0;0;;if kdir = -1 goto idlesoutheast else call nextride

# Idle south
8;50;0;0;car_idle.wav;label idlesouth,call usefuel
9;50;0;0;;if kdir = -1 goto idlesouth else call nextride

# Idle southwest
10;50;0;0;car_idle.wav;label idlesouthwest,call usefuel
11;50;0;0;;if kdir = -1 goto idlesouthwest else call nextride

# Idle west
12;50;0;0;car_idle.wav;label idlewest,call usefuel
13;50;0;0;;if kdir = -1 goto idlewest else call nextride

# Idle northwest
14;50;0;0;car_idle.wav;label idlenorthwest,call usefuel
15;50;0;0;;if kdir = -1 goto idlenorthwest else call nextride

# Ride north
16;50;0;-3;car_driving.wav;label ridenorth,call usefuel
16;50;0;-3;;if kdir = -1 goto idlenorth,call nextride

# Ride northeast
17;50;3;-2;car_driving.wav;label ridenortheast,call usefuel
17;50;3;-2;;if kdir = -1 goto idlenortheast,call nextride

# Ride east
18;50;4;0;car_driving.wav;label rideeast,call usefuel
18;50;4;0;;if kdir = -1 goto idleeast,call nextride

# Ride southeast
19;50;3;2;car_driving.wav;label ridesoutheast,call usefuel
19;50;3;2;;if kdir = -1 goto idlesoutheast,call nextride

# Ride south
20;50;0;3;car_driving.wav;label ridesouth,call usefuel
20;50;0;3;;if kdir = -1 goto idlesouth,call nextride

# Ride southwest
21;50;-3;2;car_driving.wav;label ridesouthwest,call usefuel
21;50;-3;2;;if kdir = -1 goto idlesouthwest,call nextride

# Ride west
22;50;-4;0;car_driving.wav;label ridewest,call usefuel
22;50;-4;0;;if kdir = -1 goto idlewest,call nextride

# Ride northwest
23;50;-3;-2;car_driving.wav;label ridenorthwest,call usefuel
23;50;-3;-2;;if kdir = -1 goto idlenorthwest,call nextride

4;1500;0;0;car_turnoff.wav;label endride,break
