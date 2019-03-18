
#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


typedef enum _eAnalogReference
{
  AR_DEFAULT,
} eAnalogReference ;


/*
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * This function is kept only for compatibility with existing AVR based API.
 *
 * \param ulMmode Should be set to AR_DEFAULT.
 */
extern void analogReference( eAnalogReference ulMode ) ;

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite( uint8_t ucPin, uint32_t ulValue ) ;

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulPin
 *
 * \return Read value from selected pin, if no error.
 */
extern uint32_t analogRead( uint32_t ulPin ) ;

/*
 * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
 *
 * \param res
 */
extern void analogReadResolution(int res);

/*
 * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
 *
 * \param res
 */
extern void analogWriteResolution(int res);

/*
 * \brief Set the frequency of analogWrite PWM output. Default is 980 Hz (range from 20 to 25000).
 *
 * \param res
 */
extern void analogWriteFrequency(double freq);

extern void analogWritePhase(uint8_t pin, uint32_t phase);

extern void analogOutputInit( void ) ;

extern double dValueToDuty(uint32_t ulValue);

extern int8_t k210FpioSet(uint8_t ucPin);

extern int8_t getPwmPin(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* _WIRING_ANALOG_ */