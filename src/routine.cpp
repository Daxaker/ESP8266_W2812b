#include <routine.h>
#include <ledMatrix.h>
#include <effects.h>
#include <Timer.h>

#define NEXT_ROUTINE_BUTTON 5
#define D_EFFECT_SPEED 80
#define MODES_AMOUNT 9   // количество кастомных режимов (которые переключаются сами или кнопкой)

TimerM timer(D_EFFECT_SPEED);

void customModes() {
  switch (thisMode) {
    case 0: snowRoutine();
      break;
    case 1: sparklesRoutine();
      break;
    case 2: matrixRoutine();
      break;
    case 3: ballRoutine();
      break;
    case 4: ballsRoutine();
      break;
    case 5: rainbowRoutine();
      break;
    case 6: rainbowDiagonalRoutine();
      break;
    case 7: fireRoutine();
      break;
    case 8: starfallRoutine();
      break;
  }
}
ButtonHandler nextButton;
void initRoutines(){
  nextButton.setPin(NEXT_ROUTINE_BUTTON);
}

void nextRoutine() {
  thisMode++;
  if (thisMode >= MODES_AMOUNT) thisMode = 0;
  loadingFlag = true;
  FastLED.clear();
  FastLED.show();
}
void prevRoutine() {
  thisMode--;
  if (thisMode < 0) thisMode = MODES_AMOUNT - 1;
  loadingFlag = true;
  FastLED.clear();
  FastLED.show();
}

void buttonHandler() {
  if(nextButton.isClicked()) {
    nextRoutine();
  }
}

void setRoutine(uint8_t routineId){
  thisMode = routineId;
  thisMode = (routineId < 0 || routineId >= MODES_AMOUNT) 
    ? 0 
    : routineId;
  loadingFlag = true;
  FastLED.clear();
  FastLED.show();
}

void loopRoutine(){
  if(timer.isReady()) {
    buttonHandler();
    customModes();
    loadingFlag = false;
    FastLED.show();
  }
}