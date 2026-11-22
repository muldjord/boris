title=Go for a Car Ride
category=Fun

fun=15

oneShot
doNotDisturb

define init:var fuel = [100 + @30]

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
# STARTING
4;300;0;0;car_start.wav;label carstart
5;200;0;0;;
4;100;0;0;;
5;100;0;0;;
4;200;0;0;;
5;100;0;0;;
4;100;0;0;;
5;100;0;0;;
4;100;0;0;car_honks.wav;goto idleeast

# IDLING
# Idle north
0;50;0;0;car_idle.wav;label idlenorth,call usefuel
1;50;0;0;;if kdir = -1 goto idlenorth,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto ridenortheast,
if kdir = 3 goto ridenortheast,
if kdir = 4 goto ridenortheast,
if kdir = 5 goto ridenorthwest,
if kdir = 6 goto ridenorthwest,
if kdir = 7 goto ridenorthwest

# Idle northeast
2;50;0;0;car_idle.wav;label idlenortheast,call usefuel
3;50;0;0;;if kdir = -1 goto idlenortheast,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto rideeast,
if kdir = 3 goto rideeast,
if kdir = 4 goto rideeast,
if kdir = 5 goto rideeast,
if kdir = 6 goto ridenorth,
if kdir = 7 goto ridenorth

# Idle east
4;50;0;0;car_idle.wav;label idleeast,call usefuel
5;50;0;0;;if kdir = -1 goto idleeast,
if kdir = 0 goto ridenortheast,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto rideeast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesoutheast,
if kdir = 5 goto ridesoutheast,
if kdir = 6 goto ridesoutheast,
if kdir = 7 goto ridenortheast

# Idle southeast
6;50;0;0;car_idle.wav;label idlesoutheast,call usefuel
7;50;0;0;;if kdir = -1 goto idlesoutheast,
if kdir = 0 goto rideeast,
if kdir = 1 goto rideeast,
if kdir = 2 goto rideeast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouth,
if kdir = 6 goto ridesouth,
if kdir = 7 goto ridesouth

# Idle south
8;50;0;0;car_idle.wav;label idlesouth,call usefuel
9;50;0;0;;if kdir = -1 goto idlesouth,
if kdir = 0 goto ridesouthwest,
if kdir = 1 goto ridesoutheast,
if kdir = 2 goto ridesoutheast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridesouthwest,
if kdir = 7 goto ridesouthwest

# Idle southwest
10;50;0;0;car_idle.wav;label idlesouthwest,call usefuel
11;50;0;0;;if kdir = -1 goto idlesouthwest,
if kdir = 0 goto ridewest,
if kdir = 1 goto ridewest,
if kdir = 2 goto ridesouth,
if kdir = 3 goto ridesouth,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridewest

# Idle west
12;50;0;0;car_idle.wav;label idlewest,call usefuel
13;50;0;0;;if kdir = -1 goto idlewest,
if kdir = 0 goto ridenorthwest,
if kdir = 1 goto ridenorthwest,
if kdir = 2 goto ridenorthwest,
if kdir = 3 goto ridesouthwest,
if kdir = 4 goto ridesouthwest,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridenorthwest

# Idle northwest
14;50;0;0;car_idle.wav;label idlenorthwest,call usefuel
15;50;0;0;;if kdir = -1 goto idlenorthwest,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenorth,
if kdir = 2 goto ridenorth,
if kdir = 3 goto ridenorth,
if kdir = 4 goto ridewest,
if kdir = 5 goto ridewest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridenorthwest

# RIDING
# Ride north
16;50;0;-3;car_driving.wav;label ridenorth,call usefuel
16;50;0;-3;;if kdir = -1 goto idlenorth,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto ridenortheast,
if kdir = 3 goto ridenortheast,
if kdir = 4 goto ridenortheast,
if kdir = 5 goto ridenorthwest,
if kdir = 6 goto ridenorthwest,
if kdir = 7 goto ridenorthwest

# Ride northeast
17;50;3;-2;car_driving.wav;label ridenortheast,call usefuel
17;50;3;-2;;if kdir = -1 goto idlenortheast,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto rideeast,
if kdir = 3 goto rideeast,
if kdir = 4 goto rideeast,
if kdir = 5 goto rideeast,
if kdir = 6 goto ridenorth,
if kdir = 7 goto ridenorth

# Ride east
18;50;4;0;car_driving.wav;label rideeast,call usefuel
18;50;4;0;;if kdir = -1 goto idleeast,
if kdir = 0 goto ridenortheast,
if kdir = 1 goto ridenortheast,
if kdir = 2 goto rideeast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesoutheast,
if kdir = 5 goto ridesoutheast,
if kdir = 6 goto ridesoutheast,
if kdir = 7 goto ridenortheast

# Ride southeast
19;50;3;2;car_driving.wav;label ridesoutheast,call usefuel
19;50;3;2;;if kdir = -1 goto idlesoutheast,
if kdir = 0 goto rideeast,
if kdir = 1 goto rideeast,
if kdir = 2 goto rideeast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouth,
if kdir = 6 goto ridesouth,
if kdir = 7 goto ridesouth

# Ride south
20;50;0;3;car_driving.wav;label ridesouth,call usefuel
20;50;0;3;;if kdir = -1 goto idlesouth,
if kdir = 0 goto ridesouthwest,
if kdir = 1 goto ridesoutheast,
if kdir = 2 goto ridesoutheast,
if kdir = 3 goto ridesoutheast,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridesouthwest,
if kdir = 7 goto ridesouthwest

# Ride southwest
21;50;-3;2;car_driving.wav;label ridesouthwest,call usefuel
21;50;-3;2;;if kdir = -1 goto idlesouthwest,
if kdir = 0 goto ridewest,
if kdir = 1 goto ridewest,
if kdir = 2 goto ridesouth,
if kdir = 3 goto ridesouth,
if kdir = 4 goto ridesouth,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridewest

# Ride west
22;50;-4;0;car_driving.wav;label ridewest,call usefuel
22;50;-4;0;;if kdir = -1 goto idlewest,
if kdir = 0 goto ridenorthwest,
if kdir = 1 goto ridenorthwest,
if kdir = 2 goto ridenorthwest,
if kdir = 3 goto ridesouthwest,
if kdir = 4 goto ridesouthwest,
if kdir = 5 goto ridesouthwest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridenorthwest

# Ride northwest
23;50;-3;-2;car_driving.wav;label ridenorthwest,call usefuel
23;50;-3;-2;;if kdir = -1 goto idlenorthwest,
if kdir = 0 goto ridenorth,
if kdir = 1 goto ridenorth,
if kdir = 2 goto ridenorth,
if kdir = 3 goto ridenorth,
if kdir = 4 goto ridewest,
if kdir = 5 goto ridewest,
if kdir = 6 goto ridewest,
if kdir = 7 goto ridenorthwest

# End ride
4;2500;0;0;car_turnoff.wav;label endride,break
