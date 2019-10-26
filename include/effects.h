#ifndef EFFECTS_h
#define EFFECTS_h

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
void brightnessRoutine(); 

// *********** смена цвета активных светодиодов (рисунка) ***********
void colorsRoutine();

// *********** снегопад 2.0 ***********
void snowRoutine();

void matrixRoutine();

// ***************************** БЛУДНЫЙ КУБИК *****************************

void ballRoutine();
// *********** радуга заливка ***********
void rainbowRoutine();
// *********** радуга дигональная ***********
void rainbowDiagonalRoutine();
// *********** радуга активных светодиодов (рисунка) ***********
void rainbowColorsRoutine();

void fireRoutine(); 
// Randomly generate the next line (matrix row)

void generateLine();
//shift all values in the matrix up one row

void ballsRoutine();

// ********************* ЗВЕЗДОПАД ******************
void starfallRoutine();

// рандомные гаснущие вспышки
void sparklesRoutine();

#endif