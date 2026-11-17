reaction=pie_hit

weight=5
bounce=2
range=24

define init:var airtime = [18 + @5]

define dropped:spawn pie 0 0,
break

#Frames
0;30;0;-7;;label north
0;30;0;-7;;var airtime -= 1
0;30;0;-7;;
0;30;0;-7;;if airtime <= 0 call dropped else goto north

1;30;5;-5;;label northeast
1;30;5;-5;;var airtime -= 1
1;30;5;-5;;
1;30;5;-5;;if airtime <= 0 call dropped else goto northeast

2;30;7;0;;label east
2;30;7;0;;var airtime -= 1
2;30;7;0;;
2;30;7;0;;if airtime <= 0 call dropped else goto east

3;30;5;5;;label southeast
3;30;5;5;;var airtime -= 1
3;30;5;5;;
3;30;5;5;;if airtime <= 0 call dropped else goto southeast

4;30;0;7;;label south
4;30;0;7;;var airtime -= 1
4;30;0;7;;
4;30;0;7;;if airtime <= 0 call dropped else goto south

5;30;-5;5;;label southwest
5;30;-5;5;;var airtime -= 1
5;30;-5;5;;
5;30;-5;5;;if airtime <= 0 call dropped else goto southwest

6;30;-7;0;;label west
6;30;-7;0;;var airtime -= 1
6;30;-7;0;;
6;30;-7;0;;if airtime <= 0 call dropped else goto west

7;30;-5;-5;;label northwest
7;30;-5;-5;;var airtime -= 1
7;30;-5;-5;;
7;30;-5;-5;;if airtime <= 0 call dropped else goto northwest