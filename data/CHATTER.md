# Chatfile format
You can set the input file for the Boris chatter in config.ini. 'chatter.dat' is the default file used. The format of a line in the chatfile is as follows:

[chat type];[speech bubble text]

## Chat types
* _chat: Boris will speak normally
* _whisper: Boris will whisper
* _snicker: Boris will snicker while saying this
* _complain: Boris will be complaining while saying this

Be sure to include the underscore for the chat types exactly as noted above. You can add as many lines as you'd like in this format. Check out examples in the provided chatter.dat.
Boris will sometimes choose these lines mixed with the RSS feed lines when you choose the "Chatter" behaviour from the Social menu. Boris also sometimes talks on his own when he feels like socializing.