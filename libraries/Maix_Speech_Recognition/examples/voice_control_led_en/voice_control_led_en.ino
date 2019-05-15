#include "Maix_Speech_Recognition.h"
#include "voice_model.h"

SpeechRecognizer rec;

void setup()
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    rec.begin();
    Serial.begin(115200);
    Serial.println("init model...");
    rec.addVoiceModel(0, 0, red_0, fram_num_red_0); 
    rec.addVoiceModel(0, 1, red_1, fram_num_red_1); 
    rec.addVoiceModel(0, 2, red_2, fram_num_red_2); 
    rec.addVoiceModel(0, 3, red_3, fram_num_red_3); 
    rec.addVoiceModel(1, 0, green_0, fram_num_green_0);     
    rec.addVoiceModel(1, 1, green_1, fram_num_green_1);     
    rec.addVoiceModel(1, 2, green_2, fram_num_green_2);     
    rec.addVoiceModel(1, 3, green_3, fram_num_green_3);     
    rec.addVoiceModel(2, 0, blue_0, fram_num_blue_0);   
    rec.addVoiceModel(2, 1, blue_1, fram_num_blue_1);   
    rec.addVoiceModel(2, 2, blue_2, fram_num_blue_2);   
    rec.addVoiceModel(2, 3, blue_3, fram_num_blue_3);   
    rec.addVoiceModel(3, 0, turnoff_0, fram_num_turnoff_0);  
    rec.addVoiceModel(3, 1, turnoff_1, fram_num_turnoff_1);  
    rec.addVoiceModel(3, 2, turnoff_2, fram_num_turnoff_2);  
    rec.addVoiceModel(3, 3, turnoff_3, fram_num_turnoff_3);  
    Serial.println("init model ok!");
}
void loop()
{
    int res;
    res = rec.recognize();
    Serial.printf("res : %d ", res);
    if (res > 0){
        switch (res)
        {
        case 1:
            digitalWrite(LED_RED, LOW); //power on red led
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_BLUE, HIGH);
            Serial.println("rec : red ");
            break;
        case 2:
            digitalWrite(LED_GREEN, LOW); //power on green led
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_BLUE, HIGH);
            Serial.println("rec : green ");
            break;
        case 3:
            digitalWrite(LED_BLUE, LOW); //power on blue led
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            Serial.println("rec : blue ");
            break;
        case 4:
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_BLUE, HIGH);   //power off all leds
            Serial.println("rec : turnoff ");
        default:
            break;
        }
    }else
    {
        Serial.println("recognize failed.");
    }
    delay(1000);
}