#include "touchscreen.h"
#include "Wire.h"

extern "C"
{
#include "tscal.h"
}

TouchScreen::TouchScreen()
{

}

TouchScreen::~TouchScreen()
{

}


int 
TouchScreen::begin()
{
    int err;

    Wire.begin();

    ts_ns2009_pdata = ts_ns2009_probe(calibration, &err); //calibration
    if (ts_ns2009_pdata == NULL || err!=0)
        return err;
    _is_init = true;
    return 0;
}

bool
TouchScreen::is_init()
{
    return _is_init;
}

int
TouchScreen::read()
{
    ts_ns2009_poll(ts_ns2009_pdata);
    _x = 0;
    _y = 0;
    switch (ts_ns2009_pdata->event->type)
    {
        case TOUCH_BEGIN:
            _x = ts_ns2009_pdata->event->x;
            _y = ts_ns2009_pdata->event->y;
            _status = TOUCHSCREEN_STATUS_PRESS;
            break;

        case TOUCH_MOVE:
            _x = ts_ns2009_pdata->event->x;
            _y = ts_ns2009_pdata->event->y;
            _status = TOUCHSCREEN_STATUS_MOVE;
            break;

        case TOUCH_END:
            _x= ts_ns2009_pdata->event->x;
            _y= ts_ns2009_pdata->event->y;
            _status = TOUCHSCREEN_STATUS_RELEASE;
            break;
        default:
            break;
    }
    return 0;

}

int
TouchScreen::getStatus()
{
    return _status;
}

int
TouchScreen::getX()
{
    return _x;
}

int
TouchScreen::getY()
{
    return _y;
}

int 
TouchScreen::end()
{
    ts_ns2009_remove(ts_ns2009_pdata);
    _is_init = false;
    return 0;
}

int
TouchScreen::calibrate(int w, int h)
{
    return do_tscal(ts_ns2009_pdata, w, h, calibration);
}

//////////////HAL//////////////

int ns2009_hal_i2c_recv(const uint8_t *send_buf, size_t send_buf_len, uint8_t *receive_buf,
                  size_t receive_buf_len)
{
    Wire.beginTransmission(NS2009_SLV_ADDR);
    Wire.write(send_buf,send_buf_len);
    Wire.endTransmission();
    Wire.requestFrom(NS2009_SLV_ADDR,receive_buf_len);
    while(Wire.available()){
        *receive_buf++ = Wire.read();
    }
    return 0;
}
