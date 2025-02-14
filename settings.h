#ifndef SETTINGS_H
#define SETTINGS_H

//Weather Settings
#define CITY_ID "5128581" //New York City https://openweathermap.org/current#cityid

//You can also use LAT,LON for your location instead of CITY_ID, but not both
//#define LAT "40.7127" //New York City, Looked up on https://www.latlong.net/
//#define LON "-74.0059"

#ifdef CITY_ID
    #define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/weather?id={cityID}&lang={lang}&units={units}&appid={apiKey}" //open weather api using city ID
#else
    #define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&lang={lang}&units={units}&appid={apiKey}" //open weather api using lat lon
#endif

#define OPENWEATHERMAP_APIKEY "f058fe1cad2afe8e2ddc5d063a64cecb" //use your own API key :)
#define TEMP_UNIT "metric" //metric = Celsius , imperial = Fahrenheit
#define TEMP_LANG "en"
#define WEATHER_UPDATE_INTERVAL 30 //must be greater than 5, measured in minutes
//NTP Settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600 * -5 //New York is UTC -5 EST, -4 EDT, will be overwritten by weather data

#define DARKMODE true
#define SERIAL_DEBUG 0

#define BACKGROUND (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define FOREGROUND (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)

// Placement of the analogue watch face.
// In general, you want to place this in the centre of the screen
#define FACE_CENTER_X 100
#define FACE_CENTER_Y 100

// Outer radius of the clock face - in pixels
//
// Depending on the size of the screen and placement, this may cause
// parts of the face to not fit on screen.
#define FACE_RADIUS 99

// The centre of the watch face is a filled circle.
// Set to zero to disable
#define CENTRE_COVER_RADIUS 7

// Whether we want thick lines
#define THICK_LINES false

#endif
