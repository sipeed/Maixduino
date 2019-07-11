#include "Ticker.h"
#include "sysctl.h"

static int timer_callback(void* ctx);

Ticker::Ticker(timer_id_t id)
:timer_id(id)
{
    timer_init(timer_device_number_t(timer_id));
}

Ticker::~Ticker()
{
    detach();
}

void
Ticker::_attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, size_t arg)
{
    user_callback = callback;
    _arg = arg;
    timer_set_interval(timer_device_number_t(timer_id), TIMER_CHANNEL_0, milliseconds * 1000000);
    timer_irq_register(timer_device_number_t(timer_id), TIMER_CHANNEL_0, !repeat, 4, timer_callback, this);
    timer_set_enable(timer_device_number_t(timer_id), TIMER_CHANNEL_0, 1);
    sysctl_enable_irq();
}

void
Ticker::_attach_us(uint32_t microseconds, bool repeat, callback_with_arg_t callback, size_t arg)
{
    user_callback = callback;
    _arg = arg;
    timer_set_interval(timer_device_number_t(timer_id), TIMER_CHANNEL_0, microseconds * 1000);
    timer_irq_register(timer_device_number_t(timer_id), TIMER_CHANNEL_0, !repeat, 4, timer_callback, this);
    timer_set_enable(timer_device_number_t(timer_id), TIMER_CHANNEL_0, 1);
    sysctl_enable_irq();
}

void 
Ticker::detach()
{
    timer_irq_unregister(timer_device_number_t(timer_id), TIMER_CHANNEL_0);
    timer_set_enable(timer_device_number_t(timer_id), TIMER_CHANNEL_0, 0);
}

static int timer_callback(void* ctx)
{
    auto &driver = *reinterpret_cast<Ticker *>(ctx);
    driver.user_callback((void *)driver._arg);
    return 0;
}
