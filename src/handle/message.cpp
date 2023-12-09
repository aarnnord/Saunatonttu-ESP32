#include <UniversalTelegramBot.h>

#include <./handle/event.h>
#include <functions.h>
#include "../../.env.h"

void handleMessage(UniversalTelegramBot bot, telegramMessage msg)
{
    String chat_id = msg.chat_id;
    String text = msg.text;

    String chat_id_display = chat_id;
    if (chat_id_display == String(SAUNA_CHAT))
    {
        chat_id_display = "SAUNA_CHAT";
    }

    if (chat_id_display == String(MAINTENANCE_CHAT))
    {
        chat_id_display = "MAINTENANCE_CHAT";
    }

    if (chat_id != MAINTENANCE_CHAT or chat_id != SAUNA_CHAT)
    {
        // Skip unknown chats
        Serial.println("Message " + String(msg.message_id) + " skipped");
        return;
    }

    blink(10);

    // Avaa komennot ja selitteet
    if (text == "/apua" or text == "/start")
    {
        Serial.println("Replaing to " + String(msg.message_id) + " as start");

        String keyboardJson = "[[{ \"text\" : \"Kiukaan kuulumiset\", \"callback_data\" : \"/kiuas\" }]]";
        bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
    }

    // Kertoo kiukaan lämpötilan ja tilan
    if (text == "/kiuas")
    {
        Serial.println("Replaing to " + String(msg.message_id) + " as kiuas");

        String on = getKiuas() ? "päällä" : "pois";
        bot.sendMessage(chat_id, "Kiuas on " + on + ", lämpötila " + String(getTemperature()) + "°C", "Markdown");
    }
}