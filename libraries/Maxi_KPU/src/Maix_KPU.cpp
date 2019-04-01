

#include "Maix_KPU.h"
#include "sysctl.h"


#define AI_STATUS_IDLE 0
#define AI_STATUS_BUSY 1
#define AI_STATUS_OK   2


static volatile uint32_t g_ai_done_flag = AI_STATUS_IDLE;

KPUClass::KPUClass()
:_flagGotResult(true)
{

}

KPUClass::~KPUClass()
{
    end();
}

int  KPUClass::begin(uint8_t* model)
{
    sysctl_clock_enable(SYSCTL_CLOCK_AI);
    if (kpu_load_kmodel(&_task, model) != 0)
        return KPU_ERROR_BAD_MODEL;
    return KPU_ERROR_NONE;
}

void KPUClass::end()
{
    sysctl_clock_disable(SYSCTL_CLOCK_AI);
}

extern "C" {
    static void ai_done(void* userdata)
    {
        bool* user_used = (bool*)userdata;
        g_ai_done_flag = AI_STATUS_OK;
        *user_used = false;
    }
}

int  KPUClass::forward(uint8_t* imgSrc, uint8_t dmaCh)
{
    if(g_ai_done_flag == AI_STATUS_BUSY)
        return KPU_ERROR_BUSY;
    g_ai_done_flag = AI_STATUS_BUSY;
    if (kpu_run_kmodel(&_task, imgSrc, (dmac_channel_number_t)dmaCh, ai_done, (void*)&_flagGotResult) != 0)
    {
        g_ai_done_flag = AI_STATUS_IDLE;
        return KPU_ERROR_BAD_MODEL;
    }
    return KPU_ERROR_NONE;
}

bool KPUClass::isForwardOk()
{
    return g_ai_done_flag==AI_STATUS_OK;
}

int  KPUClass::getResult(uint8_t**data, size_t* count, uint32_t startIndex )
{
    _flagGotResult = true;
    kpu_get_output(&_task, startIndex, data, count);
    return KPU_ERROR_NONE;
}




