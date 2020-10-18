#ifndef TwoWire_h
#define TwoWire_h

#include "Arduino.h"
#include "Stream.h"
#include "RingBuffer.h"

#include "fpioa.h"
#include "i2c.h"

#define I2C_BUFFER_LENGTH 128

#define I2C_CON_SPEED_STANDARD                        1 // <=100Kbit/s
#define I2C_CON_SPEED_FAST                            2 // <=400Kbit/s or <=1000Kbit/s
#define I2C_CON_SPEED_HIGH                            3 // <=3.4Mbit/s

typedef void(*user_onRequest)(void);
typedef void(*user_onReceive)(uint8_t*, int);

class TwoWire : public Stream
{
public:
    i2c_device_number_t _i2c_num;

    TwoWire(i2c_device_number_t i2c_device);
    ~TwoWire();
    void begin(uint8_t sda = SDA, uint8_t scl = SCL, uint32_t frequency = 400000);
    void begin(uint16_t slave_address, uint8_t sda = SDA, uint8_t scl = SCL);

    void setClock(uint32_t frequency); 
    uint32_t getClock();

    void setTimeOut(uint16_t timeOutMillis);
    uint16_t getTimeOut();

    int writeTransmission(uint16_t address, uint8_t* send_buf, size_t send_buf_len, bool sendStop);
    int readTransmission(uint16_t address, uint8_t* receive_buf, size_t receive_buf_len, bool sendStop);

    void beginTransmission(uint16_t address);
    void beginTransmission(uint8_t  address);
    void beginTransmission(int      address);

    uint8_t endTransmission(bool sendStop);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint16_t address, uint8_t size, bool sendStop);
    uint8_t requestFrom(uint16_t address, uint8_t size, uint8_t sendStop);
    uint8_t requestFrom(uint16_t address, uint8_t size);
    uint8_t requestFrom(uint8_t address, uint8_t size, uint8_t sendStop);
    uint8_t requestFrom(uint8_t address, uint8_t size);
    uint8_t requestFrom(int address, int size, int sendStop);
    uint8_t requestFrom(int address, int size);

    size_t write(uint8_t);
    size_t write(const uint8_t *, int);
    int available(void);
    int read(void);
    int peek(void);
    int flush(void);

    inline int write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline int write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline int write(long n)
    {
        return write((uint8_t)n);
    }
    inline int write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline int write(int n)
    {
        return write((uint8_t)n);
    }

    void onReceive( void (*)(int) );
    void onRequest( void (*)(void) );

    uint8_t on_event(i2c_event_t event);
    uint8_t on_transmit();
    void on_receive(uint8_t rec_data);

    bool busy();

    void scan();

private:
    uint16_t i2c_slave_address;
    bool is_master_mode = false;
    uint32_t address_width = 7;
    uint8_t sda = FPIOA_NUM_IO;
    uint8_t scl = FPIOA_NUM_IO;
    uint32_t i2c_clk;
    fpioa_function_t sda_func;
    fpioa_function_t scl_func;

    RingBuffer *i2c_tx_buff;
    RingBuffer *i2c_rx_buff;

    uint16_t txAddress;
    uint8_t transmitting;

    uint16_t _timeOutMillis;

    bool slave_irq_event_start = false;

    void (*slave_recv_handler)(int) ;
    void (*slave_send_handler)(void) ;

};



extern volatile i2c_t* const i2c[3];

extern TwoWire Wire;
extern TwoWire Wire1;
extern TwoWire Wire2;

#endif //TwoWire_h