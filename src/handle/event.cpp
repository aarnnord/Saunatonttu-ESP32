
#include <UniversalTelegramBot.h>

#include "../../.env.h"
#include "ruuvi_tag.h"
#include "app.h"
#include <Arduino.h>

bool kiuas_before = false;
int temperature_before = 0;
bool ready_before = false;

void handleEvent(bool kiuas, uint8_t temperature, UniversalTelegramBot bot)
{
    if (kiuas_before != kiuas)
    {
        Serial.println("Kiuas changed from " + String(kiuas_before) + " to " + String(kiuas));

        // Handle Kiuas events
        if (kiuas)
        {
            bot.sendMessage(SAUNA_CHAT, "Sauna lämpiää, lämpötila " + String(temperature) + "°C", "Markdown");
        }

        if (!kiuas)
        {
            bot.sendMessage(SAUNA_CHAT, "Sauna sammui, lämpötila " + String(temperature) + "°C", "Markdown");
            ready_before = false;
        }

        kiuas_before = kiuas;
    }

    if (temperature_before != temperature)
    {
        Serial.println("Temperature changed from " + String(temperature_before) + " to " + String(temperature));

        // Handle Temperature events
        if (temperature >= 70 and kiuas and !ready_before)
        {
            bot.sendMessage(SAUNA_CHAT, "Sauna valmis, lämpötila " + String(temperature) + "°C", "Markdown");
        }

        temperature_before = temperature;
    }
}

bool getKiuas()
{
    return kiuas_before;
}

uint8_t getTemperature()
{
    receivedTemperatureRequestOverWiFi = True;
    // uint8_t temperature_before = check_ruuvi_tag_data((uint8_t *)RUUVI_RAWV2, RUUVI_RAWV2ADV_LENGTH, RUUVI_RAWV2_LENGTH);
    return temperature_before;
}