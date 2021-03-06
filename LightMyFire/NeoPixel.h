/* ======================================================================
  Function: NeoPixel
  ====================================================================== */
// RGB Led on GPIO0 comment this line if you have no RGB LED
#define RGB_LED_PIN 12

// Number of RGB Led (can be a ring or whatever) 
#define RGB_LED_COUNT 8

// Select The line of your LED Type (see NeoPixel Library)
//#define RGB_TYPE NeoGrbwFeature
//#define RGB_TYPE NeoRgbwFeature
#define RGB_TYPE NeoGrbFeature
// #define RGB_TYPE NeoRgbFeature // hsl wrong for my neopixels, Axel

#ifdef RGB_LED_PIN
#include <NeoPixelBus.h>
#endif

#ifdef RGB_LED_PIN
  #ifdef ARDUINO_ARCH_ESP8266
    NeoPixelBus<RGB_TYPE, NeoEsp8266BitBang800KbpsMethod>rgb_led(RGB_LED_COUNT, RGB_LED_PIN);
  #else
    NeoPixelBus<RGB_TYPE, NeoEsp32BitBang800KbpsMethod>rgb_led(RGB_LED_COUNT, RGB_LED_PIN);
  #endif
#endif


#ifdef RGB_LED_PIN
void LedRGBOFF(uint16_t led = 0);
void LedRGBON (float hue, uint16_t led = 0);
#else
void LedRGBOFF(uint16_t led = 0) {};
void LedRGBON (float hue, uint16_t led = 0) {};
#endif

uint8_t rgb_luminosity    = 100 ;    // Luminosity from 0 to 100%


#ifdef RGB_LED_PIN
/* ======================================================================
  Function: LedRGBON
  Purpose : Set RGB LED strip color, but does not lit it
  Input   : Hue of LED (0..360)
          led number (from 1 to ...), if 0 then all leds
  Output  : -
  Comments:
  ====================================================================== */
void LedRGBON (float hue, uint16_t led)
{
  uint8_t start = 0;
  uint8_t end   = RGB_LED_COUNT - 1; // Start at 0

  // Convert to neoPixel API values
  // H should be between 0.0 and 1.0
  // S is saturation keep it to 1
  // L is brightness should be between 0.0 and 0.5
  // rgb_luminosity is between 0 and 100 (percent)
  RgbColor target = HslColor( hue, 1.0f, 0.005f * rgb_luminosity);

  // just one LED ?
  // Strip start 0 not 1
  if (led) {
    led--;
    start = led ;
    end   = start ;
  }

  for (uint8_t i = start ; i <= end; i++) {
    rgb_led.SetPixelColor(i, target);
    rgb_led.Show();
  }
}

/* ======================================================================
  Function: LedRGBOFF
  Purpose : light off the RGB LED strip
  Input   : Led number starting at 1, if 0=>all leds
  Output  : -
  Comments: -
  ====================================================================== */
void LedRGBOFF(uint16_t led)
{
  uint8_t start = 0;
  uint8_t end   = RGB_LED_COUNT - 1; // Start at 0

  // just one LED ?
  if (led) {
    led--;
    start = led ;
    end   = start ;
  }

  // stop animation, reset params
  for (uint8_t i = start ; i <= end; i++) {
    // clear the led strip
    rgb_led.SetPixelColor(i, RgbColor(0));
    rgb_led.Show();
  }
}
#endif
