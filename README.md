# Boris
Boris desktop screenmate written entirely in Qt

* Programming: Lars Muldjord
* Graphics: Lars Muldjord
* Website: http://www.muldjord.com/boris

Boris was programmed using the Qt framework (http://www.qt.io) and C++.

Bug reports, suggestions and / or comments can be emailed to me at:
muldjordlars@gmail.com.

Copyright 2016 Lars Muldjord. This program is distributed under the terms of the GNU General Public License. Be sure to read the license in COPYING.TXT or check out the web page http://www.gnu.org/licenses/gpl-3.0.html.

## Building from source (Linux)
### Requirements
* Qt 5 dev libs including multimedia package

### Build commands
* qmake
* make
* ./Boris

## Release notes

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
