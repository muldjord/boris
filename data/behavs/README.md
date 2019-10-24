# Behavior documentation
The files in this folder defines the behaviours of Boris. You can add new behaviours simply by supplying new pairs of png+dat files. The format is documented below. Check the existing ones for examples.

Note: All behaviour filenames that start with `_` (underscore) MUST NOT be deleted. These are hardcoded behaviours. Deleting any of them will make Boris crash eventually. Any behaviour filename beginning with an underscore will also be ignored whenever Boris chooses random behaviours, so this is useful for test behaviours when the `iddqd=true` setting is set in `config.ini`. This enables the Boris right-click `iddqd` behaviour menu that allows you to select and queue any available behaviour.

## PNG sprite sheets
The png files are horizontal sets of sprites that are loaded together with the `<FILENAME>.dat` files. The dimensions of each sprite inside the sprite sheet must be 32x32 pixels. The color format of the pngs are not important as long as they are pngs. Alpha channel / transparency is supported.

## DAT behaviour definition files
The format of the dat files is as follows:
```
<Behaviour options and flags>
#Frames
sprite;time;deltax;deltay;soundfx;script
```
The line `#Frames` must be there so the software knows when the frame definitions begin.

### Options and flags
Options or flags, one per line of any of the following:
* title=&lt;STRING&gt;: The title used by the behaviour when right-clicking Boris
* category=&lt;STRING&gt;: The category this behaviour is categorically categorized into:
  * Health: Used for behaviours that heal Boris. He will not choose these on his own
  * Energy: Boris will choose from this category when he is low on energy / sleepy
  * Hunger: Boris will choose from this category when he is hungry
  * Bladder: Boris will choose from this category when he needs to go to the bathroom
  * Hygiene: Boris will choose from this category when he is dirty
  * Social: Boris will choose from this category when you don't pay him enough attention
  * Fun: Boris will choose from this category when he is feeling depressed
  * Movement: These are only used entirely at random when Boris is feeling well and satisfied
  * Idle: When Boris is feeling well and satisfied he will often choose from this category when idling about
* health=&lt;INTEGER&gt;: The amount of Boris' health this behaviour will increase or decrease when starting the behaviour
* energy=&lt;INTEGER&gt;: The amount of Boris' energy this behaviour will increase or decrease when starting the behaviour
* hyper=&lt;INTEGER&gt;: The amount of Boris' hyperactivity level this behaviour will increase or decrease when starting the behaviour
* hunger=&lt;INTEGER&gt;: The amount of Boris' hunger this behaviour will increase or decrease when starting the behaviour
* bladder=&lt;INTEGER&gt;: The amount of Boris' need for a toilet this behaviour will increase or decrease when starting the behaviour
* social=&lt;INTEGER&gt;: The amount of Boris' social needs this behaviour will increase or decrease when starting the behaviour
* fun=&lt;INTEGER&gt;: The amount of Boris' fun level this behaviour will increase or decrease when starting the behaviour
* hygiene=&lt;INTEGER&gt;: The amount of Boris' filth level this behaviour will increase or decrease when starting the behaviour
* oneShot: This behaviour will be played from start to finish and then switch to a new behaviour
* doNotDisturb: If this exists, Boris will not be disturbed by the mouse or other Borises while this behaviour is in progress
* allowFlip: If this line exists there is a 50% chance the behaviour will be mirrored horizontally
* pitchLock: Disable the minute pitch randomness otherwise used when playing sounds from this behaviour

### Frame definitions
Each frame in a behaviour must be contained on a single line under the `#Frames` line. Empty lines are allowed (they will simply be ignored) and so are comment lines starting with `#` (these are also ignored).
* sprite: The sprite number from the sprite sheet that will be used by this frame. The first sprite in a sheet is number 0
* time: Time in miliseconds this frame will be shown before proceeding to next frame
* deltax: Move Boris this number of pixels on the x axis when this frame is shown. Use 'rand' if you want him to move to a random location on the x axis
* deltay: Same as deltax but for the y axis. Keep in mind that it counts from top to bottom. Use 'rand' if you want him to move to a random location on the y axis
* soundfx: Filename for sound fx that will be played when this frame is reached. This is optional and can be left out if it's a silent frame
* script: The script that will be run at the end of showing the frame. This is optional. Scripting language is detailed below

### Scripting language definition
Boris can be scripted quite heavily to allow for some fun and interesting behaviour outcomes. The rudimentary language allows for the following commands to be used:

#### Commands

##### var
Use this command to create and assign values to variables for later use inside the behaviour. All variables are local to this behaviour and are reset / deleted whenever a new behaviour is started.
* Definition:
`var <VARIABLE> =|+=|-=|*=|/= <VARIABLE or VALUE>`
* Examples:
```
var a = 1
var b = a
var a = @42
```
The `@` results in a random value from 1 to the number following the `@`.

##### stat
Use this to dynamically change any supported Boris stat while the behaviour is running.
* Definition:
`stat <STAT> +=|-= <VARIABLE or VALUE>`
* Examples:
```
stat energy += 5
stat hyper -= 20
stat fun += <VARIABLE>
stat bladder += @4
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
Good old if sentence. It can do a lot, but there's also some stuff i can't do.
* Definition:
`if <EXPRESSION> [then if ...] <COMMAND> else <COMMAND>`
* Expression definition:
`<VARIABLE or VALUE> =|==|<|>|<=|>= <VARIABLE or VALUE>`
* Expression examples:
```
this = 1
@4 = that and 4 = @7
that = 1 or @5 = this or @2 = 1
```
You can also combine `and` and `or` expressions but they probably won't work as you would expect. As this is a very rudimentary language I only implemented it in a very basic manner so I would strongly suggest sticking to only using `or` or `and` within one call to `if`.

###### The 'then' keyword and when to use it
Due to the nature of how I parse the scripting language there is one instance where you would need to use the keyword `then` to get some of the more advanced sentences working. For single expression if's it is not needed. So you can do the following just fine:
`if a = 1 goto here else goto there`
But it IS needed if an `if` is directly followed by another `if`.
* Examples:
`if a = 1 then if b = 1 goto here else if b = 2 goto there`
`if a = 2 then if b = 1 goto here else if b = 2 goto there`
This allows for some quite complex scripting where `a` can be any value and you can subsequently test for other conditions if the first condition was true.
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
