# Configuration file readme
When Boris is run for the first time, a default config.ini file will be created in the working directory. Below follows a complete list of accepted key/value pairs:
* behavs_path (string): The path where all Boris behaviours are loaded from.
* boris_x, boris_y (integers): The x and y coordinate from where new Borises will spawn from.
* clones (integer): Number of Boris clones that will spawn on startup.
* independence (integer between 0-100): Defines how independent Boris will be when stats are in the red. If he is very independent he will automatically try to satisfy his needs.
* lemmy_mode (true/false): RIP Lemmy... :(
* show_welcome: Defines whether the config/about box will be show on startup.
* size (integer): The height and width of Borises in pixels. 0 will choose random sizes.
* sound (true/false): Do you want sound?
* volume (integer between 0-100): Sound volume.
* time_factor (integer at 1 or above): Boris' time factor. 1-10 is ok, but highger than that and he sortof breaks.
* weather (true/false): Always show weather or not. If 'false' it will still show the weather occasionally which is preferred.
* weather_city (string): Weather will be fetched for this city.
* weather_key (string): The API key for OpenWeatherMap. Get one from openweathermap.org/appid.
* weather_path (string): The directory containing the weather types and animations.
* weather_force_type (string): Force this type of weather. Check http://openweathermap.org/weather-conditions for more info. An example of a type could be '11d' for thunderstorm (try this one, it's pretty funny seeing Boris act on lightning).
* weather_force_temp (decimal): Force the temperatue. Note that this is in Celsius.

Note that if you ever end up with a faulty config, just delete it. A default one will then be created when you summon Boris the next time.