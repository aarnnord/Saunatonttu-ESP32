#include <Arduino.h>
#include <UniversalTelegramBot.h>

void handleEvent(bool kiuas, float temperature, UniversalTelegramBot bot);
bool getKiuas();
float getTemperature();