/**
 * @file MBNet_1000.h
 * @brief Detect object type
 * @author Neucrack@sipeed.com
 */


#ifndef __MBNET_1000_H
#define __MBNET_1000_H

#include "Sipeed_OV2640.h"
#include "Sipeed_ST7789.h"
#include <SD.h>
#include <Maix_KPU.h>

#define KMODEL_SIZE (4220 * 1024)
#define STATISTICS_NUM 5

typedef struct{
    const char* name;
    float       sum;
    bool        updated;
} statistics_t;


class MBNet_1000{

public:
    MBNet_1000(KPUClass& kpu, Sipeed_ST7789& lcd, Sipeed_OV2640& camera);
    ~MBNet_1000();
    int begin(const char* kmodel_name);
    int detect();
    void show();
    const char** _names;

private:
    KPUClass&      _kpu;
    Sipeed_ST7789& _lcd;
    Sipeed_OV2640& _camera;
    uint8_t*       _model;
    size_t         _count;
    statistics_t   _statistics[STATISTICS_NUM];
    float*         _result;
    uint16_t       _index[1000];

    void label_init();
    void label_get(uint16_t index, float* prob, const char** name);
    void label_sort(void);
};


#endif

