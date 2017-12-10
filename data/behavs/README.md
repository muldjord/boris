# Behavior documentation
The files in this folder defines the behaviours of Boris. You can add new behaviours simply by supplying a new pair of png+dat files. Check the existing ones for examples.

Note: All behaviour filenames that start with "_" MUST NOT be deleted. These are hardcoded behaviours. Deleting any of them will make Boris crash eventually.

## PNG sprite sheets
The png files are a vertical set of sprites using 32x32 pixels. The color format of the pngs are not important as long as they are pngs. Alpha channel / transparency is supported.

## DAT description files
The format of the dat files is as follows:

---- file start, this line not included ----
flags
#Frames
sprite;time;deltax;deltay;hide;show;soundfx(optional);
---- file end, this line not included ----

### Flags
Flags, one flag per line, available flags are:
* title=string: The title used by the behaviour when right-clicking Boris
* category=string: The stat category for this behaviour:
  * Energy, Hunger, Bladder, Hygiene, Social, Fun, Idle (category used for Boris' various idle behaviours which have a higher bias when Boris is just minding his own business)
* health=int: The amount of health this behaviour will increase or decrease stats
* energy=int: The amount of energy this behaviour will increase or decrease stats
* hunger=int: The amount of hunger this behaviour will increase or decrease stats
* bladder=int: The amount of bladder this behaviour will increase or decrease stats
* social=int: The amount of social this behaviour will increase or decrease stats
* fun=int: The amount of fun this behaviour will increase or decrease stats
* hygiene=int: The amount of filth this behaviour will increase or decrease stats
* oneShot: This behaviour will be played from start to finish and then switch to a new behaviour.
* doNotDisturb: If this exists, Boris will not be disturbed by the mouse while this behaviour is in progress.
* allowFlip: If this line exists there is a 50% chance the behaviour will be mirrored horizontally.

The line "#Frames" MUST be there, so the software knows when the frame
descriptions begin and the flag section ends.

## Frame descriptor line
* sprite: The sprite no. that will be used by this frame. This correlates directly to the png file. Sprite zero would be the sprite located at 0,0 to 32,32 and so on.
* time: Time in miliseconds this frame will be shown before proceeding to next frame.
* deltax: Move Boris this number of pixels on the x axis when this frame is shown. Use 'rand' if you want him to move to a random location on the x axis.
* deltay: Same as deltax but for the y axis. Keep in mind that it counts from top to bottom. Use 'rand' if you want him to move to a random location on the y axis.
* hide: Bool (0/1) which tells Boris to hide when this frame is shown.
* show: Similar to hide, but show instead.
* soundfx: Filename for sound fx that will be played when this frame is reached. This is optional and can be left out if it's a silent frame.
