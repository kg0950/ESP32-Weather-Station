#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// OpenWeather API
String apiKey = "YOUR_OPENWEATHER_API_KEY";
String city = "YOUR_CITY";
String countryCode = "YOUR_COUNTRY_CODE";

// GPS Module
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// BMP280 Sensor
Adafruit_BMP280 bmp;

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    // Initialize BMP280 Sensor
    if (!bmp.begin(0x76)) {
        Serial.println("BMP280 Sensor not found!");
        while (1);
    }
}

void loop() {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F;
    float altitude = bmp.readAltitude(1013.25);
    float latitude = 0.0, longitude = 0.0;

    // Get GPS Data
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
            }
        }
    }

    // Print Sensor Data
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
    Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("Altitude: "); Serial.print(altitude); Serial.println(" m");
    Serial.print("Latitude: "); Serial.print(latitude);
    Serial.print(" Longitude: "); Serial.println(longitude);

    // Fetch Weather Data from API
    getWeatherData();
    
    delay(10000); // Update every 10 seconds
}

void getWeatherData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Weather Data: " + payload);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected!");
    }
}
