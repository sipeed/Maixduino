
#include "Sipeed_OV2640.h"

//////////// HAL ///////////////
#include "sysctl.h"
#include "fpioa.h"
#include "dvp.h"
#include "sleep.h"
#include "ov2640_regs.h"
#include "stdlib.h"
#include "utils.h"
#include "plic.h"
#include "math.h"
#include "Arduino.h" // millis

volatile static uint8_t g_dvp_finish_flag = 0;

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })


typedef enum {
    ACTIVE_LOW,
    ACTIVE_HIGH,
    ACTIVE_BINOCULAR,
} polarity_t;

#define DCMI_RESET_LOW()      dvp->cmos_cfg &= ~DVP_CMOS_RESET
#define DCMI_RESET_HIGH()     dvp->cmos_cfg |= DVP_CMOS_RESET
#define DCMI_PWDN_LOW()       dvp->cmos_cfg |= DVP_CMOS_POWER_DOWN
#define DCMI_PWDN_HIGH()      dvp->cmos_cfg &= ~DVP_CMOS_POWER_DOWN

#define SVGA_HSIZE     (800)
#define SVGA_VSIZE     (600)

#define UXGA_HSIZE     (1600)
#define UXGA_VSIZE     (1200)
static const uint8_t ov2640_default[][2] = { //k210 
	{0xff, 0x01},
	{0x12, 0x80},
	{0xff, 0x00},
	{0x2c, 0xff},
	{0x2e, 0xdf},
	{0xff, 0x01},
	{0x3c, 0x32},
	{0x11, 0x00},
	{0x09, 0x02},
	{0x04, 0xa8},//v flip and h mirro
	{0x13, 0xe5},
	{0x14, 0x48},
	{0x2c, 0x0c},
	{0x33, 0x78},
	{0x3a, 0x33},
	{0x3b, 0xfb},
	{0x3e, 0x00},
	{0x43, 0x11},
	{0x16, 0x10},
	{0x39, 0x92},//test 92
	{0x23, 0x00},
	{0x36, 0x1a},
	{0x07, 0xc0},
	{0x4c, 0x00},
	{0x48, 0x00},
	{0x5b, 0x00},
	{0x4a, 0x81},
	{0x21, 0x99},
	{0x24, 0x40},
	{0x25, 0x38},
	{0x26, 0x82},
	{0x5c, 0x00},
	{0x63, 0x00},
	{0x46, 0x22},
	{0x0c, 0x3c},
	{0x61, 0x70},
	{0x62, 0x80},
	{0x7c, 0x05},
	{0x20, 0x80},
	{0x28, 0x30},
	{0x6c, 0x00},
	{0x6d, 0x80},
	{0x6e, 0x00},
	{0x70, 0x02},
	{0x71, 0x94},
	{0x73, 0xc1},
	{0x5a, 0x57},
	{0x37, 0xc0},
	{0x4f, 0xca},
	{0x50, 0xa8},
	{0x5a, 0x23},
	{0x6d, 0x00},
	{0x3d, 0x38},
	{0xff, 0x00},
	{0xe5, 0x7f},
	{0xf9, 0xc0},
	{0x41, 0x24},
	{0xe0, 0x14},
	{0x76, 0xff},
	{0x33, 0xa0},
	{0x42, 0x20},
	{0x43, 0x18},
	{0x4c, 0x00},
	{0x87, 0xd5},
	{0x88, 0x3f},
	{0xd7, 0x03},//[pixformat]:
	{0xd9, 0x10},
	{0xd3, 0x82},
	{0xc8, 0x08},
	{0xc9, 0x80},
	{0x7c, 0x00},
	{0x7d, 0x00},
	{0x7c, 0x03},
	{0x7d, 0x48},
	{0x7d, 0x48},
	{0x7c, 0x08},
	{0x7d, 0x20},
	{0x7d, 0x10},
	{0x7d, 0x0e},
	{0x90, 0x00},
	{0x91, 0x0e},
	{0x91, 0x1a},
	{0x91, 0x31},
	{0x91, 0x5a},
	{0x91, 0x69},
	{0x91, 0x75},
	{0x91, 0x7e},
	{0x91, 0x88},
	{0x91, 0x8f},
	{0x91, 0x96},
	{0x91, 0xa3},
	{0x91, 0xaf},
	{0x91, 0xc4},
	{0x91, 0xd7},
	{0x91, 0xe8},
	{0x91, 0x20},
	{0x92, 0x00},
	{0x93, 0x06},
	{0x93, 0xe3},
	{0x93, 0x05},
	{0x93, 0x05},
	{0x93, 0x00},
	{0x93, 0x04},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x93, 0x00},
	{0x96, 0x00},
	{0x97, 0x08},
	{0x97, 0x19},
	{0x97, 0x02},
	{0x97, 0x0c},
	{0x97, 0x24},
	{0x97, 0x30},
	{0x97, 0x28},
	{0x97, 0x26},
	{0x97, 0x02},
	{0x97, 0x98},
	{0x97, 0x80},
	{0x97, 0x00},
	{0x97, 0x00},
	{0xc3, 0xed},
	{0xa4, 0x00},
	{0xa8, 0x00},
	{0xc5, 0x11},
	{0xc6, 0x51},
	{0xbf, 0x80},
	{0xc7, 0x10},
	{0xb6, 0x66},
	{0xb8, 0xa5},
	{0xb7, 0x64},
	{0xb9, 0x7c},
	{0xb3, 0xaf},
	{0xb4, 0x97},
	{0xb5, 0xff},
	{0xb0, 0xc5},
	{0xb1, 0x94},
	{0xb2, 0x0f},
	{0xc4, 0x5c},
	{0x5a, 0xc8},
	{0x5b, 0x96},
	{0x5c, 0x00},
	{0xc3, 0xed},
	{0x7f, 0x00},
	{0xda, 0x08},//pixformat
	{0xe5, 0x1f},
	{0xe1, 0x67},//pixformat
	{0xe0, 0x00},
	{0xdd, 0x7f},
	{0x05, 0x00},
#if 1	//color bar
	{0xff, 0x01},
	{0x12, 0x02},
#endif
	{0x00, 0x00}

};
static const uint8_t svga_config[][2] = { //k210 
	{0xff, 0x01},//bank sel
	{0x35, 0xda},//[SVGA]:
	{0x22, 0x1a},//[SVGA]:
	{0x37, 0xc3},//[SVGA]:
	{0x34, 0xc0},//[SVGA]:
	{0x06, 0x88},//[SVGA]:
	{0x0d, 0x87},//[SVGA]:
	{0x0e, 0x41},//[SVGA]:
	{0x42, 0x03},//[SVGA]:
	{0x3d, 0x34},//[SVGA]:
	{0x12, 0x40},//[SVGA]:  COM7,COM7_RES_SVGA  SVGA
	{0x03, 0x0f},//[SVGA]:  COM1,0x0F  
	{0x17, 0x11},//[SVGA]:HSTART
	{0x18, 0x43},//[SVGA]:HSTOP
	{0x19, 0x00},//[SVGA]:VSTART
	{0x1a, 0x4b},//[SVGA]:VSTOP
	{0x32, 0x09},//[SVGA]:REG32
	
	{0xff, 0x00},//bank sel
	{0xc0, 0x64},//[SVGA]:HSIZE8 SVGA_HSIZE>>3
	{0xc1, 0x4b},//[SVGA]:VSIZE8 SVGA_VSIZE>>3
	{0x8c, 0x00},//[SVGA]:SIZEL
	{0x86, 0x3d},//[SVGA]:
	{0x50, 0x00},//[SVGA]:CTRLI
	{0x51, 0xc8},//[SVGA]:HSIZE
	{0x52, 0x96},//[SVGA]:VSIZE
	{0x53, 0x00},//[SVGA]:XOFFL
	{0x54, 0x00},//[SVGA]:YOFFL
	{0x55, 0x00},//[SVGA]:VHYX
	{0xd3, 0x02},//[SVGA]:R_DVP_SP	
};



static const uint8_t default_regs[][2] = {
    { BANK_SEL, BANK_SEL_DSP },
    { 0x2c,     0xff },
    { 0x2e,     0xdf },
    { BANK_SEL, BANK_SEL_SENSOR },
    { 0x3c,     0x32 },
    { CLKRC,    0x80 }, /* Set PCLK divider */
    { COM2,     COM2_OUT_DRIVE_3x }, /* Output drive x2 */
#ifdef OPENMV2
    { REG04,    0xF8}, /* Mirror/VFLIP/AEC[1:0] */
#else
    { REG04_SET(REG04_HREF_EN)},
#endif
    { COM8,     COM8_SET(COM8_BNDF_EN | COM8_AGC_EN | COM8_AEC_EN) },
    { COM9,     COM9_AGC_SET(COM9_AGC_GAIN_8x)},
    { 0x2c,     0x0c },
    { 0x33,     0x78 },
    { 0x3a,     0x33 },
    { 0x3b,     0xfb },
    { 0x3e,     0x00 },
    { 0x43,     0x11 },
    { 0x16,     0x10 },
    { 0x39,     0x02 },
    { 0x35,     0x88 },
    { 0x22,     0x0a },
    { 0x37,     0x40 },
    { 0x23,     0x00 },
    { ARCOM2,   0xa0 },
    { 0x06,     0x02 },
    { 0x06,     0x88 },
    { 0x07,     0xc0 },
    { 0x0d,     0xb7 },
    { 0x0e,     0x01 },
    { 0x4c,     0x00 },
    { 0x4a,     0x81 },
    { 0x21,     0x99 },
    { AEW,      0x40 },
    { AEB,      0x38 },
    /* AGC/AEC fast mode operating region */
    { VV,       VV_AGC_TH_SET(0x08, 0x02) },
    { COM19,    0x00 }, /* Zoom control 2 MSBs */
    { ZOOMS,    0x00 }, /* Zoom control 8 MSBs */
    { 0x5c,     0x00 },
    { 0x63,     0x00 },
    { FLL,      0x00 },
    { FLH,      0x00 },

    /* Set banding filter */
    { COM3,     COM3_BAND_SET(COM3_BAND_AUTO) },
    { REG5D,    0x55 },
    { REG5E,    0x7d },
    { REG5F,    0x7d },
    { REG60,    0x55 },
    { HISTO_LOW,   0x70 },
    { HISTO_HIGH,  0x80 },
    { 0x7c,     0x05 },
    { 0x20,     0x80 },
    { 0x28,     0x30 },
    { 0x6c,     0x00 },
    { 0x6d,     0x80 },
    { 0x6e,     0x00 },
    { 0x70,     0x02 },
    { 0x71,     0x94 },
    { 0x73,     0xc1 },
    { 0x3d,     0x34 },
    //{ COM7,   COM7_RES_UXGA | COM7_ZOOM_EN },
    { 0x5a,     0x57 },
    { BD50,     0xbb },
    { BD60,     0x9c },

    { BANK_SEL, BANK_SEL_DSP },
    { 0xe5,     0x7f },
    { MC_BIST,  MC_BIST_RESET | MC_BIST_BOOT_ROM_SEL },
    { 0x41,     0x24 },
    { RESET,    RESET_JPEG | RESET_DVP },
    { 0x76,     0xff },
    { 0x33,     0xa0 },
    { 0x42,     0x20 },
    { 0x43,     0x18 },
    { 0x4c,     0x00 },
    { CTRL3,    CTRL3_BPC_EN | CTRL3_WPC_EN | 0x10 },
    { 0x88,     0x3f },
    { 0xd7,     0x03 },
    { 0xd9,     0x10 },
    { R_DVP_SP , R_DVP_SP_AUTO_MODE | 0x2 },
    { 0xc8,     0x08 },
    { 0xc9,     0x80 },
    { BPADDR,   0x00 },
    { BPDATA,   0x00 },
    { BPADDR,   0x03 },
    { BPDATA,   0x48 },
    { BPDATA,   0x48 },
    { BPADDR,   0x08 },
    { BPDATA,   0x20 },
    { BPDATA,   0x10 },
    { BPDATA,   0x0e },
    { 0x90,     0x00 },
    { 0x91,     0x0e },
    { 0x91,     0x1a },
    { 0x91,     0x31 },
    { 0x91,     0x5a },
    { 0x91,     0x69 },
    { 0x91,     0x75 },
    { 0x91,     0x7e },
    { 0x91,     0x88 },
    { 0x91,     0x8f },
    { 0x91,     0x96 },
    { 0x91,     0xa3 },
    { 0x91,     0xaf },
    { 0x91,     0xc4 },
    { 0x91,     0xd7 },
    { 0x91,     0xe8 },
    { 0x91,     0x20 },
    { 0x92,     0x00 },
    { 0x93,     0x06 },
    { 0x93,     0xe3 },
    { 0x93,     0x03 },
    { 0x93,     0x03 },
    { 0x93,     0x00 },
    { 0x93,     0x02 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x93,     0x00 },
    { 0x96,     0x00 },
    { 0x97,     0x08 },
    { 0x97,     0x19 },
    { 0x97,     0x02 },
    { 0x97,     0x0c },
    { 0x97,     0x24 },
    { 0x97,     0x30 },
    { 0x97,     0x28 },
    { 0x97,     0x26 },
    { 0x97,     0x02 },
    { 0x97,     0x98 },
    { 0x97,     0x80 },
    { 0x97,     0x00 },
    { 0x97,     0x00 },
    { 0xa4,     0x00 },
    { 0xa8,     0x00 },
    { 0xc5,     0x11 },
    { 0xc6,     0x51 },
    { 0xbf,     0x80 },
    { 0xc7,     0x10 },
    { 0xb6,     0x66 },
    { 0xb8,     0xA5 },
    { 0xb7,     0x64 },
    { 0xb9,     0x7C },
    { 0xb3,     0xaf },
    { 0xb4,     0x97 },
    { 0xb5,     0xFF },
    { 0xb0,     0xC5 },
    { 0xb1,     0x94 },
    { 0xb2,     0x0f },
    { 0xc4,     0x5c },
    { 0xa6,     0x00 },
    { 0xa7,     0x20 },
    { 0xa7,     0xd8 },
    { 0xa7,     0x1b },
    { 0xa7,     0x31 },
    { 0xa7,     0x00 },
    { 0xa7,     0x18 },
    { 0xa7,     0x20 },
    { 0xa7,     0xd8 },
    { 0xa7,     0x19 },
    { 0xa7,     0x31 },
    { 0xa7,     0x00 },
    { 0xa7,     0x18 },
    { 0xa7,     0x20 },
    { 0xa7,     0xd8 },
    { 0xa7,     0x19 },
    { 0xa7,     0x31 },
    { 0xa7,     0x00 },
    { 0xa7,     0x18 },
    { 0x7f,     0x00 },
    { 0xe5,     0x1f },
    { 0xe1,     0x77 },
    { 0xdd,     0x7f },
    { CTRL0,    CTRL0_YUV422 | CTRL0_YUV_EN | CTRL0_RGB_EN },
    { 0x00,     0x00 }
};


static const uint8_t svga_regs[][2] = {
        { BANK_SEL, BANK_SEL_SENSOR },
        /* DSP input image resoultion and window size control */
        { COM7,    COM7_RES_SVGA},
        { COM1,    0x0F }, /* UXGA=0x0F, SVGA=0x0A, CIF=0x06 */
        { REG32,   0x09 }, /* UXGA=0x36, SVGA/CIF=0x09 */

        { HSTART,  0x11 }, /* UXGA=0x11, SVGA/CIF=0x11 */
        { HSTOP,   0x43 }, /* UXGA=0x75, SVGA/CIF=0x43 */

        { VSTART,  0x00 }, /* UXGA=0x01, SVGA/CIF=0x00 */
        { VSTOP,   0x4b }, /* UXGA=0x97, SVGA/CIF=0x4b */
        { 0x3d,    0x38 }, /* UXGA=0x34, SVGA/CIF=0x38 */

        { 0x35,    0xda },
        { 0x22,    0x1a },
        { 0x37,    0xc3 },
        { 0x34,    0xc0 },
        { 0x06,    0x88 },
        { 0x0d,    0x87 },
        { 0x0e,    0x41 },
        { 0x42,    0x03 },

        /* Set DSP input image size and offset.
           The sensor output image can be scaled with OUTW/OUTH */
        { BANK_SEL, BANK_SEL_DSP },
        { R_BYPASS, R_BYPASS_DSP_BYPAS },

        { RESET,   RESET_DVP },
        { HSIZE8,  (SVGA_HSIZE>>3)}, /* Image Horizontal Size HSIZE[10:3] */
        { VSIZE8,  (SVGA_VSIZE>>3)}, /* Image Vertiacl Size VSIZE[10:3] */

        /* {HSIZE[11], HSIZE[2:0], VSIZE[2:0]} */
        { SIZEL,   ((SVGA_HSIZE>>6)&0x40) | ((SVGA_HSIZE&0x7)<<3) | (SVGA_VSIZE&0x7)},

        { XOFFL,   0x00 }, /* OFFSET_X[7:0] */
        { YOFFL,   0x00 }, /* OFFSET_Y[7:0] */
        { HSIZE,   ((SVGA_HSIZE>>2)&0xFF) }, /* H_SIZE[7:0]= HSIZE/4 */
        { VSIZE,   ((SVGA_VSIZE>>2)&0xFF) }, /* V_SIZE[7:0]= VSIZE/4 */

        /* V_SIZE[8]/OFFSET_Y[10:8]/H_SIZE[8]/OFFSET_X[10:8] */
        { VHYX,    ((SVGA_VSIZE>>3)&0x80) | ((SVGA_HSIZE>>7)&0x08) },
        { TEST,    (SVGA_HSIZE>>4)&0x80}, /* H_SIZE[9] */

        { CTRL2,   CTRL2_DCW_EN | CTRL2_SDE_EN |
          CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN },

        /* H_DIVIDER/V_DIVIDER */
        { CTRLI,   CTRLI_LP_DP | 0x00},
        /* DVP prescalar */
        { R_DVP_SP, R_DVP_SP_AUTO_MODE},

        { R_BYPASS, R_BYPASS_DSP_EN },
        { RESET,    0x00 },
        {0, 0},
};

static const uint8_t uxga_regs[][2] = {
        { BANK_SEL, BANK_SEL_SENSOR },
        /* DSP input image resoultion and window size control */
        { COM7,    COM7_RES_UXGA},
        { COM1,    0x0F }, /* UXGA=0x0F, SVGA=0x0A, CIF=0x06 */
        { REG32,   0x36 }, /* UXGA=0x36, SVGA/CIF=0x09 */

        { HSTART,  0x11 }, /* UXGA=0x11, SVGA/CIF=0x11 */
        { HSTOP,   0x75 }, /* UXGA=0x75, SVGA/CIF=0x43 */

        { VSTART,  0x01 }, /* UXGA=0x01, SVGA/CIF=0x00 */
        { VSTOP,   0x97 }, /* UXGA=0x97, SVGA/CIF=0x4b */
        { 0x3d,    0x34 }, /* UXGA=0x34, SVGA/CIF=0x38 */

        { 0x35,    0x88 },
        { 0x22,    0x0a },
        { 0x37,    0x40 },
        { 0x34,    0xa0 },
        { 0x06,    0x02 },
        { 0x0d,    0xb7 },
        { 0x0e,    0x01 },
        { 0x42,    0x83 },

        /* Set DSP input image size and offset.
           The sensor output image can be scaled with OUTW/OUTH */
        { BANK_SEL, BANK_SEL_DSP },
        { R_BYPASS, R_BYPASS_DSP_BYPAS },

        { RESET,   RESET_DVP },
        { HSIZE8,  (UXGA_HSIZE>>3)}, /* Image Horizontal Size HSIZE[10:3] */
        { VSIZE8,  (UXGA_VSIZE>>3)}, /* Image Vertiacl Size VSIZE[10:3] */

        /* {HSIZE[11], HSIZE[2:0], VSIZE[2:0]} */
        { SIZEL,   ((UXGA_HSIZE>>6)&0x40) | ((UXGA_HSIZE&0x7)<<3) | (UXGA_VSIZE&0x7)},

        { XOFFL,   0x00 }, /* OFFSET_X[7:0] */
        { YOFFL,   0x00 }, /* OFFSET_Y[7:0] */
        { HSIZE,   ((UXGA_HSIZE>>2)&0xFF) }, /* H_SIZE[7:0] real/4 */
        { VSIZE,   ((UXGA_VSIZE>>2)&0xFF) }, /* V_SIZE[7:0] real/4 */

        /* V_SIZE[8]/OFFSET_Y[10:8]/H_SIZE[8]/OFFSET_X[10:8] */
        { VHYX,    ((UXGA_VSIZE>>3)&0x80) | ((UXGA_HSIZE>>7)&0x08) },
        { TEST,    (UXGA_HSIZE>>4)&0x80}, /* H_SIZE[9] */

        { CTRL2,   CTRL2_DCW_EN | CTRL2_SDE_EN |
            CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN },

        /* H_DIVIDER/V_DIVIDER */
        { CTRLI,   CTRLI_LP_DP | 0x00},
        /* DVP prescalar */
        { R_DVP_SP, R_DVP_SP_AUTO_MODE | 0x04},

        { R_BYPASS, R_BYPASS_DSP_EN },
        { RESET,    0x00 },
        {0, 0},
};

static const uint8_t yuv422_regs[][2] = {
        { BANK_SEL, BANK_SEL_DSP },
        { RESET,   RESET_DVP},
        { 0xD7,     0x01 },
        { IMAGE_MODE, IMAGE_MODE_YUV422 },
        { 0xE1,     0x67 },
        { RESET,    0x00 },
        {0, 0},
};

static const uint8_t rgb565_regs[][2] = {
        { BANK_SEL,   BANK_SEL_DSP },
        { RESET,      RESET_DVP},
        { 0xD7,       0x03},
        { IMAGE_MODE, IMAGE_MODE_RGB565 },
        { 0xE1,       0x77 },
        { RESET,      0x00 },
        {0,           0},
};

static const uint8_t jpeg_regs[][2] = {
        { BANK_SEL, BANK_SEL_DSP },
        { RESET,   RESET_DVP},
        { IMAGE_MODE, IMAGE_MODE_JPEG_EN|IMAGE_MODE_RGB565 },
        { 0xD7,     0x03 },
        { 0xE1,     0x77 },
        { QS,       0x0C },
        { RESET,    0x00 },
        {0, 0},
};

#define NUM_BRIGHTNESS_LEVELS (5)
static const uint8_t brightness_regs[NUM_BRIGHTNESS_LEVELS + 1][5] = {
    { BPADDR, BPDATA, BPADDR, BPDATA, BPDATA },
    { 0x00, 0x04, 0x09, 0x00, 0x00 }, /* -2 */
    { 0x00, 0x04, 0x09, 0x10, 0x00 }, /* -1 */
    { 0x00, 0x04, 0x09, 0x20, 0x00 }, /*  0 */
    { 0x00, 0x04, 0x09, 0x30, 0x00 }, /* +1 */
    { 0x00, 0x04, 0x09, 0x40, 0x00 }, /* +2 */
};

#define NUM_CONTRAST_LEVELS (5)
static const uint8_t contrast_regs[NUM_CONTRAST_LEVELS + 1][7] = {
    { BPADDR, BPDATA, BPADDR, BPDATA, BPDATA, BPDATA, BPDATA },
    { 0x00, 0x04, 0x07, 0x20, 0x18, 0x34, 0x06 }, /* -2 */
    { 0x00, 0x04, 0x07, 0x20, 0x1c, 0x2a, 0x06 }, /* -1 */
    { 0x00, 0x04, 0x07, 0x20, 0x20, 0x20, 0x06 }, /*  0 */
    { 0x00, 0x04, 0x07, 0x20, 0x24, 0x16, 0x06 }, /* +1 */
    { 0x00, 0x04, 0x07, 0x20, 0x28, 0x0c, 0x06 }, /* +2 */
};

#define NUM_SATURATION_LEVELS (5)
static const uint8_t saturation_regs[NUM_SATURATION_LEVELS + 1][5] = {
    { BPADDR, BPDATA, BPADDR, BPDATA, BPDATA },
    { 0x00, 0x02, 0x03, 0x28, 0x28 }, /* -2 */
    { 0x00, 0x02, 0x03, 0x38, 0x38 }, /* -1 */
    { 0x00, 0x02, 0x03, 0x48, 0x48 }, /*  0 */
    { 0x00, 0x02, 0x03, 0x58, 0x58 }, /* +1 */
    { 0x00, 0x02, 0x03, 0x58, 0x58 }, /* +2 */
};



Sipeed_OV2640::Sipeed_OV2640( framesize_t frameSize, pixformat_t pixFormat)
:Camera(frameSize, pixFormat),
_dataBuffer(NULL), _aiBuffer(NULL),
_resetPolarity(ACTIVE_HIGH), _pwdnPolarity(ACTIVE_HIGH),
_slaveAddr(0x00),
_id(0)
{
    configASSERT(pixFormat == PIXFORMAT_RGB565 || pixFormat==PIXFORMAT_YUV422);
}



Sipeed_OV2640::Sipeed_OV2640(uint16_t width, uint16_t height, pixformat_t pixFormat)
:Camera(width, height, pixFormat),
_dataBuffer(NULL), _aiBuffer(NULL),
_resetPolarity(ACTIVE_HIGH), _pwdnPolarity(ACTIVE_HIGH),
_slaveAddr(0x00),
_id(0)
{
    configASSERT(pixFormat == PIXFORMAT_RGB565 || pixFormat==PIXFORMAT_YUV422);
}


Sipeed_OV2640::~Sipeed_OV2640()
{
    end();
}

bool Sipeed_OV2640::begin()
{
    if(_dataBuffer)
        free(_dataBuffer);
    if(_aiBuffer)
        free(_aiBuffer);
    _dataBuffer = (uint8_t*)malloc(_width*_height*2); //RGB565
    if(!_dataBuffer)
    {
        _width = 0;
        _height = 0;
        return false;
    }
    _aiBuffer = (uint8_t*)malloc(_width*_height*3);   //RGB888
    if(!_aiBuffer)
    {
        _width = 0;
        _height = 0;
        free(_dataBuffer);
        return false;
    }
    if(!reset())
        return false;
    if( !setPixFormat(_pixFormat))
        return false;
    if(!setFrameSize(_frameSize))
        return false;
    return true;
}

void Sipeed_OV2640::end()
{
    if(_dataBuffer)
        free(_dataBuffer);
    if(_aiBuffer)
        free(_aiBuffer);
    _dataBuffer = nullptr;
    _aiBuffer   = nullptr;
}

bool Sipeed_OV2640::reset()
{
    if(dvpInit() != 0)
        return false;
    if(ov2640_reset() != 0)
        return false;
    if(dvpInitIrq() != 0)
        return false;
    return true;
}

bool Sipeed_OV2640::setPixFormat(pixformat_t pixFormat)
{
    if(ov2640_set_pixformat(pixFormat) != 0)
        return false;
    return true;
}

bool Sipeed_OV2640::setFrameSize(framesize_t frameSize)
{
    if(ov2640_set_framesize(frameSize) != 0)
        return false;
    return true;
}  

bool Sipeed_OV2640::run(bool run)
{
	if(run)
	{
		dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
		plic_irq_enable(IRQN_DVP_INTERRUPT);
		dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
	}
	else{
		plic_irq_disable(IRQN_DVP_INTERRUPT);
		dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
		dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
	}
    return true;
}

int Sipeed_OV2640::id()
{
    return _id;
}

/**
 * @return pixels 
 *         If pixels format is RGB565: return RGB565 pixels with every uint16_t one pixel, e.g. RED: 0xF800
 */
uint8_t* Sipeed_OV2640::snapshot()
{
    if ( sensor_snapshot() != 0)
        return nullptr;
    return _dataBuffer;
}

void Sipeed_OV2640::setRotation(uint8_t rotation)
{
    //FIXME
}

void Sipeed_OV2640::setInvert(bool invert)
{
    //FIXME
    ov2640_set_hmirror(!invert); 
    //ov2640_set_vflip(1);
    return;
}



int Sipeed_OV2640::dvpInit(uint32_t freq)
{
    // just support RGB565 and YUV422 on k210
    configASSERT(_pixFormat==PIXFORMAT_RGB565 || _pixFormat==PIXFORMAT_YUV422);
    _freq  = freq;

	fpioa_set_function(47, FUNC_CMOS_PCLK);
	fpioa_set_function(46, FUNC_CMOS_XCLK);
	fpioa_set_function(45, FUNC_CMOS_HREF);
	fpioa_set_function(44, FUNC_CMOS_PWDN);
	fpioa_set_function(43, FUNC_CMOS_VSYNC);
	fpioa_set_function(42, FUNC_CMOS_RST);
	fpioa_set_function(41, FUNC_SCCB_SCLK);
	fpioa_set_function(40, FUNC_SCCB_SDA);

    /* Do a power cycle */
    DCMI_PWDN_HIGH();
    msleep(10);

    DCMI_PWDN_LOW();
    msleep(10);

    // Initialize the camera bus, 8bit reg
    dvp_init(8);
	 // Initialize dvp interface
	dvp_set_xclk_rate(freq);
	dvp->cmos_cfg |= DVP_CMOS_CLK_DIV(3) | DVP_CMOS_CLK_ENABLE;
	dvp_enable_burst();
	dvp_disable_auto();
	dvp_set_output_enable(DVP_OUTPUT_AI, 1);	//enable to AI
	dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 1);	//enable to lcd
    if( _pixFormat == PIXFORMAT_YUV422)
        dvp_set_image_format(DVP_CFG_YUV_FORMAT);
    else
	    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
	dvp_set_image_size(_width, _height);	//set QVGA default
	dvp_set_ai_addr( (uint32_t)((long)_aiBuffer), (uint32_t)((long)(_aiBuffer+_width*_height)), (uint32_t)((long)(_aiBuffer+_width*_height*2)));
	dvp_set_display_addr( (uint32_t)((long)_dataBuffer) );

    if(0 == sensor_ov_detect()){//find ov sensor
        // printf("find ov sensor\n");
    }
    else if(0 == sensor_gc_detect()){//find gc0328 sensor
        // printf("find gc3028\n");
    }

    return 0;
}

int Sipeed_OV2640::cambus_scan()
{

	uint16_t manuf_id = 0;
	uint16_t device_id = 0;
    for (uint8_t addr=0x08; addr<=0x77; addr++) {
		cambus_read_id(addr ,&manuf_id,&device_id);
		if(0xffff != device_id)
		{
			return addr ;
		}
    }
    return 0;
}

int Sipeed_OV2640::cambus_read_id(uint8_t addr,uint16_t *manuf_id, uint16_t *device_id)
{
	dvp_sccb_send_data(addr, 0xFF, 0x01);
	*manuf_id = (dvp_sccb_receive_data(addr, 0x1C) << 8) | dvp_sccb_receive_data(addr, 0x1D);
	*device_id = (dvp_sccb_receive_data(addr, 0x0A) << 8) | dvp_sccb_receive_data(addr, 0x0B);
	return 0;
}

int Sipeed_OV2640::cambus_scan_gc0328(void)
{
    dvp_sccb_send_data(GC0328_ADDR, 0xFE, 0x00);
    uint8_t id = dvp_sccb_receive_data(GC0328_ADDR, 0xf0);
    if (id != 0x9d)
    {
        // printf("error gc0328 detect, ret id is 0x%x\r\n", id);
        return 0;
    }
    return id;
}

int Sipeed_OV2640::cambus_readb(uint8_t slv_addr, uint8_t reg_addr, uint8_t *reg_data)
{

    int ret = 0;
	*reg_data = dvp_sccb_receive_data(slv_addr, reg_addr);

	if(0xff == *reg_data)
		ret = -1;

    return ret;

}


int Sipeed_OV2640::cambus_writeb(uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data)
{

	dvp_sccb_send_data(slv_addr,reg_addr,reg_data);
	return 0;
}

int Sipeed_OV2640::cambus_readw(uint8_t slv_addr, uint8_t reg_addr, uint16_t *reg_data)
{
    return 0;
}

int Sipeed_OV2640::cambus_writew(uint8_t slv_addr, uint8_t reg_addr, uint16_t reg_data)
{
    return 0;
}

int Sipeed_OV2640::cambus_readw2(uint8_t slv_addr, uint16_t reg_addr, uint16_t *reg_data)
{
    return 0;
}

int Sipeed_OV2640::cambus_writew2(uint8_t slv_addr, uint16_t reg_addr, uint16_t reg_data)
{
    return 0;
}



int Sipeed_OV2640::sensor_ov_detect()
{
    /* Reset the sensor */
    DCMI_RESET_HIGH();
    msleep(10);

    DCMI_RESET_LOW();
    msleep(10);

    /* Probe the ov sensor */
    _slaveAddr = cambus_scan();
    if (_slaveAddr == 0) {
        /* Sensor has been held in reset,
           so the reset line is active low */
        _resetPolarity = ACTIVE_LOW;

        /* Pull the sensor out of the reset state,systick_sleep() */
        DCMI_RESET_HIGH();
        msleep(10);

        /* Probe again to set the slave addr */
        _slaveAddr = cambus_scan();
        if (_slaveAddr == 0) {
            _pwdnPolarity = ACTIVE_LOW;

            DCMI_PWDN_HIGH();
            msleep(10);

            _slaveAddr = cambus_scan();
            if (_slaveAddr == 0) {
                _resetPolarity = ACTIVE_HIGH;

                DCMI_RESET_LOW();
                msleep(10);

                _slaveAddr = cambus_scan();
                if(_slaveAddr == 0) {
                    //should do something?
                    return -2;
                }
            }
        }
    }

    // Clear sensor chip ID.
    _id = 0;

    if (_slaveAddr == LEPTON_ID) {
        _id = LEPTON_ID;
		/*set LEPTON xclk rate*/
		/*lepton_init*/
    } else {
        // Read ON semi sensor ID.
        cambus_readb(_slaveAddr, ON_CHIP_ID, &_id);
        if (_id == MT9V034_ID) {
			/*set MT9V034 xclk rate*/
			/*mt9v034_init*/
        } else { // Read OV sensor ID.
            cambus_readb(_slaveAddr, OV_CHIP_ID, &_id);
            // Initialize sensor struct.
            switch (_id) {
                case OV9650_ID:
					/*ov9650_init*/
                    break;
                case OV2640_ID:
                    // printf("detect ov2640, id:%x\n", _slaveAddr);
                    break;
                case OV7725_ID:
					/*ov7725_init*/
                    break;
                default:
                    // Sensor is not supported.
                    return -3;
            }
        }
    }

    // if (init_ret != 0 ) {
    //     // Sensor init failed.
    //     return -4;
    // }
    return 0;
}

int Sipeed_OV2640::sensor_gc_detect()
{
    DCMI_PWDN_HIGH();//enable gc0328 要恢复 normal 工作模式，需将 PWDN pin 接入低电平即可，同时写入初始化寄存器即可
    DCMI_RESET_LOW();//reset gc3028
    msleep(10);
    DCMI_RESET_HIGH();
    msleep(10);
    uint8_t id = cambus_scan_gc0328();
    if(0 == id)
    {
        return -3;
    }
    else
    {
        // printf("[MAIXPY]: gc0328 id = %x\n",id); 
        _slaveAddr = GC0328_ADDR;
        _id = id;
    }
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

static int sensor_irq(void *ctx)
{
	if (dvp_get_interrupt(DVP_STS_FRAME_FINISH)) {	//frame end
		dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
		g_dvp_finish_flag = 1;
	} else {	//frame start
        if(g_dvp_finish_flag == 0)  //only we finish the convert, do transmit again
            dvp_start_convert();	//so we need deal img ontime, or skip one framebefore next
		dvp_clear_interrupt(DVP_STS_FRAME_START);
	}

	return 0;
}

#ifdef __cplusplus
}
#endif



int Sipeed_OV2640::dvpInitIrq()
{
	dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
	plic_set_priority(IRQN_DVP_INTERRUPT, 2);
    /* set irq handle */
	plic_irq_register(IRQN_DVP_INTERRUPT, sensor_irq, (void*)NULL);

	plic_irq_disable(IRQN_DVP_INTERRUPT);
	dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
	dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

	return 0;
}




int Sipeed_OV2640::ov2640_reset()
{
    int i=0;
    const uint8_t (*regs)[2];

    /* Reset all registers */
    cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_SENSOR);
    cambus_writeb(_slaveAddr, COM7, COM7_SRST);

    /* delay n ms */
    msleep(10);

    i = 0;
    regs = ov2640_default;//default_regs,ov2640_default
    /* Write initial regsiters */
    while (regs[i][0]) {
        cambus_writeb(_slaveAddr, regs[i][0], regs[i][1]);
        i++;
    }
    i = 0;
    regs = svga_config;//svga_regs,svga_config
    /* Write DSP input regsiters */
    while (regs[i][0]) {
        cambus_writeb(_slaveAddr, regs[i][0], regs[i][1]);
        i++;
    }

    return 0;
}

int Sipeed_OV2640::ov2640_read_reg(uint8_t reg_addr)
{
    uint8_t reg_data;
    if (cambus_readb(_slaveAddr, reg_addr, &reg_data) != 0) {
        return -1;
    }
    return reg_data;
}

int Sipeed_OV2640::ov2640_write_reg(uint8_t reg_addr, uint8_t reg_data)
{
    return cambus_writeb(_slaveAddr, reg_addr, reg_data);
}


int Sipeed_OV2640::ov2640_set_pixformat(pixformat_t pixformat)
{
    int i=0;
    const uint8_t (*regs)[2]=NULL;

    /* read pixel format reg */
    switch (pixformat) {
        case PIXFORMAT_RGB565:
            regs = rgb565_regs;
            break;
        case PIXFORMAT_YUV422:
        case PIXFORMAT_GRAYSCALE:
            regs = yuv422_regs;
            break;
        case PIXFORMAT_JPEG:
            regs = jpeg_regs;

            break;
        default:
            return -1;
    }

    /* Write initial regsiters */
    while (regs[i][0]) {
        cambus_writeb(_slaveAddr, regs[i][0], regs[i][1]);
        i++;
    }
    switch (pixformat) {
        case PIXFORMAT_RGB565:
			dvp_set_image_format(DVP_CFG_RGB_FORMAT);
            break;
        case PIXFORMAT_YUV422:
            dvp_set_image_format(DVP_CFG_YUV_FORMAT);
            break;
        case PIXFORMAT_GRAYSCALE:
			dvp_set_image_format(DVP_CFG_Y_FORMAT);
            break;
        case PIXFORMAT_JPEG:
			dvp_set_image_format(DVP_CFG_RGB_FORMAT);
            break;
        default:
            return -1;
    }
    /* delay n ms */
    msleep(30);
    return 0;
}

int Sipeed_OV2640::ov2640_set_framesize(framesize_t framesize)
{
    int ret=0;
    uint8_t clkrc;
    uint16_t w = _width;
    uint16_t h = _height;

    int i=0;
    const uint8_t (*regs)[2];

    if ((w <= 800) && (h <= 600)) {
        clkrc =0x80;
        regs = svga_config;
//		regs = ov2640_config;
    } else {
        clkrc =0x81;
        regs = uxga_regs;
    }

    /* Disable DSP */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);
    ret |= cambus_writeb(_slaveAddr, R_BYPASS, R_BYPASS_DSP_BYPAS);

    /* Set CLKRC */
	if(clkrc == 0x81)
	{
	    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_SENSOR);
	    ret |= cambus_writeb(_slaveAddr, CLKRC, clkrc);
	}

    /* Write DSP input regsiters */
    while (regs[i][0]) {
        cambus_writeb(_slaveAddr, regs[i][0], regs[i][1]);
        i++;
    }
	 /* Write output width */
	ret |= cambus_writeb(_slaveAddr,0xe0,0x04 ); /* OUTH[8]/OUTW[9:8] */
    ret |= cambus_writeb(_slaveAddr, ZMOW, (w>>2)&0xFF); /* OUTW[7:0] (real/4) */
    ret |= cambus_writeb(_slaveAddr, ZMOH, (h>>2)&0xFF); /* OUTH[7:0] (real/4) */
    ret |= cambus_writeb(_slaveAddr, ZMHH, ((h>>8)&0x04)|((w>>10)&0x03)); /* OUTH[8]/OUTW[9:8] */
	ret |= cambus_writeb(_slaveAddr,0xe0,0x00 ); /* OUTH[8]/OUTW[9:8] */

    /* Enable DSP */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);
    ret |= cambus_writeb(_slaveAddr, R_BYPASS, R_BYPASS_DSP_EN);

    /* delay n ms */
    msleep(30);
	dvp_set_image_size(w, h);
    return ret;
}

int Sipeed_OV2640::ov2640_set_framerate(framerate_t framerate)
{
    return 0;
}

int Sipeed_OV2640::ov2640_set_contrast(int level)
{
    int ret=0;

    level += (NUM_CONTRAST_LEVELS / 2 + 1);
    if (level < 0 || level > NUM_CONTRAST_LEVELS) {
        return -1;
    }

    /* Switch to DSP register bank */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    for (int i=0; i<sizeof(contrast_regs[0])/sizeof(contrast_regs[0][0]); i++) {
        ret |= cambus_writeb(_slaveAddr, contrast_regs[0][i], contrast_regs[level][i]);
    }

    return ret;
}

int Sipeed_OV2640::ov2640_set_brightness(int level)
{
    int ret=0;

    level += (NUM_BRIGHTNESS_LEVELS / 2 + 1);
    if (level < 0 || level > NUM_BRIGHTNESS_LEVELS) {
        return -1;
    }

    /* Switch to DSP register bank */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);

    /* Write brightness registers */
    for (int i=0; i<sizeof(brightness_regs[0])/sizeof(brightness_regs[0][0]); i++) {
        ret |= cambus_writeb(_slaveAddr, brightness_regs[0][i], brightness_regs[level][i]);
    }

    return ret;
}

int Sipeed_OV2640::ov2640_set_saturation(int level)
{
    int ret=0;

    level += (NUM_SATURATION_LEVELS / 2 + 1);
    if (level < 0 || level > NUM_SATURATION_LEVELS) {
        return -1;
    }

    /* Switch to DSP register bank */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);

    /* Write contrast registers */
    for (int i=0; i<sizeof(saturation_regs[0])/sizeof(saturation_regs[0][0]); i++) {
        ret |= cambus_writeb(_slaveAddr, saturation_regs[0][i], saturation_regs[level][i]);
    }

    return ret;
}

int Sipeed_OV2640::ov2640_set_gainceiling( gainceiling_t gainceiling)
{
    int ret =0;

    /* Switch to SENSOR register bank */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_SENSOR);

    /* Write gain ceiling register */
    ret |= cambus_writeb(_slaveAddr, COM9, COM9_AGC_SET(gainceiling));

    return ret;
}

int Sipeed_OV2640::ov2640_set_quality(int qs)
{
    int ret=0;

    /* Switch to DSP register bank */
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_DSP);

    /* Write QS register */
    ret |= cambus_writeb(_slaveAddr, QS, qs);

    return ret;
}

int Sipeed_OV2640::ov2640_set_colorbar(int enable)
{
    uint8_t reg;
    /* Switch to SENSOR register bank */
    int ret = cambus_writeb(_slaveAddr, BANK_SEL, BANK_SEL_SENSOR);

    /* Update COM7 */
    ret |= cambus_readb(_slaveAddr, COM7, &reg);

    if (enable) {
        reg |= COM7_COLOR_BAR;
    } else {
        reg &= ~COM7_COLOR_BAR;
    }

    return cambus_writeb(_slaveAddr, COM7, reg) | ret;
}

int Sipeed_OV2640::ov2640_set_auto_gain(int enable, float gain_db, float gain_db_ceiling)
{
   uint8_t reg;
   int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
   ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
   ret |= cambus_readb(_slaveAddr, COM8, &reg);
   ret |= cambus_writeb(_slaveAddr, COM8, (reg & (~COM8_AGC_EN)) | ((enable != 0) ? COM8_AGC_EN : 0));

   if ((enable == 0) && (!isnanf(gain_db)) && (!isinff(gain_db))) {
       float gain = IM_MAX(IM_MIN(expf((gain_db / 20.0) * log(10.0)), 32.0), 1.0);

       int gain_temp = roundf(log2(IM_MAX(gain / 2.0, 1.0)));
       int gain_hi = 0xF >> (4 - gain_temp);
       int gain_lo = IM_MIN(roundf(((gain / (1 << gain_temp)) - 1.0) * 16.0), 15);

       ret |= cambus_writeb(_slaveAddr, GAIN, (gain_hi << 4) | (gain_lo << 0));
   } else if ((enable != 0) && (!isnanf(gain_db_ceiling)) && (!isinff(gain_db_ceiling))) {
       float gain_ceiling = IM_MAX(IM_MIN(expf((gain_db_ceiling / 20.0) * log(10.0)), 128.0), 2.0);

       ret |= cambus_readb(_slaveAddr, COM9, &reg);
       ret |= cambus_writeb(_slaveAddr, COM9, (reg & 0x1F) | (((int)ceilf(log2(gain_ceiling)) - 1) << 5));
   }

   return ret;
}

int Sipeed_OV2640::ov2640_get_gain_db(float *gain_db)
{
    uint8_t reg, gain;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
    ret |= cambus_readb(_slaveAddr, COM8, &reg);

    // DISABLED
    // if (reg & COM8_AGC_EN) {
    //     ret |= cambus_writeb(_slaveAddr, COM8, reg & (~COM8_AGC_EN));
    // }
    // DISABLED

    ret |= cambus_readb(_slaveAddr, GAIN, &gain);

    // DISABLED
    // if (reg & COM8_AGC_EN) {
    //     ret |= cambus_writeb(_slaveAddr, COM8, reg | COM8_AGC_EN);
    // }
    // DISABLED

    int hi_gain = 1 << (((gain >> 7) & 1) + ((gain >> 6) & 1) + ((gain >> 5) & 1) + ((gain >> 4) & 1));
    float lo_gain = 1.0 + (((gain >> 0) & 0xF) / 16.0);
    *gain_db = 20.0 * (log(hi_gain * lo_gain) / log(10.0));

    return ret;
}

int Sipeed_OV2640::ov2640_set_auto_exposure(int enable, int exposure_us)
{
    uint8_t reg;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
    ret |= cambus_readb(_slaveAddr, COM8, &reg);
    ret |= cambus_writeb(_slaveAddr, COM8, COM8_SET_AEC(reg, (enable != 0)));

    if ((enable == 0) && (exposure_us >= 0)) {
        ret |= cambus_readb(_slaveAddr, COM7, &reg);
        int t_line = 0;

        if (COM7_GET_RES(reg) == COM7_RES_UXGA) t_line = 1600 + 322;
        if (COM7_GET_RES(reg) == COM7_RES_SVGA) t_line = 800 + 390;
        if (COM7_GET_RES(reg) == COM7_RES_CIF) t_line = 400 + 195;

        ret |= cambus_readb(_slaveAddr, CLKRC, &reg);
        int pll_mult = (reg & CLKRC_DOUBLE) ? 2 : 1;
        int clk_rc = ((reg & CLKRC_DIVIDER_MASK) + 1) * 2;

        ret |= cambus_readb(_slaveAddr, BANK_SEL, &reg);
        ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg & (~BANK_SEL_SENSOR));
        ret |= cambus_readb(_slaveAddr, IMAGE_MODE, &reg);
        int t_pclk = 0;

        if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_YUV422) t_pclk = 2;
        if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_RAW10) t_pclk = 1;
        if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_RGB565) t_pclk = 2;

        int exposure = IM_MAX(IM_MIN(((exposure_us*(((_freq/clk_rc)*pll_mult)/1000000))/t_pclk)/t_line,0xFFFF),0x0000);

        ret |= cambus_readb(_slaveAddr, BANK_SEL, &reg);
        ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);

        ret |= cambus_readb(_slaveAddr, REG04, &reg);
        ret |= cambus_writeb(_slaveAddr, REG04, (reg & 0xFC) | ((exposure >> 0) & 0x3));

        ret |= cambus_readb(_slaveAddr, AEC, &reg);
        ret |= cambus_writeb(_slaveAddr, AEC, (reg & 0x00) | ((exposure >> 2) & 0xFF));

        ret |= cambus_readb(_slaveAddr, REG04, &reg);
        ret |= cambus_writeb(_slaveAddr, REG04, (reg & 0xC0) | ((exposure >> 10) & 0x3F));
    }

    return ret;
}

int Sipeed_OV2640::ov2640_get_exposure_us(int *exposure_us)
{
    uint8_t reg, aec_10, aec_92, aec_1510;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
    ret |= cambus_readb(_slaveAddr, COM8, &reg);

    // DISABLED
    // if (reg & COM8_AEC_EN) {
    //     ret |= cambus_writeb(_slaveAddr, COM8, reg & (~COM8_AEC_EN));
    // }
    // DISABLED

    ret |= cambus_readb(_slaveAddr, REG04, &aec_10);
    ret |= cambus_readb(_slaveAddr, AEC, &aec_92);
    ret |= cambus_readb(_slaveAddr, REG45, &aec_1510);

    // DISABLED
    // if (reg & COM8_AEC_EN) {
    //     ret |= cambus_writeb(_slaveAddr, COM8, reg | COM8_AEC_EN);
    // }
    // DISABLED

    ret |= cambus_readb(_slaveAddr, COM7, &reg);
    int t_line = 0;

    if (COM7_GET_RES(reg) == COM7_RES_UXGA) t_line = 1600 + 322;
    if (COM7_GET_RES(reg) == COM7_RES_SVGA) t_line = 800 + 390;
    if (COM7_GET_RES(reg) == COM7_RES_CIF) t_line = 400 + 195;

    ret |= cambus_readb(_slaveAddr, CLKRC, &reg);
    int pll_mult = (reg & CLKRC_DOUBLE) ? 2 : 1;
    int clk_rc = ((reg & CLKRC_DIVIDER_MASK) + 1) * 2;

    ret |= cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg & (~BANK_SEL_SENSOR));
    ret |= cambus_readb(_slaveAddr, IMAGE_MODE, &reg);
    int t_pclk = 0;

    if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_YUV422) t_pclk = 2;
    if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_RAW10) t_pclk = 1;
    if (IMAGE_MODE_GET_FMT(reg) == IMAGE_MODE_RGB565) t_pclk = 2;

    uint16_t exposure = ((aec_1510 & 0x3F) << 10) + ((aec_92 & 0xFF) << 2) + ((aec_10 & 0x3) << 0);
    *exposure_us = (exposure*t_line*t_pclk)/(((_freq/clk_rc)*pll_mult)/1000000);

    return ret;
}

int Sipeed_OV2640::ov2640_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db)
{
    uint8_t reg;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg & (~BANK_SEL_SENSOR));
    ret |= cambus_readb(_slaveAddr, CTRL1, &reg);
    ret |= cambus_writeb(_slaveAddr, CTRL1, (reg & (~CTRL1_AWB)) | ((enable != 0) ? CTRL1_AWB : 0));

    if ((enable == 0) && (!isnanf(r_gain_db)) && (!isnanf(g_gain_db)) && (!isnanf(b_gain_db))
                      && (!isinff(r_gain_db)) && (!isinff(g_gain_db)) && (!isinff(b_gain_db))) {
    }

    return ret;
}

int Sipeed_OV2640::ov2640_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db)
{
    uint8_t reg;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg & (~BANK_SEL_SENSOR));
    ret |= cambus_readb(_slaveAddr, CTRL1, &reg);

    // DISABLED
    // if (reg & CTRL1_AWB) {
    //     ret |= cambus_writeb(_slaveAddr, CTRL1, reg & (~CTRL1_AWB));
    // }
    // DISABLED

    // DISABLED
    // if (reg & CTRL1_AWB) {
    //     ret |= cambus_writeb(_slaveAddr, CTRL1, reg | CTRL1_AWB);
    // }
    // DISABLED

    return ret;
}

int Sipeed_OV2640::ov2640_set_hmirror(int enable)
{
    uint8_t reg;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
    ret |= cambus_readb(_slaveAddr, REG04, &reg);

    if (enable) {
        reg |= REG04_HFLIP_IMG;
    } else {
        reg &= ~REG04_HFLIP_IMG;
    }

    ret |= cambus_writeb(_slaveAddr, REG04, reg);

    return ret;
}

int Sipeed_OV2640::ov2640_set_vflip(int enable)
{
    uint8_t reg;
    int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
    ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
    ret |= cambus_readb(_slaveAddr, REG04, &reg);

    if (enable) {
        reg |= REG04_VFLIP_IMG;
        reg |= REG04_VREF_EN;
    } else {
        reg &= ~REG04_VFLIP_IMG;
        reg &= ~REG04_VREF_EN;
    }

    ret |= cambus_writeb(_slaveAddr, REG04, reg);

    return ret;
}


int Sipeed_OV2640::ov2640_set_flip(bool horiz, bool vert) 
{
	uint8_t reg;
	int ret = cambus_readb(_slaveAddr, BANK_SEL, &reg);
	ret |= cambus_writeb(_slaveAddr, BANK_SEL, reg | BANK_SEL_SENSOR);
	ret |= cambus_readb(_slaveAddr, REG04, &reg);

	if (horiz) {
		reg |= REG04_HFLIP_IMG;
		reg |= REG04_HREF_EN;
	} 
	else {
		reg &= ~REG04_HFLIP_IMG;
		reg &= ~REG04_HREF_EN;
	}

	if (vert) {
		reg |= REG04_VFLIP_IMG;
		reg |= REG04_VREF_EN;
	} 
	else {
		reg &= ~REG04_VFLIP_IMG;
		reg &= ~REG04_VREF_EN;
	}

	ret |= cambus_writeb(_slaveAddr, REG04, reg);

	return ret;
}


int Sipeed_OV2640::reverse_u32pixel(uint32_t* addr,uint32_t length)
{
  if(NULL == addr)
    return -1;

  uint32_t data;
  uint32_t* pend = addr+length;
  for(;addr<pend;addr++)
  {
	  data = *(addr);
	  *(addr) = ((data & 0x000000FF) << 24) | ((data & 0x0000FF00) << 8) | 
                ((data & 0x00FF0000) >> 8) | ((data & 0xFF000000) >> 24) ;
  }  //1.7ms
  
  
  return 0;
}


int Sipeed_OV2640::sensor_snapshot( )
{	
    //wait for new frame
    g_dvp_finish_flag = 0;
    uint32_t start =  millis();
    while (g_dvp_finish_flag == 0)
    {
        usleep(50);
        if(millis() - start > 300)//wait for 300ms
            return -1;
    }
    reverse_u32pixel((uint32_t*)_dataBuffer, _width*_height/2);
    return 0;
}

int Sipeed_OV2640::flip( bool horiz, bool vert ) {
	return ov2640_set_flip(horiz, vert);
}