#ifndef __MAIX_SPEECH_RECOGNITION_H
#define __MAIX_SPEECH_RECOGNITION_H

#include "Arduino.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sysctl.h"
#include "plic.h"
#include "uarths.h"
#include "util/g_def.h"
#include "i2s.h"

#include "util/VAD.h"
#include "util/MFCC.h"
#include "util/DTW.h"
#include "util/flash.h"
#include "util/ADC.h"


class SpeechRecognizer{
public:
    SpeechRecognizer();
    ~SpeechRecognizer();
    int begin();    //初始化i2s
    int record(uint8_t keyword_num, uint8_t model_num);  //记录关键词 
    int recognize();  //识别，返回关键词号
    int addVoiceModel(uint8_t keyword_num, uint8_t model_num, const int16_t *voice_model, uint16_t frame_num);
    int print_model(uint8_t keyword_num, uint8_t model_num);
private:
    uint8_t save_mdl(uint16_t *v_dat, uint32_t addr);
    uint8_t spch_recg(uint16_t *v_dat, uint32_t *mtch_dis);

private:
    uint8_t comm; //关键词号
    

};

#endif