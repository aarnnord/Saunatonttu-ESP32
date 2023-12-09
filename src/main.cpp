#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <functions.h>

// Handlers
#include <./handle/message.h>
#include <./handle/event.h>

#include <../.env.h>

// Mean time between scan messages
const unsigned long BOT_MTBS = 1000;

// Mean time between scan events
const unsigned long EVENT_MTBS = 400;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Last time messages scan has been done
unsigned long bot_lasttime;

// Last time events scan has been done
unsigned long event_lasttime;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  setupLed();

  // Attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org", "fi.pool.ntp.org", "time.mikes.fi"); // Get UTC time via NTP
  secured_client.setTrustAnchors(&cert);                                // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    blink(50);
    Serial.print(".");
    delay(50);
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(MAINTENANCE_CHAT, "Saunatonttu on käynnistynyt.", "Markdown");
}

void loop()
{

  // Handle events
  bool kiuas = false;
  int temperature = 0;

  if (millis() - event_lasttime > EVENT_MTBS)
  {
    handleEvent(kiuas, temperature, bot);
    event_lasttime = millis();
  }

  // Handle incoming messages
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      for (int i = 0; i < numNewMessages; i++)
      {
        handleMessage(bot, bot.messages[i]);
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
