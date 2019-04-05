#ifndef __TOUCHSCREEN_H_
#define __TOUCHSCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

typedef enum{
    TOUCHSCREEN_STATUS_IDLE =0,
    TOUCHSCREEN_STATUS_RELEASE,
    TOUCHSCREEN_STATUS_PRESS,
    TOUCHSCREEN_STATUS_MOVE
} touchscreen_type_t;

#ifdef __cplusplus
}//extern "C"
#endif

#include "Wire.h"
#include "ns2009.h"
#define CALIBRATION_SIZE 7

#ifdef __cplusplus
class TouchScreen{
public :
    TouchScreen();
    ~TouchScreen();
    int begin();
    int read();
    int getStatus();
    int getX();
    int getY();
    int calibrate(int w = 320, int h = 240);
    bool is_init();
    int end();
private:
    int calibration[CALIBRATION_SIZE] = {
        -6,
        -5941,
        22203576,
        4232,
        -8,
        -700369,
        65536
    };
    bool _is_init = false ;
    ts_ns2009_pdata_t *ts_ns2009_pdata;
    int _status = TOUCHSCREEN_STATUS_IDLE;
    int _x = 0;
    int _y = 0;
};
#endif

#include "stdlib.h"


#define touchscreen_malloc(p) malloc(p)
#define touchscreen_free(p) free(p)



#endif
