## Hardcoded BorisScript variables
The following variables are reserved and hardcoded. You should not try to set these yourself (you can but it can cause weird behaviour).

Use them to make some interesting behaviours.

### The variables
* `energy`: Boris' current energy level (0-100, 100 means he's well rested)
* `health`: Boris' current health level (0-100, 100 means he's doing well)
* `hyper`: Boris' current hyper level (0-100, 100 means he will speed around the desktop like crazy)
* `hunger`: Boris' current hunger level (0-100, 100 means he's VERY hungry)
* `toilet`: Boris' current toilet need level (0-100, 100 means he needs to go NOW!)
* `social`: Boris' current social level (0-100, 0 means Boris will seek your attention)
* `fun`: Boris' current fun level (0-100, 100 means Boris has been having a lot of fun)
* `hygiene`: Boris' current hygiene level (0-100, 100 means he's clean)
* `queue`: Tells how many behaviours are currently in behaviour queue. This is useful for creating breaking points in long behaviours if a new behaviour is waiting activation
* `xvel`: The entity velocity in pixels on the y axis during the last 30 ms
** This variable can be set with `var xvel = n` to put the entity in motion
* `yvel`: The entity velocity in pixels on the x axis during the last 30 ms
** This variable can be set with `var yvel = n` to put the entity in motion
* `borisx`: Current Boris x center coordinate
* `borisy`: Current Boris y center coordinate
* `mx`: Current mouse pointer x coordinate
* `my`: Current mouse pointer x coordinate
* `mxvel`: The velocity of the mouse on the y axis during the last 30 ms
* `myvel`: The velocity of the mouse on the x axis during the last 30 ms
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
