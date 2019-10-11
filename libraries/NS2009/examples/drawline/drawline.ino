#include <Sipeed_ST7789.h>
#include "touchscreen.h"

SPIClass spi_(SPI0); // MUST be SPI0 for Maix series on board LCD
Sipeed_ST7789 lcd(320, 240, spi_);
TouchScreen touchscreen;
int key = KEY0;

int touchscreen_status = 0;
int touchscreen_x = 0;
int touchscreen_y = 0;
int status_last = TOUCH_NONE;
int x_last = 0;
int y_last = 0;
bool draw = false;


void setup()
{
    pinMode(key, INPUT);  
    lcd.begin(15000000, COLOR_RED);    
    touchscreen.begin();
    touchscreen.calibrate(320, 240);
}

void loop()
{
    touchscreen.read();
    touchscreen_status = touchscreen.getStatus();
    touchscreen_x = touchscreen.getX();
    touchscreen_y = touchscreen.getY();
    if (draw) {
        lcd.writeLine(x_last,y_last,touchscreen_x,touchscreen_y,COLOR_WHITE);
    }
    if (status_last != touchscreen_status) {
        draw = (touchscreen_status == TOUCH_BEGIN || touchscreen_status == TOUCH_MOVE);
        status_last = touchscreen_status;
    }
    x_last = touchscreen_x;
    y_last = touchscreen_y;
    if (digitalRead(key) == LOW) {
        lcd.fillScreen(COLOR_BLACK);
    }
}
