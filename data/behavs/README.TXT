The files in this folder defines the behaviours of Boris. You can add
new behaviours simply by supplying a new set of png+dat files.

Note: The "_dropped" and "_grabbed" are special behaviours that are used
when Boris is dragged and dropped using the mouse. These MUST exist!

The png files are a vertical set of sprites using 32x32 pixels. The color
format of the pngs are not important as long as they are pngs.

The format of the dat files is as follows:

---- file start, this line not included ----
flags
#Frames
sprite;time;deltax;deltay;hide;show;change;soundfx;
---- file end, this line not included ----

flags, one flag per line, available flags are:
       energy=int: The amount of energy this behaviour will increase or decrease stats
       hunger=int: The amount of hunger this behaviour will increase or decrease stats
       bladder=int: The amount of bladder this behaviour will increase or decrease stats
       social=int: The amount of social this behaviour will increase or decrease stats
       fun=int: The amount of fun this behaviour will increase or decrease stats
       oneShot: This behaviour will be played from start to finish and then
                switch to a new behaviour.

The line "#Frames" MUST be there, so the software knows when the frame
descriptions begin and the flag section ends.

sprite: The sprite that will be used by this frame. This correlates directly
to the png file. Sprite zero would be the sprite located at 0,0 to 32,32 and
so on.

time: Time in miliseconds this frame will be shown before proceeding to next
frame

deltax: Move Boris this number of pixels on the x axis when this frame is
shown

deltay: Same as deltax but for the y axis. Keep in mind that it counts from
top to bottom

hide: Bool (0/1) which tells Boris to hide when this frame is shown

show: Similar to hide, but show instead.

change: Bool (0/1) that tells Boris to change behaviour when this frame
is reached. Used for bahaviours that don't work well when looped.

soundfx: Filename for sound fx that will be played when this frame is
reached.
