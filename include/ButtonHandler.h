#include <Arduino.h>

#define QUERY_INTERVAL 200
class ButtonHandler {
  public:
    void setPin(uint8_t pin) {
      _pin = pin;
      pinMode(_pin, INPUT);
    };
    bool isClicked();
  private:
    uint8_t _pin;
    uint32_t _timer = millis();
    bool _clicked = false;
    bool _released = true;
    uint32_t _interval = QUERY_INTERVAL;
    void tick();
};