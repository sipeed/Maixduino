#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PIXFORMAT_INVLAID = 0,
    PIXFORMAT_BAYER,     // RAW
    PIXFORMAT_RGB565,    // RGB565
    PIXFORMAT_YUV422,    // YUV422
    PIXFORMAT_GRAYSCALE, // GRAYSCALE
    PIXFORMAT_JPEG,      // JPEG/COMPRESSED
} pixformat_t;

typedef enum {
    FRAMESIZE_INVALID = 0,
    // C/SIF Resolutions
    FRAMESIZE_QQCIF,    // 88x72
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_CIF,      // 352x288
    FRAMESIZE_QQSIF,    // 88x60
    FRAMESIZE_QSIF,     // 176x120
    FRAMESIZE_SIF,      // 352x240
    // VGA Resolutions
    FRAMESIZE_QQQQVGA,  // 40x30
    FRAMESIZE_QQQVGA,   // 80x60
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_HQQQVGA,  // 60x40
    FRAMESIZE_HQQVGA,   // 120x80
    FRAMESIZE_HQVGA,    // 240x160
    // FFT Resolutions
    FRAMESIZE_64X32,    // 64x32
    FRAMESIZE_64X64,    // 64x64
    FRAMESIZE_128X64,   // 128x64
    FRAMESIZE_128X128,  // 128x128
    // Other
    FRAMESIZE_LCD,      // 128x160
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_WVGA,     // 720x480
    FRAMESIZE_WVGA2,    // 752x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_CUSTOM,
} framesize_t;


class Camera{

public:
    Camera(framesize_t frameSize, pixformat_t pixFormat);
    Camera(int16_t width, uint16_t height, pixformat_t pixFormat);
    ~Camera();
    virtual bool begin( ) = 0;
    virtual void end() = 0;
    // virtual bool reset() = 0;
    // virtual bool setPixFormat(int pixFormat) = 0;
    // virtual bool setFrameSize(int frameSize) = 0;
    virtual bool run(bool run) = 0;
    virtual int id() = 0;
    /**
     * @return pixels 
     *         If pixels format is RGB565: return RGB565 pixels with every uint16_t one pixel, e.g. RED: 0xF800
     */
    virtual uint8_t* snapshot() = 0;
    /**
     * @return pixels with RGB565 format,  every uint16_t one pixel, e.g. RED: 0xF800, so two pixels: {0xF800, 0xF800}
     */
    virtual uint16_t* getRGB565(){ return nullptr; };
    /**
     * 
     * @return pixels with RGB888 format, for n pixels: {{R0,R1,...,Rn-1,},{G0,G1,...,Gn-1},{B0,B1,...,Bn-1}}
     *                                    e.g. two RED pixel: {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}
     */
    virtual uint8_t* getRGB888(){ return nullptr; };
    virtual void setRotation(uint8_t rotation) = 0;
    virtual void setInvert(bool invert) = 0;


    virtual int width(){ return _width; }
    virtual int height(){ return _height; }

protected:
    pixformat_t _pixFormat;
    framesize_t _frameSize;
    int _width;
    int _height;
 };



#endif

