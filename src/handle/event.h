#include <UniversalTelegramBot.h>
#include <Arduino.h>

void handleEvent(bool kiuas, uint8_t temperature, UniversalTelegramBot bot);
bool getKiuas();
uint8_t getTemperature();