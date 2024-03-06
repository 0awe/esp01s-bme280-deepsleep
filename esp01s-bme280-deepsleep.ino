#include <ESP8266WiFi.h>
#include <include/WiFiState.h>  // WiFiState structure details
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define STASSID "WIFI_SSID"
#define STAPSK "WIFI_PASSCODE"
#define URL "http://RPI_IP/weather/"

#define SDA 0             // GPIO0
#define TXD 1             // GPIO1 (Use TXD to power BME280 and reset it on every wake cycle)
#define SLC 2             // GPIO2 (ESP-01S LED)
//#define DSS 900e6       // 900 seconds = 15 minutes (900 * 1000 * 1000)
#define DSS 600e6         // 600 seconds = 10 minutes (900 * 1000 * 1000) (Production as of 1/2023)
//#define DSS 10e6        // 10 seconds for testing (10 * 1000 * 1000)
#define RTC_USER_DATA_SLOT_WIFI_STATE 33u

String s;

WiFiState state;
Adafruit_BME280 bme;

ADC_MODE(ADC_VCC);          // Required for ESP.getVcc() to work

void fnDataCollect() {
  pinMode(TXD, OUTPUT);     // Enable power on GPIO1
  digitalWrite(TXD, HIGH);  // Power up GPIO1 (BMx280 on TXD pin)
  delay(50);                // Recommended per https://community.particle.io/t/bme280-sensor-problem/49627/6

  Wire.pins(SDA, SLC);
  Wire.begin();

  if (!bme.begin(0x76)) {
    s += "&t=E&h=E&p=E";
  } else {
    // Setup BME280 for weather/climate measurements
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1, // temperature
      Adafruit_BME280::SAMPLING_X1, // pressure
      Adafruit_BME280::SAMPLING_X1, // humidity
      Adafruit_BME280::FILTER_OFF);
    bme.takeForcedMeasurement();
    s += "&t=" + String(bme.readTemperature());
    s += "&h=" + String(bme.readHumidity());
    s += "&p=" + String(bme.readPressure());
  }
  s += "&m2=" + String(millis()) + "&v2=" + String(ESP.getVcc());

  digitalWrite(TXD, LOW); // Power down GPIO1
}

void fnDataSend() {
  WiFiClient client;
  HTTPClient http;
  s = "?d=" + String(WiFi.hostname()) + s;
  s += "&m3=" + String(millis()) + "&v3=" + String(ESP.getVcc()) + "&r=" + String(WiFi.RSSI());
  http.begin(client, URL + s);
  http.GET();
  http.end();
}

void setup() {
  delay(1);         // Required per https://www.esp8266.com/viewtopic.php?f=28&t=22958

  // Start logging here for cleanest data
  s = "&m1=" + String(millis()) + "&v1=" + String(ESP.getVcc());

  fnDataCollect();  // Collect data now to minimize WiFi uptime.

  ESP.rtcUserMemoryRead(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t*>(&state), sizeof(state));
  unsigned long start = millis();

  if (!WiFi.resumeFromShutdown(state) || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
    WiFi.persistent(false);

    if (!WiFi.mode(WIFI_STA) || !WiFi.begin(STASSID, STAPSK) || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
      WiFi.mode(WIFI_OFF);
      ESP.deepSleep(DSS, RF_DISABLED);
      return;
    }
  }

  fnDataSend();

  WiFi.shutdown(state);
  ESP.rtcUserMemoryWrite(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t*>(&state), sizeof(state));

  ESP.deepSleep(DSS, RF_DISABLED);
}

void loop() {
  // Nothing to do here, leave blank.
}
