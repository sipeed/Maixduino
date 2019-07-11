#ifndef TICKER_H
#define TICKER_H


#include "utils.h"
#include "timer.h"
#include "stdio.h"
#include "stdint.h"

typedef enum{
    TIMER0 = 0,
    TIMER1,
    TIMER2,
    TIMER_MAX
}timer_id_t;

class Ticker
{
public:
    Ticker(timer_id_t id = TIMER2);
    ~Ticker();
    typedef void (*callback_t)(void);
    typedef void (*callback_with_arg_t)(void*);

    void attach(float seconds, callback_t callback)
    {
        _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    void attach_ms(uint32_t milliseconds, callback_t callback)
    {
        _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    void attach_us(uint32_t microseconds, callback_t callback)
    {
        _attach_us(microseconds, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    template<typename TArg>
    void attach(float seconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)arg;
        _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template<typename TArg>
    void attach_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)arg;
        _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template<typename TArg>
    void attach_us(uint32_t microseconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)arg;
        _attach_us(microseconds, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    void once(float seconds, callback_t callback)
    {
        _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    void once_ms(uint32_t milliseconds, callback_t callback)
    {
        _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), 0);	
    }

    void once_us(uint32_t microseconds, callback_t callback)
    {
        _attach_us(microseconds, false, reinterpret_cast<callback_with_arg_t>(callback), 0);	
    }

    template<typename TArg>
    void once(float seconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)(arg);
        _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template<typename TArg>
    void once_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)(arg);
        _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template<typename TArg>
    void once_us(uint32_t microseconds, void (*callback)(TArg), TArg arg)
    {
        configASSERT(sizeof(TArg) <= sizeof(size_t));
        size_t arg32 = (size_t)(arg);
        _attach_us(microseconds, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    void detach();
    bool active();

    callback_with_arg_t user_callback = NULL;
    size_t _arg;

protected:
    void _attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, size_t arg);
    void _attach_us(uint32_t microseconds, bool repeat, callback_with_arg_t callback, size_t arg);

private:
    timer_id_t timer_id;

}__attribute__((packed));

#endif
