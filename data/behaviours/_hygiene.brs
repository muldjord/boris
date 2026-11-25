energy=-4
fun=-10
social=-8

# Adding 'doNotDisturb' to this one will cause Boris to be unflattenable when a car runs him over
oneShot
allowFlip

#Frames
0;75;0;0;flies.wav;
1;75;0;0;;
2;75;0;0;;
3;75;0;0;;
4;75;0;0;;
5;75;0;0;;
6;75;0;0;;var loops = [2 + @3]

7;75;0;0;;label loop
8;75;0;0;;
9;75;0;0;;
10;75;0;0;;
11;75;0;0;;
12;75;0;0;;
13;75;0;0;;
14;75;0;0;;var loops -= 1,if loops > 0 goto loop

6;75;0;0;;
5;75;0;0;;
4;75;0;0;;
3;75;0;0;;
2;75;0;0;;
1;75;0;0;;
0;75;0;0;;break
