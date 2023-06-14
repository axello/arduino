
/* 
   MetrifulMQTT.ino, based on MQTT_logging.ino, with enhancements
   
   Example MQTT data logging code for the Metriful MS430. 
   
   This example is designed for the following WiFi enabled hosts:
   * Arduino Nano 33 IoT
   * Arduino MKR WiFi 1010
   * ESP8266 boards (e.g. Wemos D1, NodeMCU)
   * ESP32 boards (e.g. DOIT DevKit v1)
   
   Environmental data values are measured and logged to an internet 
   cloud account every 100 seconds, using a WiFi network. The example 
   gives the choice of using either the Tago.io or Thingspeak.com 
   clouds – both of these offer a free account for low data rates.

   Copyright 2020 Metriful Ltd. 
   Licensed under the MIT License - for further details see LICENSE.txt

   For code examples, datasheet and user guide, visit 
   https://github.com/metriful/sensor
*/

/* FUTURE
  once every couple of hours, disable the DS18b20 for a while
  then enable; to reset them.
  when buffer is too full, return an error state in an MQTT message with the buffer size

*/

#include "secrets.h"
#include "Metriful_sensor.h"
#include "WiFi_functions.h"
#include "graph_web_page.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "host_pin_definitions.h"
#include "sensor_constants.h"
#include <OneWire.h>
#include <DallasTemperature.h>

void MQTT_connect();

// override Adafruit_MQTT buffer size!
// (Can't do that here, do it in Adafruit_MQTT.h)
// #define MAXBUFFERSIZE (500)

//////////////////////////////////////////////////////////
// USER-EDITABLE SETTINGS
#define SERIAL_BAUD_RATE 57600

// How often to read and log data (every 100 or 300 seconds)
// Note: due to data rate limits on free cloud services, this should 
// be set to 100 or 300 seconds, not 3 seconds.
uint8_t cycle_period = CYCLE_PERIOD_100_S;

// The details of the WiFi network:
char SSID[] = WIFI_SSID; // network SSID (name)
char password[] = WIFI_PASSWORD; // network password

// The details of the MQTT PUBLISHER:
#define NODENAME oranjeslaap
#define MQTT_FEED "metriful/oranjeslaap/tele"
#define MQTT_STATE "metriful/oranjeslaap/state"

// Define DALLAS for 1 ds18b20, or DALLAS2 for 2 dallas
#define DALLAS

#ifdef DALLAS
#define ONE_WIRE_BUS_1 D7   // labeled D7
#define DS_ENABLE_PIN D5     // labeled D5
#endif
// #define ONE_WIRE_BUS_2 15   // labeled D8

// END OF USER-EDITABLE SETTINGS
//////////////////////////////////////////////////////////

#if !defined(HAS_WIFI)
#error ("This example program has been created for specific WiFi enabled hosts only.")
#endif

WiFiClient client;

// Buffers for assembling MQTT requests
char postBuffer[MAXBUFFERSIZE] = {0};
char fieldBuffer[70] = {0};

unsigned long nextTicks = 0;
#define bleep_interval 300

// Structs for data
AirData_t airData = {0};
AirQualityData_t airQualityData = {0};
LightData_t lightData = {0}; 
ParticleData_t particleData = {0};
SoundData_t soundData = {0};

// MQTT
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT);
Adafruit_MQTT_Publish feedPublisher = Adafruit_MQTT_Publish(&mqtt, MQTT_FEED);
Adafruit_MQTT_Publish statePublisher = Adafruit_MQTT_Publish(&mqtt, MQTT_STATE);

// DALLAS DS18B20
#ifdef DALLAS
OneWire oneWire(ONE_WIRE_BUS_1);
DallasTemperature temp_sensors(&oneWire);
#define EMPTYTHERMOMETER "0:0:0:0:0:0:0:0\0"
#define DeviceAddressStringLength 17
#define DeviceAddressLength 8

// arrays to hold device addresses
#define MAXTHERMOMETERS 5
char thermometers[MAXTHERMOMETERS][DeviceAddressStringLength];
float temperatures[MAXTHERMOMETERS];

int dallasDeviceCount;
#endif
enum Status { error, ok , nodallas};

void post_MQTT_state(Status state) {
    String input;
    switch(state) {
      case error: 
        input = "{\"status\": \"ERROR\"}";
        break;
      case ok:
        input = "{\"status\": \"OK\"}";
        break;
      case nodallas:
        input = "{\"status\": \"NO DALLAS DEVICE DETECTED\"}";
        break;
      default:
        input = "{\"status\": \"Serious error\"}";
    }
        // json.copy(&postBuffer, POSTBUFFER_SIZE);
    const char* postBuf = input.c_str();
    // postBuffer[json.length()] = '\0';

    size_t len = strlen(postBuf);
    // Serial.println(len);
    postBuffer[len] = '\0';

    if (!statePublisher.publish(postBuf, len)) {
      // Serial.println("Could not send mqtt.");
    }

}

void searchOneWireDevices() {
  temp_sensors.begin();

    // locate devices on the bus
  Serial.print("\nLocating devices...");
  Serial.print("Found ");
  dallasDeviceCount = temp_sensors.getDeviceCount();
  Serial.print(dallasDeviceCount, DEC);
  Serial.println(" devices.");
  if (dallasDeviceCount > MAXTHERMOMETERS) {
    Serial.println("Device count is bigger than predefined array! [MAXTHERMOMETERS]");
    dallasDeviceCount = MAXTHERMOMETERS;
  }

  if (dallasDeviceCount == 0) {
    post_MQTT_state(nodallas);
  } else {
    post_MQTT_state(ok);
  }

  // report parasite power requirements
  Serial.print("Parasite OneWire power is: ");
  if (temp_sensors.isParasitePowerMode()) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // search
  oneWire.reset_search();
  int index = 0;
  char buffer[DeviceAddressStringLength];
  char *ptr = &buffer[0];

  DeviceAddress tempThermo;
  while (index < dallasDeviceCount && oneWire.search(tempThermo)) {
    addressToString(tempThermo, thermometers[index]);
    printDevice(index, tempThermo);
    index++;
  }

  if (index < dallasDeviceCount) {
    Serial.println("Did not store all detected Onewire devices.");
  }
}

// show the addresses we found on the bus
void printDevice(int index, DeviceAddress deviceAddress) {
    Serial.print("Device ");
    Serial.print(index);
    Serial.print(" Address: ");
    printAddress(deviceAddress);
    Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < DeviceAddressLength; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void addressToString(DeviceAddress deviceAddress, char *stringBuf)
{
  char *ptr = stringBuf;

  for (int i = 0; i < DeviceAddressLength; i++) {
    ptr += snprintf(ptr, DeviceAddressStringLength, "%02X", deviceAddress[i]);
  }
  stringBuf[DeviceAddressStringLength-1] = '\0';
}

void fetchTemperatures()
{
  for (int index = 0; index < dallasDeviceCount; index++) {
    temperatures[index] = temp_sensors.getTempCByIndex(index);
    // DEVICE_DISCONNECTED_C (-127) is a perfectly valid and visible indication of something amiss.
    // if(tempC != DEVICE_DISCONNECTED_C) {
    // }
  }
}
/*******************************************************************
*
*     SETUP
* 
*******************************************************************/
void setup() {
  // Serial.begin(SERIAL_BAUD_RATE);

  // Initialize the host's pins, set up the serial port and reset:
  Serial.begin(SERIAL_BAUD_RATE);

  Serial.print("interupt on: GPIO");
  Serial.println(S_INT_PIN);

#ifdef DALLAS
  pinMode(DS_ENABLE_PIN, OUTPUT);
  digitalWrite(DS_ENABLE_PIN, HIGH);
  for (int index = 0; index < MAXTHERMOMETERS; index++) {
    temperatures[index] = DEVICE_DISCONNECTED_C;
    strncpy(thermometers[index], EMPTYTHERMOMETER, 17);
  }
  searchOneWireDevices();
  delay(100);
#else
  Serial.println("NO DALLAS\n");
#endif
    
  connectToWiFi(SSID, password);
  
  // Apply chosen settings to the MS430
  uint8_t particleSensor = PARTICLE_SENSOR;
  TransmitI2C(I2C_ADDRESS, PARTICLE_SENSOR_SELECT_REG, &particleSensor, 1);
  TransmitI2C(I2C_ADDRESS, CYCLE_TIME_PERIOD_REG, &cycle_period, 1);

  SensorHardwareSetup(I2C_ADDRESS);
  // Enter cycle mode
  ready_assertion_event = false;
  TransmitI2C(I2C_ADDRESS, CYCLE_MODE_CMD, 0, 0);

  nextTicks = millis() + bleep_interval;
}

/*******************************************************************
*
*     LOOP
* 
*******************************************************************/
void loop() {

  // Wait for the next new data release, indicated by a falling edge on READY
  while (!ready_assertion_event) {
    yield();
  }
  ready_assertion_event = false;

  MQTT_connect();

  /* Read DS18B20 extra temperature data */
#ifdef DALLAS
  digitalWrite(DS_ENABLE_PIN, 1);
  temp_sensors.requestTemperatures();
#endif

  /* Read data from the MS430 into the data structs. 
  For each category of data (air, sound, etc.) a pointer to the data struct is 
  passed to the ReceiveI2C() function. The received byte sequence fills the 
  struct in the correct order so that each field within the struct receives
  the value of an environmental quantity (temperature, sound level, etc.)
  */ 
  
  // Air data
  // Choose output temperature unit (C or F) in Metriful_sensor.h
  ReceiveI2C(I2C_ADDRESS, AIR_DATA_READ, (uint8_t *) &airData, AIR_DATA_BYTES);
  
  /* Air quality data
  The initial self-calibration of the air quality data may take several
  minutes to complete. During this time the accuracy parameter is zero 
  and the data values are not valid.
  */ 
  ReceiveI2C(I2C_ADDRESS, AIR_QUALITY_DATA_READ, (uint8_t *) &airQualityData, AIR_QUALITY_DATA_BYTES);
  
  // Light data
  ReceiveI2C(I2C_ADDRESS, LIGHT_DATA_READ, (uint8_t *) &lightData, LIGHT_DATA_BYTES);
  
  // Sound data
  ReceiveI2C(I2C_ADDRESS, SOUND_DATA_READ, (uint8_t *) &soundData, SOUND_DATA_BYTES);

  /* Particle data
  This requires the connection of a particulate sensor (invalid 
  values will be obtained if this sensor is not present).
  Specify your sensor model (PPD42 or SDS011) in Metriful_sensor.h
  Also note that, due to the low pass filtering used, the 
  particle data become valid after an initial initialization 
  period of approximately one minute.
  */ 
  if (PARTICLE_SENSOR != PARTICLE_SENSOR_OFF) {
    ReceiveI2C(I2C_ADDRESS, PARTICLE_DATA_READ, (uint8_t *) &particleData, PARTICLE_DATA_BYTES);
  }

#ifdef DALLAS
  fetchTemperatures();
#endif

  // Check that WiFi is still connected
  uint8_t wifiStatus = WiFi.status();
  if (wifiStatus != WL_CONNECTED) {
    // There is a problem with the WiFi connection: attempt to reconnect.
    Serial.print("Wifi status: ");
    Serial.println(interpret_WiFi_status(wifiStatus));
    connectToWiFi(SSID, password);
    ready_assertion_event = false;
  }

  post_MQTT();
  // toggle_LED();
}

void toggle_LED(void) {
  if (millis() > nextTicks) {
    digitalWrite(DS_ENABLE_PIN, !digitalRead(DS_ENABLE_PIN));
    nextTicks = millis() + bleep_interval;
  }
}
/* For both example cloud providers, the following quantities will be sent:
1 Temperature (C or F)
2 Pressure/Pa
3 Humidity/%
4 Air quality index
5 bVOC/ppm
6 SPL/dBA
7 Illuminance/lux
8 Particle concentration
  
   Additionally, for Tago, the following is sent:
9  Air Quality Assessment summary (Good, Bad, etc.) 
10 Peak sound amplitude / mPa
*/ 

// Assemble the data into the required format, then send it to the MQTT Publisher
void post_MQTT(void) {

  uint8_t T_intPart = 0;
  uint8_t T_fractionalPart = 0;
  bool isPositive = true;
  getTemperature(&airData, &T_intPart, &T_fractionalPart, &isPositive);
  sprintf(postBuffer,"{\"temperature\":%u.%u,\n", T_intPart, T_fractionalPart);

#ifdef DALLAS
  // preamble
  strcat(postBuffer, "\"dallas\":{\n");

  for (int index = 0; index < dallasDeviceCount; index++) {
    float temp = temperatures[index];
    if(temp != DEVICE_DISCONNECTED_C) {
      sprintf(fieldBuffer, "\"%s\":%5.2f,\n", thermometers[index], temp);
      strcat(postBuffer, fieldBuffer);
    }
  }

  // postamble
  strcat(postBuffer, "},\n");
#endif
    // https://stackoverflow.com/questions/45922817/what-is-unquoted-priu32-in-printf-in-c
    
    sprintf(fieldBuffer, "\"pressure\":%" PRIu32 ",\n", airData.P_Pa);
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"humidity\":%u.%u,\n", 
            airData.H_pc_int, airData.H_pc_fr_1dp);
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"aqi\":%u.%u,\n", 
            airQualityData.AQI_int, airQualityData.AQI_fr_1dp);
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"aqi_string\":\"%s\",\n", 
            interpret_AQI_value(airQualityData.AQI_int));
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"bvoc\":%u.%02u,\n", 
            airQualityData.bVOC_int, airQualityData.bVOC_fr_2dp);
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"spl\":%u.%u,\n", 
            soundData.SPL_dBA_int, soundData.SPL_dBA_fr_1dp);
    strcat(postBuffer, fieldBuffer);

    sprintf(fieldBuffer,"\"peak_amp\":%u.%02u,\n", 
            soundData.peak_amp_mPa_int, soundData.peak_amp_mPa_fr_2dp);
    strcat(postBuffer, fieldBuffer);

    sprintf(fieldBuffer,"\"particulates\":%u.%02u,\n", 
            particleData.concentration_int, particleData.concentration_fr_2dp);
    strcat(postBuffer, fieldBuffer);
    
    sprintf(fieldBuffer,"\"illuminance\":%u.%02u,\n", 
            lightData.illum_lux_int, lightData.illum_lux_fr_2dp);
    strcat(postBuffer, fieldBuffer);

    // show rest of buffer length minus offset
    size_t len = MAXBUFFERSIZE - (strlen(postBuffer) + 15);
    sprintf(fieldBuffer,"\"rest_buf\":%u\n", len);
    strcat(postBuffer, fieldBuffer);
    strcat(postBuffer, "}");

    len = strlen(postBuffer);
    // Serial.println(len);
    postBuffer[len] = '\0';
    // Serial.print(" : ");
    // Serial.print(postBuffer);
    uint8_t *otherBuffer = (uint8_t*) postBuffer;

    if (!feedPublisher.publish(otherBuffer, len)) {
      Serial.println("Could not send mqtt.");
    }
    // client.println(fieldBuffer);
    // client.println();
    // client.print(postBuffer);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}

