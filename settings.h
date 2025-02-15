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

#define DARKMODE 1

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
#define THICK_LINES 0

// Serial line debugging
//
// Enables the code which writes writes diagnostic information to the
// serial line.
#define SERIAL_DEBUG 0

// Speed of the serial line
//
// This is only used if SERIAL_DEBUG or SCREENSHOT_MODE is set
//
// This should match SERIAL_SPEED in Makefile
#define SERIAL_SPEED 921600

// Screenshot mode
//
// When screenshot mode is enabled, then the clock will be set to the
// given time.
//
// If you have patched GxEPD2_BW.h (see README-development.md), then a
// screen dump will be emitted on the serial console (which will be
// implicitly enabled).
//
// The clock will NOT show the correct current time. It will appear
// frozen. But a screenshot will be done once a minute.
#define SCREENSHOT_MODE 0

// These two only have effect when SCREENSHOT_MODE is activated.
//
// The "classic" watch face is 10:08 +/- a couple of minutes.
#define SCREENSHOT_HOUR 10
#define SCREENSHOT_MINUTE 8

#endif
