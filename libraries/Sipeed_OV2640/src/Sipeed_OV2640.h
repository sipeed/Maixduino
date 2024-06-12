#ifndef __SIPEED_OV2640_H
#define __SIPEED_OV2640_H

#include "Camera.h"


#define OV9650_ID       (0x96)
#define OV2640_ID       (0x26)
#define OV7725_ID       (0x77)
#define MT9V034_ID      (0x13)
#define LEPTON_ID       (0x54)
#define OV_CHIP_ID      (0x0A)
#define ON_CHIP_ID      (0x00)
#define GC0328_ID       (0x9d)
#define GC0328_ADDR     (0x42)

typedef enum {
    GAINCEILING_2X,
    GAINCEILING_4X,
    GAINCEILING_8X,
    GAINCEILING_16X,
    GAINCEILING_32X,
    GAINCEILING_64X,
    GAINCEILING_128X,
} gainceiling_t;

typedef enum {
    FRAMERATE_2FPS =0x9F,
    FRAMERATE_8FPS =0x87,
    FRAMERATE_15FPS=0x83,
    FRAMERATE_30FPS=0x81,
    FRAMERATE_60FPS=0x80,
} framerate_t;


class Sipeed_OV2640 : public Camera{

public:
    Sipeed_OV2640(framesize_t frameSize = FRAMESIZE_QVGA, pixformat_t pixFormat = PIXFORMAT_RGB565);
    Sipeed_OV2640(uint16_t width, uint16_t height, pixformat_t pixFormat = PIXFORMAT_RGB565);
    ~Sipeed_OV2640();
    
    virtual bool begin();
    virtual void end();
    bool reset();
    bool setPixFormat(pixformat_t pixFormat);
    bool setFrameSize(framesize_t frameSize);
    virtual bool run(bool run);
    virtual int id();
    /**
     * @return pixels 
     *         If pixels format is RGB565: return RGB565 pixels with every uint16_t one pixel, e.g. RED: 0xF800
     */
    virtual uint8_t* snapshot();
    virtual uint16_t* getRGB565(){ return (uint16_t*)_dataBuffer; };
    virtual uint8_t* getRGB888(){ return _aiBuffer; };
    virtual void setRotation(uint8_t rotation);
    virtual void setInvert(bool invert);
    void flip(bool horiz, bool vert);


private:
    uint8_t* _dataBuffer;    // put RGB565 data
    uint8_t* _aiBuffer;      // put RGB888 data
    uint8_t  _resetPolarity; // reset polarity flag
    uint8_t  _pwdnPolarity;  // PWDN polarity flag
    uint8_t  _slaveAddr;     // camera address
    uint8_t  _id;
    uint32_t _freq;

    int dvpInit(uint32_t freq = 24000000);
    int dvpInitIrq();
    
    int cambus_scan();
    int cambus_read_id(uint8_t addr,uint16_t *manuf_id, uint16_t *device_id);
    int cambus_scan_gc0328(void);
    int cambus_readb(uint8_t slv_addr, uint8_t reg_addr, uint8_t *reg_data);
    int cambus_writeb(uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data);
    int cambus_readw(uint8_t slv_addr, uint8_t reg_addr, uint16_t *reg_data);
    int cambus_writew(uint8_t slv_addr, uint8_t reg_addr, uint16_t reg_data);
    int cambus_readw2(uint8_t slv_addr, uint16_t reg_addr, uint16_t *reg_data);
    int cambus_writew2(uint8_t slv_addr, uint16_t reg_addr, uint16_t reg_data);

    int sensor_ov_detect();
    int sensor_gc_detect();

    int ov2640_reset();
    int ov2640_read_reg(uint8_t reg_addr);
    int ov2640_write_reg(uint8_t reg_addr, uint8_t reg_data);

    int ov2640_set_pixformat(pixformat_t pixformat);
    int ov2640_set_framesize(framesize_t framesize);
    int ov2640_set_framerate(framerate_t framerate);
    int ov2640_set_contrast(int level);
    int ov2640_set_brightness(int level);
    int ov2640_set_saturation(int level);
    int ov2640_set_gainceiling( gainceiling_t gainceiling);
    int ov2640_set_quality(int qs);
    int ov2640_set_colorbar(int enable);
    int ov2640_set_auto_gain(int enable, float gain_db, float gain_db_ceiling);
    int ov2640_get_gain_db(float *gain_db);
    int ov2640_set_auto_exposure(int enable, int exposure_us);
    int ov2640_get_exposure_us(int *exposure_us);
    int ov2640_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db);
    int ov2640_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db);
    int ov2640_set_hmirror(int enable);
    int ov2640_set_vflip(int enable);
    int ov2640_set_flip(bool horiz, bool vert);

    int sensor_snapshot( );
    int reverse_u32pixel(uint32_t* addr,uint32_t length);

};

#endif

