#ifndef LEDMATRIX_h
#define LEDMATRIX_h

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 4
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS 64
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
const bool kMatrixSerpentineLayout = true;

extern CRGB* const leds;
extern uint8_t globalBrightness;
extern uint8_t modeCode;
extern uint32_t globalColor;
extern bool loadingFlag;
extern uint8_t thisMode;
const uint8_t maxDim = max(MATRIX_WIDTH, MATRIX_HEIGHT);

void init_ledMatrix();
uint32_t getPixColorXY(uint8_t x, uint8_t y);
uint32_t getPixColor(int thisSegm);
void drawPixelXY(int8_t x, int8_t y, CRGB color); 
uint16_t XYsafe( uint8_t x, uint8_t y);
uint16_t XY( uint8_t x, uint8_t y);
#endif