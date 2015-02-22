var ajax = require('ajax');
//var UI = require('ui');


var locationOptions = {timeout: 10000, maximumAge: 1000}; 

function locationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
	
  var URL = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longtiude;

  // Download data
  ajax({url: URL, type: 'json'},
    function(json) {
      var tempC = ((json.main.temp - 273.15) * 1.8);
      var tempF = (tempC + 32).toFixed(1);
      tempC = tempC.toFixed(1);

      Pebble.sendAppMessage({temperature: tempF + ' F.' + tempC + ' C.'});
    },
    function(error) {
      console.log('Ajax failed: ' + error);
      Pebble.sendAppMessage("Temp Error");
    }
  );
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

// Make an asynchronous request
navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);


 
Pebble.addEventListener('ready', function(e) {
  console.log('Initial event begin');
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});