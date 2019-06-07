#include "Arduino.h"
#include "encoding.h"
#include "pwm.h"

extern pwm_fpio_set_t pwm_pins[VARIANT_NUM_PWM];

void tone(uint8_t pin, unsigned int frequency, unsigned long duration)
{
    int8_t _pin = k210FpioSet(MD_PIN_MAP(pin));
    if(_pin >= 0){
        if (duration > 0) {
            pwm_set_frequency(pwm_pins[_pin].device, pwm_pins[_pin].channel,(double)frequency,0.5);
            uint64_t _start_time = millis();
            pwm_set_enable(pwm_pins[_pin].device, pwm_pins[_pin].channel,1);
            while ( millis() - _start_time < duration );
            pwm_set_enable(pwm_pins[_pin].device, pwm_pins[_pin].channel,0);
        }else{
            pwm_set_frequency(pwm_pins[_pin].device, pwm_pins[_pin].channel,(double)frequency,0.5);
            pwm_set_enable(pwm_pins[_pin].device, pwm_pins[_pin].channel,1);
        }
    }

}

void noTone(uint8_t pin) 
{
    int8_t _pin = k210FpioSet(MD_PIN_MAP(pin));
    if(_pin >= 0){
        pwm_set_enable(pwm_pins[_pin].device, pwm_pins[_pin].channel,0);
    }
}