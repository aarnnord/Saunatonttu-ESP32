#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <../.env.h>

// Mean time between scan messages
const unsigned long BOT_MTBS = 1000;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// Last time messages' scan has been done
unsigned long bot_lasttime;

const int ledPin = LED_BUILTIN;
int ledStatus = 0;

// Use blink to blink led ;)
void blink(long time)
{
  digitalWrite(ledPin, LOW);
  delay(time);
  digitalWrite(ledPin, HIGH);
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon")
    {
      // digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff")
    {
      ledStatus = 0;
      // digitalWrite(ledPin, HIGH); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/options")
    {
      String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/status\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }

    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Flash Led Bot example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println();

  pinMode(ledPin, OUTPUT); // Initialize digital ledPin as an output.

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
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      blink(20);
      Serial.println("New message");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
