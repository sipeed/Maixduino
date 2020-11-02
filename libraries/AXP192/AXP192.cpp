#include "AXP192.h"

AXP192::AXP192()
{
  
}

void AXP192::begin(bool isInited)
{
    if(!isInited){
        Wire.begin((uint8_t) SDA, (uint8_t) SCL, 400000);
    }

    Write1Byte(0x46, 0xFF); //Clear interupts
    Write1Byte(0x23, 0x08); //Set DC-DC2 (VCore) to 0V9
    Write1Byte(0x33, 0xC0); //Set Bat Charging Voltage to 4V2, Current to 100mA
    Write1Byte(0x36, 0x0C); //Set 128ms power on, 4s power off
    Write1Byte(0x91, 0xF0); //Set GPIO0 (TFT_BL) (from 0x70 (min)) to 3V3 (max)
    Write1Byte(0x90, 0x02); //Set GPIO0 to LDO mode
    Write1Byte(0x28, 0xF0); //VDD2.8V net: LDO2 3.3V,  VDD 1.5V net: LDO3 1.8V
    Write1Byte(0x27, 0x2C); //VDD1.8V net: DC-DC3 1.8V
    Write1Byte(0x12, 0xFF); //Enble all LDO/DC_DC and EXTEN
    Write1Byte(0x23, 0x08); //VDD 0.9v net: DC-DC2 0.9V
    Write1Byte(0x32, 0x46); //Enable bat detection
    Write1Byte(0x39, 0xFC); //Disable Temp Protection (Sensor doesn't exist!)
    Write1Byte(0x31, (Read8bit(0x31) & 0xf8) | (1 << 2)); // Set Power off voltage 3V0

    fpioa_set_function(INTL_INT, (fpioa_function_t)(FUNC_GPIOHS0 + 26));
    gpiohs_set_drive_mode(26, GPIO_DM_OUTPUT);
    gpiohs_set_pin(26, GPIO_PV_HIGH); //Disable VBUS As Input, BAT->5V Boost->VBUS->Charing Cycle

    msleep(20);
}

void AXP192::Write1Byte( uint8_t Addr ,  uint8_t Data )
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.write(Data);
    Wire.endTransmission();
}

uint8_t AXP192::Read8bit( uint8_t Addr )
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1);
    return Wire.read();
}

uint16_t AXP192::Read12Bit( uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr,2,buf);
    Data = ((buf[0] << 4) + buf[1]);
    return Data;
}

uint16_t AXP192::Read13Bit( uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr,2,buf);
    Data = ((buf[0] << 5) + buf[1]);
    return Data;
}

uint16_t AXP192::Read16bit( uint8_t Addr )
{
    uint16_t ReData = 0;
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2);
    for( int i = 0 ; i < 2 ; i++ )
    {
        ReData <<= 8;
        ReData |= Wire.read();
    }
    return ReData;
}

uint32_t AXP192::Read24bit( uint8_t Addr )
{
    uint32_t ReData = 0;
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 3);
    for( int i = 0 ; i < 3 ; i++ )
    {
        ReData <<= 8;
        ReData |= Wire.read();
    }
    return ReData;
}

uint32_t AXP192::Read32bit( uint8_t Addr )
{
    uint32_t ReData = 0;
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 4);
    for( int i = 0 ; i < 4 ; i++ )
    {
        ReData <<= 8;
        ReData |= Wire.read();
    }
    return ReData;
}

void AXP192::ReadBuff( uint8_t Addr , uint8_t Size , uint8_t *Buff )
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, (int)Size);
    for (int i = 0; i < Size; i++)
    {
        *( Buff + i )  = Wire.read();
    }
}

void AXP192::ScreenBreath(uint8_t brightness)
{
    if (brightness > 12) 
    {
        brightness = 12;
    }
    uint8_t buf = Read8bit( 0x28 );
    Write1Byte( 0x28 , ((buf & 0x0f) | (brightness << 4)) );
}

// Return True = Battery Exist
bool AXP192::GetBatState()
{
    if( Read8bit(0x01) | 0x20 )
        return true;
    else
        return false;
}

// Input Power Status 
uint8_t AXP192::GetInputPowerStatus()
{
    return Read8bit(0x00);
}

// Battery Charging Status 
uint8_t AXP192::GetBatteryChargingStatus()
{
    return Read8bit(0x01);
}

//---------coulombcounter_from_here---------
//enable: void EnableCoulombcounter(void); 
//disable: void DisableCOulombcounter(void);
//stop: void StopCoulombcounter(void);
//clear: void ClearCoulombcounter(void);
//get charge data: uint32_t GetCoulombchargeData(void);
//get discharge data: uint32_t GetCoulombdischargeData(void);
//get coulomb val affter calculation: float GetCoulombData(void);
//------------------------------------------
void  AXP192::EnableCoulombcounter(void)
{
    Write1Byte( 0xB8 , 0x80 );
}

void  AXP192::DisableCoulombcounter(void)
{
    Write1Byte( 0xB8 , 0x00 );
}

void  AXP192::StopCoulombcounter(void)
{
    Write1Byte( 0xB8 , 0xC0 );
}

void  AXP192::ClearCoulombcounter(void)
{
    Write1Byte( 0xB8, Read8bit(0xB8) | 0x20);    // Only set the Clear Flag
}

uint32_t AXP192::GetCoulombchargeData(void)
{
    return Read32bit(0xB0);
}

uint32_t AXP192::GetCoulombdischargeData(void)
{
    return Read32bit(0xB4);
}

float AXP192::GetCoulombData(void)
{
    uint32_t coin = GetCoulombchargeData();
    uint32_t coout = GetCoulombdischargeData();
    uint32_t valueDifferent = 0;
    bool bIsNegative = false;

    if (coin > coout)
    {    // Expected, in always more then out
        valueDifferent = coin - coout;
    }
    else
    {    // Warning: Out is more than In, the battery is not started at 0% 
        // just Flip the output sign later
        bIsNegative = true;
        valueDifferent = coout - coin;
    }
    //c = 65536 * current_LSB * (coin - coout) / 3600 / ADC rate
    //Adc rate can be read from 84H, change this variable if you change the ADC reate
    float ccc = (65536 * 0.5 * valueDifferent) / 3600.0 / 200.0;  // Note the ADC has defaulted to be 200 Hz

    if( bIsNegative )
        ccc = 0.0 - ccc;    // Flip it back to negative
    return ccc;
}
//----------coulomb_end_at_here----------

uint8_t AXP192::GetWarningLeve(void)
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x47);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1);
    uint8_t buf = Wire.read();
    return (buf & 0x01);
}

void AXP192::SetSleep(void)
{
    Write1Byte(0x31 , Read8bit(0x31) | ( 1 << 3)); // Turn on short press to wake up
    Write1Byte(0x90 , Read8bit(0x90) | 0x07); // GPIO1 floating
    Write1Byte(0x82, 0x00); // Disable ADCs
    Write1Byte(0x12, Read8bit(0x12) & 0xA1); // Disable all outputs but DCDC1
}

// Return 0 = not press, 0x01 = long press(1.5s), 0x02 = short press
uint8_t AXP192::GetBtnPress()
{
    uint8_t state = Read8bit(0x46);  // IRQ 3 status.  
    if(state) 
    {
        Write1Byte( 0x46 , 0x03 );   // Write 1 back to clear IRQ
    }
    return state;
}

// Low Volt Level 1, when APS Volt Output < 3.4496 V
// Low Volt Level 2, when APS Volt Output < 3.3992 V, then this flag is SET (0x01)
// Flag will reset once battery volt is charged above Low Volt Level 1
// Note: now AXP192 have the Shutdown Voltage of 3.0V (B100) Def in REG 31H
uint8_t AXP192::GetWarningLevel(void)
{
    return Read8bit(0x47) & 0x01;
}

float AXP192::GetBatVoltage()
{
    float ADCLSB = 1.1 / 1000.0;
    uint16_t ReData = Read12Bit( 0x78 );
    return ReData * ADCLSB;
}

float AXP192::GetBatCurrent()
{
    float ADCLSB = 0.5;
    uint16_t CurrentIn = Read13Bit( 0x7A );
    uint16_t CurrentOut = Read13Bit( 0x7C );
    return ( CurrentIn - CurrentOut ) * ADCLSB;
}

float AXP192::GetVinVoltage()
{
    float ADCLSB = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit( 0x56 );
    return ReData * ADCLSB;
}

float AXP192::GetVinCurrent()
{
    float ADCLSB = 0.625;
    uint16_t ReData = Read12Bit( 0x58 );
    return ReData * ADCLSB;
}

float AXP192::GetVBusVoltage()
{
    float ADCLSB = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit( 0x5A );
    return ReData * ADCLSB;
}

float AXP192::GetVBusCurrent()
{
    float ADCLSB = 0.375;
    uint16_t ReData = Read12Bit( 0x5C );
    return ReData * ADCLSB;
}

float AXP192::GetTempInAXP192()
{
    float ADCLSB = 0.1;
    const float OFFSET_DEG_C = -144.7;
    uint16_t ReData = Read12Bit( 0x5E );
    return OFFSET_DEG_C + ReData * ADCLSB;
}

float AXP192::GetBatPower()
{
    float VoltageLSB = 1.1;
    float CurrentLCS = 0.5;
    uint32_t ReData = Read24bit( 0x70 );
    return  VoltageLSB * CurrentLCS * ReData/ 1000.0;
}

float AXP192::GetBatChargeCurrent()
{
    float ADCLSB = 0.5;
    uint16_t ReData = Read13Bit( 0x7A );
    return ReData * ADCLSB;
}

float AXP192::GetAPSVoltage()
{
    float ADCLSB = 1.4  / 1000.0;
    uint16_t ReData = Read12Bit( 0x7E );
    return ReData * ADCLSB;
}

float AXP192::GetBatCoulombInput()
{
    uint32_t ReData = Read32bit( 0xB0 );
    return ReData * 65536 * 0.5 / 3600 /25.0;
}

float AXP192::GetBatCoulombOut()
{
    uint32_t ReData = Read32bit( 0xB4 );
    return ReData * 65536 * 0.5 / 3600 /25.0;
}

// Can turn LCD Backlight OFF for power saving
void AXP192::SetLDO2( bool State )
{
    uint8_t buf = Read8bit(0x12);
    if( State == true )
    {
        buf = (1<<2) | buf;
    }
    else
    {
        buf = ~(1<<2) & buf;
    }
    Write1Byte( 0x12 , buf );
}

void AXP192::SetLDO3(bool State)
{
    uint8_t buf = Read8bit(0x12);
    if( State == true )
    {
        buf = (1<<3) | buf;
    }
    else
    {
        buf = ~(1<<3) & buf;
    }
    Write1Byte( 0x12 , buf );
}

// Not recommend to set charge current > 100mA, since Battery is only 80mAh.
// more then 1C charge-rate may shorten battery life-span.
void AXP192::SetChargeCurrent(uint8_t current)
{
    uint8_t buf = Read8bit(0x33);
    buf = (buf & 0xf0) | (current & 0x07);
    Write1Byte(0x33, buf);
}

// Cut all power, except for LDO1 (RTC)
void AXP192::PowerOff()
{
    Write1Byte(0x32, Read8bit(0x32) | 0x80);     // MSB for Power Off
}

void AXP192::SetAdcState(bool state)
{
    Write1Byte(0x82, state ? 0xff : 0x00);  // Enable / Disable all ADCs
}

// AXP192 have a 6 byte storage, when the power is still valid, the data will not be lost
void AXP192::Read6BytesStorage( uint8_t *bufPtr )
{
    // Address from 0x06 - 0x0B
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x06);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 6);
    for( int i = 0; i < 6; ++i )
    {
        bufPtr[i] = Wire.read();
    }
}

// AXP192 have a 6 byte storage, when the power is still valid, the data will not be lost
void AXP192::Write6BytesStorage( uint8_t *bufPtr )
{
    // Address from 0x06 - 0x0B
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x06);
    Wire.write(bufPtr[0]);
    Wire.write(0x07);
    Wire.write(bufPtr[1]);
    Wire.write(0x08);
    Wire.write(bufPtr[2]);
    Wire.write(0x09);
    Wire.write(bufPtr[3]);
    Wire.write(0x0A);
    Wire.write(bufPtr[4]);
    Wire.write(0x0B);
    Wire.write(bufPtr[5]);
    Wire.endTransmission();
}
