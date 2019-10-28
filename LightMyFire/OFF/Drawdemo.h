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
// Display Settings
// OLED will be checked with this address and this address+1
// so here 0x03c and 0x03d
#define I2C_DISPLAY_ADDRESS 0x3c
// Choose OLED Driver Type (one only)
#define OLED_SSD1306
// #define OLED_SH1106

  #if defined (OLED_SSD1306)
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#elif defined (OLED_SH1106)
#include <SH1106Wire.h>
#include <OLEDDisplayUi.h>
#endif

bool has_display          = false;  // if I2C display detected

#include "Drawdemo2.h"

	// this array keeps function pointers to all frames
	// frames are the single views that slide from right to left
	FrameCallback frames[] = { drawFrameWifi, drawFrameI2C, drawFrameNet, drawFrameLogo};
	int numberOfFrames = 4;

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
	void drawFrameI2C(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	  char buff[16];
	  sprintf(buff, "%d I2C Device%c", NumberOfI2CDevice, NumberOfI2CDevice > 1 ? 's' : ' ');

	  display->clear();
	  display->setTextAlignment(TEXT_ALIGN_CENTER);
	  display->setFont(Roboto_Condensed_Bold_Bold_16);
	  display->drawString(x + 64, y +  0, buff);
	  display->setTextAlignment(TEXT_ALIGN_LEFT);
	  //display->setFont(Roboto_Condensed_Plain_16);
	  display->setFont(Roboto_Condensed_12);

	  for (uint8_t i = 0; i < NumberOfI2CDevice; i++) {
		if (i < I2C_DISPLAY_DEVICE)
		  display->drawString(x + 0, y + 16 + 12 * i, i2c_dev[i]);
	  }
	  ui.disableIndicator();
	}

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

	  char *robo = (char*) Roboto_Condensed_12;
  
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
	  Function: drawFrameLogo
	  Purpose : Company logo info screen (called by OLED ui)
	  Input   : OLED display pointer
	  Output  : -
	  Comments: -
	  ====================================================================== */
	void drawFrameLogo(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
	  display->clear();
	  display->drawXbm(x + (128 - ch2i_width) / 2, y, ch2i_width, ch2i_height, ch2i_bits);
	  ui.disableIndicator();
	}
