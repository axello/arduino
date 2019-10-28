/* ======================================================================
  Function: drawProgress
  Purpose : prograss indication
  Input   : OLED display pointer
          percent of progress (0..100)
          String above progress bar
          String below progress bar
  Output  : -
  Comments: -
  ====================================================================== */
/********************************
 * Include headers
 ********************************/
#include <Wire.h>
#include "icons.h"
#include "fonts.h"

/********************************
 * Constants and variables
 ********************************/
// Display Settings
// OLED will be checked with this address and this address+1
// so here 0x03c and 0x03d
#define I2C_DISPLAY_ADDRESS 0x3c
// Choose OLED Driver Type (one only)
#define OLED_SSD1306
// #define OLED_SH1106

// I2C Pins Settings
#ifdef ARDUINO_ARCH_ESP8266
#define SDA_PIN 4
#define SDC_PIN 5
#else
// Change this, depending on your board I2C pins used
#define SDA_PIN 5
#define SDC_PIN 4
#endif


  #if defined (OLED_SSD1306)
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#elif defined (OLED_SH1106)
#include <SH1106Wire.h>
#include <OLEDDisplayUi.h>
#endif

// Number of line to display for devices and Wifi
#define I2C_DISPLAY_DEVICE  4
#define WIFI_DISPLAY_NET    4

// OLED Driver Instantiation
#ifdef OLED_SSD1306
SSD1306Wire  display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
#else
SH1106Wire  display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
#endif
OLEDDisplayUi ui( &display );

bool has_display          = false;  // if I2C display detected
uint8_t NumberOfI2CDevice = 0;      // number of I2C device detected
int8_t NumberOfNetwork    = 0;      // number of wifi networks detected
char i2c_dev[I2C_DISPLAY_DEVICE][32]; // Array on string displayed

  // this array keeps function pointers to all frames
  // frames are the single views that slide from right to left
//  FrameCallback frames[] = { drawFrameWifi, drawFrameI2C, drawFrameNet, drawFrameLogo};
//  int numberOfFrames = 4;

/********************************
 * Function headers
 ********************************/
  void displaySetup();
  void drawFrameWifi(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  uint8_t i2c_scan(uint8_t address = 0xff);
  
/********************************
 * Functions
 ********************************/
  
void displaySetup() {
        //Wire.pins(SDA, SCL);
    Wire.begin(SDA_PIN, SDC_PIN);
    Wire.setClock(100000);
  
    if (i2c_scan(I2C_DISPLAY_ADDRESS)) {
      has_display = true;
    } else {
      if (i2c_scan(I2C_DISPLAY_ADDRESS + 1)) {
        has_display = true;
      }
    }
  
    if (has_display) {
      Serial.println(F("Display found"));
      // initialize dispaly
      display.init();
      display.flipScreenVertically();
      display.clear();
      display.drawXbm((128 - ch2i_width) / 2, 0, ch2i_width, ch2i_height, ch2i_bits);
      display.display();
  
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setContrast(255);
      delay(500);
    }
  
  
  }
	void drawProgress(OLEDDisplay *display, int percentage, String labeltop, String labelbot) {
	  if (has_display) {
		display->clear();
		display->setTextAlignment(TEXT_ALIGN_CENTER);
		display->setFont(Roboto_Condensed_Bold_Bold_16);
		display->drawString(64, 8, labeltop);
		display->drawProgressBar(10, 28, 108, 12, percentage);
		display->drawString(64, 48, labelbot);
		display->display();
	  }
	}

	/* ======================================================================
	  Function: drawProgress
	  Purpose : prograss indication
	  Input   : OLED display pointer
			  percent of progress (0..100)
			  String above progress bar
	  Output  : -
	  Comments: -
	  ====================================================================== */
	void drawProgress(OLEDDisplay *display, int percentage, String labeltop ) {
	  drawProgress(display, percentage, labeltop, String(""));
	}

	/* ======================================================================
	  Function: drawFrameWifi
	  Purpose : WiFi logo and IP address
	  Input   : OLED display pointer
	  Output  : -
	  Comments: -
	  ====================================================================== */
	void drawFrameWifi(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	  display->clear();
	  display->setTextAlignment(TEXT_ALIGN_CENTER);
	  display->setFont(Roboto_Condensed_Bold_Bold_16);
	  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
	  // on how to create xbm files
	  display->drawXbm( x + (128 - WiFi_width) / 2, 0, WiFi_width, WiFi_height, WiFi_bits);
	  display->drawString(x + 64, WiFi_height + 4, WiFi.localIP().toString());
	  ui.disableIndicator();
	}

	/* ======================================================================
	  Function: drawFrameI2C
	  Purpose : I2C info screen (called by OLED ui)
	  Input   : OLED display pointer
	  Output  : -
	  Comments: -
	  ====================================================================== */
//	void drawFrameI2C(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//	  char buff[16];
//	  sprintf(buff, "%d I2C Device%c", NumberOfI2CDevice, NumberOfI2CDevice > 1 ? 's' : ' ');
//
//	  display->clear();
//	  display->setTextAlignment(TEXT_ALIGN_CENTER);
//	  display->setFont(Roboto_Condensed_Bold_Bold_16);
//	  display->drawString(x + 64, y +  0, buff);
//	  display->setTextAlignment(TEXT_ALIGN_LEFT);
//	  //display->setFont(Roboto_Condensed_Plain_16);
//	  display->setFont(Roboto_Condensed_12);
//
//	  for (uint8_t i = 0; i < NumberOfI2CDevice; i++) {
//		if (i < I2C_DISPLAY_DEVICE)
//		  display->drawString(x + 0, y + 16 + 12 * i, i2c_dev[i]);
//	  }
//	  ui.disableIndicator();
//	}

	/* ======================================================================
	  Function: drawFrameNet
	  Purpose : WiFi network info screen (called by OLED ui)
	  Input   : OLED display pointer
	  Output  : -
	  Comments: -
	  ====================================================================== */
	void drawFrameNet(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	  char buff[64];
	  sprintf(buff, "%d Wifi Network", NumberOfNetwork);
	  display->clear();
	  display->setTextAlignment(TEXT_ALIGN_CENTER);
	  display->setFont(Roboto_Condensed_Bold_Bold_16);
	  display->drawString(x + 64, y + 0 , buff);
	  display->setTextAlignment(TEXT_ALIGN_LEFT);

	  uint8_t *robo = (uint8_t*) Roboto_Condensed_12;
  
	  display->setFont(robo);

	  for (int i = 0; i < NumberOfNetwork; i++) {
		// Print SSID and RSSI for each network found
		if (i < WIFI_DISPLAY_NET) {

		  #ifdef ARDUINO_ARCH_ESP8266
		  sprintf(buff, "%s %c", WiFi.SSID(i).c_str(), WiFi.encryptionType(i) == ENC_TYPE_NONE ? ' ' : '*' );
		  #else  
		  sprintf(buff, "%s %c", WiFi.SSID(i).c_str(), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? ' ' : '*' );
		  #endif
		  display->drawString(x + 0, y + 16 + 12 * i, buff);
		}
	  }

	  ui.disableIndicator();
	}

 /* ======================================================================
  Function: i2cScan
  Purpose : scan I2C bus
  Input   : specifc address if looking for just 1 specific device
  Output  : number of I2C devices seens
  Comments: -
  ====================================================================== */
uint8_t i2c_scan(uint8_t address)
{
  uint8_t error;
  int nDevices;
  uint8_t start = 1 ;
  uint8_t end   = 0x7F ;
  uint8_t index = 0;
  char device[16];
  char buffer[32];

  if (address >= start && address <= end) {
    start = address;
    end   = address + 1;
    Serial.print(F("Searching for device at address 0x"));
    Serial.printf("%02X ", address);
  } else {
    Serial.println(F("Scanning I2C bus ..."));
  }

  nDevices = 0;
  for (address = start; address < end; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("Device ");

      if (address == 0x40)
        strcpy(device, "TH02" );
      else if (address == 0x29 || address == 0x39 || address == 0x49)
        strcpy(device, "TSL2561" );

      else if (address==0x50) {
        strcpy(device, "24AA02E64" );
        // This device respond to 0x50 to 0x57 address
        address+=0x07;
      }
      else if (address == 0x55 )
        strcpy(device, "BQ72441" );
      else if (address == I2C_DISPLAY_ADDRESS || address == I2C_DISPLAY_ADDRESS + 1)
        strcpy(device, "OLED SSD1306" );
      else if (address >= 0x60 && address <= 0x62 ) {
        strcpy(device, "MCP4725_Ax" );
        device[9]= '0' + (address & 0x03);
      } else if (address >= 0x68 && address <= 0x6A ) {
        strcpy(device, "MCP3421_Ax" );
        device[9]= '0' + (address & 0x03);
      } else if (address == 0x64)
        strcpy(device, "ATSHA204" );
      else
        strcpy(device, "Unknown" );

      sprintf(buffer, "0x%02X : %s", address, device );
      if (index < I2C_DISPLAY_DEVICE) {
        strcpy(i2c_dev[index++], buffer );
      }

      Serial.println(buffer);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.printf("Unknow error at address 0x%02X", address);
    }

    yield();
  }
  if (nDevices == 0)
    Serial.println(F("No I2C devices found"));
  else
    Serial.printf("Scan done, %d device found\r\n", nDevices);

  return nDevices;
}
