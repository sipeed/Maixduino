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
    rec.addVoiceModel(0, 0, hongse_0, fram_num_hongse_0); //36
    rec.addVoiceModel(0, 1, hongse_1, fram_num_hongse_1); //20
    rec.addVoiceModel(0, 2, hongse_2, fram_num_hongse_2); //23
    rec.addVoiceModel(0, 3, hongse_3, fram_num_hongse_3); //21
    rec.addVoiceModel(1, 0, lvse_0, fram_num_lvse_0);     //20
    rec.addVoiceModel(1, 1, lvse_1, fram_num_lvse_1);     //37
    rec.addVoiceModel(1, 2, lvse_2, fram_num_lvse_2);     //19
    rec.addVoiceModel(1, 3, lvse_3, fram_num_lvse_3);     //26
    rec.addVoiceModel(2, 0, lanse_0, fram_num_lanse_0);   //19
    rec.addVoiceModel(2, 1, lanse_1, fram_num_lanse_1);   //21
    rec.addVoiceModel(2, 2, lanse_2, fram_num_lanse_2);   //28
    rec.addVoiceModel(2, 3, lanse_3, fram_num_lanse_3);   //22
    rec.addVoiceModel(3, 0, guandeng_0, fram_num_guandeng_0);  //26
    rec.addVoiceModel(3, 1, guandeng_1, fram_num_guandeng_1);  //29
    rec.addVoiceModel(3, 2, guandeng_2, fram_num_guandeng_2);  //33
    rec.addVoiceModel(3, 3, guandeng_3, fram_num_guandeng_3);  //29
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
            Serial.println("rec : hongse ");
            break;
        case 2:
            digitalWrite(LED_GREEN, LOW); //power on green led
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_BLUE, HIGH);
            Serial.println("rec : lvse ");
            break;
        case 3:
            digitalWrite(LED_BLUE, LOW); //power on blue led
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            Serial.println("rec : lanse ");
            break;
        case 4:
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_BLUE, HIGH);   //power off all leds
            Serial.println("rec : guandeng ");
        default:
            break;
        }
    }else
    {
        Serial.println("recognize failed.");
    }
    delay(1000);
}