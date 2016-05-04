# THOUGHTS
The following are ideas / thoughts I have for future development of Boris.

## Implementing weather ##
### Type ###
Icon above his head, signifying how the weather is. Icon based sortof.
### Temperature ###
If it's chilly he'll clack his teeth. If it's hot, he'll sweat.
### Combination ###
If the temperature is high, and the type is "sunny" Boris could lay down with sunglasses on on a beach chair.
If the temperature is below freezing and it's snowing, he could built a snowman.

## LUA scripting
I'd like to implement LUA scripting of Boris' AI. Currently it is hardcoded and not very streamlined.

I could implement functions to allow for checking for encounters. For instance to use when meeting other Boris instances, or if I ever implement objects in the scene such as presents or food. These should interrupt Boris' current behaviour.

### Considerations
Currently Boris has several different behaviours depending on his mental state. I need to implement some kind of priority for these, probably based on how urgent each state is. I need to do a dice roll among all states before deciding on which state is more relevant. And the dice roll should be weighted depending on the mental state of each stat. As it is now, all state possibilities are assessed top down, and when one is "true" it simply doesn't assess the rest, leaving them to be chosen less often than the ones higher up. This is not good practice.

### Frame or behaviour based scripting
Should the scripting be evaluated per frame or per behaviour change? This poses a current problem, since some of his current behaviour is frame based, while other stuff is only assessed when he changes from one behaviour to the next. For instance, if he meets another Boris while walking, he will end the walking behav and start a wave behav. But most of the time the next behaviour chosen is decided when the last behaviour ends. So you could say that there are some behaviours that needs to interrupt Boris, while others follow the current behaviour when it finishes.