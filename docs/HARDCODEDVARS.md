## Hardcoded BorisScript variables
The following variables are reserved and hardcoded. You should not try to set these yourself (you can but it can cause weird behaviour).

Use them to make some interesting behaviours.

### The variables
* `energy`: Boris' current energy level (0-100, 100 means he's well rested)
* `social`: Boris' current social level (0-100, 0 means Boris will seek your attention)
* `fun`: Boris' current fun level (0-100, 100 means Boris has been having a lot of fun)
* `hunger`: Boris' current hunger level (0-100, 100 means he's VERY hungry)
* `toilet`: Boris' current toilet need level (0-100, 100 means he needs to go NOW!)
* `health`: Boris' current health level (0-100, 100 means he's doing well)
* `hygiene`: Boris' current hygiene level (0-100, 100 means he's clean)
* `hyper`: Boris' current hyper level (0-100, 100 means he will speed around the desktop like crazy)
* `anxiety`: Boris' current anxiety level (0-100, 100 means he's very anxious)
* `queue`: Tells how many behaviours are currently in the behaviour queue. This is useful for creating breaking points in long behaviours if a new behaviour is waiting activation
* `hvel`: The current horizontal velocity of the entity during the last 30 ms
* `vvel`: The current vertical velocity of the entity during the last 30 ms
* `xpos`: Current entity x center coordinate
* `ypos`: Current entity y center coordinate
* `mx`: Current mouse pointer x coordinate
* `my`: Current mouse pointer x coordinate
* `mhvel`: The horizontal velocity of the mouse during the last 30 ms
* `mvvel`: The vertical velocity of the mouse during the last 30 ms
* `mdist`: The current pixel distance from Boris to the mouse cursor
* `mdir`: The direction the mouse is in relative to Boris. Directions are 0-7 clockwise where 0 is north. 1 is northeast and so on.
* `bsize`: The current size of Boris in pixels (After Boris 4.0.0 this is always 32 pixels. I might remove it in future versions.)
* `bdir`: The direction a close Boris friend is located in relative to Boris. If no Boris is close by it will be `-1`. Otherwise directions are 0-7 clockwise where 0 is north. 1 is northeast and so on.
* `idir`: The direction a close item is located in relative to Boris. If no item is close by it will be `-1`. Otherwise directions are 0-7 clockwise where 0 is north. 1 is northeast and so on.
* `day`: The day (1-31)
* `month`: The month (1-12)
* `year`: The year
* `hour`: The current hour (0-23)
* `minute`: The current minute within the hour
* `second`: The current second within the current minute
* `weather`: The current weather type which can be any of the following
** 1: Clear sky
** 2: Few clouds
** 3: Scattered clouds
** 4: Broken clouds
** 9: Shower rain
** 10: Rain
** 11: Thunderstorm
** 13: Snow
** 50: Mist
* `wind`: The current wind speed in meters per second
* `temp`: The current temperature in celsius
