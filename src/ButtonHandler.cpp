#include <ButtonHandler.h>

#define THRESHOLD 300

void ButtonHandler::tick(){
  uint32_t elapsed = (uint32_t)millis() - _timer;
  if(_released && elapsed > _interval){
    _timer = millis();
    _clicked = digitalRead(_pin) == HIGH;
    _released = false;
  }
  else {
    if(digitalRead(_pin) == LOW)
      _released = true;
    
    _clicked = false;
  }
}

bool ButtonHandler::isClicked(){
  tick();
  return _clicked;
}