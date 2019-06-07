#include "Arduino.h"
#include "wiring_analog.h"
#include "pwm.h"
#include "fpioa.h"
#include "stdio.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

static int _readResolution = 0;
static int _writeResolution = 8;
static double _writeFreq = 200000;

pwm_fpio_set_t pwm_pins[VARIANT_NUM_PWM]={
    {.channel = PWM_CHANNEL_0, .device = PWM_DEVICE_0, .inuse = 0x0},
    {.channel = PWM_CHANNEL_1, .device = PWM_DEVICE_0, .inuse = 0x0},
    {.channel = PWM_CHANNEL_2, .device = PWM_DEVICE_0, .inuse = 0x0},
    {.channel = PWM_CHANNEL_3, .device = PWM_DEVICE_0, .inuse = 0x0},
    {.channel = PWM_CHANNEL_0, .device = PWM_DEVICE_1, .inuse = 0x0},
    {.channel = PWM_CHANNEL_1, .device = PWM_DEVICE_1, .inuse = 0x0},
    {.channel = PWM_CHANNEL_2, .device = PWM_DEVICE_1, .inuse = 0x0},
    {.channel = PWM_CHANNEL_3, .device = PWM_DEVICE_1, .inuse = 0x0},
    {.channel = PWM_CHANNEL_0, .device = PWM_DEVICE_2, .inuse = 0x0},
    {.channel = PWM_CHANNEL_1, .device = PWM_DEVICE_2, .inuse = 0x0},
    {.channel = PWM_CHANNEL_2, .device = PWM_DEVICE_2, .inuse = 0x0},
    {.channel = PWM_CHANNEL_3, .device = PWM_DEVICE_2, .inuse = 0x0},
};
static uint8_t _pwm_used_pin = 0;
/* K210 CAN'T READ */
void analogReadResolution(int res){
    _readResolution = res;
}

void analogWriteResolution(int res){
    _writeResolution = res;
}

void analogWriteFrequency(double res){
    _writeFreq = res;
}

void analogWritePhase(uint8_t pin, uint32_t phase)
{

}

void analogOutputInit(void)
{

}

void analogWrite(uint8_t ucPin, uint32_t ulValue )
{
    int8_t _pin = k210FpioSet(MD_PIN_MAP(ucPin));
    double _duty;
    if(_pin >= 0){
        _duty = dValueToDuty(ulValue);
        pwm_set_frequency(pwm_pins[_pin].device, pwm_pins[_pin].channel,_writeFreq,_duty>1?1:_duty);
        pwm_set_enable(pwm_pins[_pin].device, pwm_pins[_pin].channel,1);
    }
}

double dValueToDuty(uint32_t ulValue){
    return (double)ulValue/(double)((1<<_writeResolution)-1);
}

int8_t k210FpioSet(uint8_t ucPin)
{
    int8_t _pin;
    for(int i = 0; i < _pwm_used_pin; i++){            
        if(ucPin == pwm_pins[i].inuse){
            return i;
        }
    }
    _pin = getPwmPin();
    if(_pin >= 0){
        fpioa_set_function(ucPin, FUNC_TIMER0_TOGGLE1 + _pin);  
        pwm_pins[_pin].inuse = ucPin;
        pwm_init(pwm_pins[_pin].device);
        _pwm_used_pin += 1;
        return _pin;
    }else{
        return -1;
    }

}

int8_t getPwmPin(void)
{
    for(int i = _pwm_used_pin; i < VARIANT_NUM_PWM; i++){
        if(pwm_pins[i].inuse == 0){
            return i;
        }
    }
    return -1;
}

uint32_t analogRead( uint32_t ulPin ) 
{
#if (defined(BOARD_SIPEED_MAIX_GO) || defined(BOARD_SIPEED_MAIX_ONE_DOCK) )
    return 0;
#endif
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
