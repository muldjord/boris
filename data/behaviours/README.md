# Behaviour documentation
The files in this folder defines the behaviours of Boris. You can add new behaviours simply by supplying new pairs of png+dat files. The format is documented below. Check the existing ones for examples.

Note: All behaviour filenames that start with `_` (underscore) MUST NOT be deleted. These are hardcoded behaviours. Deleting any of them will make Boris crash eventually. Any behaviour filename beginning with an underscore will also be ignored whenever Boris chooses random behaviours, so this is useful for test behaviours when the `idkfa=true` setting is set in `config.ini`. This enables the Boris right-click `idkfa` behaviour menu that allows you to select and queue any available behaviour.

## PNG sprite sheets
The png files are horizontal sets of sprites that are loaded together with the `<FILENAME>.dat` files. The dimensions of each sprite inside the sprite sheet must be 32x32 pixels. The color format of the pngs are not important as long as they are pngs. Alpha channel / transparency is supported.

## DAT behaviour definition files
The format of the dat files is as follows:
```
<Behaviour options>
<Behaviour flags>
<Code defines>
#Frames
<Frame definitions>
```
The line `#Frames` must be there so the software knows when the frame definitions begin.

### Behaviour options
* title=&lt;STRING&gt;: The title used by the behaviour. This is the title shown in the system tray menus.

#### Options specific to Boris behaviours
* coins=&lt;INTEGER&gt;: The price in Boris coins it costs to unlock this behaviour. If undefined, it defaults to 10c.
* category=&lt;STRING&gt;: The category this behaviour is categorically categorized into:
  * Health: Used for behaviours that heal Boris. He will not choose these on his own
  * Energy: Boris will choose from this category when he is low on energy / sleepy
  * Hunger: Boris will choose from this category when he is hungry
  * Toilet: Boris will choose from this category when he needs to go to the bathroom
  * Hygiene: Boris will choose from this category when he is dirty
  * Social: Boris will choose from this category when you don't pay him enough attention
  * Fun: Boris will choose from this category when he is feeling depressed
  * Movement: These are only used entirely at random when Boris is feeling well and satisfied
  * Idle: When Boris is feeling well and satisfied he will often choose from this category when idling about
  * Locomotion: These behaviours are fully scripted 360 degree locomotion behaviours used to move Boris around the desktop (eg. when Boris walks around)
* health=&lt;INTEGER&gt;: The amount of Boris' health this behaviour will increase or decrease when starting the behaviour
* energy=&lt;INTEGER&gt;: The amount of Boris' energy this behaviour will increase or decrease when starting the behaviour
* hyper=&lt;INTEGER&gt;: The amount of Boris' hyperactivity level this behaviour will increase or decrease when starting the behaviour
* hunger=&lt;INTEGER&gt;: The amount of Boris' hunger this behaviour will increase or decrease when starting the behaviour
* toilet=&lt;INTEGER&gt;: The amount of Boris' need for a toilet this behaviour will increase or decrease when starting the behaviour
* social=&lt;INTEGER&gt;: The amount of Boris' social needs this behaviour will increase or decrease when starting the behaviour
* fun=&lt;INTEGER&gt;: The amount of Boris' fun level this behaviour will increase or decrease when starting the behaviour
* hygiene=&lt;INTEGER&gt;: The amount of Boris' filth level this behaviour will increase or decrease when starting the behaviour

#### Options specific to item behaviours
* reaction=&lt;STRING&gt;: This tells Boris what behaviour he should react with when user drags the item on top of him, or when he walks into it while roaming the desktop. You can add as many as you like separated by commas (eg. 'reaction=play_piano,play_drums').

### Behaviour flags
* allowFlip: If this line exists there is a 50% chance the behaviour will be mirrored horizontally when initiated
* pitchLock: Disable the minute pitch randomness otherwise used when playing sounds from this behaviour

#### Flags specific to Boris behaviours
* oneShot: This behaviour will be played from start to finish and then switch to a new behaviour
* doNotDisturb: If this exists, Boris will not be disturbed by the mouse or other Borises while this behaviour is in progress

### Code defines
The scripting language supports very basic code block defines. These are not functions, but rather blocks of code that can then be reused throughout a behaviour by using the `call <DEFINE>` command.

NOTE! Anything defined in the hardcoded 'init' define will always be run first thing when a behaviour is started. See example.

##### Example:
```
define mainblock:if progress <= 0 break,
var rand = @4,
if rand = 1 goto label1 else if rand = 2 goto label2 else if rand = 3 goto label3
```
This defines a block of code called `mainblock` which can then be called at any time with `call mainblock` which simply replaces that call with the code contained in the block.
```
define init:var score = 0,if @2 = 1 goto game1 else goto game2
```
This uses the hardcoded 'init' define and sets the variable 'score' to 0 before proceeding to goto a frame label. This is very useful for declaring initial behaviour variables and in cases where it is undesirable to start at frame 0.

### Frame definitions
Each frame in a behaviour must be contained on a single line under the `#Frames` line. Empty lines are allowed (they will simply be ignored) and so are comment lines starting with `#` (these are also ignored).
* sprite: The sprite number from the sprite sheet that will be used by this frame. The first sprite in a sheet is number 0
* time: Time in miliseconds this frame will be shown before proceeding to the next frame
* deltax: Move Boris this number of pixels on the x axis when this frame is shown. Use 'rand' if you want him to move to a random location on the x axis
* deltay: Same as deltax but for the y axis. Keep in mind that it counts from top to bottom. Use 'rand' if you want him to move to a random location on the y axis
* soundfx: Filename for sound fx that will be played when this frame is reached. This is optional and can be left out if it's a silent frame
* script: The script that will be run at the end of showing the frame. This is optional. Scripting language is detailed below

### Scripting language definition
Boris can be scripted quite heavily to allow for some fun and interesting behaviour outcomes. A frame can have as many lines of script added to it as needed. Comma (`,`) is used to seperate each line from the next. Multiline scripts are possible just end a line with a comma and the following line will be seen as part of the same frame script. The last line of a script must NOT end with a comma.

This:
```
10;75;0;0;;label this,
var a = @6,
if a = 1 goto that
11;75;0;0;;break
12;75;0;0;;label that
```
Is the same as this:
```
10;75;0;0;;label this,var a = @6,if a = 1 goto that
11;75;0;0;;break
12;75;0;0;;label that
```
The following will cause weird behaviour or might even make it crash because of the stray comma after `goto that`:
```
10;75;0;0;;label this,
var a = @6,
if a = 1 goto that,
11;75;0;0;;break
12;75;0;0;;label that
```
The Boris scripting language allows for the following commands to be used:

#### Commands

##### var
Use this command to create and assign values to variables for later use inside the behaviour. All variables are local to this behaviour and are reset / deleted whenever a new behaviour is started.
* Definition:
`var <VARIABLE> =|+=|-=|*=|/= <VARIABLE or VALUE>`
* Examples:
```
var a = 1
var b = a + @5
var a = @42
var c = a - b
var d = 13 % 3
var e = ((45 - 3 * d) + 4) % 2
```
The `@` results in a random value from 1 to the number following the `@`. You can use `+`, `-`, `*`, `/` and `%`. The last one is modulo which gives you the remainder after a division. IMPORTANT! Boris does not support floating point values. If you divide values the result will be converted to an integer by stripping any decimals from the value (eg. 3.45 becomes 3).

##### stat
Use this to dynamically change any supported Boris stat while the behaviour is running.
* Definition:
`stat <STAT> +=|-= <VARIABLE or VALUE>`
* Examples:
```
stat energy += 5
stat hyper -= 20
stat fun += <VARIABLE>
stat toilet += @4
```
The `@` results in a random value from 1 to the number following the `@`. The `<VARIABLE>` can be any previously defined variable (see `var` command).

##### label
Defines a `goto` point you can go to using `goto`.
* Definition:
`label <NAME>`
* Examples:
```
label youcangoherewithgoto
label start
label dothis
label end
```
As you can see a label can be named pretty much anything. Do not use any of the reserved keywords used by the language though. It will give you some very odd behaviour. So make sure you steer clear of `if`, `var`, `goto` and so on.

##### if
Good old if sentence. This is probably the most important of all the commands as it allows for conditions to be tested and acted upon.
* Definition:
`if <EXPRESSION> [then if ...] [{] <COMMANDs> [}] else [{] <COMMAND(s)> [}]`
* Expression definition:
`<VARIABLE or VALUE> =|==|<|>|<=|>= <VARIABLE or VALUE>`
* Expression examples:
```
this = 1
@4 = that and 4 = @7
that = 1 or @5 = this or @2 = 1
((45 - 3 * d) + 4) % 2 = 5
```
You can also combine `and` and `or` expressions but as this is a very rudimentary language I only implemented it in a very basic manner so I would strongly suggest sticking to only using `or` or `and` within one call to `if`.

###### Scoped if's
For advanced scripting it is also possible to scope the if sentence like so:
```
if this = 1 { command1, command2, command3... and so on } else { command1, command2... and so on }
```
This allows you to run several commands if a condition is true.

###### The 'then' keyword and when to use it
Due to the nature of how the scripting language is parsed there is one instance where you would need to use the keyword `then` to get some of the more advanced sentences working. For single expression if's it is not needed. So you can do the following just fine:
`if a = 1 goto here else goto there`
But it IS needed if an `if` is directly followed by another `if`.
* Examples:
`if a = 1 then if b = 1 goto here else if b = 2 goto there`
`if a = 2 then if b = 1 goto here else if b = 2 goto there`
This allows for some quite complex scripting where `a` can be any value and you can subsequently test for other conditions if the first condition was true. You can also just scope the code after the first if to avoid 'then' altogether.
* Examples:
```
if a = 2 goto here
if @5 = 3 goto here else goto there
if a >= @5 var b = 4 else break
if something = 4 or foo = 42 then if other = 1 and c = @5 goto here else if other = 2 goto there else var b += 10
```
With this `if` command available you can do some pretty fun stuff. Just check out the `play_piano.dat` behaviour for a quite elaborate example of what it can be used for.

##### goto
A simple `goto` command that let's you go to any previously defined label.
* Definition:
`goto <LABEL>`
* Examples:
```
goto here
goto there
```

##### print
Prints any variable to the terminal. Useful for debugging.
* Definition:
`print <VARIABLE>`
* Examples
```
print this
print that
```

##### break
Forces the behaviour to exit entirely and change to the next behaviour. Can be used by itself or in if sentences.
* Definition:
`break`
* Example:
```
if a = 0 break
break
```

##### stop
Similar to 'break' but doesn't change the behaviour. It simply stops the frame timer so all progression within the current behaviour is stalled completely. This is useful for behaviours that have a last frame you wish to stop at and stay there.
* Definition:
`stop`
* Example:
```
if a = 1 goto dostuff else stop
```

##### behav
Changes the current behaviour to the behaviour with the requested behaviour filename (without the .dat suffix). NOTE! You cannot use spaces in the filename.
* Definition:
`behav <FILENAME>`
* Example:
```
behav play_piano
```

##### call
Calls and runs a block of code from a define (documented further up).
* Definition:
`call <DEFINE>`
* Example:
```
call mainblock
```
This will replace the call with the code from the define and run it. Very useful if you have the same piece of code several times throughout. Then just create a single define for the block instead and call it.

##### sound
Plays a sound from the sound effect folder.
* Definition:
`sound <FILENAME>`
* Example:
```
sound data/sounds/soundfile.wav
```
Keep in mind that this command DOES NOT allow spaces in the filename. So a file called `sound file.wav` will not work. Rename it to `soundfile.wav` instead. Filename and path is relative to the 'data/sounds' path (eg. 'data/sounds/soundfile.wav' should be typed in as simply 'soundfile.wav').

##### say
Spawns a speech bubble containing the specified text. Alternatively you can use the keyword "rss" (without the double-quotes) which will make Boris pick a headline from the configured RSS feed.

The length of the text will determine how long it is shown for.

* Definition:
`say "<TEXT>"|rss`
* Examples:
```
say "This is the text that will be shown in the speech bubble!"
say rss
```
The double-quotes at the beginning and end are important! If you leave them out it will lead to undefined behaviour or crashes. Double-quotes inside the text is not allowed.

NOTE! You can use variables inside the double quoted text by prepending them with a '$'. Any variables will be replaced with the contained value. See the 'hula_hoop.dat' behaviour for an example.

##### think
Spawns a thought bubble containing the specified text. Alternatively you can use the keyword "rss" (without the double-quotes) which will make Boris pick a headline from the configured RSS feed.

The length of the text will determine how long it is shown for.

* Definition:
`think "<TEXT>"|rss`
* Examples:
```
think "This is the text that will be shown in the thought bubble!"
think rss
```
The double-quotes at the beginning and end are important! If you leave them out it will lead to undefined behaviour or crashes. Double-quotes inside the text is not allowed.

NOTE! You can use variables inside the double quoted text by prepending them with a '$'. Any variables will be replaced with the contained value. See the 'hula_hoop.dat' behaviour for an example.

##### draw
There are several drawing routines in Boris scripting. This allows you to draw on top of Boris on each frame. It is also possible to begin drawing on one frame and end drawing on another for additive drawing over several frames.

Most drawing commands consist of the color, then the type and lastly the parameters that are required for the chosen type. The only exception is the 'sprite' type.

###### Colors
Drawing on Boris have hardcoded colors. These are specified as below:
* white
* cyan
* darkcyan
* red
* darkred
* magenta
* darkmagenta
* green
* darkgreen
* yellow
* darkyellow
* blue
* darkblue
* grey / gray
* darkgrey / darkgray
* lightgrey / lightgray

###### pixel
* Definition:
`draw <COLOR> pixel x y`
* Example:
```
draw yellow pixel 6 5
```
This will draw a single yellow pixel at 6, 5.

###### line
* Definition:
`draw <COLOR> line x1 y1 x2 y2`
* Example:
```
draw blue line 0 2 7 8
```
This will draw a blue line from 0, 2 to 7, 8.

###### ellipse
* Definition:
`draw <COLOR> ellipse x y w h`
* Example:
```
draw red ellipse 3 4 5 5
```
This will draw a red ellipse at 3, 4 with a width and height of 5.

###### rectangle
* Definition:
`draw <COLOR> rectangle x y w h`
* Example:
```
draw red rectangle 3 4 5 5
```
This will draw a red rectangle at 3, 4 with a width and height of 5.

###### text
* Definition:
`draw <COLOR> text x y this_is_the_text`
* Example:
```
draw yellow text 2 2 sometext
```
This will draw the yellow text `sometext` at 2, 2 using the internal minimal pixel font. NOTE! The text line cannot have spaces in it!

###### sprite
* Definition:
`draw sprite <NAME> frame x y`
* Example:
```
draw sprite test 0 8 4
```
This will draw frame 0 from the sprite `test` at the coordinates 8, 4. All sprites are loaded from the `data/sprites` folder. The sprite names are the basename of the files contained within that folder (only lower case letters allowed and spaces *not* allowed). Each frame is cut from horizontal sprite sheets separated by the *exact* color R:255, G:0, B:0(known as magenta). See the example sprites in the folder.

NOTE! The 'draw sprite' command is very useful if you want to have different facial expressions for Boris within a single behaviour. Then you can simply have the base Boris frame in the behaviour, and use sprites to apply different facial expressions to that same base frame.

###### value
* Definition:
`draw <COLOR> value x y <VARIABLE or VALUE>`
* Example:
```
draw yellow value 2 2 somevariable
draw red value 2 2 42
```
The first example will draw the value of the `somevariable` variable at 2, 2 using a yellow color. The second example will draw the value `42` at 2, 2 using a red color.

###### Additive drawing
Per default all drawing routines are performed on the current frame sprite and cleared when moving on to the next. By using additive drawing you can begin drawing on one frame, then draw more stuff on subsequent frames, and end the drawing routine on a frame later on. It is only after the drawing routine has been ended it will be cleared upon moving on to the next frame.
* Example:
```
10;75;0;0;;draw begin,draw red point 5 6
11;75;0;0;;draw blue line 3 5 8 7
12;75;0;0;;draw yellow ellipse 3 4 5 5
15;75;0;0;;draw end
```

##### spawn
Spawn an item from the items folder (default is `data/items`). The item name is the name of the corresponding dat+png file pair without a suffix. Items can be animated and scripted mostly the same way Boris behaviours can.
* Definition:
`spawn <ITEM FILENAME> x y`
* Example:
```
spawn christmas_tree 10 2
```
This will spawn an item at x, y relative to Boris using the graphics from `data/items/christmas_tree.png`. The coordinates and item are scaled according to the current size of Boris.

#### Hardcoded / reserved variables
The following variables are reserved and hardcoded. You should not try to set these yourself, but feel free to use them to make some interesting behaviours.

* energy: Boris' current energy level (0-100, 100 means he's well rested)
* health: Boris' current health level (0-100, 100 means he's doing well)
* hyper: Boris' current hyper level (0-100, 100 means he will speed around the desktop like crazy)
* hunger: Boris' current hunger level (0-100, 100 means he's VERY hungry)
* toilet: Boris' current toilet need level (0-100, 100 means he need to go NOW!)
* social: Boris' current social level (0-100, 0 means Boris will seek your attention)
* fun: Boris' current fun level (0-100, 100 means Boris has been having a lot of fun)
* hygiene: Boris' current hygiene level (0-100, 100 means he's clean)
* yvel: The delta movement of the mouse on the x axis (0-n, 0 means no movement)
* xvel: The delta movement of the mouse on the y axis (0-n, 0 means no movement)
* borisx: Current Boris x center coordinate
* borisy: Current Boris y center coordinate
* mousex: Current mouse pointer x coordinate
* mousey: Current mouse pointer x coordinate
* mdist: The current pixel distance from Boris to the mouse cursor
* bsize: The current size of Boris in pixels
* msec: The direction sector the mouse is within relative to Boris. Sectors are 0-7 clockwise where 0 is north. 1 is northeast and so on.
* day: The day (1-31)
* month: The month (1-12)
* year: The year
* hour: The current hour (0-23)
* minute: The current minute within the hour
* second: The current second within the current minute
* weather: The current weather type
** 1: Clear sky
** 2: Few clouds
** 3: Scattered clouds
** 4: Broken clouds
** 9: Shower rain
** 10: Rain
** 11: Thunderstorm
** 13: Snow
** 50: Mist
* wind: The current wind speed in meters per second
* temp: The current temperature in celsius
