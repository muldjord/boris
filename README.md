# Boris
A scriptable desktop screenmate written in Qt (for graphics) and SFML (for sound). Boris will keep you company while you work. He has a bit of a prankster personality and needs to be cared for in order to stay alive. Move your mouse over him to check on his health stats.

* All behaviours are fully scriptable using [BorisScript](https://github.com/muldjord/boris/blob/master/data/behavs/README.md)
* Supports weather visualization using the OpenWeatherMap API
* Supports RSS feed speech bubbles using custom RSS feeds

# About
* Programming: Lars Muldjord
* Graphics: Lars Muldjord
* Sound: Lars Muldjord
* Website: http://www.muldjord.com/boris

Boris was programmed in C++ using the [Qt framework](http://www.qt.io) for graphics and the [SFML library](https://www.sfml-dev.org) for sound. Boris requires Qt 5.10 or later.

Bug reports, suggestions and / or comments can be emailed to me at:
muldjordlars@gmail.com.

Copyright 2020 Lars Muldjord. This program is distributed under the terms of the GNU General Public License. Be sure to read the license in COPYING.TXT or check out the web page http://www.gnu.org/licenses/gpl-3.0.html.

## Building from source (Ubuntu Linux and Debian derivatives)
### Prerequisites
Boris requires Qt 5.10 or later. Install all prerequisites with the following commands:
```
$ sudo apt update
$ sudo apt install build-essential qt5-default qtmultimedia5-dev libsfml-dev
```

### Build commands
```
$ qmake
$ make -j8
$ ./Boris
```

## Release notes
#### Version x.x.x, Roadmap:
* Make wind behaviours that are appended before or after weather reaction behaviour when weather is shown
* Add a 'usec' that points towards any direction the arrow keys are currently pressed

#### Version 3.5.0 (In progress, unreleased):
* Reworked chatter / speech bubble class
* Boris will now think about how he feels when stats are down
* Added 'script_output' variable to config.ini to allow disabling script output in terminal
* Replaced deprecated qrand with QRandomGenerator (which unfortunately has the side-effect of now requiring Qt 5.10)
* Added '_thought' chatter type
* Added 'Dance' idle behaviour
* Gave Boris a bed to sleep in
* Improved / reworked '_fun' behaviour used when fun stat is low
* Improved / reworked '_social' behaviour used when social stat is low
* Added 'say' BorisScript command
* Added 'think' BorisScript command
* All speech bubbles are now in scripts instead of being hardcoded
* RSS feed functionality is now imlemented through BorisScript instead of being hardcoded
* Added 'Tell me the news' Social behaviour which will show rss feed output
* Added 'What are you thinking?' Social behaviour which shows a more philosophical side of Boris
* Added conditional speech bubbles to several behaviours (For instance: Boris now keeps score when using the hula hoop)
* Added 'How are you?" Social behaviour which will assess Boris' stats and give a response in a speech bubble
* Added a few missing wall hit sounds to 'Pong' game
* Improved BorisScript arithmetic expressions considerably, now using 'tinyexpr.c' by Lewis Van Winkle
* Added user item spawning using the tray icon menu
* Added item interactions using the BorisScript 'reaction' header option
* Added 'draw sprite' BorisScript command that uses sprites from 'data/sprites'
* Added random item spawn interval to about box
* Now always runs code from 'define init' before first frame is shown from a behaviour
* Added 'Coins' that can be spend on unlocking behaviours (no real money involved, it's just to add a sense of progression)
* Added 'queued' script variable that tells how many behaviours are currently queued
* Moved all '_flee' behaviours into a single behaviour that checks 'msec' variable to determine direction
* Moved all '_sad' behaviours into a single behaviour that checks 'fsec' variable to determine direction
* Moved all '_wave' behaviours into a single behaviour that checks 'fsec' and 'msec' variables to determine direction

#### Version 3.0.1 (15th September 2020):
* Boris now sometimes farts in his sleep, a very important feature

#### Version 3.0.0 (8th December 2019):
* Boris is now fully scriptable allowing for random outcomes of behaviours
* Behaviours now allows comments inside frame area in .dat files
* Boris now gets hyper when drinking coffee
* Boris now sometimes uses his umbrella when falling
* Boris will now bounce when hitting the ground at high velocity
* High winds (> 10 m/s) now moves Boris when showing weather
* Boris now moves to opposite side when reaching horizontal borders
* Added idkfa "secret" menu
* Removed "time_factor" config as it is now based on hyper stat instead
* Added wind data to tray menu
* Boris will now get sad when encountering a deceased friend
* Added dynamic shadow based on the current Boris sprite size and location
* Switched to SFML for sound
* Added slight pitch randomness to all sounds
* All sounds are now panned in stereo depending on Boris' location
* Improved many existing behaviours
* Added scripting to many existing behaviours for more interesting outcomes
* Added a bunch of new dynamic behaviours
* Added drawing routines to scripting language
* Added scoping to 'if' sentences in scripting language
* Boris is now ticklish (scroll mouse wheel on him)
* Boris now less likely to pick a "social" behav when moving mouse over him
* Refactored speech bubble class
* Added tray icon behaviour menu that allows user to queue a behaviour for all Boris instances at once
* Added items that can be spawned from boris scripts. The items are even scriptable themselves and can spawn new items

#### Version 2.0.2 (4th December 2018):
* Added 'allowFlip' flag to behaviours giving those a 50% of being flipped horizontally

#### Version 2.0.1 (4th December 2017):
* Added 'casual_standing_looking' behaviour
* Added 'casual_standing_hatpull' behaviour
* Added 'casual_standing_tassel' behaviour
* Updated directional walk behaviours
* Fixed somewhat glitchy stat flasher
* Balanced a lot of behaviours and main stat deltas
* Added config combobox for stat display behaviour

#### Version 2.0.0 (24th November 2017):
* Added RSS feed support
* Added wind support. Boris will now be blown around the desktop in strong winds.
* Made it so Boris much more often just relaxes or walks around.
* Boris now has a chance of, erm, having an accident if 'toilet' reaches 0.
* Boris can now go to the toilet.
* Boris now sometimes spins the yoyo.
* Boris can now wash his hands.
* Refactored a bunch of data type code. Still shitty, but better than before.
* Fixed a nasty sound bug that made Pulseaudio crash when exiting Boris under Linux.
* Fixed looping animation bug when showing weather.
* Code updated with Qt5 style signal/slots.
* Added icons for 'teleport' and 'earthquake' in tray menu.

#### Version 1.9.4 (07th November 2016):
* Added complain behaviour, and added sound to all chatter behaviours.

#### Version 1.9.3 (05th November 2016):
* Added speech bubbles. Edit the text in data/chatter.dat. Read CHATTER.md for more info.

#### Version 1.9.2 (24th August 2016):
* Limited the weather api call to once every hour. Added OpenWeatherMap API key lineedit to config.

#### Version 1.9.1 (29th July 2016):
* Limited the weather api call to once every 30 minutes. Weather now shown every 30th behaviour change instead.

#### Version 1.9.0 (27th July 2016):
* Added animated weather using the OpenWeatherMap API (http://www.openweathermap.org)

#### Version 1.8.0 (21th January 2016):
* Added health stat, deacreses when thrown. Don't throw him too much...
* Chooses "social" behaviour if fun stat is high enough and the mouse comes close

#### Version 1.7.2 (20th January 2016):
* Moved AI back into Boris class, it was too ugly the other way
* All stats are now handled inside Stats class

#### Version 1.7.1 (18th January 2016):
* Boris walks a bit more than before
* Moved AI behaviour to own function

#### Version 1.7.0 (13th January 2016):
* Added hygiene stat
* Added shower behaviour and "complain" behaviour for when hygiene is low

#### Version 1.6.8 (12th January 2016):
* Boris now greets other Borises
* Added teleport behaviour (but no sound on it yet)

#### Version 1.6.6 (8th January 2016):
* Stat changes are now applied over a period of time instead of instantly
* Bug fixes

#### Version 1.6.5 (8th January 2016):
* Added independece slider to adjust how much attention Boris needs
* Added guitar shredding behaviour

#### Version 1.6.3 (7th January 2016):
* First public release on Github
* Now categorizes behaviours
* Stat window is now placed below Boris when he's at the top of the screen

#### Version 1.6.2 (6th January 2016):
* Balancing and bug fixes

#### Version 1.6.1 (5th January 2016):
* Boris now has vitality stats and needs to be looked after
* He might also die... So please take good care of him

#### Version 1.2.1 (1st January 2016):
* Added earthquake menu item

#### Version 1.2.0 (1st January 2016):
* Added simple physics, allowing Boris to be thrown around

#### Version 1.1.3 (30th December 2015):
* Added "secret" Lemmy mode (R.I.P)

#### Version 1.1.2:
* Graphics optimized, now uses QGraphicsView instead of QLabel

#### Version 1.1.1:
* Added volume slider.

#### Version 1.1.0:
* Added Boris outfits that change with the seasons (winter, spring, summer, autumn, christmas).

#### Version 1.0.1 (28th December 2015):
* Less shitty code.
* Added sound.
* Added new behaviours.
* Boris now flees from the mouse cursor.
* The active Boris can be controlled using the cursor keys.

#### Version 0.7.3:
* First public release.
* LOTS of fixes and additions. Still shitty code, but whatever, it works.

#### Version 0.6.0:
* Internal test release

## Known issues
With some graphics drivers / vendors, Boris might eat up a lot of cpu time. The cause of this is out of my hands, since it has to do with how the driver and Qt makes the window borderless and the background translucent. This has only been observed with an onboard Intel display device on Windows. Linux doesn't seem to have this problem. If you have a lot of tearing in the graphics while moving Boris around on the desktop, you too have this problem.

Any issues and / or comments concerning Boris can be emailed to me at muldjordlars@gmail.com
