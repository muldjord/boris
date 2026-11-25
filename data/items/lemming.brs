title=Lemming
category=Fun

weight=2
bounce=2
range=24

noShadow

define init:call nextmove

define nextmove:var rand = @10,
if rand = 1 goto walkright,
if rand = 2 goto walkleft,
if rand = 3 call initdropright,
if rand = 4 call initdropleft,
if rand = 5 goto crawlright,
if rand = 6 goto crawlleft,
if rand = 7 { var blocks = 7,goto builderright },
if rand = 8 { var blocks = 7,goto builderleft },
if rand = 9 goto blocker,
if rand = 10 goto digger

define initdropright:var altitude = [@20 + 2],
var dropheight = 0,
goto dropright

define initdropleft:var altitude = [@20 + 2],
var dropheight = 0,
goto dropleft

#Frames

# Walk right
0;75;1;0;;label walkright
1;75;1;0;;
2;75;1;0;;
3;75;1;0;;
4;75;1;0;;
5;75;1;0;;
6;75;1;0;;
7;75;1;0;;if @5 > 1 goto walkright,call nextmove

# Walk left
8;75;-1;0;;label walkleft
9;75;-1;0;;
10;75;-1;0;;
11;75;-1;0;;
12;75;-1;0;;
13;75;-1;0;;
14;75;-1;0;;
15;75;-1;0;;if @5 > 1 goto walkleft,call nextmove

# Drop right
16;75;0;2;;label dropright
17;75;0;2;;var dropheight += 1
18;75;0;2;;var altitude -= 2
19;75;0;2;;if @5 = 1 goto umbrellaright,if altitude >= 0 goto dropright,if dropheight >= 15 goto groundsplat,goto walkright

# Drop left
20;75;0;2;;label dropleft
21;75;0;2;;var dropheight += 1
22;75;0;2;;var altitude -= 2
23;75;0;2;;if @5 = 1 goto umbrellaleft,if altitude >= 0 goto dropleft,if dropheight >= 15 goto groundsplat,goto walkleft

# Crawl right
24;75;0;0;;label crawlright
25;75;0;0;;
26;75;0;0;;
27;75;0;0;;
28;75;0;-1;;
29;75;0;-1;;
30;75;0;-1;;
31;75;0;-1;;if @5 > 1 goto crawlright,goto endcrawlright

# End crawl right
32;75;0;0;;label endcrawlright
33;75;0;0;;
34;75;0;0;;
35;75;0;0;;
36;75;0;0;;
37;75;0;0;;
38;75;1;0;;
39;75;1;0;;goto walkright

# Crawl left
40;75;0;0;;label crawlleft
41;75;0;0;;
42;75;0;0;;
43;75;0;0;;
44;75;0;-1;;
45;75;0;-1;;
46;75;0;-1;;
47;75;0;-1;;if @5 > 1 goto crawlleft,goto endcrawlleft

# End crawl left
48;75;0;0;;label endcrawlleft
49;75;0;0;;
50;75;0;0;;
51;75;0;0;;
52;75;0;0;;
53;75;0;0;;
54;75;-1;0;;
55;75;0;-1;;goto walkleft

# Umbrella right
56;75;0;2;;label umbrellaright
57;75;0;2;;
58;75;0;2;;
59;75;0;2;;
60;75;0;1;;label loopumbrellaright
61;75;0;1;;var altitude -= 1
62;75;0;1;;
63;75;0;1;;
62;75;0;1;;
61;75;0;1;;if altitude >= 0 goto loopumbrellaright,goto walkright

# Umbrella left
64;75;0;2;;label umbrellaleft
65;75;0;2;;
66;75;0;2;;
67;75;0;2;;
68;75;0;1;;label loopumbrellaleft
69;75;0;1;;var altitude -= 1
70;75;0;1;;
71;75;0;1;;
70;75;0;1;;
69;75;0;1;;if altitude >= 0 goto loopumbrellaleft,goto walkleft

# Ground splat
83;75;0;0;;label groundsplat
84;75;0;0;;
85;75;0;0;;
86;75;0;0;;
87;75;0;0;;
88;75;0;0;;
89;75;0;0;;
90;75;0;0;;
91;75;0;0;;
92;75;0;0;;
93;75;0;0;;
94;75;0;0;;
95;75;0;0;;
96;75;0;0;;
97;75;0;0;;
98;75;0;0;;break

# Blocker
100;75;0;0;;label blocker
101;75;0;0;;
102;75;0;0;;
103;75;0;0;;
104;75;0;0;;
105;75;0;0;;
106;75;0;0;;
107;75;0;0;;
108;75;0;0;;
109;75;0;0;;
110;75;0;0;;
111;75;0;0;;
112;75;0;0;;
113;75;0;0;;
114;75;0;0;;if @5 > 1 goto blocker,call nextmove

# Builder right
115;75;0;0;;label builderright
116;75;0;0;;var blocks -= 1
117;75;0;0;;
118;75;0;0;;
119;75;0;0;;
120;75;0;0;;
121;75;0;0;;
122;75;0;0;;
123;75;0;0;;
124;75;0;0;;
125;75;0;0;;
126;75;0;0;;
127;75;0;0;;
128;75;0;0;;
129;75;1;-1;;
130;75;1;0;;if blocks >= 0 goto builderright
131;75;0;0;;
132;75;0;0;;
133;75;0;0;;
134;75;0;0;;
135;75;0;0;;
136;75;0;0;;
137;75;0;0;;if @2 = 1 goto walkright else call initdropright

# Builder left
138;75;0;0;;label builderleft
139;75;0;0;;var blocks -= 1
140;75;0;0;;
141;75;0;0;;
142;75;0;0;;
143;75;0;0;;
144;75;0;0;;
145;75;0;0;;
146;75;0;0;;
147;75;0;0;;
148;75;0;0;;
149;75;0;0;;
150;75;0;0;;
151;75;0;0;;
152;75;-1;-1;;
153;75;-1;0;;if blocks >= 0 goto builderleft
154;75;0;0;;
155;75;0;0;;
156;75;0;0;;
157;75;0;0;;
158;75;0;0;;
159;75;0;0;;
160;75;0;0;;if @2 = 1 goto walkleft else call initdropleft

# Digger
161;75;0;1;;label digger
162;75;0;0;;
163;75;0;0;;
164;75;0;0;;
165;75;0;0;;
166;75;0;0;;
167;75;0;0;;
168;75;0;0;;
169;75;0;1;;
170;75;0;0;;
171;75;0;0;;
172;75;0;0;;
173;75;0;0;;
174;75;0;0;;
175;75;0;0;;
176;75;0;0;;if @5 > 1 goto digger,call nextmove
