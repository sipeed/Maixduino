#include "AXP173.h"

AXP173::AXP173()
{
  
}

void AXP173::begin(bool isInited)
{
    if(!isInited){
        Wire.begin((uint8_t) SDA, (uint8_t) SCL, 400000);
    }
#if defined (ARDUINO_MAIX_AMIGO)
    Write1Byte(0x27, 0x20); //LDO4 - 0.8V (default 0x48 1.8V)
    Write1Byte(0x28, 0x0C); //LDO2/3 - LDO2 1.8V / LDO3 3.0V
#else   //ARDUINO_MAIX_CUBE
    Write1Byte(0x46, 0xFF); //Clear interupts
    Write1Byte(0x33, 0xC1); //Set Bat Charging Voltage to 4V2, Current to 190mA
    Write1Byte(0x10, (Read8bit(0x10) & 0xFC)); //EXTEN & DC-DC2 control
#endif    
}

void AXP173::Write1Byte( uint8_t Addr ,  uint8_t Data )
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.write(Data);
    Wire.endTransmission();
}

uint8_t AXP173::Read8bit( uint8_t Addr )
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(Addr);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1);
    return Wire.read();
}

uint16_t AXP173::Read12Bit( uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr,2,buf);
    Data = ((buf[0] << 4) + buf[1]);
    return Data;
}

uint16_t AXP173::Read13Bit( uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr,2,buf);
    Data = ((buf[0] << 5) + buf[1]);
    return Data;
}

uint16_t AXP173::Read16bit( uint8_t Addr )
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

uint32_t AXP173::Read24bit( uint8_t Addr )
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

uint32_t AXP173::Read32bit( uint8_t Addr )
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

void AXP173::ReadBuff( uint8_t Addr , uint8_t Size , uint8_t *Buff )
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


