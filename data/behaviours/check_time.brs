title=Check the time
category=Idle

fun=1
hunger=1
social=-2

oneShot

#Frames
0;75;0;0;;
1;75;0;0;;
2;75;0;0;;
3;75;0;0;;
4;75;0;0;;
5;75;0;0;;
6;75;0;0;;
7;100;0;0;;
8;1200;0;0;;
9;800;0;0;;draw begin,draw darkgreen value 0 0 hour,draw darkgreen text 9 0 :,draw darkgreen value 13 0 minute
10;50;0;0;;if hour <= 9 think "Still early...",
if hour = 10 think "I should take a break...",
if hour = 11 think "Almost time for lunch...",
if hour = 12 think "Time for lunch!",
if hour = 13 think "Did I remember to eat lunch...?",
if hour = 14 think "Should I grab a cup of coffee...?",
if hour = 15 think "It's coffee o'clock!",
if hour = 16 think "Too late for coffee...?",
if hour = 17 think "Maybe I should get dinner started...",
if hour = 18 think "Time for dinner!",
if hour = 19 think "Have I done my chores for today?",
if hour = 20 think "Should i play a game...?",
if hour = 21 think "Do I have time to watch a movie?",
if hour = 22 think "It's getting late...",
if hour = 23 think "I should go to bed..."

9;50;0;0;;
10;50;0;0;;
9;1900;0;0;;
7;100;0;0;;draw end
6;75;0;0;;
5;75;0;0;;
4;75;0;0;;
3;75;0;0;;
2;75;0;0;;
1;75;0;0;;
0;75;0;0;;
