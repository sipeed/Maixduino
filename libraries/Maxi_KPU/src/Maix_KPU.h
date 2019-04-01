
#ifndef __MOBILENET_V1_H
#define __MOBILENET_V1_H

extern "C" {
#include "kpu.h"
}
#include "stdint.h"
#include "stdbool.h"


typedef enum{
    KPU_ERROR_BUSY          = -3,
    KPU_ERROR_BAD_MODEL     = -2,
    KPU_ERROR_MODEL_VERSION = -1,
    KPU_ERROR_NONE          = 0,
} KPU_Error_t;


class KPUClass{

public:
    KPUClass();
    ~KPUClass();
    int  begin(uint8_t* model);
    void end();
    /**
     * @param imgSrc RGB888 image source
     * @param dmaCh [0,5)
     */
    int  forward(uint8_t* imgSrc, uint8_t dmaCh=0);
    bool isForwardOk();
    int  getResult(uint8_t**data, size_t* count, uint32_t startIndex = 0);


private:
    kpu_model_context_t          _task;
    bool  _flagGotResult;


};


#endif

