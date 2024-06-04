#ifndef __SIPEED_GC0328_H
#define __SIPEED_GC0328_H

#include "Camera.h"
#include "Wire.h"

#define GC0328_ID       (0x9d)
#define GC0328_ADDR     (0x21)

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


class Sipeed_GC0328 : public Camera{

public:
    Sipeed_GC0328(framesize_t frameSize = FRAMESIZE_QVGA, pixformat_t pixFormat = PIXFORMAT_RGB565, TwoWire *i2c = &Wire);
    Sipeed_GC0328(uint16_t width, uint16_t height, pixformat_t pixFormat = PIXFORMAT_RGB565, TwoWire *i2c = &Wire);
    ~Sipeed_GC0328();
    
    virtual bool begin();
    bool begin(bool binocular);
    virtual void end();
    bool reset(bool binocular = false);
    void debug(bool enable);
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
    virtual void shutdown(bool enable);
    void setFlip(bool flip);

private:
    uint8_t* _dataBuffer;    // put RGB565 data
    uint8_t* _aiBuffer;      // put RGB888 data
    uint8_t  _resetPolarity; // reset polarity flag
    uint8_t  _pwdnPolarity;  // PWDN polarity flag
    uint8_t  _slaveAddr;     // camera address
    uint8_t  _id;
    uint32_t _freq;
    TwoWire *_i2c; // replace sccb
    bool _vflip;
    bool _hmirror;
    bool _debug;

    int dvpInit(uint32_t freq = 14000000);
    int dvpInitIrq();

    int cambus_scan();
    int cambus_read_id(uint8_t addr,uint16_t *manuf_id, uint16_t *device_id);
    int cambus_scan_gc0328(void);
    int cambus_readb(uint8_t slv_addr, uint8_t reg_addr, uint8_t *reg_data);
    int cambus_writeb(uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data);

    int sensor_gc_detect();

    int gc0328_reset();
    int gc0328_read_reg(uint8_t reg_addr);
    int gc0328_write_reg(uint8_t reg_addr, uint8_t reg_data);

    int gc0328_set_pixformat(pixformat_t pixformat);
    int gc0328_set_framesize(framesize_t framesize);
    int gc0328_set_framerate(framerate_t framerate);
    int gc0328_set_contrast(int level);
    int gc0328_set_brightness(int level);
    int gc0328_set_saturation(int level);
    int gc0328_set_gainceiling( gainceiling_t gainceiling);
    int gc0328_set_quality(int qs);
    int gc0328_set_colorbar(int enable);
    int gc0328_set_auto_gain(int enable, float gain_db, float gain_db_ceiling);
    int gc0328_get_gain_db(float *gain_db);
    int gc0328_set_auto_exposure(int enable, int exposure_us);
    int gc0328_get_exposure_us(int *exposure_us);
    int gc0328_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db);
    int gc0328_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db);
    int gc0328_set_hmirror(int enable);
    int gc0328_set_vflip(int enable);
    int sensor_snapshot( );
    int reverse_u32pixel(uint32_t* addr,uint32_t length);

};

#endif
