// origin from: github.com/AlexGyver/GyverMatrixBT/firmware/GyverMatrixOS_v1.12/effects.ino

#include <effects.h>
#include <Arduino.h>
#include <ledMatrix.h>
#include <FastLED.h>

// эффекты

// **************** НАСТРОЙКИ ЭФФЕКТОВ ****************
// эффект "синусоиды" - ОТКЛЮЧЕН
#define WAVES_AMOUNT 2    // количество синусоид

// эффект "шарики"
#define BALLS_AMOUNT 3    // количество "шариков"
#define CLEAR_PATH 1      // очищать путь
#define BALL_TRACK 1      // (0 / 1) - вкл/выкл следы шариков
#define DRAW_WALLS 0      // режим с рисованием препятствий для шаров (не работает на ESP и STM32)
#define TRACK_STEP 70     // длина хвоста шарика (чем больше цифра, тем хвост короче)

// эффект "квадратик"
#define BALL_SIZE 3       // размер шара
#define RANDOM_COLOR 1    // случайный цвет при отскоке

// эффект "огонь"
#define SPARKLES 1        // вылетающие угольки вкл выкл
#define HUE_ADD 0         // добавка цвета в огонь (от 0 до 230) - меняет весь цвет пламени

// эффект "кометы"
#define TAIL_STEP 100     // длина хвоста кометы
#define SATURATION 150    // насыщенность кометы (от 0 до 255)
#define STAR_DENSE 60     // количество (шанс появления) комет

// эффект "конфетти"
#define DENSE 3           // плотность конфетти
#define BRIGHT_STEP 70    // шаг уменьшения яркости

// эффект "снег"
#define SNOW_DENSE 10     // плотность снегопада

// --------------------- ДЛЯ РАЗРАБОТЧИКОВ ----------------------

// *********** "дыхание" яркостью ***********
uint8_t modeCode = 0;
byte breathBrightness = BRIGHTNESS;
bool brightnessDirection;
void brightnessRoutine() {
  if (brightnessDirection) {
    breathBrightness += 2;
    if (breathBrightness > globalBrightness - 1) {
      brightnessDirection = false;
    }
  } else {
    breathBrightness -= 2;
    if (breathBrightness < 1) {
      brightnessDirection = true;
    }
  }
  FastLED.setBrightness(breathBrightness);
}

// *********** смена цвета активных светодиодов (рисунка) ***********
byte hue;
void colorsRoutine() {
  hue += 4;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (getPixColor(i) > 0) leds[i] = CRGB(CHSV(hue, 255, 255));
  }
}

// *********** снегопад 2.0 ***********
void snowRoutine() {
  modeCode = 12;
  // сдвигаем всё вниз
  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    for (byte y = 0; y < MATRIX_HEIGHT - 1; y++) {
      leds[XYsafe(x, y)] = leds[XYsafe(x, y + 1)];
    }
  }

  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, MATRIX_HEIGHT - 2) == 0 && (random(0, SNOW_DENSE) == 0))
      drawPixelXY(x, MATRIX_HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, MATRIX_HEIGHT - 1, 0x000000);
  }
}

// ***************************** БЛУДНЫЙ КУБИК *****************************
int coordB[2];
int8_t vectorB[2];
CRGB ballColor;

void ballRoutine() {
  if (loadingFlag) {
    for (byte i = 0; i < 2; i++) {
      coordB[i] = MATRIX_WIDTH / 2 * 10;
      vectorB[i] = random(8, 20);
      ballColor = CHSV(random(0, 9) * 28, 255, 255);
    }
    modeCode = 16;
    loadingFlag = false;
  }
  for (byte i = 0; i < 2; i++) {
    coordB[i] += vectorB[i];
    if (coordB[i] < 0) {
      coordB[i] = 0;
      vectorB[i] = -vectorB[i];
      if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
      //vectorB[i] += random(0, 6) - 3;
    }
  }
  if (coordB[0] > (MATRIX_WIDTH - BALL_SIZE) * 10) {
    coordB[0] = (MATRIX_WIDTH - BALL_SIZE) * 10;
    vectorB[0] = -vectorB[0];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
    //vectorB[0] += random(0, 6) - 3;
  }
  if (coordB[1] > (MATRIX_HEIGHT - BALL_SIZE) * 10) {
    coordB[1] = (MATRIX_HEIGHT - BALL_SIZE) * 10;
    vectorB[1] = -vectorB[1];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255, 255);
    //vectorB[1] += random(0, 6) - 3;
  }
  FastLED.clear();
  for (byte i = 0; i < BALL_SIZE; i++)
    for (byte j = 0; j < BALL_SIZE; j++)
      leds[XYsafe(coordB[0] / 10 + i, coordB[1] / 10 + j)] = ballColor;
}

// *********** радуга заливка ***********
void rainbowRoutine() {
  modeCode = 18;
  hue += 3;
  for (byte i = 0; i < MATRIX_WIDTH; i++) {
    CHSV thisColor = CHSV((byte)(hue + i * float(255 / MATRIX_WIDTH)), 255, 255);
    for (byte j = 0; j < MATRIX_HEIGHT; j++)      
      drawPixelXY(i, j, thisColor);   //leds[XYsafe(i, j)] = thisColor;
  }
}

// *********** радуга дигональная ***********
void rainbowDiagonalRoutine() {
  modeCode = 19;
  hue += 3;
  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    for (byte y = 0; y < MATRIX_HEIGHT; y++) {
      CHSV thisColor = CHSV((byte)(hue + (float)(MATRIX_WIDTH / MATRIX_HEIGHT * x + y) * (float)(255 / maxDim)), 255, 255);      
      drawPixelXY(x, y, thisColor); //leds[XYsafe(i, j)] = thisColor;
    }
  }
}


// *********** радуга активных светодиодов (рисунка) ***********
void rainbowColorsRoutine() {
  hue++;
  for (byte i = 0; i < MATRIX_WIDTH; i++) {
    CHSV thisColor = CHSV((byte)(hue + i * float(255 / MATRIX_WIDTH)), 255, 255);
    for (byte j = 0; j < MATRIX_HEIGHT; j++)
      if (getPixColor(XYsafe(i, j)) > 0) drawPixelXY(i, j, thisColor);
  }
}


// ********************** огонь **********************
unsigned char matrixValue[8][16];
unsigned char line[MATRIX_WIDTH];
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};
void shiftUp();
void drawFrame(int pcnt);

void fireRoutine() {
  if (loadingFlag) {
    modeCode = 20;
    loadingFlag = false;
    FastLED.clear();
    generateLine();
    memset(matrixValue, 0, sizeof(matrixValue));
  }
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 30;
}


// Randomly generate the next line (matrix row)

void generateLine() {
  for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    line[x] = random(64, 255);
  }
}

//shift all values in the matrix up one row

void shiftUp() {
  for (uint8_t y = MATRIX_HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

void drawFrame(int pcnt) {
  int nextv;

  //each row interpolates with the one before it
  for (unsigned char y = MATRIX_HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y < 8) {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - pgm_read_byte(&(valueMask[y][newX]));

        CRGB color = CHSV(
                       HUE_ADD + pgm_read_byte(&(hueMask[y][newX])), // H
                       255, // S
                       (uint8_t)max(0, nextv) // V
                     );

        leds[XYsafe(x, y)] = color;
      } else if (y == 8 && SPARKLES) {
        if (random(0, 20) == 0 && getPixColorXY(x, y - 1) != 0) drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);
      } else if (SPARKLES) {

        // старая версия для яркости
        if (getPixColorXY(x, y - 1) > 0)
          drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);

      }
    }
  }

  //first row interpolates with the "next" line
  for (unsigned char x = 0; x < MATRIX_WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    CRGB color = CHSV(
                   HUE_ADD + pgm_read_byte(&(hueMask[0][newX])), // H
                   255,           // S
                   (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
                 );
    leds[XYsafe(newX, 0)] = color;
  }
}

// **************** МАТРИЦА *****************
void matrixRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    modeCode = 14;
    FastLED.clear();
  }
  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getPixColorXY(x, MATRIX_HEIGHT - 1);
    if (thisColor == 0)
      drawPixelXY(x, MATRIX_HEIGHT - 1, 0x00FF00 * (random(0, 10) == 0));
    else if (thisColor < 0x002000)
      drawPixelXY(x, MATRIX_HEIGHT - 1, 0);
    else
      drawPixelXY(x, MATRIX_HEIGHT - 1, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (byte x = 0; x < MATRIX_WIDTH; x++) {
    for (byte y = 0; y < MATRIX_HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}
void fadePixel(byte i, byte j, byte step);
// функция плавного угасания цвета для всех пикселей
void fader(uint8_t step) {
  for (byte i = 0; i < MATRIX_WIDTH; i++) {
    for (byte j = 0; j < MATRIX_HEIGHT; j++) {
      fadePixel(i, j, step);
    }
  }
}
void fadePixel(byte i, byte j, byte step) {     // новый фейдер
  int pixelNum = XYsafe(i, j);
  if (getPixColor(pixelNum) == 0) return;

  if (leds[pixelNum].r >= 30 ||
      leds[pixelNum].g >= 30 ||
      leds[pixelNum].b >= 30) {
    leds[pixelNum].fadeToBlackBy(step);
  } else {
    leds[pixelNum] = 0;
  }
}


// ********************************* ШАРИКИ *********************************
int coord[BALLS_AMOUNT][2];
int8_t vector[BALLS_AMOUNT][2];
CRGB ballColors[BALLS_AMOUNT];

void ballsRoutine() {
  if (loadingFlag) {
    modeCode = 17;
    loadingFlag = false;
    for (byte j = 0; j < BALLS_AMOUNT; j++) {
      int sign;

      // забиваем случайными данными
      coord[j][0] = MATRIX_WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][0] = random(4, 15) * sign;
      coord[j][1] = MATRIX_HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][1] = random(4, 15) * sign;
      ballColors[j] = CHSV(random(0, 9) * 28, 255, 255);
    }
  }


  if (!BALL_TRACK)    // если режим БЕЗ следов шариков
    FastLED.clear();  // очистить
  else {              // режим со следами
    fader(TRACK_STEP);
  }

  // движение шариков
  for (byte j = 0; j < BALLS_AMOUNT; j++) {

    // отскок от нарисованных препятствий
    if (DRAW_WALLS) {
      uint32_t thisColor = getPixColorXY(coord[j][0] / 10 + 1, coord[j][1] / 10);
      if (thisColor == globalColor/* && vector[j][0] > 0*/) {
        vector[j][0] = -vector[j][0];
      }
      thisColor = getPixColorXY(coord[j][0] / 10 - 1, coord[j][1] / 10);
      if (thisColor == globalColor/* && vector[j][0] < 0*/) {
        vector[j][0] = -vector[j][0];
      }
      thisColor = getPixColorXY(coord[j][0] / 10, coord[j][1] / 10 + 1);
      if (thisColor == globalColor/* && vector[j][1] > 0*/) {
        vector[j][1] = -vector[j][1];
      }
      thisColor = getPixColorXY(coord[j][0] / 10, coord[j][1] / 10 - 1);
      if (thisColor == globalColor/* && vector[j][1] < 0*/) {
        vector[j][1] = -vector[j][1];
      }
    }

    // движение шариков
    for (byte i = 0; i < 2; i++) {
      coord[j][i] += vector[j][i];
      if (coord[j][i] < 0) {
        coord[j][i] = 0;
        vector[j][i] = -vector[j][i];
      }
    }

    if (coord[j][0] > (MATRIX_WIDTH - 1) * 10) {
      coord[j][0] = (MATRIX_WIDTH - 1) * 10;
      vector[j][0] = -vector[j][0];
    }
    if (coord[j][1] > (MATRIX_HEIGHT - 1) * 10) {
      coord[j][1] = (MATRIX_HEIGHT - 1) * 10;
      vector[j][1] = -vector[j][1];
    }
    leds[XYsafe(coord[j][0] / 10, coord[j][1] / 10)] =  ballColors[j];
  }
}

// ********************* ЗВЕЗДОПАД ******************
void starfallRoutine() {
  modeCode = 15;
  // заполняем головами комет левую и верхнюю линию
  for (byte i = MATRIX_HEIGHT / 2; i < MATRIX_HEIGHT; i++) {
    if (getPixColorXY(0, i) == 0
        && (random(0, STAR_DENSE) == 0)
        && getPixColorXY(0, i + 1) == 0
        && getPixColorXY(0, i - 1) == 0)
      drawPixelXY(0, i, CHSV(random(0, 200), SATURATION, 255));
  }
  for (byte i = 0; i < MATRIX_WIDTH / 2; i++) {
    if (getPixColorXY(i, MATRIX_HEIGHT - 1) == 0
        && (random(0, STAR_DENSE) == 0)
        && getPixColorXY(i + 1, MATRIX_HEIGHT - 1) == 0
        && getPixColorXY(i - 1, MATRIX_HEIGHT - 1) == 0)
      drawPixelXY(i, MATRIX_HEIGHT - 1, CHSV(random(0, 200), SATURATION, 255));
  }

  // сдвигаем по диагонали
  for (byte y = 0; y < MATRIX_HEIGHT - 1; y++) {
    for (byte x = MATRIX_WIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, getPixColorXY(x - 1, y + 1));
      fadePixel(x,y, TAIL_STEP);
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  // for (byte i = MATRIX_HEIGHT / 2; i < MATRIX_HEIGHT; i++) {
  //   fadePixel(0, i, TAIL_STEP);
  // }
  // for (byte i = 0; i < MATRIX_WIDTH / 2; i++) {
  //   fadePixel(i, MATRIX_HEIGHT - 1, TAIL_STEP);
  // }
}

// рандомные гаснущие вспышки
void sparklesRoutine() {
  modeCode = 13;
  for (byte i = 0; i < DENSE; i++) {
    byte x = random(0, MATRIX_WIDTH);
    byte y = random(0, MATRIX_HEIGHT);
    if (getPixColorXY(x, y) == 0)
      leds[XYsafe(x, y)] = CHSV(random(0, 255), 255, 255);
  }
  fader(BRIGHT_STEP);
}