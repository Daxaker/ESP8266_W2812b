#include <ledMatrix.h>

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);
uint8_t globalBrightness = BRIGHTNESS;
uint32_t globalColor = 0x00ff00;
bool loadingFlag = true;
uint8_t thisMode = 0;

void init_ledMatrix(){
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

uint32_t getPixColorXY(uint8_t x, uint8_t y){
  uint16_t thisPixel = XYsafe(x, y);
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

uint32_t getPixColor(int thisSegm) {
  int thisPixel = thisSegm;
  if (thisPixel < 0 || thisPixel > NUM_LEDS - 1) return -1;
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

void drawPixelXY(int8_t x, int8_t y, CRGB color) {
  int thisPixel = XYsafe(x, y);
  leds[thisPixel] = color;
}

uint16_t XY( uint8_t x, uint8_t y){
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * MATRIX_WIDTH) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (MATRIX_WIDTH - 1) - x;
      i = (y * MATRIX_WIDTH) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * MATRIX_WIDTH) + x;
    }
  }
  
  return i;
}

uint16_t XYsafe( uint8_t x, uint8_t y){
  if( x >= MATRIX_WIDTH) return -1;
  if( y >= MATRIX_HEIGHT) return -1;
  return XY(x,y);
}
