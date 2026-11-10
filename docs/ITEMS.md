# Item documentation
The following is a complete description of the item format used to give Boris something to interact with. Every item that you can spawn uses the format described in this document.

All items are located in the `data/items` folder.

## How to create an item
A single item is made up of two files. A 32x32 horizontal PNG sprite sheet and a `.dat` that describes all aspects of the item. Both files must have the same base filename and be placed in the `data/items` folder.

##### Example
* Sprite sheet: `data/items/myitem.png`
* DAT file definition: `data/items/myitem.dat`

### The PNG sprite sheet
All frames used in an item are taken from the sprite sheet. Sprites *must* be 32x32 pixels in dimensions and the sprite sheet *must* have a horizontal layout. Check the existing sprite sheets if you are in doubt what that means. I can recommend using the open source software [LibreSprite](https://libresprite.github.io/#!/) as it supports all necessary features including exporting animations as a horizontal sprite sheet. Many of the existing items are also available in the `.ase` format in the repository to use as a nice starting point.

In general I highly recommend using an indexed color format of your PNG with the first color being transparent. But an indexed palette is not a requirement. You can use a true-color PNG with alpha if you want to.

### The DAT file definition
The format of the dat files is as follows:
```
[Item options]
[Item reaction]
[Item flags]
[BorisScript defines]
#Frames
[Frame definitions]
```
The line `#Frames` marks the beginning of the frame definitions. It *must* be there to tell the parser when the header ends.

##### Example
```
title=Football
category=Fun
coins=50
weight=2
bounce=7
range=64

reaction=kickball_w,kickballw,-12,7;kickball_e,kickballe,12,7

oneShot
allowFlip

define init:var loops = @6 + 1
define other:var loop -= 1,if loop <= 0 goto end
```

### Item options
#### General options
* `title=STRING`: The title used by the item. This is the title used in the menus when running Boris
* `category=STRING`: The category of this item. It can be one of the following:
  * `Energy`: Used for items that will make Boris more rested when he interacts with it
  * `Social`: Used for items that makes Boris more social when he interacts with it
  * `Fun`: Used for items that are fun for Boris to interact with
  * `Hunger`: Used for items that makes Boris less hungry when interacting with it
  * `Toilet`: Used for items that relieves Boris' need for a toilet visit when he interacts with it
  * `Health`: Used for items that heal Boris when he interacts with it
  * `Hygiene`: Used for items that cleans Boris when he interacts with it
* `coins=PRICE`: The amount of Boris coins this item costs to unlock before it can be spawned from the menu
* `weight=INTEGER`: Set the weight of the item. Value can be from 0-10. 0 means floating. 10 means really heavy
* `bounce=INTEGER`: Sets the bounce factor of the object. Value can be from 0-10. 0 means no bounce. 10 means bounce forever
* `range=INTEGER`: This set the reaction range for the item in pixels. If Boris comes closer than this he will react with the item

### Item reaction
#### Definition
```
reaction=REACTION,LABEL,XCOORD,YCOORD;...
```

#### Examples
```
reaction=kickball_w,kickballw,-12,7
reaction=kickball_w,kickballw,-12,7;kickball_e,kickballe,12,7
reaction=sit_and_relax,interacting
reaction=sit_and_relax,nil
reaction=eat_steak
```
The first example will tell the interacting Boris to initiate the `kickball_w` behaviour. It also tells the item to go to the frame label `kickballw` when the reaction starts. It will also move the item -12, 7 pixels in relation to Boris. This can be used to synchronize the placement of the item with whatever Boris is going to do in the reaction behaviour.

The second example is the same as the first example except it contains two different reactions to choose from randomly. You can define as many as you'd like by separating each of them with a `;`.

The third example tells Boris to initiate the `sit_and_relax` behaviour. It also tells the item to go to the frame label `interacting` when the reaction starts. The item is *not* destroyed when the interaction starts.

The fourth example tells Boris to initiate the `sit_and_relax` behaviour. The hardcoded `nil` keyword can be used for the LABEL definition which means that the current item frame will *not* be changed when the interaction starts. The item is *not* destroyed when the interaction starts.

The fifth example tells Boris to initiate the `eat_steak` behaviour. The item *is* destroyed when the interaction starts.

### Item flags
* `allowFlip`: Set this to enable a 50% chance of the item being mirrored horizontally when spawned
* `pitchLock`: Set this to disable the slight pitch randomness otherwise used when playing the sounds of this item
* `hidden`: If this is set the item won't be added to the tray icon item menu

### BorisScript defines
BorisScript supports blocks of code to be defined which can be called from the frame definitions to avoid repeating the same code over and over again. They can be seen as parameter-less functions. See the `call` command specification in the [BorisScript](BORISSCRIPT.md) documentation for more details.

##### Example:
```
define mainblock:[BorisScript code]
```
This defines a block of code called `mainblock` which can then be called from the frame definitions at any time with `call mainblock` which simply replaces that call with the code contained in the block.

NOTE: All items allow the hardcoded 'init' define which will always be run as the first thing (even before the first frame) when an item is spawned. This is great for setting up variables before the item animation starts running.

##### Example:
```
define init:[BorisScript code]
```

### Frame definitions
Each frame in an item must be contained on a single line under the `#Frames` line. Empty lines are allowed (they will simply be ignored) and so are comment lines starting with `#` (these are also ignored).

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
