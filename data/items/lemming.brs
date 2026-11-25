title=Lemming
category=Fun

weight=8
bounce=2
range=24

noShadow

define init:var direction = @2,if @2 = 1 call initdropright else call initdropleft

define nextmove:if direction = 1 { var rand = @8,
if rand = 1 goto walkright,
if rand = 2 call initdropright,
if rand = 3 goto crawlright,
if rand = 4 { var blocks = 7,goto builderright },
if rand = 5 goto blocker,
if rand = 6 goto digger,
if rand = 7 goto basherright,
if rand = 8 goto hatchetright } else { var rand = @8,
if rand = 1 goto walkleft,
if rand = 2 call initdropleft,
if rand = 3 goto crawlleft,
if rand = 4 { var blocks = 7,goto builderleft },
if rand = 5 goto blocker,
if rand = 6 goto digger,
if rand = 7 goto basherleft,
if rand = 8 goto hatchetleft }

define initdropright:var altitude = [@64 + 64],
var dropheight = 0,
goto dropright

define initdropleft:var altitude = [@64 + 64],
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
7;75;1;0;;if @5 > 1 goto walkright,if @2 = 1 { var direction = 2,goto walkleft },call nextmove

# Walk left
8;75;-1;0;;label walkleft
9;75;-1;0;;
10;75;-1;0;;
11;75;-1;0;;
12;75;-1;0;;
13;75;-1;0;;
14;75;-1;0;;
15;75;-1;0;;if @5 > 1 goto walkleft,if @2 = 1 { var direction = 1,goto walkright },call nextmove

# Drop right
16;75;0;2;;label dropright
17;75;0;2;;var dropheight += 8
18;75;0;2;;var altitude -= 8
19;75;0;2;;if @5 = 1 goto umbrellaright,if altitude >= 0 goto dropright,if dropheight >= 64 goto groundsplat,goto walkright

# Umbrella right
56;75;0;2;;label umbrellaright
57;75;0;2;;var altitude -= 16
58;75;0;2;;
59;75;0;3;;
61;75;0;2;;
61;75;0;-1;;
61;75;0;0;;
61;75;0;1;;
61;75;0;1;;
61;75;0;2;;
62;75;0;2;;
63;75;0;2;;label loopumbrellaright
63;75;0;2;;var altitude -= 16
62;75;0;2;;
61;75;0;2;;
60;75;0;2;;
60;75;0;2;;
61;75;0;2;;
62;75;0;2;;if altitude >= 0 goto loopumbrellaright,goto walkright

# Drop left
20;75;0;2;;label dropleft
21;75;0;2;;var dropheight += 8
22;75;0;2;;var altitude -= 8
23;75;0;2;;if @5 = 1 goto umbrellaleft,if altitude >= 0 goto dropleft,if dropheight >= 64 goto groundsplat,goto walkleft

# Umbrella left
64;75;0;2;;label umbrellaleft
65;75;0;2;;var altitude -= 16
66;75;0;2;;
67;75;0;3;;
69;75;0;2;;
69;75;0;-1;;
69;75;0;0;;
69;75;0;1;;
69;75;0;1;;
69;75;0;2;;
70;75;0;2;;
71;75;0;2;;label loopumbrellaleft
71;75;0;2;;var altitude -= 16
70;75;0;2;;
69;75;0;2;;
68;75;0;2;;
68;75;0;2;;
69;75;0;2;;
70;75;0;2;;if altitude >= 0 goto loopumbrellaleft,goto walkleft

# Ground splat
82;75;0;0;;label groundsplat
83;75;0;0;;
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
97;75;0;0;;break

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
32;75;0;-4;;label endcrawlright
33;75;0;-2;;
34;75;0;-1;;
35;75;0;0;;
36;75;0;0;;
37;75;0;0;;
38;75;0;0;;
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
48;75;0;-4;;label endcrawlleft
49;75;0;-2;;
50;75;0;-1;;
51;75;0;0;;
52;75;0;0;;
53;75;0;0;;
54;75;0;0;;
55;75;-1;0;;goto walkleft

# Blocker
99;75;0;0;;label blocker
100;75;0;0;;
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
113;75;0;0;;if @5 > 1 goto blocker,var direction = @2,call nextmove

# Builder right
114;75;0;0;;label builderright
115;75;0;0;;var blocks -= 1
116;75;0;0;;
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
128;75;1;-1;;
129;75;1;0;;if blocks >= 0 goto builderright
130;75;0;0;;
131;75;0;0;;
132;75;0;0;;
133;75;0;0;;
134;75;0;0;;
135;75;0;0;;
136;75;0;0;;if @2 = 1 goto walkright else call initdropright

# Builder left
137;75;0;0;;label builderleft
138;75;0;0;;var blocks -= 1
139;75;0;0;;
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
151;75;-1;-1;;
152;75;-1;0;;if blocks >= 0 goto builderleft
153;75;0;0;;
154;75;0;0;;
155;75;0;0;;
156;75;0;0;;
157;75;0;0;;
158;75;0;0;;
159;75;0;0;;if @2 = 1 goto walkleft else call initdropleft

# Digger
160;75;0;1;;label digger
161;75;0;0;;
162;75;0;0;;
163;75;0;0;;
164;75;0;0;;
165;75;0;0;;
166;75;0;0;;
167;75;0;0;;
168;75;0;1;;
169;75;0;0;;
170;75;0;0;;
171;75;0;0;;
172;75;0;0;;
173;75;0;0;;
174;75;0;0;;
175;75;0;0;;if @5 > 1 goto digger,var direction = @2,call nextmove

# Basher right
176;75;1;0;;label basherright
177;75;0;0;;
178;75;0;0;;
179;75;1;0;;
180;75;0;0;;
181;75;1;0;;
182;75;0;0;;
183;75;0;0;;
184;75;1;0;;
185;75;1;0;;
186;75;0;0;;
187;75;0;0;;
188;75;0;0;;
189;75;0;0;;
190;75;0;0;;
191;75;0;0;;
192;75;0;0;;
193;75;1;0;;
194;75;0;0;;
195;75;0;0;;
196;75;0;0;;
197;75;1;0;;
198;75;1;0;;
199;75;0;0;;
200;75;1;0;;
201;75;1;0;;
202;75;0;0;;
203;75;0;0;;
204;75;0;0;;
205;75;0;0;;
206;75;0;0;;
207;75;0;0;;if @4 > 1 goto basherright,call nextmove

# Basher left
208;75;-1;0;;label basherleft
209;75;0;0;;
210;75;0;0;;
211;75;-1;0;;
212;75;0;0;;
213;75;-1;0;;
214;75;0;0;;
215;75;0;0;;
216;75;-1;0;;
217;75;-1;0;;
218;75;0;0;;
219;75;0;0;;
220;75;0;0;;
221;75;0;0;;
222;75;0;0;;
223;75;0;0;;
224;75;0;0;;
225;75;-1;0;;
226;75;0;0;;
227;75;0;0;;
228;75;0;0;;
229;75;-1;0;;
230;75;-1;0;;
231;75;0;0;;
232;75;-1;0;;
233;75;-1;0;;
234;75;0;0;;
235;75;0;0;;
236;75;0;0;;
237;75;0;0;;
238;75;0;0;;
239;75;0;0;;if @4 > 1 goto basherleft,call nextmove

# Hatchet right
240;75;0;0;;label hatchetright
241;75;0;0;;
242;75;0;0;;
243;75;1;0;;
244;75;1;0;;
245;75;1;0;;
246;75;1;0;;
247;75;0;0;;
248;75;0;0;;
249;75;0;0;;
250;75;0;0;;
251;75;0;0;;
252;75;0;0;;
253;75;0;0;;
254;75;0;0;;
255;75;0;0;;
256;75;0;0;;
257;75;0;0;;
258;75;0;1;;
259;75;0;1;;
260;75;0;0;;
261;75;0;0;;
262;75;0;0;;if @5 > 1 goto hatchetright,call nextmove

# Hatchet left
263;75;0;0;;label hatchetleft
264;75;0;0;;
265;75;0;0;;
266;75;-1;0;;
267;75;-1;0;;
268;75;-1;0;;
269;75;-1;0;;
270;75;0;0;;
271;75;0;0;;
272;75;0;0;;
273;75;0;0;;
274;75;0;0;;
275;75;0;0;;
276;75;0;0;;
277;75;0;0;;
278;75;0;0;;
279;75;0;0;;
280;75;0;0;;
281;75;0;1;;
282;75;0;1;;
283;75;0;0;;
284;75;0;0;;
285;75;0;0;;if @5 > 1 goto hatchetleft,call nextmove
