# Behaviour documentation
The following is a complete description of the behaviour format used to bring Boris alive. Everything Boris does is made using the format described in this document.

All behaviours are located in the `data/behaviours` folder. There are 3 different types of behaviours.

### Common behaviours
These are the self-contained behaviours Boris uses when he's just going about his day on your desktop. If you want to create your own this is probably the type you are looking for.

What defines a common behaviour? It has a category set of either `Energy`, `Social`, `Fun`, `Hunger`, `Toilet`, `Hygiene` or `Health`. That means Boris will choose from it when his vitality stats require something from one of these categories.

### Reaction behaviours
These behaviours are *only* used as reactions to Boris encountering [items](ITEMS.md) that requests them. Items can be spawned on the desktop either by you or at random. When Boris encounters these items a specific reaction behaviour can be set.

It's important to note that a behaviour that is *not* a reaction behaviour can still be used as a reaction when encountering an item. The difference being that a behaviour specifically set with the `Reaction` category will *only* ever be used as a reaction to encountering an item that requests it.

What defines a reaction behaviour? It has its category set to `Reaction`.

### Static behaviours
These are the hardcoded behaviours that must exist for Boris to function at all. They can be identified by the `_` in front of the filenames. If one or more of these are deleted Boris might stop functioning and even crash (so don't do that). You can create a static behaviour yourself simply by adding a `_` in front of the filename.

Usually there is no reason for you to create static behaviours yourself except for one instance: Boris sometimes chooses a behaviour completely at random if all of his needs are met. If you want your behaviour to be excluded from this roster you can create it as a static behaviour. Boris might still choose it if he has a need that is fulfilled by the category set in the behaviour though. This is the only functional difference between a static behaviour and a common behaviour.

### Other notes
If you want to enable *all* behaviours in the menus including the ones you wouldn't usually select manually you can add `idkfa=true` to the `config.ini` file. This will add the invisible behaviours to an `idkfa` behaviour submenu.

## How to create a behaviour
A single behaviour is made up of two files. A 32x32 horizontal PNG sprites sheet and a `.dat` that describes all aspects of the behaviour. Both files must have the same base filename and be placed in the `data/behaviours` folder.

##### Example
* Sprite sheet: `data/behaviours/mybehav.png`
* DAT file definition: `data/behaviours/mybehav.dat`

### The PNG sprite sheet
All frames used in a behaviour are taken from the sprite sheet. Sprites *must* be 32x32 pixels in dimensions and the sprite sheet *must* have a horizontal layout. Check the existing sprite sheets if you are in doubt what that means. I can recommend using the open source software [LibreSprite](https://libresprite.github.io/#!/) as it supports all necessary features including exporting animations as a horizontal sprite sheet. Many of the existing behaviours are also available in the `.ase` format in the repository to use as a nice starting point.

NOTE: If your behaviour includes Boris himself it is necessary to use an indexed palette taken from one of the existing sprite sheets that has Boris in it. This is due to Boris' clothes changing color depending on the season. This is done by switching the colors in the palette when Boris is loading and it is therefore necessary for the first 19 colors to remain where they are in the index.

In general I highly recommend using an indexed color format of your PNG with the first color being transparent and the next 18 colors matching the ones from the existing behaviours.

But an indexed palette is not a requirement. You can use a true-color PNG with alpha if you want to as long as it doesn't have Boris in it (because a true-color spritesheet will break the clothes color change).

### The DAT file definition
The format of the dat files is as follows:
```
[Behaviour options]
[Stat changes]
[Behaviour flags]
[BorisScript defines]
#Frames
[Frame definitions]
```
The line `#Frames` marks the beginning of the frame definitions. It *must* be there to tell the parser when the header ends.

##### Example
```
title=Hula hoop
category=Fun

hunger=5
hygiene=-5

oneShot
allowFlip

define init:var loops = @6 + 1
define other:var loop -= 1,if loop <= 0 goto end
```

### Behaviour options
#### General options
* `title=STRING`: The title used by the behaviour. This is the title used in the menus when running Boris
* `category=STRING`: The category of this behaviour. It can be one of the following:
  * `Energy`: Boris will choose from this category when he is low on energy / sleepy
  * `Social`: Boris will choose from this category when you don't pay him enough attention
  * `Fun`: Boris will choose from this category when he is feeling depressed
  * `Hunger`: Boris will choose from this category when he is hungry
  * `Toilet`: Boris will choose from this category when he needs to go to the bathroom
  * `Health`: Used for behaviours that heal Boris if he is injured. These are never chosen by Boris himself and must therefore be initiated by the user
  * `Hygiene`: Boris will choose from this category when he is dirty
  * `Idle`: When Boris is feeling well and satisfied he will often choose from this category when idling
  * `Movement`: These are only used entirely at random when Boris is feeling well and satisfied. These usually move Boris around somehow (eg. when taking the elevator or teleporting). Note that this is different from the `Locomotion` category that should only be used for behaviours that contain a full 360 movement cycle of some kind
  * `Locomotion`: These behaviours are fully scripted 360 degree locomotion behaviours used to move Boris around the desktop (eg. when Boris walks around)
  * `Reaction`: This marks the behaviour as a `Reaction` behaviour that is *only* used when an encountered item requests it

### Stat changes
* energy=INTEGER: The amount of Boris' energy this behaviour will increase or decrease when starting the behaviour
* social=INTEGER: The amount of Boris' social needs this behaviour will increase or decrease when starting the behaviour
* fun=INTEGER: The amount of Boris' fun level this behaviour will increase or decrease when starting the behaviour
* hunger=INTEGER: The amount of Boris' hunger this behaviour will increase or decrease when starting the behaviour
* toilet=INTEGER: The amount of Boris' need for a toilet this behaviour will increase or decrease when starting the behaviour
* health=INTEGER: The amount of Boris' health this behaviour will increase or decrease when starting the behaviour
* hygiene=INTEGER: The amount of Boris' filth level this behaviour will increase or decrease when starting the behaviour
* hyper=INTEGER: The amount of Boris' hyperactivity level this behaviour will increase or decrease when starting the behaviour

### Behaviour flags
* allowFlip: Set this to enable a 50% chance of the behaviour being mirrored horizontally when initiated
* pitchLock: Set this to disable the slight pitch randomness otherwise used when playing the sounds in this behaviour
* oneShot: Set this to tell the behaviour to move on to a different behaviour when the end of the frame definitions are reached. If this is unset it will instead loop back to the first frame definition and keep playing for a randomly chosen amount of time before moving on to the next behaviour.
* doNotDisturb: Set this to make sure Boris cannot be disturbed while this behaviour is playing (eg. when encountering another Boris or if the Mouse moves close to him)

### BorisScript defines
BorisScript supports blocks of code to be defined which can be called from the frame definitions to avoid repeating the same code over and over again. They can be seen as parameter-less functions. See the `call` command specification in the [BorisScript](BORISSCRIPT.md) documentation for more details.

##### Example:
```
define mainblock:[BorisScript code]
```
This defines a block of code called `mainblock` which can then be called from the frame definitions at any time with `call mainblock` which simply replaces that call with the code contained in the block.

NOTE: All behaviours allow the hardcoded 'init' define which will always be run as the first thing (even before the first frame) when a behaviour is initiated. This is great for setting up variables before the behaviour starts running.

##### Example:
```
define init:[BorisScript code]
```

### Frame definitions
Each frame in a behaviour must be contained on a single line under the `#Frames` line. Empty lines are allowed (they will simply be ignored) and so are comment lines starting with `#` (these are also ignored).

##### Format
```
sprite;time;deltax;deltay;soundfx;script
```
* `sprite`: The frame / sprite number from the sprite sheet that will be used by this frame. The first sprite in a sheet is number 0
* `time`: Time in miliseconds this frame will be shown before proceeding to the next frame
* `deltax`: Move Boris this number of pixels on the x axis when this frame is shown. Use 'rand' if you want him to move to a random location on the x axis
* `deltay`: Same as deltax but for the y axis. Keep in mind that it counts from top to bottom. Use 'rand' if you want him to move to a random location on the y axis
* `soundfx`: Filename for sound effect that will be played when this frame is reached. This is optional and can be left empty if it's a silent frame
* `script`: The BorisScript that will be run at the end of showing the frame. This is optional and can be empty in which case the frame will simply proceed to the next frame when the frame time runs out. Check the [BorisScript](BORISSCRIPT.md) documentation for more information
