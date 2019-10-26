#include <Arduino.h>
class TimerM
{
  public:
    TimerM(uint32_t interval);				// объявление таймера с указанием интервала
    void setInterval(uint32_t interval);	// установка интервала работы таймера
    boolean isReady();						// возвращает true, когда пришло время. Сбрасывается в false сам (AUTO) или вручную (MANUAL)
    void reset();							// ручной сброс таймера на установленный интервал

  private:
    uint32_t _timer = 0;
    uint32_t _interval = 0;
};

TimerM::TimerM(uint32_t interval) {
  _interval = interval;
  _timer = millis();
}

void TimerM::setInterval(uint32_t interval) {
  _interval = interval;
}

bool TimerM::isReady() {
  if ((long)millis() - _timer >= _interval) {
    _timer = millis();
    return true;
  } else {
    return false;
  }
}

void TimerM::reset() {
  _timer = millis();
}