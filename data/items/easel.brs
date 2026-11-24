title=Easel
category=Fun

reaction=paint_picture,initpaint,8,-2

weight=7
bounce=2

doNotDisturb

define init:draw begin,var canvas = @8,
if canvas = 1 goto white,
if canvas = 2 goto red,
if canvas = 3 goto orange,
if canvas = 4 goto yellow,
if canvas = 5 goto green,
if canvas = 6 goto cyan,
if canvas = 7 goto blue,
if canvas = 8 goto purple

define paintpart:var progress += 1,
draw sprite paint element xcoord ycoord 0 0 8 progress,
draw transparent rectangle 6 0 20 25,
draw transparent rectangle 7 0 18 24

#Frames
0;3000;0;0;;label white,stop
1;3000;0;0;;label red,stop
2;3000;0;0;;label orange,stop
3;3000;0;0;;label yellow,stop
4;3000;0;0;;label green,stop
5;3000;0;0;;label cyan,stop
6;3000;0;0;;label blue,stop
7;3000;0;0;;label purple,stop

0;0;0;0;;label initpaint,
if canvas = 1 goto paintwhite,
if canvas = 2 goto paintred,
if canvas = 3 goto paintorange,
if canvas = 4 goto paintyellow,
if canvas = 5 goto paintgreen,
if canvas = 6 goto paintcyan,
if canvas = 7 goto paintblue,
if canvas = 8 goto paintpurple

# Turn around
0;50;0;0;;label paintwhite
0;50;0;0;;var stepbacks = 1
0;50;0;0;;var cycles = 4
0;400;0;0;;

# Reach in to paint
0;50;0;0;;label initstrokeswhite
0;50;0;0;;var strokes = 6
0;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

0;50;0;0;;label nextstrokewhite,call paintpart

0;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokewhite else goto waitwhite

0;50;0;0;;label waitwhite
0;50;0;0;;var cycles -= 1
0;50;0;0;;
0;1000;0;0;;if cycles > 0 goto initstrokeswhite else { if stepbacks > 0 goto stepbackwhite else goto endpaintwhite }

0;50;0;0;;label stepbackwhite
0;50;0;0;;var stepbacks -= 1
0;50;0;0;;var cycles = 4
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;3000;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;
0;50;0;0;;goto initstrokeswhite

0;100;0;0;;label endpaintwhite,stop

# Turn around
1;50;0;0;;label paintred
1;50;0;0;;var stepbacks = 1
1;50;0;0;;var cycles = 4
1;400;0;0;;

# Reach in to paint
1;50;0;0;;label initstrokesred
1;50;0;0;;var strokes = 6
1;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

1;50;0;0;;label nextstrokered,call paintpart

1;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokered else goto waitred

1;50;0;0;;label waitred
1;50;0;0;;var cycles -= 1
1;50;0;0;;
1;1000;0;0;;if cycles > 0 goto initstrokesred else { if stepbacks > 0 goto stepbackred else goto endpaintred }

1;50;0;0;;label stepbackred
1;50;0;0;;var stepbacks -= 1
1;50;0;0;;var cycles = 4
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;3000;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;
1;50;0;0;;goto initstrokesred

1;100;0;0;;label endpaintred,stop

# Turn around
2;50;0;0;;label paintorange
2;50;0;0;;var stepbacks = 1
2;50;0;0;;var cycles = 4
2;400;0;0;;

# Reach in to paint
2;50;0;0;;label initstrokesorange
2;50;0;0;;var strokes = 6
2;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

2;50;0;0;;label nextstrokeorange,call paintpart

2;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokeorange else goto waitorange

2;50;0;0;;label waitorange
2;50;0;0;;var cycles -= 1
2;50;0;0;;
2;1000;0;0;;if cycles > 0 goto initstrokesorange else { if stepbacks > 0 goto stepbackorange else goto endpaintorange }

2;50;0;0;;label stepbackorange
2;50;0;0;;var stepbacks -= 1
2;50;0;0;;var cycles = 4
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;3000;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;
2;50;0;0;;goto initstrokesorange

2;100;0;0;;label endpaintorange,stop

# Turn around
3;50;0;0;;label paintyellow
3;50;0;0;;var stepbacks = 1
3;50;0;0;;var cycles = 4
3;400;0;0;;

# Reach in to paint
3;50;0;0;;label initstrokesyellow
3;50;0;0;;var strokes = 6
3;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

3;50;0;0;;label nextstrokeyellow,call paintpart

3;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokeyellow else goto waityellow

3;50;0;0;;label waityellow
3;50;0;0;;var cycles -= 1
3;50;0;0;;
3;1000;0;0;;if cycles > 0 goto initstrokesyellow else { if stepbacks > 0 goto stepbackyellow else goto endpaintyellow }

3;50;0;0;;label stepbackyellow
3;50;0;0;;var stepbacks -= 1
3;50;0;0;;var cycles = 4
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;3000;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;
3;50;0;0;;goto initstrokesyellow

3;100;0;0;;label endpaintyellow,stop

# Turn around
4;50;0;0;;label paintgreen
4;50;0;0;;var stepbacks = 1
4;50;0;0;;var cycles = 4
4;400;0;0;;

# Reach in to paint
4;50;0;0;;label initstrokesgreen
4;50;0;0;;var strokes = 6
4;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

4;50;0;0;;label nextstrokegreen,call paintpart

4;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokegreen else goto waitgreen

4;50;0;0;;label waitgreen
4;50;0;0;;var cycles -= 1
4;50;0;0;;
4;1000;0;0;;if cycles > 0 goto initstrokesgreen else { if stepbacks > 0 goto stepbackgreen else goto endpaintgreen }

4;50;0;0;;label stepbackgreen
4;50;0;0;;var stepbacks -= 1
4;50;0;0;;var cycles = 4
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;3000;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;
4;50;0;0;;goto initstrokesgreen

4;100;0;0;;label endpaintgreen,stop

# Turn around
5;50;0;0;;label paintcyan
5;50;0;0;;var stepbacks = 1
5;50;0;0;;var cycles = 4
5;400;0;0;;

# Reach in to paint
5;50;0;0;;label initstrokescyan
5;50;0;0;;var strokes = 6
5;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

5;50;0;0;;label nextstrokecyan,call paintpart

5;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokecyan else goto waitcyan

5;50;0;0;;label waitcyan
5;50;0;0;;var cycles -= 1
5;50;0;0;;
5;1000;0;0;;if cycles > 0 goto initstrokescyan else { if stepbacks > 0 goto stepbackcyan else goto endpaintcyan }

5;50;0;0;;label stepbackcyan
5;50;0;0;;var stepbacks -= 1
5;50;0;0;;var cycles = 4
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;3000;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;
5;50;0;0;;goto initstrokescyan

5;100;0;0;;label endpaintcyan,stop

# Turn around
6;50;0;0;;label paintblue
6;50;0;0;;var stepbacks = 1
6;50;0;0;;var cycles = 4
6;400;0;0;;

# Reach in to paint
6;50;0;0;;label initstrokesblue
6;50;0;0;;var strokes = 6
6;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

6;50;0;0;;label nextstrokeblue,call paintpart

6;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokeblue else goto waitblue

6;50;0;0;;label waitblue
6;50;0;0;;var cycles -= 1
6;50;0;0;;
6;1000;0;0;;if cycles > 0 goto initstrokesblue else { if stepbacks > 0 goto stepbackblue else goto endpaintblue }

6;50;0;0;;label stepbackblue
6;50;0;0;;var stepbacks -= 1
6;50;0;0;;var cycles = 4
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;3000;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;
6;50;0;0;;goto initstrokesblue

6;100;0;0;;label endpaintblue,stop

# Turn around
7;50;0;0;;label paintpurple
7;50;0;0;;var stepbacks = 1
7;50;0;0;;var cycles = 4
7;400;0;0;;

# Reach in to paint
7;50;0;0;;label initstrokespurple
7;50;0;0;;var strokes = 6
7;50;0;0;;var element = [@68 - 1],
var progress = 0,
var xcoord = [6 + (@14 - 1)],
var ycoord = [@15 - 1]

7;50;0;0;;label nextstrokepurple,call paintpart

7;50;0;0;;call paintpart,var strokes -= 1,if strokes > 0 goto nextstrokepurple else goto waitpurple

7;50;0;0;;label waitpurple
7;50;0;0;;var cycles -= 1
7;50;0;0;;
7;1000;0;0;;if cycles > 0 goto initstrokespurple else { if stepbacks > 0 goto stepbackpurple else goto endpaintpurple }

7;50;0;0;;label stepbackpurple
7;50;0;0;;var stepbacks -= 1
7;50;0;0;;var cycles = 4
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;3000;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;
7;50;0;0;;goto initstrokespurple

7;100;0;0;;label endpaintpurple,stop
