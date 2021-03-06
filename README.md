# ConciseDateWithTimeWatchface
Repo for Watchface "Concise Date, Time, Temperature"

## Description
Similar to the stock watchface, this face displays the date and the time in a concise manner. It also displays an abbreviation for the day of the week in front of the abbreviated month name. Also displays weather information along the top of the watchface. 


Available at:
https://apps.getpebble.com/applications/54dfe5dcc789cf55b200003b

Version 1.0 (as of Feb. 15, 2015)
* Gives date and time information.

Version 1.1 Currently published (as of Mar. 24, 2015)
* This update brings with it weather information along the top of the watchface. Currently it only supports Fahrenheit. It gives the current temperature, daily low, and daily high. Ensure location services is enabled for the pebble app in order for the weather functionality to work. If you don't want the weather information you can disable location services for the pebble app to turn them off. Settings are currently not available, sorry. The weather information only refreshes when you change the screen manually so as to avoid excessive battery usage.
* Uses the user's location coordinates to find weather information from OpenWeatherMap.org's API and then uses JSON to parse the information. 
