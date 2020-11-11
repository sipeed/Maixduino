#include "Arduino.h"
#include "pins_arduino.h"
#include "Wire.h"

#include "fpioa.h"
#include "i2c.h"

static int maix_i2c_slave_irq(void *userdata);

TwoWire::TwoWire(i2c_device_number_t i2c_device)
{
    i2c_tx_buff = 0;
    i2c_rx_buff = 0;

    _i2c_num = i2c_device;
    switch(i2c_device)
    {
        case I2C_DEVICE_0:
            sda_func = FUNC_I2C0_SDA;
            scl_func = FUNC_I2C0_SCLK;
            break;
        case I2C_DEVICE_1:
            sda_func = FUNC_I2C1_SDA;
            scl_func = FUNC_I2C1_SCLK;
            break;
        case I2C_DEVICE_2:
            sda_func = FUNC_I2C2_SDA;
            scl_func = FUNC_I2C2_SCLK;
            break;
        default:
            break;
    }

}

TwoWire::~TwoWire()
{
    //clear
}

void 
TwoWire::begin(uint8_t sda, uint8_t scl, uint32_t frequency)
{
    fpioa_set_function(sda, sda_func);
    fpioa_set_function(scl, scl_func);

    sysctl_clock_enable((sysctl_clock_t)(SYSCTL_CLOCK_I2C0 + _i2c_num));
    sysctl_clock_set_threshold((sysctl_threshold_t)(SYSCTL_THRESHOLD_I2C0 + _i2c_num), 3);

    volatile i2c_t *i2c_adapter = i2c[_i2c_num];
    i2c_adapter->enable = 0;
    i2c_adapter->intr_mask = 0;
    i2c_adapter->dma_cr = 0x3;
    i2c_adapter->dma_rdlr = 0;
    i2c_adapter->dma_tdlr = 4;
    
    is_master_mode = true;
    
    delete i2c_tx_buff;
    delete i2c_rx_buff;
    i2c_tx_buff = new RingBuffer();
    i2c_rx_buff = new RingBuffer();

    setClock(frequency);
}
    
void 
TwoWire::begin(uint16_t slave_address, uint8_t sda, uint8_t scl)
{
    fpioa_set_function(sda, sda_func);
    fpioa_set_function(scl, scl_func);
    
    sysctl_clock_enable((sysctl_clock_t)(SYSCTL_CLOCK_I2C0 + _i2c_num));
    sysctl_clock_set_threshold((sysctl_threshold_t)(SYSCTL_THRESHOLD_I2C0 + _i2c_num), 3);

    volatile i2c_t *i2c_adapter = i2c[_i2c_num];
    i2c_adapter->enable = 0;
    i2c_adapter->con =  I2C_CON_SPEED(1) | I2C_CON_STOP_DET_IFADDRESSED;
    i2c_adapter->ss_scl_hcnt = I2C_SS_SCL_HCNT_COUNT(37);
    i2c_adapter->ss_scl_lcnt = I2C_SS_SCL_LCNT_COUNT(40);
    i2c_adapter->sar = I2C_SAR_ADDRESS(slave_address);
    i2c_adapter->rx_tl = I2C_RX_TL_VALUE(0);
    i2c_adapter->tx_tl = I2C_TX_TL_VALUE(0);
    i2c_adapter->intr_mask = I2C_INTR_MASK_RX_FULL | I2C_INTR_MASK_START_DET | I2C_INTR_MASK_STOP_DET | I2C_INTR_MASK_RD_REQ;

    is_master_mode = false;

    plic_set_priority((plic_irq_t)(IRQN_I2C0_INTERRUPT + _i2c_num), 1);
    plic_irq_register((plic_irq_t)(IRQN_I2C0_INTERRUPT + _i2c_num), maix_i2c_slave_irq, this);
    plic_irq_enable((plic_irq_t)(IRQN_I2C0_INTERRUPT + _i2c_num));

    delete i2c_tx_buff;
    delete i2c_rx_buff;
    i2c_tx_buff = new RingBuffer();
    i2c_rx_buff = new RingBuffer();

    i2c_adapter->enable = I2C_ENABLE_ENABLE;
}

void
TwoWire::setTimeOut(uint16_t timeOutMillis)
{
    _timeOutMillis = timeOutMillis;
}

uint16_t 
TwoWire::getTimeOut()
{
    return _timeOutMillis;
}

void 
TwoWire::setClock(uint32_t frequency)
{
    i2c_clk = frequency;
    volatile i2c_t *i2c_adapter = i2c[_i2c_num];
    uint8_t speed_mode = I2C_CON_SPEED_STANDARD;

    uint32_t v_i2c_freq = sysctl_clock_get_freq((sysctl_clock_t)(SYSCTL_CLOCK_I2C0 + _i2c_num));
    uint16_t v_period_clk_cnt = floor( (v_i2c_freq*1.0 / i2c_clk / 2) + 0.5 );

    if(v_period_clk_cnt <= 6)
        v_period_clk_cnt = 6;
    if(v_period_clk_cnt >= 65525)//65535-10
        v_period_clk_cnt = 65525;
    if((i2c_clk>100000) && (i2c_clk<=1000000))
        speed_mode = I2C_CON_SPEED_FAST;
    else
        speed_mode = I2C_CON_SPEED_HIGH;
    i2c_adapter->enable = 0;
    i2c_adapter->con = I2C_CON_MASTER_MODE | I2C_CON_SLAVE_DISABLE | I2C_CON_RESTART_EN |
                       (address_width == 10 ? I2C_CON_10BITADDR_SLAVE : 0) | I2C_CON_SPEED(speed_mode);
    i2c_adapter->ss_scl_hcnt = I2C_SS_SCL_HCNT_COUNT(v_period_clk_cnt);
    i2c_adapter->ss_scl_lcnt = I2C_SS_SCL_LCNT_COUNT(v_period_clk_cnt);
    i2c_adapter->enable = I2C_ENABLE_ENABLE;
}

uint32_t 
TwoWire::getClock()
{
    return i2c_clk;
}

int
TwoWire::writeTransmission(uint16_t address, uint8_t* send_buf, size_t send_buf_len, bool sendStop)
{
    volatile i2c_t* i2c_adapter = i2c[_i2c_num];
    size_t fifo_len, index;
    //uint32_t abrt_source;

    if(is_master_mode)
        i2c_adapter->tar = I2C_TAR_ADDRESS(address);
    while (send_buf_len)
    {
        fifo_len = 8 - i2c_adapter->txflr;
        fifo_len = send_buf_len < fifo_len ? send_buf_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            i2c_adapter->data_cmd = I2C_DATA_CMD_DATA(*send_buf++);
        if (i2c_adapter->tx_abrt_source != 0)
        {
            i2c_adapter->clr_tx_abrt;
            usleep(10);
            return 4;
        }
        send_buf_len -= fifo_len;
    }
    while ( (i2c_adapter->status & I2C_STATUS_ACTIVITY) ||
            !(i2c_adapter->status & I2C_STATUS_TFE) )
    {
        if (i2c_adapter->tx_abrt_source != 0)
        {
            i2c_adapter->clr_tx_abrt;
            usleep(10);
            return 4;
        }
    }

    return 0;
}

int
TwoWire::readTransmission(uint16_t address, uint8_t* receive_buf, size_t receive_buf_len, bool sendStop)
{ 
    size_t fifo_len, index;
    // size_t buf_len; 
    size_t rx_len;
    rx_len = receive_buf_len;
    // buf_len = rx_len;
    volatile i2c_t* i2c_adapter = i2c[_i2c_num];
    uint32_t abrt_source;

    if(is_master_mode)
        i2c_adapter->tar = I2C_TAR_ADDRESS(address);

    while (receive_buf_len || rx_len)
    {
        fifo_len = i2c_adapter->rxflr;
        fifo_len = rx_len < fifo_len ? rx_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            *receive_buf++ = (uint8_t)i2c_adapter->data_cmd;
        rx_len -= fifo_len;
        fifo_len = 8 - i2c_adapter->txflr;
        fifo_len = receive_buf_len < fifo_len ? receive_buf_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            i2c_adapter->data_cmd = I2C_DATA_CMD_CMD;
        abrt_source = i2c_adapter->tx_abrt_source;
        if (abrt_source != 0)
        {
            i2c_adapter->clr_tx_abrt;
            usleep(10);
            // if(receive_buf_len == buf_len)
            
            
            return 4;
            // retur_buf_len -= fifo_len;
        }
        receive_buf_len -= fifo_len;
    }
    
    return 0;
}

void 
TwoWire::beginTransmission(uint16_t address)
{
    // Clear buffers when new transation/packet starts
    flush();

    transmitting = 1;
    txAddress = address;
}

uint8_t 
TwoWire::endTransmission(bool sendStop)  //结束时从rxbuff发送数据？
{
    int state = -1;
    int index = 0;
    uint8_t temp = 0;
    size_t tx_data_length = i2c_tx_buff->available();
    if(tx_data_length == 0){
        state = readTransmission(txAddress, &temp, 1, sendStop);
        return state;
    }
    uint8_t tx_data[RING_BUFFER_SIZE];
    while(i2c_tx_buff->available())
    {
        tx_data[index++] = i2c_tx_buff->read_char();
    }
    state = writeTransmission(txAddress, tx_data, tx_data_length,sendStop);
    return state;
}

uint8_t
TwoWire::requestFrom(uint16_t address, uint8_t size, bool sendStop)  //请求数据，存入rxbuff，供read读
{
    // Clear buffers when new transation/packet starts
    flush();

    int state,index = 0;
    uint8_t rx_data[RING_BUFFER_SIZE];
    state = readTransmission(address, rx_data, size, sendStop);
    if(0 == state){
        while(index < size)
        {
            i2c_rx_buff->store_char(rx_data[index++]); 
        }
        return size;
    }
    return 0;
}

size_t 
TwoWire::write(uint8_t data) //写到txbuff
{
    if(transmitting && !i2c_tx_buff->isFull()) {
        i2c_tx_buff->store_char(data);
        return 1;
    }
    return 0;
}

size_t 
TwoWire::write(const uint8_t *data, int quantity)
{
    for(size_t i = 0; i < quantity; i++) {
        if(!write(data[i])) {
            return i;
        }
    }
    return quantity;
}

int TwoWire::available(void)   //rxbuff.available
{
    return i2c_rx_buff->available();
}

int TwoWire::read(void)    //rxbuff.read
{
    return i2c_rx_buff->read_char();
}

int TwoWire::peek(void)    
{ 
    return i2c_rx_buff->peek();
}

int TwoWire::flush(void)
{
    i2c_rx_buff->clear();
    i2c_tx_buff->clear();
    return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity)
{
    return requestFrom(address, static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
    return static_cast<uint8_t>(requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop)));
}

void TwoWire::beginTransmission(int address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

void TwoWire::beginTransmission(uint8_t address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

bool 
TwoWire::busy(void){ 
    return false;
}

void 
TwoWire::onReceive( void (*handler)(int) )
{
    slave_recv_handler = handler;
}
void
TwoWire::onRequest( void (*handler)(void) )
{
    slave_send_handler = handler;
}

void
TwoWire::on_receive(uint8_t rec_data)
{
    if(slave_irq_event_start)
    {
        i2c_rx_buff->store_char(rec_data);
        (*slave_recv_handler)((int)1);//此处跳到接收事件函数
    }
    return;
}

uint8_t
TwoWire::on_transmit()
{
    if(slave_irq_event_start){
        (*slave_send_handler)();//跳到发送事件函数
        return i2c_tx_buff->read_char();
    }
    return 0;
}

uint8_t 
TwoWire::on_event(i2c_event_t event)
{
    if(event == I2C_EV_START)
        slave_irq_event_start = true ;
    else
        slave_irq_event_start = false;
    return 0;
}

static int maix_i2c_slave_irq(void *userdata)
{
    auto &driver = *reinterpret_cast<TwoWire *>(userdata); 
    volatile i2c_t *i2c_adapter = i2c[driver._i2c_num];
    uint32_t status = i2c_adapter->intr_stat;
    if (status & I2C_INTR_STAT_START_DET)
    {
        driver.on_event(I2C_EV_START);
        readl(&i2c_adapter->clr_start_det);
    }
    if (status & I2C_INTR_STAT_RX_FULL)
    {
        driver.on_receive(i2c_adapter->data_cmd);
    }
    if (status & I2C_INTR_STAT_RD_REQ)
    {
        i2c_adapter->data_cmd = driver.on_transmit();
        readl(&i2c_adapter->clr_rd_req);
    }
    if (status & I2C_INTR_STAT_STOP_DET)
    {
        driver.on_event(I2C_EV_STOP);
        readl(&i2c_adapter->clr_stop_det);
    }
    return 0;
}


void
TwoWire::scan(){
    uint8_t temp;
    for (int addr = 0x08; addr < 0x78; ++addr) {
        // int ret = i2c_p->writeto(self, addr, NULL, 0, true);
        // printf("find %x\n",addr);
        int ret = readTransmission(addr,&temp, 1, 1);
        // printf("ret:%x\n",ret);
        if (ret == 0) {
            Serial.print("SCAN Find device:");
            Serial.println(addr,HEX);
        }
    }
}

TwoWire Wire = TwoWire(I2C_DEVICE_0);
TwoWire Wire1 = TwoWire(I2C_DEVICE_1);
TwoWire Wire2 = TwoWire(I2C_DEVICE_2);