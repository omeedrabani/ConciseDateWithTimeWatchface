//var ajax = require('ajax');
//var UI = require('ui');


var locationOptions = {timeout: 15000, maximumAge: 60000}; 

function locationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
	getWeather(pos.coords.latitude, pos.coords.longitude);
}


function getWeather(latitude, longitude) {	
	var req = new XMLHttpRequest();
	req.open('GET', "http://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longitude, true);
	console.log("XMLHttp Request Opened.");
	req.onload = function(e) {
		if (req.readyState == 4) { 
			if (req.status == 200) {
				console.log("Req state entered."); 
				var response = JSON.parse(req.responseText);

				var tempC = (response.main.temp - 273.15);
				var tempF = ((tempC * 1.8) + 32).toFixed(0);
				tempC = tempC.toFixed(0);
				var tempMin = ((response.main.temp_min - 273.15) * 1.8 + 32).toFixed(0);
				var tempMax = ((response.main.temp_max - 273.15) * 1.8 + 32).toFixed(0);

				Pebble.sendAppMessage({TEMPERATURE: tempF + " F - L : " + tempMin + " H : " + tempMax});
				console.log("Temperature sent.");
				//send.temperature_f = Number(response.currently.temperature).toFixed(0);
				//send.temperature_c = Number(FtoC(response.currently.temperature)).toFixed(0);


				//Pebble.sendAppMessage(send);
			} else {
					console.log("Error");
			}	
		}
	};
	
req.send(null);	
	
	/*
  // Download data
  ajax({url: URL, type: 'json'},
    function(json) {
      var tempC = ((json.main.temp - 273.15) * 1.8);
      var tempF = (tempC + 32).toFixed(0);
      tempC = tempC.toFixed(0);

      Pebble.sendAppMessage({temperature: tempF + ' F.' + tempC + ' C.'});
    },
    function(error) {
      console.log('Ajax failed: ' + error);
      Pebble.sendAppMessage("Temp Error");
    }
  );
	*/
	
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
	Pebble.sendAppMessage({TEMPERATURE: ""});
}

// Make an asynchronous request
//navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);


 
Pebble.addEventListener('ready', function(e) {
  console.log('Initial event begin');
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});

Pebble.addEventListener("appmessage", function(e) {
	console.log("Weather requested.");
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});
