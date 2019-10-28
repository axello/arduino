/* ======================================================================
	drawdemo.h
  ====================================================================== */

#include "icons.h"
#include "fonts.h"

#ifndef HEADER_DRAWDEMO
  #define HEADER_DRAWDEMO

Class Drawdemo {
  public:
  	FrameCallback frames[];
  	int numberOfFrames;
  
     	void drawProgress(OLEDDisplay *display, int percentage, String labeltop, String labelbot);
  	void drawProgress(OLEDDisplay *display, int percentage, String labeltop );
  	
  private:
  	void drawFrameWifi(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  	void drawFrameI2C(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  	void drawFrameNet(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  	void drawFrameLogo(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  
}
#endif
