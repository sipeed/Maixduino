
#include "Sipeed_GC0328.h"

//////////// HAL ///////////////
#include "sysctl.h"
#include "fpioa.h"
#include "dvp.h"
#include "sleep.h"
#include "stdlib.h"
#include "utils.h"
#include "plic.h"
#include "Arduino.h" // millis
#include "stdio.h"

volatile static uint8_t g_dvp_finish_flag = 0;

typedef enum {
    ACTIVE_LOW,
    ACTIVE_HIGH,
    ACTIVE_BINOCULAR,
} polarity_t;

#define DCMI_RESET_LOW()      dvp->cmos_cfg &= ~DVP_CMOS_RESET
#define DCMI_RESET_HIGH()     dvp->cmos_cfg |= DVP_CMOS_RESET
#define DCMI_PWDN_LOW()       dvp->cmos_cfg |= DVP_CMOS_POWER_DOWN
#define DCMI_PWDN_HIGH()      dvp->cmos_cfg &= ~DVP_CMOS_POWER_DOWN

uint8_t gc0328_default_regs[][2] = {
	{0xfe , 0x80},
	{0xfe , 0x80},
	{0xfc , 0x16},
	{0xfc , 0x16},
	{0xfc , 0x16},
	{0xfc , 0x16},
	{0xf1 , 0x00},
	{0xf2 , 0x00},
	{0xfe , 0x00},
	{0x4f , 0x00},
	{0x42 , 0x00},  
	{0x03 , 0x00},  
	{0x04 , 0xc0},  
	{0x77 , 0x62},  
	{0x78 , 0x40},  
	{0x79 , 0x4d},  

	{0xfe , 0x00},
	{0x16 , 0x00},
	{0x17 , 0x14},
	{0x18 , 0x0e},
	{0x19 , 0x06},

	{0x1b , 0x48},
	{0x1f , 0xC8},
	{0x20 , 0x01},
	{0x21 , 0x78},
	{0x22 , 0xb0},
	{0x23 , 0x04},//0x06  20140519 GC0328C
	{0x24 , 0x11}, 
	{0x26 , 0x00},

	//global gain for range 
	{0x70 , 0x85},   

	/////////////banding/////////////
	{0x05 , 0x00},//hb
	{0x06 , 0x6a},//
	{0x07 , 0x00},//vb
	{0x08 , 0x0c},//
	{0xfe , 0x01},//
	{0x29 , 0x00},//anti-flicker step [11:8]
	{0x2a , 0x96},//anti-flicker step [7:0]
	{0xfe , 0x00},//

	///////////////AWB//////////////
	{0xfe , 0x01},
	{0x50 , 0x00},
	{0x4f , 0x00},
	{0x4c , 0x01},
	{0x4f , 0x00},
	{0x4f , 0x00},
	{0x4f , 0x00},
	{0x4f , 0x00},
	{0x4f , 0x00}, 
	{0x4d , 0x30},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x40},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x50},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x60},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x70},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},	
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04}, 
	{0x4e , 0x04},	
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4f , 0x01},
	{0x50 , 0x88},
	{0xfe , 0x00},

	//////////// BLK//////////////////////
	{0xfe , 0x00}, 
	{0x27 , 0xb7},
	{0x28 , 0x7F},
	{0x29 , 0x20},
	{0x33 , 0x20},
	{0x34 , 0x20},
	{0x35 , 0x20},
	{0x36 , 0x20},
	{0x32 , 0x08},
	{0x3b , 0x00}, 
	{0x3c , 0x00},
	{0x3d , 0x00},
	{0x3e , 0x00},
	{0x47 , 0x00},
	{0x48 , 0x00}, 

	//////////// block enable/////////////
	{0x40 , 0x7f}, 
	{0x41 , 0x26}, 
	{0x42 , 0xfb},
	{0x44 , 0x00}, //yuv
	{0x45 , 0x00},
	{0x46 , 0x03},
	{0x4f , 0x01},
	{0x4b , 0x01},
	{0x50 , 0x01}, 

	/////DN & EEINTP/////
	{0x7e , 0x0a}, 
	{0x7f , 0x03}, 
	{0x81 , 0x15}, 
	{0x82 , 0x85},    
	{0x83 , 0x03},
	{0x84 , 0xe5},
	{0x90 , 0xac},    
	{0x92 , 0x02},
	{0x94 , 0x02},
	{0x95 , 0x32},    

	////////////YCP///////////
	{0xd1 , 0x28},
	{0xd2 , 0x28},
	{0xd3 , 0x40},
	{0xdd , 0x58},
	{0xde , 0x36},
	{0xe4 , 0x88},
	{0xe5 , 0x40}, 
	{0xd7 , 0x0e}, 

	///////////rgb gamma ////////////
	{0xfe , 0x00},
	{0xbf , 0x0e},
	{0xc0 , 0x1c},
	{0xc1 , 0x34},
	{0xc2 , 0x48},
	{0xc3 , 0x5a},
	{0xc4 , 0x6e},
	{0xc5 , 0x80},
	{0xc6 , 0x9c},
	{0xc7 , 0xb4},
	{0xc8 , 0xc7},
	{0xc9 , 0xd7},
	{0xca , 0xe3},
	{0xcb , 0xed},
	{0xcc , 0xf2},
	{0xcd , 0xf8},
	{0xce , 0xfd},
	{0xcf , 0xff},

	/////////////Y gamma//////////
	{0xfe , 0x00},
	{0x63 , 0x00},
	{0x64 , 0x05},
	{0x65 , 0x0b},
	{0x66 , 0x19},
	{0x67 , 0x2e},
	{0x68 , 0x40},
	{0x69 , 0x54},
	{0x6a , 0x66},
	{0x6b , 0x86},
	{0x6c , 0xa7},
	{0x6d , 0xc6},
	{0x6e , 0xe4},
	{0x6f , 0xff},

	//////////////ASDE/////////////
	{0xfe , 0x01},
	{0x18 , 0x02},
	{0xfe , 0x00},
	{0x98 , 0x00},    
	{0x9b , 0x20},    
	{0x9c , 0x80},    
	{0xa4 , 0x10},    
	{0xa8 , 0xB0},    
	{0xaa , 0x40},    
	{0xa2 , 0x23},    
	{0xad , 0x01},    

	//////////////abs///////////
	{0xfe , 0x01},
	{0x9c , 0x02},   
	{0x9e , 0xc0}, 
	{0x9f , 0x40},	

	////////////// AEC////////////
	{0x08 , 0xa0},
	{0x09 , 0xe8},
	{0x10 , 0x00},  
	{0x11 , 0x11},   
	{0x12 , 0x10},   
	{0x13 , 0x98},   
	{0x15 , 0xfc},   
	{0x18 , 0x03},
	{0x21 , 0xc0},   
	{0x22 , 0x60},   
	{0x23 , 0x30},
	{0x25 , 0x00},
	{0x24 , 0x14},
	{0x3d , 0x80},
	{0x3e , 0x40},

	////////////////AWB///////////
	{0xfe , 0x01},
	{0x51 , 0x88},
	{0x52 , 0x12},
	{0x53 , 0x80},
	{0x54 , 0x60},
	{0x55 , 0x01},
	{0x56 , 0x02},
	{0x58 , 0x00},
	{0x5b , 0x02},
	{0x5e , 0xa4},
	{0x5f , 0x8a},
	{0x61 , 0xdc},
	{0x62 , 0xdc},
	{0x70 , 0xfc},
	{0x71 , 0x10},
	{0x72 , 0x30},
	{0x73 , 0x0b},
	{0x74 , 0x0b},
	{0x75 , 0x01},
	{0x76 , 0x00},
	{0x77 , 0x40},
	{0x78 , 0x70},
	{0x79 , 0x00},
	{0x7b , 0x00},
	{0x7c , 0x71},
	{0x7d , 0x00},
	{0x80 , 0x70},
	{0x81 , 0x58},
	{0x82 , 0x98},
	{0x83 , 0x60},
	{0x84 , 0x58},
	{0x85 , 0x50},
	{0xfe , 0x00},	

	////////////////LSC////////////////
	{0xfe , 0x01},
	{0xc0 , 0x10},
	{0xc1 , 0x0c},
	{0xc2 , 0x0a},
	{0xc6 , 0x0e},
	{0xc7 , 0x0b},
	{0xc8 , 0x0a},
	{0xba , 0x26},
	{0xbb , 0x1c},
	{0xbc , 0x1d},
	{0xb4 , 0x23},
	{0xb5 , 0x1c},
	{0xb6 , 0x1a},
	{0xc3 , 0x00},
	{0xc4 , 0x00},
	{0xc5 , 0x00},
	{0xc9 , 0x00},
	{0xca , 0x00},
	{0xcb , 0x00},
	{0xbd , 0x00},
	{0xbe , 0x00},
	{0xbf , 0x00},
	{0xb7 , 0x07},
	{0xb8 , 0x05},
	{0xb9 , 0x05},
	{0xa8 , 0x07},
	{0xa9 , 0x06},
	{0xaa , 0x00},
	{0xab , 0x04},
	{0xac , 0x00},
	{0xad , 0x02},
	{0xae , 0x0d},
	{0xaf , 0x05},
	{0xb0 , 0x00},
	{0xb1 , 0x07},
	{0xb2 , 0x03},
	{0xb3 , 0x00},
	{0xa4 , 0x00},
	{0xa5 , 0x00},
	{0xa6 , 0x00},
	{0xa7 , 0x00},
	{0xa1 , 0x3c},
	{0xa2 , 0x50},
	{0xfe , 0x00},

	///////////////CCT ///////////
	{0xb1 , 0x12},
	{0xb2 , 0xf5},
	{0xb3 , 0xfe},
	{0xb4 , 0xe0},
	{0xb5 , 0x15},
	{0xb6 , 0xc8},

	/////skin CC for front //////
	{0xb1 , 0x00},
	{0xb2 , 0x00},
	{0xb3 , 0x05},
	{0xb4 , 0xf0},
	{0xb5 , 0x00},
	{0xb6 , 0x00},
	
	///////////////AWB////////////////
	{0xfe , 0x01},
	{0x50 , 0x00},
	{0xfe , 0x01}, 
	{0x4f , 0x00},
	{0x4c , 0x01},
	{0x4f , 0x00},
	{0x4f , 0x00},
	{0x4f , 0x00}, 
	{0x4d , 0x34},
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4e , 0x02},
	{0x4e , 0x02},
	{0x4d , 0x44},
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x53},
	{0x4e , 0x00},
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4e , 0x04},
	{0x4d , 0x65},
	{0x4e , 0x04},
	{0x4d , 0x73},
	{0x4e , 0x20},
	{0x4d , 0x83},
	{0x4e , 0x20},
	{0x4f , 0x01}, 
	{0x50 , 0x88}, 

    {0xfe , 0x00},
    // window
        //windowing mode
	// {0x09 , 0x00},
    // {0x0a , 0x78},
	// {0x0b , 0x00},
	// {0x0c , 0xa0},
    // {0x0d , 0x00},
	// {0x0e , 0xf8},
	// {0x0f , 0x01},
	// {0x10 , 0x48},
        //crop mode 
    {0x50 , 0x01},
    // {0x51, 0x00},
    // {0x52, 0x78},
    // {0x53, 0x00},
    // {0x54, 0xa0},
    // {0x55, 0x00},
    // {0x56, 0xf0},
    // {0x57, 0x01},
    // {0x58, 0x40},
    //subsample 1/2
    {0x59, 0x22},
    {0x5a, 0x00},
    {0x5b, 0x00},
    {0x5c, 0x00},
    {0x5d, 0x00},
    {0x5e, 0x00},
    {0x5f, 0x00},
    {0x60, 0x00},
    {0x61, 0x00},
    {0x62, 0x00},

    //Exp level
    {0xfe, 0x01},
    {0x2b , 0x02},//exp level 0  30fps => 16fps
	{0x2c , 0x00},//			 
	{0x2d , 0x02},//exp level 1  12.50fps
	{0x2e , 0x00},//			 
	{0x2f , 0x02},//exp level 2  10.00fps
	{0x30 , 0x00},//			 
	{0x31 , 0x02},//exp level 3  7.14fps
	{0x32 , 0x00},//
    {0x33, 0x00},

	/////////output//////// 
	{0xfe , 0x00},	
	{0xf1 , 0x07}, 
	{0xf2 , 0x01}, 

    {0x00, 0x00}
};

static const uint8_t qvga_config[][2] = { //k210 
    {0xfe , 0x00},
    // window
        //windowing mode
	// {0x09 , 0x00},
    // {0x0a , 0x78},
	// {0x0b , 0x00},
	// {0x0c , 0xa0},
    // {0x0d , 0x00},
	// {0x0e , 0xf8},
	// {0x0f , 0x01},
	// {0x10 , 0x48},
        //crop mode 
    {0x50 , 0x01},
    // {0x51, 0x00},
    // {0x52, 0x78},
    // {0x53, 0x00},
    // {0x54, 0xa0},
    // {0x55, 0x00},
    // {0x56, 0xf0},
    // {0x57, 0x01},
    // {0x58, 0x40},
    //subsample 1/2
    {0x59, 0x22},
    {0x5a, 0x00},
    {0x5b, 0x00},
    {0x5c, 0x00},
    {0x5d, 0x00},
    {0x5e, 0x00},
    {0x5f, 0x00},
    {0x60, 0x00},
    {0x61, 0x00},
    {0x62, 0x00},

    {0x00, 0x00}
};

static const uint8_t vga_config[][2] = { //k210 
    {0xfe , 0x00},
    // window
        //windowing mode
	// {0x09 , 0x00},
    // {0x0a , 0x78},
	// {0x0b , 0x00},
	// {0x0c , 0xa0},
    // {0x0d , 0x00},
	// {0x0e , 0xf8},
	// {0x0f , 0x01},
	// {0x10 , 0x48},
        //crop mode 
    {0x50 , 0x00},
    // {0x51, 0x00},
    // {0x52, 0x78},
    // {0x53, 0x00},
    // {0x54, 0xa0},
    // {0x55, 0x00},
    // {0x56, 0xf0},
    // {0x57, 0x01},
    // {0x58, 0x40},
    //subsample 1/2
    // {0x59, 0x00},
    // {0x5a, 0x00},
    // {0x5b, 0x00},
    // {0x5c, 0x00},
    // {0x5d, 0x00},
    // {0x5e, 0x00},
    // {0x5f, 0x00},
    // {0x60, 0x00},
    // {0x61, 0x00},
    // {0x62, 0x00},
    {0x00, 0x00}
};

static const uint8_t gc0328_yuv422_regs[][2] = {
    {0xfe , 0x00},
	{0x44 , 0x00}, //yuv
    {0x00, 0x00}
};

static const uint8_t gc0328_rgb565_regs[][2] = {
    {0xfe , 0x00},
	{0x44 , 0x06},
    {0x00, 0x00}
};

Sipeed_GC0328::Sipeed_GC0328( framesize_t frameSize, pixformat_t pixFormat, TwoWire *i2c)
:Camera(frameSize, pixFormat),
_dataBuffer(NULL), _aiBuffer(NULL),
_resetPoliraty(ACTIVE_HIGH), _pwdnPoliraty(ACTIVE_HIGH),
_slaveAddr(0x00),
_id(0),
_i2c(i2c)
{
    configASSERT(pixFormat == PIXFORMAT_RGB565 || pixFormat==PIXFORMAT_YUV422);
}

Sipeed_GC0328::Sipeed_GC0328(uint16_t width, uint16_t height, pixformat_t pixFormat, TwoWire *i2c)
:Camera(width, height, pixFormat),
_dataBuffer(NULL), _aiBuffer(NULL),
_resetPoliraty(ACTIVE_HIGH), _pwdnPoliraty(ACTIVE_HIGH),
_slaveAddr(0x00),
_id(0),
_i2c(i2c)
{
    configASSERT(pixFormat == PIXFORMAT_RGB565 || pixFormat==PIXFORMAT_YUV422);
}

Sipeed_GC0328::~Sipeed_GC0328()
{
    end();
}

bool Sipeed_GC0328::begin()
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
    if(!setPixFormat(_pixFormat))
        return false;
    if(!setFrameSize(_frameSize))
        return false; 
    return true;
}

void Sipeed_GC0328::end()
{
    if(_dataBuffer)
        free(_dataBuffer);
    if(_aiBuffer)
        free(_aiBuffer);
    _dataBuffer = nullptr;
    _aiBuffer   = nullptr;
}

bool Sipeed_GC0328::reset()
{
    if(dvpInit() != 0) // dvp hardware init
        return false;
    if(sensro_gc_detect() != 0) // gc0328 camera detect
        return false;
    if(gc0328_reset() != 0)
        return false;
    if(dvpInitIrq() != 0)
        return false;
    return true;
}

bool Sipeed_GC0328::setPixFormat(pixformat_t pixFormat)
{
    if(gc0328_set_pixformat(pixFormat) != 0)
        return false;
    return true;
}

bool Sipeed_GC0328::setFrameSize(framesize_t frameSize)
{
    if(_id == 0x9d)
    {
        if(gc0328_set_framesize(frameSize) != 0)
            return false;
    }
    else
    {
        return false;
    }
    return true;
}  

bool Sipeed_GC0328::run(bool run)
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

int Sipeed_GC0328::id()
{
    return _id;
}

/**
 * @return pixels 
 *         If pixels format is RGB565: return RGB565 pixels with every uint16_t one pixel, e.g. RED: 0xF800
 */
uint8_t* Sipeed_GC0328::snapshot()
{
    if ( sensor_snapshot() != 0)
        return nullptr;
    return _dataBuffer;
}

void Sipeed_GC0328::setRotaion(uint8_t rotation)
{
    //FIXME
}

void Sipeed_GC0328::setInvert(bool invert)
{
    //FIXME
    gc0328_set_hmirror(!invert); 
    //gc0328_set_vflip(1);
    return;
}

int Sipeed_GC0328::dvpInit(uint32_t freq)
{
    // just support RGB565 and YUV442 on k210
    configASSERT(_pixFormat==PIXFORMAT_RGB565 || _pixFormat==PIXFORMAT_YUV422);
    _freq  = freq;

	fpioa_set_function(47, FUNC_CMOS_PCLK);
	fpioa_set_function(46, FUNC_CMOS_XCLK);
	fpioa_set_function(45, FUNC_CMOS_HREF);
	fpioa_set_function(44, FUNC_CMOS_PWDN);
	fpioa_set_function(43, FUNC_CMOS_VSYNC);
	fpioa_set_function(42, FUNC_CMOS_RST);
    
    /* Do a power cycle */
    DCMI_PWDN_HIGH();
    msleep(10);

    DCMI_PWDN_LOW();
    msleep(10);

    // Initialize the camera bus, 8bit reg
    dvp_init(8);
    
    _i2c->begin(uint8_t(40), uint8_t(41), uint32_t(100000));
    _i2c->setTimeOut(10);

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

    return 0;
}


int Sipeed_GC0328::cambus_scan()
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

int Sipeed_GC0328::cambus_read_id(uint8_t addr,uint16_t *manuf_id, uint16_t *device_id)
{
	dvp_sccb_send_data(addr, 0xFF, 0x01);
	*manuf_id = (dvp_sccb_receive_data(addr, 0x1C) << 8) | dvp_sccb_receive_data(addr, 0x1D);
	*device_id = (dvp_sccb_receive_data(addr, 0x0A) << 8) | dvp_sccb_receive_data(addr, 0x0B);
	return 0;
}

int Sipeed_GC0328::cambus_scan_gc0328()
{
    uint8_t id;
    cambus_writeb(GC0328_ADDR, 0xFE, 0x00);
    cambus_readb(GC0328_ADDR, 0xf0, &id);
    if (id != 0x9d)
    {
        Serial.printf("error gc0328 detect, ret id is 0x%x\r\n", id);
        return 0;
    }
    return id;
}

int Sipeed_GC0328::cambus_readb(uint8_t slv_addr, uint8_t reg_addr, uint8_t *reg_data)
{
    int ret = 0;
    _i2c->beginTransmission(slv_addr);
    _i2c->write(reg_addr);
    _i2c->endTransmission();

    _i2c->requestFrom((uint16_t)slv_addr, (uint8_t)1);
    *reg_data = _i2c->read();
    if(0xff == *reg_data)
		ret = -1;
    return ret;
}

int Sipeed_GC0328::cambus_writeb(uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data)
{
    _i2c->beginTransmission(slv_addr);
    _i2c->write(reg_addr);
    _i2c->write(reg_data);
    _i2c->endTransmission();
	return 0;
}

int Sipeed_GC0328::sensro_gc_detect()
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
        Serial.printf("detected gc0328 id = %#x\r\n",id); 
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



int Sipeed_GC0328::dvpInitIrq()
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

int Sipeed_GC0328::gc0328_reset()
{    
    for (uint16_t index = 0; gc0328_default_regs[index][0]; index++)
    {
        uint8_t reg_data;
        cambus_writeb(GC0328_ADDR, gc0328_default_regs[index][0], gc0328_default_regs[index][1]);
        cambus_readb(GC0328_ADDR,gc0328_default_regs[index][0],&reg_data);
        if(reg_data != gc0328_default_regs[index][1])
            Serial.printf("%d, %#x, %#x, %#x\r\n", index, gc0328_default_regs[index][0], gc0328_default_regs[index][1], reg_data);//debug
    }
    return 0;
}

int Sipeed_GC0328::gc0328_read_reg(uint8_t reg_addr)
{
    uint8_t reg_data;
    if (cambus_readb(_slaveAddr, reg_addr, &reg_data) != 0) {
        return -1;
    }
    return reg_data;
}

int Sipeed_GC0328::gc0328_write_reg(uint8_t reg_addr, uint8_t reg_data)
{
    return cambus_writeb(_slaveAddr, reg_addr, reg_data);
}

int Sipeed_GC0328::gc0328_set_pixformat(pixformat_t pixformat)
{
       int i=0;
    const uint8_t (*regs)[2]=NULL;

    /* read pixel format reg */
    switch (pixformat) {
        case PIXFORMAT_RGB565:
            regs = gc0328_rgb565_regs;
            break;
        case PIXFORMAT_YUV422:
        case PIXFORMAT_GRAYSCALE:
            regs = gc0328_yuv422_regs;
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

int Sipeed_GC0328::gc0328_set_framesize(framesize_t framesize)
{
    int ret=0;
    uint16_t w = _width;
    uint16_t h = _height;

    int i=0;
    const uint8_t (*regs)[2];

    if ((w <= 320) && (h <= 240)) {
        regs = qvga_config;
    } else {
        regs = vga_config;
    }

    while (regs[i][0]) {
        cambus_writeb(_slaveAddr, regs[i][0], regs[i][1]);
        i++;
    }
    msleep(30);
	dvp_set_image_size(w, h);
    return ret;
}

int Sipeed_GC0328::gc0328_set_framerate(framerate_t framerate)
{
    return 0;
}

#define NUM_CONTRAST_LEVELS (5)
static uint8_t contrast_regs[NUM_CONTRAST_LEVELS][2]={
	{0x80, 0x00},
	{0x80, 0x20},
	{0x80, 0x40},
	{0x80, 0x60},
	{0x80, 0x80}
};

int Sipeed_GC0328::gc0328_set_contrast(int level)
{
    int ret=0;

    level += (NUM_CONTRAST_LEVELS / 2);
    if (level < 0 || level > NUM_CONTRAST_LEVELS) {
        return -1;
    }
	cambus_writeb(_slaveAddr, 0xfe, 0x00);
	cambus_writeb(_slaveAddr, 0xd4, contrast_regs[level][0]);
	cambus_writeb(_slaveAddr, 0xd3, contrast_regs[level][1]);
    return ret;
}

int Sipeed_GC0328::gc0328_set_brightness(int level)
{
    int ret = 0;
    return ret;
}

#define NUM_SATURATION_LEVELS (5)
static uint8_t saturation_regs[NUM_SATURATION_LEVELS][3]={
	{0x00, 0x00, 0x00},
	{0x10, 0x10, 0x10},
	{0x20, 0x20, 0x20},
	{0x30, 0x30, 0x30},
	{0x40, 0x40, 0x40},
};

int Sipeed_GC0328::gc0328_set_saturation(int level)
{
   int ret = 0;
    level += (NUM_CONTRAST_LEVELS / 2);
    if (level < 0 || level > NUM_CONTRAST_LEVELS) {
        return -1;
    }
	cambus_writeb(_slaveAddr, 0xfe, 0x00);
	cambus_writeb(_slaveAddr, 0xd0, saturation_regs[level][0]);
	cambus_writeb(_slaveAddr, 0xd1, saturation_regs[level][1]);
	cambus_writeb(_slaveAddr, 0xd2, saturation_regs[level][2]);
    return ret;
}

int Sipeed_GC0328::gc0328_set_gainceiling( gainceiling_t gainceiling)
{
    int ret = 0;

    return ret;
}

int Sipeed_GC0328::gc0328_set_quality(int qs)
{
    int ret = 0;

    return ret;
}

int Sipeed_GC0328::gc0328_set_colorbar(int enable)
{
    int ret = 0;

    return ret;
}

int Sipeed_GC0328::gc0328_set_auto_gain(int enable, float gain_db, float gain_db_ceiling)
{
    int ret = 0;

    return ret;
}

int Sipeed_GC0328::gc0328_get_gain_db(float *gain_db)
{
    int ret = 0;

    return ret;
}

int Sipeed_GC0328::gc0328_set_auto_exposure(int enable, int exposure_us)
{
  int ret = 0;
	uint8_t temp;
	cambus_writeb(_slaveAddr, 0xfe, 0x00);
	cambus_readb(_slaveAddr, 0x4f, &temp);
	if(enable != 0)
	{
		cambus_writeb(_slaveAddr,0x4f, temp|0x01); // enable
		if(exposure_us != -1)
		{
			cambus_writeb(_slaveAddr, 0xfe, 0x01);
			cambus_writeb(_slaveAddr,0x2f, (uint8_t)(((uint16_t)exposure_us)>>8));
			cambus_writeb(_slaveAddr,0x30, (uint8_t)(((uint16_t)exposure_us)));
		}
	}
	else
	{
		cambus_writeb(_slaveAddr,0x4f, temp&0xfe); // disable
		if(exposure_us != -1)
		{
			cambus_writeb(_slaveAddr, 0xfe, 0x01);
			cambus_writeb(_slaveAddr,0x2f, (uint8_t)(((uint16_t)exposure_us)>>8));
			cambus_writeb(_slaveAddr,0x30, (uint8_t)(((uint16_t)exposure_us)));
		}
	}
    return ret;
}

int Sipeed_GC0328::gc0328_get_exposure_us(int *exposure_us)
{
    int ret = 0;
    return ret;
}

int Sipeed_GC0328::gc0328_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db)
{
    int ret = 0;
	uint8_t temp;
	cambus_writeb(_slaveAddr, 0xfe, 0x00);
	cambus_readb(_slaveAddr, 0x42, &temp);
	if(enable != 0)
	{
		cambus_writeb(_slaveAddr,0x42, temp|0x02); // enable
		if(!isnanf(r_gain_db))
			cambus_writeb(_slaveAddr,0x80, (uint8_t)r_gain_db); //limit
		if(!isnanf(g_gain_db))
			cambus_writeb(_slaveAddr,0x81, (uint8_t)g_gain_db);
		if(!isnanf(b_gain_db))
			cambus_writeb(_slaveAddr,0x82, (uint8_t)b_gain_db);
	}
	else
	{
		cambus_writeb(_slaveAddr,0x42, temp&0xfd); // disable
		if(!isnanf(r_gain_db))
			cambus_writeb(_slaveAddr,0x77, (uint8_t)r_gain_db);
		if(!isnanf(g_gain_db))
			cambus_writeb(_slaveAddr,0x78, (uint8_t)g_gain_db);
		if(!isnanf(b_gain_db))
			cambus_writeb(_slaveAddr,0x79, (uint8_t)b_gain_db);
	}
    return ret;
}

int Sipeed_GC0328::gc0328_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db)
{
    int ret = 0;
    return ret;
}

int Sipeed_GC0328::gc0328_set_hmirror(int enable)
{
    uint8_t data;
    cambus_readb(GC0328_ADDR, 0x17, &data);
	data = data & 0xFC;
    if(enable){
        data = data | 0x01 | (_vflip ? 0x02 : 0x00);
    }else{
        data = data | (_vflip ? 0x02 : 0x00);
    }
    return cambus_writeb(GC0328_ADDR, 0x17, data);
}

int Sipeed_GC0328::gc0328_set_vflip(int enable)
{
    uint8_t data;
    cambus_readb(GC0328_ADDR, 0x17, &data);
	data = data & 0xFC;
    if(enable){
        data = data | 0x02 | (_hmirror ? 0x01 : 0x00);
    }else{
        data = data | (_hmirror ? 0x01 : 0x00);
    }
    return cambus_writeb(GC0328_ADDR, 0x17, data);
}

int Sipeed_GC0328::reverse_u32pixel(uint32_t* addr,uint32_t length)
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


int Sipeed_GC0328::sensor_snapshot( )
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
